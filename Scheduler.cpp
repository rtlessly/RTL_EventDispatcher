#define DEBUG 0

#include <Arduino.h>
#include <RTL_Debug.h>
#include "EventSource.h"
#include "Scheduler.h"


/*******************************************************************************
Global poller and event dispatcher.

Scheduler is a global static singleton that polls objects and dispatches events
in a program. Objects register with the Scheduler via the Add() methods and then
the Scheduler invokes the Poll() method of each Object.

The Scheduler::DispatchEvents() method should be invoked on every iteration
of the sketch's loop() method. The DispatchEvent() method only polls one object
per iteration. That is, it will poll the first object on the first iteration, the
second object on the second iteration, and so on. When it reaches the end of the
polling list it will start over. This technique is a compromise that allows each
iteration of the loop() method to finish as quickly as possible, while still
ensuring that all objects are polled in a timely fashion.
*******************************************************************************/

//static DebugHelper Debug("Scheduler");
DEFINE_CLASSNAME(Scheduler);

IPollable* Scheduler::_first;
IPollable* Scheduler::_current;

Event   Scheduler::_queue[QUEUE_SIZE];
uint8_t Scheduler::_queueHead = 0;
uint8_t Scheduler::_queueTail = 0;
uint8_t Scheduler::_queueCount = 0;


//******************************************************************************
// Add a poll-able object the polling list
//******************************************************************************
void Scheduler::Add(IPollable& obj)
{
    // If the linked list anchor is empty then we just add the object there
    if (_first == NULL)
    {
        _first = &obj;
        _current = _first;
    }
    else
    {
        // Otherwise, we have to walk the linked list to find the end and add the
        // object there
        for (IPollable* pObj = _first; pObj != NULL; pObj = pObj->_nextObject)
        {
            if (pObj->_nextObject == NULL)
            {
                pObj->_nextObject = &obj;
                break;
            }
        }
    }

    // Since the object is always added at the end of the list, make sure its
    // 'next' pointer is empty
    obj._nextObject = NULL;
}


//******************************************************************************
// Add a poll-able function the polling list
//******************************************************************************
IPollable* Scheduler::Add(POLL_FUNCTION pfPollFunction)
{
    IPollable* pollObj = new PollableDelegate(pfPollFunction);

    Add(*pollObj);

    return pollObj;
}


//******************************************************************************
// Removes an object from the polling list
//******************************************************************************
void Scheduler::Remove(IPollable& obj)
{
    // Special case if obj is the anchor object of the linked-list
    if (_first == &obj)
    {
        _first = obj._nextObject;
        TRACE(Logger(_classname_, __func__) << F("Removed first object=") << _HEX(PTR(&obj)) << endl);
    }
    else
    {
        // Otherwise, we have to walk the linked list to find the object
        for (IPollable* pObj = _first; pObj != NULL; pObj = pObj->_nextObject)
        {
            if (pObj->_nextObject == &obj)
            {
                pObj->_nextObject = obj._nextObject;
                TRACE(Logger(_classname_, __func__) << F("Removed object=") << _HEX(PTR(&obj)) << endl);
                break;
            }
        }
    }

    // Adjust the _current pointer if we are removing the current object
    if (_current == &obj) _current = (obj._nextObject != NULL) ? obj._nextObject : _first;

    // Make sure the object's 'next' pointer is empty to prevent future issues
    obj._nextObject = NULL;
}


//******************************************************************************
// Queues an event to the event queue
//******************************************************************************
bool Scheduler::Queue(Event& event)
{
    /*
    Interrupts MUST be disabled while an event is being queued to ensure stability
    while the queue is being manipulated. But, disabling interrupts MUST come
    BEFORE the full queue check is done.

    If the full queue check is FALSE and an asynchronous interrupt queues an
    event while that check is being made, thus making the queue full, the current
    insert operation will then corrupt the queue (because it will add an event to
    an already full queue). So the entire operation, from the full queue check to
    completing the insert must be atomic.

    Note that this race condition can only happen when an event is queued by normal
    (non-interrupt) code and simultaneously an interrupt also tries to queue an event.
    In the case where only normal code is queueing events, this can't happen because
    then there are no interrupts that could asynchronously queue an event. Conversely,
    if only interrupts are queueing events, it can't happen either because further
    interrupts are blocked while the current interrupt is being serviced. In the
    general case, however, we have to assume that events could enter the queue both
    ways. So, to protect against this race condition, we have to disable interrupts.

    Contrast this with the logic in the Dequeue() method.
    */

    bool isQueued = false;
    int  i = _queueTail;

    noInterrupts(); // ATOMIC BLOCK BEGIN

    if (_queueCount <= QUEUE_SIZE)
    {
        _queue[_queueTail] = event;
        _queueTail = (_queueTail + 1) % QUEUE_SIZE;
        _queueCount++;

        isQueued = true;
    }

    interrupts(); // ATOMIC BLOCK END

    return isQueued;
}


bool Scheduler::Dequeue(Event& event)
{
    TRACE(Logger(_classname_, __func__) << endl);

    /*
    Interrupts MUST be disabled while an event is being de-queued to ensure
    stability while the queue is being manipulated. HOWEVER, disabling
    interrupts (the call to cli) MUST come AFTER the empty queue check.

    There is no harm if the empty check produces an "incorrect" TRUE response
    while an asynchronous interrupt queues. It will just pick up that event
    the next time Dequeue() is called.

    If interrupts are suppressed before the empty queue check, we pretty much
    lock-up the system. This is because Dequeue() is normally called inside
    loop(), which means it is called VERY OFTEN.  Most of the time (>99%), the
    event queue will be empty. But that means that we'll have interrupts turned
    off for a significant fraction of the time. We don't want to do that. We only
    want interrupts turned off when we actually have an event to de-queue.

    Contrast this with the logic in the Queue() method.
    */

    if (_queueHead == _queueTail) return false;

    noInterrupts(); // ATOMIC BLOCK BEGIN

    event = _queue[_queueHead];
    _queueHead = (_queueHead + 1) % QUEUE_SIZE;
    _queueCount--;

    interrupts(); // ATOMIC BLOCK END

    return true;
}


//******************************************************************************
// Polls all sources to dispatch events
//******************************************************************************
void Scheduler::DispatchEvents()
{
#if DEBUG
   for (auto p = _first; p != NULL; p = p->_nextObject)
   {
       Logger(_classname_, __func__) << F("ID=") << p->ID() << F(", addr=") << _HEX(PTR(p))<< endl;
   }
#endif

    // Poll the next object in the polling list.
    if (_current != NULL)
    {
        TRACE(Logger(_classname_, __func__) << F(": Polling:") << _current->ID() << F(" addr=") << _HEX(PTR(_current))  << endl);

        _current->Poll();
        _current = (_current->_nextObject != NULL) ? _current->_nextObject : _first;
    }

//    return;

    // Remember the queue tail index before we start dispatching events. This
    // ensures that we only go around the queue at most 1 time. Any new events
    // posted as a result of processing the currently queued events will get
    // dispatched on the next go-around.
    // Otherwise, we could create an endless loop where object A post an event
    // that object B receives who, in turn, posts an event that object A receives,
    // etc... In such a scenario the event queue would never get emptied and the
    // event dispatch loop would go on forever.
    auto queueTail = _queueTail;
    Event event;

    // Dispatch all events that were queued up to this point.
    // NOTE: This does NOT dispatch any new events added as a result of processing
    //       a dispatched event. Those will get processed on the next go-around.
    while (_queueHead != queueTail)
    {
        if (!Dequeue(event)) break; // If something goes wrong then break the loop

        if (event.Source != NULL) event.Source->DispatchEvent(event);
    }
}

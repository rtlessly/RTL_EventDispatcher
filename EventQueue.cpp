#include "EventQueue.h"


/********************************************************************
Implementation for EventQueue class
********************************************************************/
EventQueue::EventQueue() : _queueHead(0), _queueTail(0), _count(0)
{
    for (int i = 0; i < QueueSize; i++)
    {
        _queue[i].EventID = 0;
        _queue[i].Data.Long = 0;
    }
}


bool EventQueue::Queue(Event& event)
{
    /*
    Interrupts MUST be disabled while an event is being queued to ensure stability
    while the queue is being manipulated. But, disabling interrupts MUST come 
    BEFORE the full queue check is done.
    
    If the call to isFull() returns FALSE and an asynchronous interrupt queues an
    event while that check is being made, thus making the queue full, the current
    insert operation will then corrupt the queue (because it will add an event to
    an already full queue). So the entire operation, from the call to isFull() to
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

    bool retVal = false;
    
    cli(); // ATOMIC BLOCK BEGIN

    if (!IsFull())
    {
        _queue[_queueTail] = event;
        _queueTail = (_queueTail + 1) % QueueSize;
        _count++;

        retVal = true;
    }

    sei(); // ATOMIC BLOCK END    
    
    return retVal;
}


bool EventQueue::Dequeue(Event& event)
{
    /*
    Interrupts MUST be disabled while an event is being de-queued to ensure 
    stability while the queue is being manipulated. HOWEVER, disabling 
    interrupts (the call to cli) MUST come AFTER the empty queue check.
    
    There is no harm if the IsEmpty() call returns an "incorrect" TRUE response 
    while an asynchronous interrupt queues an during the IsEmpty() call. It will 
    just pick up that event the next time Dequeue() is called.
    
    If interrupts are suppressed before the IsEmpty() check, we pretty much 
    lock-up the Arduino. This is because Dequeue() is normally called inside 
    loop(), which means it is called VERY OFTEN.  Most of the time (>99%), the 
    event queue will be empty. But that means that we'll have interrupts turned 
    off for a significant fraction of the time. We don't want to do that. We only
    want interrupts turned off when we actually have an event to de-queue.
    
    Contrast this with the logic in the Queue() method.
    */

    if (IsEmpty()) return false;
    
    cli(); // ATOMIC BLOCK BEGIN
    
    event = _queue[_queueHead];
    _queueHead = (_queueHead + 1) % QueueSize;
    _count--;
    
    sei(); // ATOMIC BLOCK END
        
    return true;
}


void EventQueue::Poll()
{
    Event event;
    
    if (Dequeue(event)) DispatchEvent(&event);
}

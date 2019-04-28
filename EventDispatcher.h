#ifndef _EventDispatcher_h_
#define _EventDispatcher_h_

#include <RTL_StdLib.h>
#include "IPollable.h"
#include "Event.h"


/*******************************************************************************
Global scheduler and event dispatcher.

EventDispatcher is a global static singleton that polls objects and dispatches 
events in a program. Objects that implement IPollable register with the EventDispatcher
and then the EventDispatcher periodically invokes the Poll() method of each one. 
The DispatchEvents() method must be invoked in the sketch's loop() method.

Even though EventSources are the most common kind of objects polled by the EventDispatcher,
other kinds of objects can also be polled as long as they implement the IPollable 
interface and register with the EventDispatcher.
*******************************************************************************/
class EventDispatcher       // size = 71 bytes
{
    DECLARE_CLASSNAME;

    private: static const int QUEUE_SIZE = 8;

    /***************************************************************************
     Constructors
    ***************************************************************************/
    // Private constructor prevents instances from being created
    private: EventDispatcher() {};

    /***************************************************************************
    Public Methods
    ***************************************************************************/

    /// Polls the registered poll-able objects.
    /// This method must be called from the Sketch's loop() method.
    public: static void DispatchEvents();

    /// Adds a poll-able object
    public: static void Add(IPollable& obj);

    /// Adds a poll function
    public: static IPollable* Add(POLL_FUNCTION pfPollFunction);

    /// Removes a poll-able object
    public: static void Remove(IPollable& obj);

    public: static bool Queue(Event& event);

    //public: static bool Queue(EventSource& source, Event& event);

    /// De-queues an event from the event queue.
    public: static bool Dequeue(Event& event);

    /***************************************************************************
    Internal implementation
    ***************************************************************************/
    private: inline static void _Dequeue(Event& event)
    {
        noInterrupts(); // ATOMIC BLOCK BEGIN

        event = _queue[_queueHead];
        _queueHead = (_queueHead + 1) % QUEUE_SIZE;
        _queueCount--;

        interrupts(); // ATOMIC BLOCK END
    }
    
    
    /***************************************************************************
    Internal state
    ***************************************************************************/
    /// The event queue 
    private: static Event _queue[QUEUE_SIZE];   // size = sizeof(Event)*QUEUE_SIZE = 8*8 = 64 bytes

    /// The event queue head and tail pointers
    private: static uint8_t _queueHead;         // size = 1
    private: static uint8_t _queueTail;         // size = 1
    private: static int8_t  _queueCount;        // size = 1

    /// The first object in the EventDispatcher's polling list (linked list)
    private: static IPollable* _first;          // size = 2

    /// The current object being polled
    private: static IPollable* _current;        // size = 2
};

#endif

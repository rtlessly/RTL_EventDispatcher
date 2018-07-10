#ifndef _Scheduler_h_
#define _Scheduler_h_

#include <RTL_StdLib.h>
#include "IPollable.h"
#include "Event.h"

/*******************************************************************************
Global scheduler and event dispatcher.

Scheduler is a global static singleton that polls objects and dispatches events 
in a program. Objects that implement IPollable register with the Scheduler and 
then the Scheduler periodically invokes the Poll() method of each one. 
The DispatchEvents() method must be invoked in the sketch's loop() method.

Even though EventSources are the most common kind of objects polled by the Scheduler,
other kinds of objects can also polled as long as they implement the IPollable interface.
*******************************************************************************/
class Scheduler       // size = 71 bytes
{
	DECLARE_CLASSNAME;

    private: static const int QUEUE_SIZE = 8;

    /***************************************************************************
     Constructors
    ***************************************************************************/
    // Private constructor prevents instances from being created
    private: Scheduler() {};

    /***************************************************************************
    Public Methods
    ***************************************************************************/

    /// Adds a poll-able object
    public: static void Add(IPollable& obj);

    /// Adds a poll function
    public: static IPollable* Add(POLL_FUNCTION pfPollFunction);

    /// Removes a poll-able object
    public: static void Remove(IPollable& obj);

    public: static bool Queue(Event& event);

    //public: static bool Queue(EventSource& source, Event& event);

    /// Polls the registered poll-able objects.
    /// This method must be called from the Sketch's loop() method.
    public: static void DispatchEvents();

    /// De-queues an event from the event queue.
    public: static bool Dequeue(Event& event);

    /***************************************************************************
    Internal state
    ***************************************************************************/
    /// The event queue 
    private: static Event _queue[QUEUE_SIZE];   // size = sizeof(Event)*QUEUE_SIZE = 8*8 = 64 bytes

    /// The event queue head and tail pointers
    private: static uint8_t _queueHead;         // size = 1
    private: static uint8_t _queueTail;         // size = 1
    private: static uint8_t _queueCount;        // size = 1

    /// The first object in the Scheduler's polling list (linked list)
    private: static IPollable* _first;          // size = 2

    /// The current object being polled
    private: static IPollable* _current;        // size = 2
};

#endif

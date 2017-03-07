#ifndef _EventDispatcher_h
#define _EventDispatcher_h

#include "EventSource.h"


/*******************************************************************************
Global event dispatcher.

EventDispatcher is a static singleton that dispatches all events in a program.
EventSource objects register with the EventDispatcher and then the EventDispatcher
invokes the Poll() method of each EventSource. The EventDispatcher::DispatchEvents()
method must be invoked in the sketch's loop() method.

Even though EventSources are the most common kind of objects polled by the EventDispatcher,
other kinds of objects can also need to be polled as long as they implement the
IPollable interface.
*******************************************************************************/
class EventDispatcher
{
    /***************************************************************************
     Constructors
    ***************************************************************************/
    // Private constructor prevents instances from being created
    private: EventDispatcher() {};

    /***************************************************************************
    Public Methods
    ***************************************************************************/

    /// Adds a poll-able object
    public: static void Add(IPollable& obj);

    /// Adds a poll function
    public: static IPollable* Add(POLL_FUNCTION pfPollFunction);

    /// Removes a poll-able object
    public: static void Remove(IPollable& obj);

    /// Polls the registered poll-able objects.
    /// This method must be called from the Sketch's loop() method.
    public: static void DispatchEvents();

    /***************************************************************************
    Internal state
    ***************************************************************************/

    /// The first object in the EventDispatcher's polling list (linked list)
    private: static IPollable* _first;

    /// The current object being polled
    private: static IPollable* _current;
};

#endif

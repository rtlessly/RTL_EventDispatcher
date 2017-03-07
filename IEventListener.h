#ifndef _IEventListener_h
#define _IEventListener_h

#include "Event.h"


typedef void (*EVENT_LISTENER)(const Event* pEvent);


/*******************************************************************************
Defines an interface for an event listener. This is an abstract base class that 
must be extended by a derived class.

An event listener monitors an EventSource for events. To do so, it attaches itself 
to the EventSource via an EventBinding. The EventSource will then send event 
notifications to the event listener's OnEvent() method as it generates events.

Multiple event listeners can be attached to the same EventSource, and an event
listener can be attached to multiple EventSources.

A class that implements this interface must provide an implementation for the
OnEvent() method to handle events dispatched to it.
*******************************************************************************/
class IEventListener
{
    //**************************************************************************
    // Constructors
    //**************************************************************************

    /// The constructor is protected to enforce abstract base class semantics
    protected: IEventListener() {};

    //**************************************************************************
    // Protected methods
    //**************************************************************************

    public: virtual void OnEvent(const Event* pEvent) = 0;
    
    //**************************************************************************
    // Internal state
    //**************************************************************************
};

#endif

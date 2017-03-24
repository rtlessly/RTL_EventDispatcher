#ifndef _EventSource_h_
#define _EventSource_h_

#include <inttypes.h>
#include <Variant.h>
#include "IPollable.h"
#include "EventCodes.h"
#include "IEventListener.h"


class IEventBinding;
class EventBinding;
class StaticEventBinding;


/*******************************************************************************
A base class for an object that sources events. This is an abstract base class 
that must be extended by a derived class.

An EventSource generates events that are of interest to other objects called 
"event listeners". To be an event listener, an object must implement the IEventListener 
interface and attach itself to an EventSource object via the EventSource::AddListener() 
method. An EventSource notifies its listeners when it generates an event by calling 
the IEventListener::OnEvent() method for each listener. The listener implements 
whatever functionality it needs in the OnEvent() method to respond to the event.

Multiple event listeners can be attached to the same EventSource. The event listeners
are chained together as a linked list through the _nextEventListener member of 
the IEventListner interface. The _first member of EventSource points to the head
of this linked list. The AddListener() and RemoveListener() methods manage this 
linked list. To allow them to do that, IEventListener declares EventSource as a 
friend class so it can access the private _nextEventListener member.

EventSources attach themselves to the global EventDispatcher object when they are
created. The EventDispatcher then calls the Poll() method of each EventSource 
whenever its DispatchEvents() method is called. To ensure events are detected 
and dispatched as expeditiously as possible, the EventDispatcher::DispatchEvents() 
method should be called on every iteration in a sketch's loop() method.

A class that extends this base class must provide an implementation for the Poll() 
method to detect and dispatch events.
*******************************************************************************/
class EventSource : public IPollable
{
    /***************************************************************************
    Constructors
    ***************************************************************************/
    
    /// The constructor is protected to enforce abstract base class semantics
    protected: EventSource();
    
    /***************************************************************************
    Public Methods
    ***************************************************************************/

    /// Adds an event binging to this source
    public: void Attach(IEventBinding& binding);

    /// Removes an event binging from this source
    public: void Detach(IEventBinding& binding);

    /// Adds a listener object to this source
    public: IEventBinding* Attach(IEventListener& listener, EventBinding* pBinding=NULL);
    
    /// Adds a listener function to this source
    public: IEventBinding* Attach(EVENT_LISTENER pfListener, StaticEventBinding* pBinding=NULL);
    
    /// Polls this source for events. 
    /// A derived class typically implements this method to detect and dispatch
    /// events (e.g., if a button was pressed, a sensor was triggered, etc.).
    public: virtual void Poll() = 0;

    /// Determines if the source has any listeners attached
    public: bool HasListeners() { return _firstBinding != NULL; };
    
    /// Returns a new unique event ID with every call. Used to assign event IDs
    public: static EVENT_ID GenerateEventID() { return _nextEventID++; }
    
    /***************************************************************************
    Protected Methods
    ***************************************************************************/

    /// Creates and dispatches an event with the given event ID and data to the 
    /// attached listeners.
    protected: void DispatchEvent(EVENT_ID eventID, variant_t eventData=0L);
    
    /// Dispatches an event to the attached listeners.
    protected: void DispatchEvent(const Event* pEvent);
       
    /***************************************************************************
    Internal state
    ***************************************************************************/

    /// The first binding in the binding chain (linked list)
    private: IEventBinding* _firstBinding;
    
    /// The next event ID
    private: static EVENT_ID _nextEventID;
};

#endif

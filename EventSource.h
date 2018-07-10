#ifndef _EventSource_h_
#define _EventSource_h_

#include <inttypes.h>
#include <RTL_Stdlib.h>
#include <RTL_Variant.h>

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
interface and attach itself to an EventSource object via one of the Attach() method
overloads or through an event binding object. An EventSource notifies its listeners
when it generates an event by calling the IEventListener::OnEvent() method for each
listener. The listener implements whatever functionality it needs in the OnEvent()
method to respond to the event.

Multiple event listeners can be attached to the same EventSource. The event listeners
are chained together as a linked list of event bindings through the _nextLink
member of the IEventBinding interface. The _first member of EventSource points to
the head of this linked list. The Attach() and Detach() methods manage this linked
list. To allow them to do that, IEventBinding declares EventSource as a friend class
so it can access the private _nextEventListener member.

EventSources attach themselves to the global Scheduler object when they are
created. The Scheduler then calls the Poll() method of each EventSource
whenever its DispatchEvents() method is called. To ensure events are detected
and dispatched as expeditiously as possible, the Scheduler::DispatchEvents()
method should be called on every iteration in a sketch's loop() method.
*******************************************************************************/
class EventSource : public IPollable    // Size = 6 + Base(2) = 8
{
	DECLARE_CLASSNAME;
	
    friend class Scheduler;
    friend class IEventBinding;

    /***************************************************************************
    Constructors
    ***************************************************************************/

    /// The constructor is protected to enforce abstract base class semantics
    protected: EventSource() { _id = "?"; };

    /***************************************************************************
    Public Methods
    ***************************************************************************/
    /// Adds an event binging to this source
    public: void Attach(IEventBinding& binding);
    public: IEventBinding* Attach(IEventListener& listener, EventBinding* pBinding=NULL);
    public: IEventBinding* Attach(EVENT_LISTENER pfListener, StaticEventBinding* pBinding=NULL);

    /// Removes an event binging from this source
    public: void Detach(IEventBinding& binding);

    /// Polls this source for events.
    /// A derived class typically overrides this method to detect and dispatch
    /// events (e.g., if a button was pressed, a sensor was triggered, etc.).
    public: virtual void Poll() { };

    /// Determines if the source has any listeners attached
    public: bool HasListeners() { return _firstBinding != NULL; };

    /// Returns a new unique event ID with every call. Used to assign event IDs
    public: static EVENT_ID GenerateEventID() { return _nextEventID++; }

    /***************************************************************************
    Protected Methods
    ***************************************************************************/

    /// Creates and queues an event with the given event ID and data.
    protected: void QueueEvent(EVENT_ID eventID, variant_t eventData=0L);

    /// Queues an event
    protected: void QueueEvent(Event& pEvent);

    /// Creates and dispatches an event with the given event ID and data to the
    /// attached listeners.
    protected: void DispatchEvent(EVENT_ID eventID, variant_t eventData=0L);

    /// Dispatches an event to the attached listeners.
    protected: void DispatchEvent(Event& pEvent);

    /***************************************************************************
    Internal state
    ***************************************************************************/

    /// The first binding in the binding chain (linked list)
    private: IEventBinding* _firstBinding;          // size = 2

    /// The next event ID
    private: static EVENT_ID _nextEventID;          // size = 2
};

#endif

#define DEBUG 0

#include <Arduino.h>
#include <Debug.h>
#include "EventSource.h"
#include "EventBinding.h"
#include "EventDispatcher.h"


/*******************************************************************************
A base class for an object that sources events.

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
*******************************************************************************/

EVENT_ID EventSource::_nextEventID = EventSourceID::CustomEventBegin;


static DebugHelper Debug("EventSource");


//******************************************************************************
// Constructor
//******************************************************************************
EventSource::EventSource()
{
    EventDispatcher::Add(*this);
}


//******************************************************************************
// Add an event binding to this EventSource's list of bindings
//******************************************************************************
void EventSource::Attach(IEventBinding& binding)
{
    // Insert the new binding at the head of the linked list
    binding.Unbind();
    binding._source = this;
    binding._nextBinding = (_firstBinding != NULL) ? _firstBinding : NULL;
    _firstBinding = &binding;
    Debug.Log("AddListener => Added binding=%p", &binding);
}


//******************************************************************************
// Removes an event binding from this EventSource's list of bindings
//******************************************************************************
void EventSource::Detach(IEventBinding& binding)
{
    // Special case if the binding is the head of the linked-list
    if (_firstBinding == &binding)
    {
        _firstBinding = binding._nextBinding;
        Debug.Log("RemoveListener => Removed first binding %p", &binding);
    }
    else
    {
        // Otherwise, we have to walk the linked list to find the binding
        for (IEventBinding* pBinding = _firstBinding; pBinding != NULL; pBinding = pBinding->_nextBinding)
        {
            if (pBinding->_nextBinding == &binding)
            {
                pBinding->_nextBinding = binding._nextBinding;
                Debug.Log("RemoveListener => Removed binding %p", pBinding);
                break;
            }
        }
    }

    // Make sure the binding's 'next' pointer is empty to prevent future issues
    binding._nextBinding = NULL;
}


//******************************************************************************
// Add an event listener to this EventSource's list of listeners
//******************************************************************************
IEventBinding* EventSource::Attach(IEventListener& listener, EventBinding* pBinding)
{
    if (pBinding == NULL) pBinding = new EventBinding(listener);

    Attach(*pBinding);

    return pBinding;
}


IEventBinding* EventSource::Attach(EVENT_LISTENER pfListener, StaticEventBinding* pBinding)
{
    if (pBinding == NULL) pBinding = new StaticEventBinding(pfListener);

    Attach(*pBinding);

    return pBinding;
}


//******************************************************************************
// Dispatches an event to all listeners of this EventSource
//******************************************************************************
void EventSource::DispatchEvent(const Event* pEvent)
{
    Debug.Log("DispatchEvent(pEvent->EventID=%i)", pEvent->EventID);

    for (IEventBinding* pBinding = _firstBinding; pBinding != NULL; pBinding = pBinding->_nextBinding)
    {
        pBinding->DispatchEvent(pEvent);
    }
}


//******************************************************************************
// Creates and dispatches an event with the given event ID and data to the
// attached listeners.
//******************************************************************************
void EventSource::DispatchEvent(EVENT_ID eventID, variant_t eventData)
{
    Debug.Log("DispatchEvent(eventID=%i)", eventID);

    Event event(eventID, eventData);

    DispatchEvent(&event);
}

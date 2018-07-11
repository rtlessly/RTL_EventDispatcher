#ifndef _EventBinding_h
#define _EventBinding_h

#include "EventSource.h"
#include "IEventListener.h"


/*******************************************************************************
Defines a binding between an EventSource and an EventListener.

An event binding is an intermediary between an event source and an event listener
that connects the two together.

Event bindings attach themselves (aka 'bind') to an EventSource by calling the
EventSource::AddBinding() method. The EventSource will then send event notifications
to the binding's DispatchEvent() method where the binding forwards the event to
the associated event listener.

Multiple bindings can be bound to the same EventSource. The bindings are chained
together as a linked list through the _nextLink member. The AddBinding() and
RemoveBinding() methods of the EventSource class manage this linked list. To allow
them to do that, IEventBinding declares EventSource as a friend class so it can
access the private _nextLink member.

Event bindings are needed to handle the many-to-many relationship between EventSources
and event listeners. Without bindings, multiple listeners attached to the same
source could get their event notification chains cross-linked. Since a binding
can only be bound to one event listener and one EventSource at a time, they ensure
that there is always a unique event notification chain for each event source.

A class that implements this interface must provide an implementation for the
DispatchEvent() method to handle events dispatched to it.
*******************************************************************************/
class IEventBinding
{
    friend class EventSource;

    protected: IEventBinding() : _nextLink(NULL) { };

    public: void BindTo(EventSource& source) 
    { 
        source.Attach(*this); 
    };

    protected: void Unlink(IEventBinding*& prevLink) 
    {
        prevLink == _nextLink;
        _nextLink = NULL;
    }

    protected: virtual void DispatchEvent(Event& event) = 0;

    protected: IEventBinding* _nextLink;
};


/*******************************************************************************
Defines a binding between an IEventListener and an EventSource.
*******************************************************************************/
class EventBinding : public IEventBinding
{
    friend class EventSource;

    public: EventBinding() : _pListener(NULL) { };
    public: EventBinding(IEventListener& listener) : _pListener(&listener) { };

    public: void Bind(IEventListener& listener, EventSource& source) 
    { 
        _pListener = &listener;
        source.Attach(*this); 
    };
    
    protected: void DispatchEvent(Event& event)
    {
        if (_pListener != NULL) _pListener->OnEvent(&event);
    };
    
    private: IEventListener* _pListener;
};



/*******************************************************************************
Provides an EventBinding wrapper for a stand-alone event handling method
(i.e., a static method of a class or a method not in a class).
*******************************************************************************/
class StaticEventBinding : public IEventBinding
{
    friend class EventSource;

    public: StaticEventBinding() : _pfEventListener(NULL) { };
    public: StaticEventBinding(EVENT_LISTENER pfEventListener) : _pfEventListener(pfEventListener) { };

    public: void Bind(EVENT_LISTENER pfEventListener, EventSource& source) 
    { 
        _pfEventListener = pfEventListener; 
        source.Attach(*this); 
    };

    protected: virtual void DispatchEvent(Event& event)
    {
        if (_pfEventListener != NULL) (*_pfEventListener)(&event);
    };

    private: EVENT_LISTENER _pfEventListener;
};

#endif

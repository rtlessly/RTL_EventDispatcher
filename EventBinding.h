#ifndef _EventBinding_h
#define _EventBinding_h

#include "EventSource.h"
#include "IEventListener.h"


/*******************************************************************************
Defines a binding between to an Event Source.

An event binding is an intermediary between an EventSource and an event listener.
Event listeners attach themselves to an EventSource through an event binding.

Event bindings attach themselves (aka 'bind') to an EventSource by calling the
EventSource::AddBinding() method. The EventSource will then send event notifications
to the binding's DispatchEvent() method where the binding forwards the event to
the associated event listener.

Multiple bindings can be bound to the same EventSource. The bindings are chained
together as a linked list through the _nextBinding member. The AddBindng() and
RemoveBinding() methods of the EventSource class manage this linked list. To allow
them to do that, IEventBinding declares EventSource as a friend class so it can
access the private _nextBinding member.

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

    protected: IEventBinding() : _source(NULL) { };

    protected: ~IEventBinding() { Unbind(); };

    public:    void Unbind() { if (_source != NULL) { _source->Detach(*this); _source = NULL; } };

    protected: virtual void DispatchEvent(const Event* pEvent) = 0;

    //==========================================================================
    // Internal state
    //==========================================================================
    private: IEventBinding* _nextBinding;

    private: EventSource* _source;
};


/*******************************************************************************
Defines a binding between an IEventListener and an EventSource.
*******************************************************************************/
class EventBinding : public IEventBinding
{
    friend class EventSource;

    public: EventBinding() : _pListener(NULL) { };

    public: EventBinding(IEventListener& listener) : _pListener(&listener) { };

    public: Bind(IEventListener& listener, EventSource& source) { Unbind(); _pListener = &listener; source.Attach(listener, this); };
    
    protected: virtual void DispatchEvent(const Event* pEvent)
    {
        if (_pListener != NULL) _pListener->OnEvent(pEvent);
    };

    //==========================================================================
    // Internal state
    //==========================================================================
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

    protected: virtual void DispatchEvent(const Event* pEvent)
    {
        if (_pfEventListener != NULL) (*_pfEventListener)(pEvent);
    };

    //==========================================================================
    // Internal state
    //==========================================================================
    private: EVENT_LISTENER _pfEventListener;
};

#endif

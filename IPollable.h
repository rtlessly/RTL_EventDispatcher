#ifndef _IPollable_h
#define _IPollable_h

#include<Arduino.h>

typedef void (*POLL_FUNCTION)();


/*******************************************************************************
Defines an interface for an object than can be polled by the EventDispatcher. 
This is an abstract interface base class that must be implemented by a derived class.

The EventDispatcher class calls the Poll() method of all IPollable objects that
have been registered with it. The most common kind of IPollable object is an
EventSource, but other kinds of objects that don't source events - but still need 
to be polled, can implement this interface as well. Most IPollable objects 
automatically register with the EventDispatcher either in their constructor(s)
or in an initialization method.

A class that implements this interface must provide an implementation for the
Poll() method to handle events dispatched to it.
*******************************************************************************/
class IPollable
{
    public: virtual void Poll() = 0;

    /// The next object in the polling chain (linked list)
    private: IPollable* _nextObject;

    friend class EventDispatcher;
};


/*******************************************************************************
Provides an IPollable wrapper for a stand-alone poll function (i.e., a static method 
of a class or a function not in a class).
*******************************************************************************/
class PollableDelegate : public IPollable
{
    public: PollableDelegate(POLL_FUNCTION pfPollFunction) : _pfPollFunction(pfPollFunction)
    {}
    
    public: virtual void Poll()
    {
        if (_pfPollFunction != NULL) (*_pfPollFunction)();
    }
    
    private: POLL_FUNCTION _pfPollFunction;
};


#endif

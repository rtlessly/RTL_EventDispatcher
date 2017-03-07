#define DEBUG 0

#include <Arduino.h>
#include <Debug.h>
#include "EventDispatcher.h"

/*******************************************************************************
Global event dispatcher.

EventDispatcher is a static singleton that dispatches all events in a program.
Objects register with the EventDispatcher via the Add() methods and then the
EventDispatcher invokes the Poll() method of each Object.

The EventDispatcher::DispatchEvents() method should be invoked on every iteration
of the sketch's loop() method. The DispatchEvent() method only polls one object
per iteration. That is, it will poll the first object on the first iteration, the
second object on the second iteration, and so on. When it reaches the end of the
polling list it will start over. This technique is a compromise that allows each
iteration of the loop() method to finish as quickly as possible, while still
ensuring that all objects are polled in a timely fashion. 
*******************************************************************************/

static DebugHelper Debug("EventDispatcher");

IPollable* EventDispatcher::_first;
IPollable* EventDispatcher::_current;

//******************************************************************************
// Add a poll-able object the polling list
//******************************************************************************
void EventDispatcher::Add(IPollable& obj)
{
    // If the linked list anchor is empty then we just add the object there
    if (_first == NULL)
    {
        _first = &obj;
        _current = _first;
        Debug.Log("Add => Added first object %p", &obj);
    }
    else
    {
        // Otherwise, we have to walk the linked list to find the end and add the
        // object there
        for (IPollable* pObj = _first; pObj != NULL; pObj = pObj->_nextObject)
        {
            if (pObj->_nextObject == NULL)
            {
                pObj->_nextObject = &obj;
                Debug.Log("Add => Added object %p", pObj);
                break;
            }
        }
    }

    // Since the object is always added at the end of the list, make sure its
    // 'next' pointer is empty
    obj._nextObject = NULL;
}


IPollable* EventDispatcher::Add(POLL_FUNCTION pfPollFunction)
{
    IPollable* pollObj = new PollableDelegate(pfPollFunction);

    Add(*pollObj);

    return pollObj;
}


//******************************************************************************
// Removes an object the list from the polling list
//******************************************************************************
void EventDispatcher::Remove(IPollable& obj)
{
    // Special case if obj is the anchor object of the linked-list
    if (_first == &obj)
    {
        _first = obj._nextObject;
        Debug.Log("Remove => Removed first object %p", &obj);
    }
    else
    {
        // Otherwise, we have to walk the linked list to find the object
        for (IPollable* pObj = _first; pObj != NULL; pObj = pObj->_nextObject)
        {
            if (pObj->_nextObject == &obj)
            {
                pObj->_nextObject = obj._nextObject;
                Debug.Log("Remove => Removed object %p", pObj);
                break;
            }
        }
    }

    // Adjust the _current pointer if we are removing the current object
    if (_current == &obj) _current = (obj._nextObject != NULL) ? obj._nextObject : _first;
    
    // Make sure the object's 'next' pointer is empty to prevent future issues
    obj._nextObject = NULL;
}


//******************************************************************************
// Polls all sources to dispatch events
//******************************************************************************
void EventDispatcher::DispatchEvents()
{
	Debug.Log("DispatchEvents => _current=%p", _current);
	
    if (_current != NULL)
    {
        _current->Poll();
        _current = (_current->_nextObject != NULL) ? _current->_nextObject : _first;
    }
    
    // for (IPollable* pObj = _first; pObj != NULL; pObj = pObj->_nextObject)
    // {
    //     pObj->Poll();
    // }
}

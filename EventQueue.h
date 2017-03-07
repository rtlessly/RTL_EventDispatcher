#ifndef _EventQueue_h
#define _EventQueue_h

#include <inttypes.h>
#include "Event.h"
#include "EventSource.h"


//******************************************************************************
// EventQueue class
//******************************************************************************
class EventQueue : public EventSource
{
    // The maximum number of events the queue can hold 
    // Increasing this number will consume sizeof(int)+sizeof(long) bytes of RAM for each unit.
    private: static const uint8_t QueueSize = 8;

    /***************************************************************************
    Constructors
    ***************************************************************************/
    
    public: EventQueue();
 
 
    /***************************************************************************
    IPollable interface
    ***************************************************************************/

    public: void Poll();

    
    /***************************************************************************
    Properties
    ***************************************************************************/
    
    // Returns true if no events are in the queue
    public: bool IsEmpty() { return _count == 0; };

    // Returns true if no more events can be inserted into the queue
    public: bool IsFull()  { return _count == QueueSize; };

    // Actual number of events in queue
    public: uint8_t Count() { return _count; };

    /***************************************************************************
    Public Methods
    ***************************************************************************/
    
    // Inserts an event into the queue.
    // Returns true if the event was successfully queued. Otherwise, false is
    // returned (e.g., if the queue is full).
    //
    // NOTE: This function is interrupt-safe so it can be called from interrupt handlers.  
    // This is the ONLY EventQueue function that can be called from an interrupt handler.
    public: bool Queue(Event& event);

    // Extracts an event from the queue. The de-queued event is returned in the 
    // event argument.
    // Returns true if an event was available and removed from the queue. Otherwise,
    // false is returned (e.g., the queue is empty).    
    public: bool Dequeue(Event& event);

    /***************************************************************************
    Internal state
    ***************************************************************************/
    
    // The event queue
    private: Event _queue[QueueSize];

    // Index of event queue head
    private: uint8_t _queueHead;

    // Index of event queue tail
    private: uint8_t _queueTail;

    // Actual number of events in queue
    private: uint8_t _count;
};

#endif

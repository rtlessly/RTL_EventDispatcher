#ifndef _Event_h_
#define _Event_h_

#include <inttypes.h>
#include <RTL_Variant.h>


#define WithEvent(pEvent) EVENT_ID _eventID_ = pEvent->EventID; if (false) {} // Dummy if statement that is always false to make WithEvent/When more syntactically symmetrical
#define When(eventID) else if (_eventID_ == eventID)


typedef uint16_t EVENT_ID;


class EventSource;


struct Event            // size = 8?
{
    /**************************************************************************
    Constructors
    **************************************************************************/
    Event() : EventID(0) { Data.Long = 0; };

    Event(EVENT_ID eventID, int32_t data=0) : EventID(eventID) { Data.Long = data; };

    Event(EVENT_ID eventID, int16_t data)   : EventID(eventID) { Data.Int = data; };

    Event(EVENT_ID eventID, uint32_t data)  : EventID(eventID) { Data.UnsignedLong = data; };

    Event(EVENT_ID eventID, uint16_t data)  : EventID(eventID) { Data.UnsignedInt = data; };

    Event(EVENT_ID eventID, bool data)      : EventID(eventID) { Data.Bool = data; };

    Event(EVENT_ID eventID, float data)     : EventID(eventID) { Data.Float = data; };

    Event(EVENT_ID eventID, void* pData)    : EventID(eventID) { Data.Pointer = pData; };

    Event(EVENT_ID eventID, variant_t data) : EventID(eventID) { Data = data; };

    Event(EVENT_ID eventID, variant_union_t data) : EventID(eventID) { Data = data; };

    // Copy constructor
    Event(const Event& rhs) : EventID(rhs.EventID), Source(rhs.Source), Data(rhs.Data) {  };

    /**************************************************************************
    Operators
    **************************************************************************/
    //Event& operator=(const Event& rhs) { EventID = rhs.EventID; Data = rhs.Data; Source = rhs.Source; return *this; }

    /**************************************************************************
    Data
    **************************************************************************/
    EVENT_ID EventID;           // size = 2

    variant_union_t Data;       // size = 4?

    EventSource* Source;        // size = 2
};

#endif

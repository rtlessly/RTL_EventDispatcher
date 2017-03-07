#ifndef _Event_h
#define _Event_h

#include <inttypes.h>
#include <Variant.h>

#define WithEvent(pEvent) EVENT_ID _eventID_ = pEvent->EventID; if (false) {} // Dummy if statement that is always false to make WithEvent/When more syntatically symmetrical
#define When(eventID) else if (_eventID_ == eventID)

typedef unsigned int EVENT_ID;


class Event
{
    public: Event() : EventID(0) { Data.Long = 0; };

    public: Event(EVENT_ID eventID, int32_t data=0) : EventID(eventID) { Data.Long = data; };

    public: Event(EVENT_ID eventID, int16_t data)   : EventID(eventID) { Data.Int = data; };

    public: Event(EVENT_ID eventID, uint32_t data)  : EventID(eventID) { Data.UnsignedLong = data; };

    public: Event(EVENT_ID eventID, uint16_t data)  : EventID(eventID) { Data.UnsignedInt = data; };

    public: Event(EVENT_ID eventID, bool data)      : EventID(eventID) { Data.Bool = data; };

    public: Event(EVENT_ID eventID, float data)     : EventID(eventID) { Data.Float = data; };

    public: Event(EVENT_ID eventID, void* pData)    : EventID(eventID) { Data.Pointer = pData; };

    public: Event(EVENT_ID eventID, variant_t data) : EventID(eventID) { Data = (variant_union_t)data; };

    public: Event(EVENT_ID eventID, variant_union_t data) : EventID(eventID) { Data = data; };

    public: EVENT_ID EventID;

    public: variant_union_t Data;
};

#endif

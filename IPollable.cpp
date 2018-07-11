#include <Arduino.h>
#include "EventDispatcher.h"


IPollable::IPollable(bool autoAdd) 
{ 
    _id = "?";

    if (autoAdd) EventDispatcher::Add(*this); 
}


void IPollable::Poll()
{
}

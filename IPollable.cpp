#include <Arduino.h>
#include "Scheduler.h"


IPollable::IPollable(bool autoAdd) 
{ 
    _id = "?";

    if (autoAdd) Scheduler::Add(*this); 
}


void IPollable::Poll()
{
}

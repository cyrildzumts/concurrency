#include "activeobject.h"
size_t ActiveObject::getSubmitted() const
{
    return submitted;
}

size_t ActiveObject::getFinished_tasks() const
{
    return finished_tasks;
}


bool ActiveObject::isActive() const
{
    return active;
}

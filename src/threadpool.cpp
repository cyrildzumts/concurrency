#include "threadpool.h"



size_t ThreadPool::getSubmitted() const
{
    return submitted;
}

size_t ThreadPool::getFinished_tasks() const
{
    return finished_tasks;
}

size_t ThreadPool::getActive_task() const
{
    return active_task;
}

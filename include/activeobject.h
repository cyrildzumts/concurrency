#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H

#include "function_wrapper.h"
#include "safequeue.h"
#include <mutex>
#include <memory>
#include <condition_variable>
#include <log.h>
#include <future>
#include <thread>


class ActiveObject{

public:

    ActiveObject(): done(false), active{false},  interrupted{false}{
        submitted = 0;
        finished_tasks = 0;
        worker = std::make_unique<std::thread>(&ActiveObject::run, this);
        object_id = std::hash<std::thread::id>()(worker->get_id());

    }

    ~ActiveObject(){
        interrupt();
        if(worker->joinable()){
            worker->join();
        }
    }


    template<typename Callable, typename... Args,typename = std::enable_if_t<std::is_move_constructible_v<Callable>>>
        std::future<std::invoke_result_t<std::decay_t<Callable>, std::decay_t<Args>...>> submit(Callable &&op, Args&&... args){
            using result_type =std::invoke_result_t<std::decay_t<Callable>, std::decay_t<Args>...>;
            if(interrupted){
                std::future<result_type> err;
                return err;
            }
            std::packaged_task<result_type()> task(std::bind(std::forward<Callable>(op), std::forward<Args>(args)...));
            std::future<result_type> result(task.get_future());
            work_queue.push(std::move(task));
            ++submitted;
            return result;
   }


    void run(){
        LOG("Active Object started from thread : ", object_id);
        while(!done){
            FunctionWrapper task;
            work_queue.wait_and_pop(task);
            active = true;
            task();
            active = false;
            ++finished_tasks;
            std::this_thread::yield();
        }  
    }

    void interrupt(){
        if(!interrupted){
            work_queue.push([&]{
                 done = true;
             });
            interrupted = true;
        }

    }
    bool getInterrupted() const{
        return interrupted;
    }

    size_t getSubmitted() const{
        return submitted;
    }

    size_t getFinished_tasks() const{
        return finished_tasks;
    }

    bool isActive() const{
        return active;
    }

    uint64_t getObject_id() const{
        return object_id;
    }

private:
    bool done;
    bool active;
    bool interrupted;
    size_t submitted;
    size_t finished_tasks;
    uint64_t object_id;
    ThreadSafeQueue<FunctionWrapper> work_queue;
    std::unique_ptr<std::thread> worker;
};


#endif // ACTIVEOBJECT_H

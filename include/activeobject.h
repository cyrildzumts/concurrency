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

    ActiveObject(): done(false){
        worker = std::make_unique<std::thread>(&ActiveObject::run, this);

    }

    ~ActiveObject(){
        interrupt();
        if(worker->joinable()){
            worker->join();
        }
    }
    template<typename Func>
    std::future<typename std::result_of<Func()>::type> submit(Func f){
        using result_type =typename std::result_of<Func()>::type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> result(task.get_future());
        work_queue.push(std::move(task));
        return result;
    }

    template<typename Callable, typename... Args,typename = std::enable_if_t<std::is_move_constructible_v<Callable>>>
    std::future<std::invoke_result_t<Callable, Args...>> place(Callable &&op, Args&&... args){
        using result_type =std::invoke_result_t<Callable, Args...>;
        std::packaged_task<result_type()> task(std::bind(std::forward<Callable>(op), std::ref(args)...));
        std::future<result_type> result(task.get_future());
        work_queue.push(std::move(task));
        return result;
    }

/*
    template<typename Callable, typename... Args,typename = std::enable_if_t<std::is_move_constructible_v<Callable>>>
    std::future<std::invoke_result_t<Callable, Args...>> place2(Callable &&op, Args&&... args){

        using result_type =std::invoke_result_t<Callable, Args...>;
        std::packaged_task<result_type()> task(std::bind(std::forward<Callable>(op), std::ref(args)...));
        std::future<result_type> result(task.get_future());
        work_queue.push(std::move(task));
//        std::lock_guard<std::mutex> lock(mux);
//        f = std::move(task);
//        there_is_work = true;
//        work_cond.notify_one();
        return result;
    }

*/
    template<typename Callable, typename... Args>
    std::future<std::invoke_result_t<Callable, Args...>> async_call(Callable &&op, Args&&... args){
        using result_type =std::invoke_result_t<Callable, Args...>;
        auto result = std::async(std::launch::async, std::forward<Callable>(op), std::ref(args)...);
        return result;
    }

    void run(){
        while(!done){
            FunctionWrapper task;
            work_queue.wait_and_pop(task);
            task();
            std::this_thread::yield();
        }  
    }
/*
    void run2(){
        LOG("Worker 2 run in thread", " thread id : ", std::this_thread::get_id());
        while(!done){

            std::unique_lock<std::mutex> locker(mux);
            work_cond.wait(locker,[&]{return there_is_work;});
            is_working = true;
            f();
            is_working = false;
            there_is_work = false;
        }
    }
*/
    void interrupt(){
        work_queue.push([&]{
             done = true;
         });
    }
private:
    bool done;
    //bool is_working;
    /*
    bool there_is_work;
    std::mutex mux;
    FunctionWrapper f;
    std::condition_variable work_cond;
    */
    ThreadSafeQueue<FunctionWrapper> work_queue;
    std::unique_ptr<std::thread> worker;
    std::unique_ptr<std::thread> worker2;
};


#endif // ACTIVEOBJECT_H

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "function_wrapper.h"
#include "safequeue.h"
#include <log.h>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <future>

class ThreadJoins{
private:
    std::vector<std::thread> &threads;

public:
    explicit ThreadJoins(std::vector<std::thread> &t):threads{t}{

    }

    ~ThreadJoins(){
        for(size_t i = 0; i< threads.size(); ++i){
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
        LOG("THREADPOOL has quitted");
    }
};


class ThreadPool{
private:
    std::atomic_bool done;
    bool interrupted;
    size_t workers_count;
    ThreadSafeQueue<FunctionWrapper> work_queue;
    std::vector<std::thread> threads;
    void run(){
        while (!done) {
            FunctionWrapper task;
            work_queue.wait_and_pop(task);
            task();
            std::this_thread::yield();
        }
    }

public:
    ThreadPool(): done{false}, interrupted{false}{
        workers_count = std::thread::hardware_concurrency();
        //workers_count = 4;
        try {
            for(unsigned i = 0; i < workers_count; ++i){
                threads.push_back(std::thread(&ThreadPool::run, this));
            }
        } catch (...) {
            done = true;
            throw;
        }
    }
    ~ThreadPool(){
        //Send a signal to every worker
        interrupt();

        // Wait for every worker to finish before quitting.
        std::for_each(threads.begin(), threads.end(), [&](std::thread &t){
            if(t.joinable()){
                t.join();
            }
        });
        LOG("THREADPOOL is quitting ");
    }
    /**
     * @brief submit place the a new task in the pool to be process when
     * a worker is free.
     * it
     */
    template<typename Callable, typename... Args,typename = std::enable_if_t<std::is_move_constructible_v<Callable>>>
    std::future<std::invoke_result_t<Callable, Args...>> submit(Callable &&op, Args&&... args){

        using result_type =std::invoke_result_t<Callable, Args...>;
        if(interrupted){
            std::future<result_type> err;
            return err;
        }
        std::packaged_task<result_type()> task(std::bind(std::forward<Callable>(op), std::forward<Args>(args)...));
        std::future<result_type> result(task.get_future());
        work_queue.push(std::move(task));
        return result;
    }

    void interrupt(){
        if(!interrupted){
            std::for_each(threads.begin(), threads.end(), [&](std::thread &t){
                if(t.joinable()){
                    submit([&]{
                        done = true;
                    });
                }
            });
            interrupted = true;
        }
    }


    bool getInterrupted() const{
        return interrupted;
    }
};


#endif // THREADPOOL_H

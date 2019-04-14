#include <threadpool.h>
#include <activeobject.h>
#include <iostream>

using namespace std::chrono_literals;

int job(int a,int b);

void action(int a, int b){
    LOG("Action run in thread", " thread id : ", std::this_thread::get_id());
    return;
}

int main(int argc, char **argv){

    LOG("Main thread", "thread id : ", std::this_thread::get_id());
    ActiveObject active;
    ThreadPool pool;
    int a = 10;
    int b = 20;
    int ret = 0;
    //std::vector<std::future<int>> futures;
    for(int i = 0; i < 30; i++){
        auto res = active.submit(job, a, b);
        auto pool_res = pool.submit(action,a, b);
        auto status = res.wait_for(80us);
        if(status == std::future_status::ready){
            try {
                ret = res.get();
            } catch (...) {
                ret = -1;
            }
        }else {
            ret = -10;
        }
    }
    pool.interrupt();
    active.interrupt();
    auto fut = active.submit(action, a, b);
    try {
        fut.get();
        LOG("FUTURE END : ");
    } catch (std::future_error &e) {
        LOG("FUTURE ERROR REASON  : ",e.what() );
    }
    std::cout << "Concurrency World ! " << std::endl;
    return 0;

}


int job(int a, int b){
    LOG("Job run in thread", " thread id : ", std::this_thread::get_id());
    std::this_thread::sleep_for(30us);
    //std::cout << "Job : thread id : " << std::this_thread::get_id() << std::endl;
    return  a + b;
}

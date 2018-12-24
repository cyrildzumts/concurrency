#include "safequeue.h"
#include <activeobject.h>
#include <iostream>

using namespace std::chrono_literals;

int job(int a,int b);

void action(int a, int b){
    LOG(" action called : ", __PRETTY_FUNCTION__);
    return;
}

int main(int argc, char **argv){

    LOG("Main thread", "thread id : ", std::this_thread::get_id());
    AbstractActive active;
    int a = 10;
    int b = 20;
    int ret = 0;

    for(int i = 0; i < 30; i++){
        auto start = std::chrono::steady_clock::now();
        auto res2 = active.place(job, a, b);
        auto status = res2.wait_for(50us);
        if(status == std::future_status::ready){
            try {
                ret = res2.get();
            } catch (...) {
                ret = -1;
            }
        }else {
            ret = -10;
        }
        auto end1 = std::chrono::steady_clock::now();

        LOG("RESULT 00 RET : ", ret, " : submit time end : ",
            std::chrono::duration_cast<std::chrono::microseconds>((end1 - start)).count(), "us");
    }

    std::cout << "Concurrency World ! " << std::endl;
    return 0;

}


int job(int a, int b){
    //LOG("Job run in thread", " thread id : ", std::this_thread::get_id());
    std::this_thread::sleep_for(30us);
    //std::cout << "Job : thread id : " << std::this_thread::get_id() << std::endl;
    return  a + b;
}

#include <iostream>
#include "type_name.hpp"
#include <thread>
#include <condition_variable>
#include "unistd.h"
#include <vector>

#include <future>

using namespace std;
/* Interrupt message */
std::condition_variable cv;
std::mutex m;

/* use conditional variable to notify*/
bool flag(false);
void teller(){
    std::lock_guard<std::mutex> g(m);
    flag =true;
    cv.notify_one();
}
void reactor(){
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk,[] { cout << "flag :" << flag << endl; return flag;});
}

/* Use std::future and std::promise */
std::promise<void> p;

void reactor1();

void teller1(){
    cout << "in teller1 \n";
    /* p.get_future.wait() suspend t until future is set. */
    std::thread t([]{p.get_future().wait();reactor1();});

    p.set_value();

    t.join();
}

void reactor1(){
    cout << "in reactor \n";
    //throw "abc";
    //p.get_future.wait();
}

/* ThreadRAII class */
//class ThreadRAII

/* Need process exception before promise.set_value */
void detect1(){
    auto sf = p.get_future().share();
    std::vector<std::thread> vt;
    for(int i = 0; i < 3; ++i){
        vt.emplace_back([sf]{sf.wait(); reactor1();});
    }
    // try{

    // }
    p.set_value();
    for(auto& t: vt){
        t.join();
    }
}

int main(){ 
    std::thread t1(teller);
    usleep(10000);    
    //std::thread t2(reactor);
    reactor();
    t1.join();
    teller1();
    return 0;
}
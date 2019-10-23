#include <iostream>
#include <future>
#include "type_name.hpp"
#include <thread>
#include <chrono>

using namespace std;
/* When you call std::async to execute a function, you are generally intending to run
   function asychronously.
   You're really requesting that the function be run in accord with a std::async launch policy.
 */
void printer(){
    cout << "printer" << endl;
}
using namespace std::literals;
void f(){
    std::this_thread::sleep_for(1s);
}

/* function template return async policy function  c++11*/
template<typename F, typename... Ts>
inline
std::future<typename std::result_of_t<F(Ts...)>>
reallyAsync(F&& f, Ts&&... params){
    return std::async(std::launch::async, 
                      std::forward<F>(f),
                      std::forward<Ts>(params)...);                    
}
/* function tempate return async policy functioni, C++14 */
template<typename F, typename... Ts>
inline
auto 
reallyAsync1(F&& f, Ts&&... params){
    return std::async(std::launch::async,
                      std::forward<F>(f),
                      std::forward<Ts>(params)...);
}
void test(int t)
{
    cout << t << endl;

}
int main(){
    /* default policy: 
       1. It's not possible to predict whether f will run concurrently within some time, because f might be
       scheduled to run deferred.
       2. It's not possible to predict whether f runs on a thread different from the thread 
       invoking get or wait on fut.
       3. It may not be possible to predict whether f runs at all, because it may not possible to gurantee
       that get or wait will be called on fut along with path through the program.
     */
    auto fut1 = std::async(printer);
    /* the above equals to  */
    auto fut2 = std::async(std::launch::async| std::launch::deferred, printer);
    /* deferred policy will wait for get or wait method */
    auto fut3 = std::async(std::launch::deferred, printer);
    /* test on async method, whether the printer will be invoked or not 
       result : the async policy means that the f must be run asynchronously, i.e on a different 
       thread.
       But:
    */
    auto fut4 = std::async(std::launch::async, printer);

    /* Test on forever-loop */
    //auto fut5 = std::async(std::launch::deferred, f);

    //while(fut5.wait_for(100ms) != std::future_status::ready)
    //{}
    /* Fix bug above */
    /* Use another policy */
    auto fut5 = std::async(std::launch::async, f);
    if(fut5.wait_for(0s) == std::future_status::deferred){
        cout << "The task correponding to deferred policy";
        fut5.get();
    }else{
        while(fut5.wait_for(100ms) != std::future_status::ready){
            cout << "Wait here: " << endl;
        }
    }
    /* reallyAsync return a asynchronously invoke function */
    auto fut = reallyAsync(test, 2);
    auto futx = reallyAsync1(test, 3);
    return 0;
}   
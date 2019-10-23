#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include "type_name.hpp"

using namespace std;

/* Unjoinable std::thread 
   1. Default-constructed std::threads.(no function to execute)
   2. std::threads objects that have been moved from. The result of a move is that
   the underlying thread of execution a std::thread used to correspond to (if any) now corresponds
   to a different std::thread.
   3. std::threads that have been joined. After a join, the std::thread object no longer corresponds to 
   the underlying thread of execution that has finished running.
   4. std::threads that have been detached. A detach servers the connection between a std::thread object
   and underlying thread of execution it corresponds to.
*/
constexpr auto tenMillion = 10000000;
bool doWork(std::function<bool(int)> filter, int maxVal = tenMillion){
    std::vector<int> goodVals;
    std::thread t([&filter, maxVal, &goodVals]{
        for (auto i = 0;  i < maxVal; i++){
            if(filter(i)) goodVals.push_back(i);
        }
    });
    auto nh = t.native_handle(); // use t's native handle to set t's priority
    if(1 > 0){
        cout << "Here \n";
        t.join();
        cout << "After join, the size of goodVals : " << goodVals.size() << endl;
        for(auto _g: goodVals){
            _g++;
        }

        return true;
    }
    /* If doWork return false or throws a exception, the std::thread t will be joinable when its destructor is called at 
       the end of doWork. That would cause program execution to be terminated.
       [1]    85599 abort      ./item37_make_threads_unjoinable_on_all_paths
     */
    return false;
}

bool global_filter(int x){
    return true;
}

/* The following class allows callers to specify whether join or detach should be called
   when a ThreadRAII obect (an RAII object for a std::thread) is detroyed.
 */
class ThreadRAII{
public:
    enum class DotrAction{join, detach};
    ThreadRAII(std::thread&&t, DotrAction a)
    :action(a), t(std::move(t)){
    }
    /* When ThreadRAII object's destructor is invoked, no other thread should be
       making member function calls on that object. If there are simultaneous calls,
       there is certanly a race, but it's not inside the  destructor.
     */
    ~ThreadRAII(){
        if(t.joinable()){
            if(action == DotrAction::join){
                t.join();
            }else{
                t.detach();
            }
        }
    }
    std::thread& get(){
        return t;
    }
    ThreadRAII(ThreadRAII&&) = default;
    ThreadRAII& operator=(ThreadRAII&&) = default;         

private:
    DotrAction action;
    std::thread t;
};
/* Refine doWork */
bool doWork1(std::function<bool(int)> filter, int maxVal = tenMillion/1000){
    std::vector<int> goodVals;
    ThreadRAII t(
        std::thread([&filter, maxVal, &goodVals]{
            for(auto i = 0; i < maxVal; ++i)
            {if(filter(i)) goodVals.push_back(i);}
        }),
        ThreadRAII::DotrAction::join
        );
        auto nh = t.get().native_handle();
        if(0>0){
            t.get().join();
            return true;
        }else{
            return false;
    }
}
int main(){
    doWork(std::bind(global_filter, 1));
    cout << "Hello ,world \n";
    doWork1(std::bind(global_filter, 1));

}
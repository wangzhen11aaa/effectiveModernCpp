#include <iostream>
#include <thread>
#include <future>

using namespace std;

int calcVal(int x){
    cout << "Invoke calcVal \n";
    return x;
}

int main(){
    //std::thread t(std::move(pt));
    std::packaged_task<int(int)> pt(calcVal);
    /* Return a std::future associated with the promised result */
    auto fut = pt.get_future();
    // execute somefunction.
    pt(1);
    cout << "fut :" << fut.get() <<endl;
    pt(22);
    /* second invoke 
    terminating with uncaught exception of type std::__1::future_error: The state of the promise has already been set.
    */
    cout << "fut :" << fut.get() << endl;
    return 0;
}
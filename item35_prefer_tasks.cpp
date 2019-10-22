#include <iostream>
#include "type_name.hpp"
#include <thread>
#include <future>

using namespace std;

void printer(){
    cout << "Hello from printer \n";
}

int main(){
    /* Asynchronously process from thread */
    // std::thread t(printer);
    // t.join();
    /* Asychronously process with task */
    auto fut = std::async(printer);
    cout << "fut's type : " << type_name<decltype(fut)>() << endl;
    fut.get();
    return 0;
}
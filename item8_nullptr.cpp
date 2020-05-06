#include <iostream>
#include <memory>
#include <mutex>

using namespace std;

void f(int) {
    cout << "Invoke f(int) function " << endl;
}
void f(bool){
    cout << "Invoke f(bool) function " << endl;
}
void f(void *){
    cout << "Invoke f(void *) function " << endl;
}
class Widget{};
// Test the implicit conversion (NULL, 0 etc).
int f1(std::shared_ptr<Widget> spw) {cout << "Invoke shared_ptr<Widget> " << endl; return 0;}
int f2(std::unique_ptr<Widget> upw) {cout << "Invoke unique_ptr<Widget> " << endl; return 0;}
int f3(Widget * pw) {cout << "Invoke f3(Widget *pw) " << endl; return 0;}


using MuxGuard = std::lock_guard<std::mutex>;
// Use template to test
template<typename FuncType, typename MuxType, typename PtrType>
auto lockAndCall(FuncType func, MuxType& mutex, PtrType ptr) ->decltype(func(ptr))
{
    MuxGuard g(mutex);
    return func(ptr);
}

int main(){
    std::mutex f1m, f2m, f3m;         // mutexes for f1, f2, and f3
    // tempalte dedunction.
    // candidate template ignored: substitution failure [with FuncType = int (*)(std::__1::shared_ptr<Widget>), MuxType =
    // std::__1::mutex, PtrType = int]: no viable conversion from 'int' to 'std::__1::shared_ptr<Widget>'
    // auto result1_t = lockAndCall(f1, f1m, 0);
    // auto result2_t = lockAndCall(f2, f2m, NULL);
    auto result3_t = lockAndCall(f3, f3m, nullptr);

    auto result = f1(0);
    auto result1 = f2(NULL);
    // item8_nullptr.cpp:15:4: error: call to 'f' is ambiguous
   //f(NULL);
   f(nullptr);
   return 0; 
}
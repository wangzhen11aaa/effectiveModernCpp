#include <iostream>
#include <vector>
//#include <ctype.h>

#include "type_name.hpp"

using namespace std;

/* variadic templates */
/* Perfect forwarding fails if calling f with a particular argument does one thing,
   but fwd with the same argument does something different.
 */
template<typename... Ts>
void f(Ts&&... params){
    cout << "Invoke f \n";
}

template<typename... Ts>
void fwd(Ts&&... params){
    f(std::forward<Ts>(params)...);
}


void f(const std::vector<int>& v){
    cout << "v";
}

void f(std::size_t val){
    cout << "val : " << val;
}

class Widget{
public:
    static const std::size_t MinVals = 29;
};

/* Give it a name */
const std::size_t Widget::MinVals;

/* function overloading */
int process(int value){
    return 0;
}
int process(int value, int priority){
    return 1;
}

void f(int pf(int)){
    cout << "f pf \n";
}
// template<typename... Ts>
// void fwd(Ts&&... args){
//     f(std::forward<Ts>(args)...);
// }

/* More complex function overload */
template<typename T>
T WorkOnValue(T param){
    return param;
}

/* On bitfield */
struct IPv4Header{
    std::uint32_t version:4,
                    IHL:4,
                    DSCP:6,
                    ECN:2,
                    totalLength:16;

};

void f1(std::size_t sz){
    cout << sz << endl;
}

int main(){
    f({1,2,3});
    /* fwd deduce the bracket-list failed */
    //fwd({1,2,3});
    auto il = {1,2,3};
    cout << "type of il : " << type_name<decltype(il)>() << endl;
    fwd(il);

    f(Widget::MinVals);
    /* Invoke on MinVals is not good , for 
    fwd's parameter is universal reference*/
    /* Define the variable */
    fwd(Widget::MinVals);
    /* Invoke on 20 is good */
    fwd(20);
    /* invoke on function pointer */
    f(process);
    using ProcessFuncType = int (*)(int);
    ProcessFuncType processValPtr = process;
    fwd(processValPtr);
    /* static_cast function template to function ptr */
    fwd(static_cast<ProcessFuncType>(WorkOnValue));

    /* on bit fields */
    IPv4Header h;
    f1(h.totalLength);
    /* Can not bind a non-const reference to bit field, for no pointer can be pointed to arbitrary bits */
    //f(h.totalLength);

    auto length = static_cast<std::uint16_t>(h.totalLength);
    f(length);
    return 0;
}
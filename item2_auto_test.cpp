#include <iostream>

#ifndef _MSC_VER
    #include <cxxabi.h>
#endif
#include <type_traits>
#include <memory>
#include <string>
#include <cstdlib>
#include <array>

#include <typeinfo>
using namespace std;


template <class T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
        abi::__cxa_demangle(typeid(TR).name(), nullptr,
                            nullptr, nullptr),
#else
        nullptr,
#endif
        std::free);
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

// template print the value in the initialization list.
// template does not recognize the direct initialization list.
template<typename T>
void f(T param){
    cout << "Type in f: " << type_name<T>() << endl;
    cout << "Param's type : " << type_name<decltype(param)>() << endl;
    cout << "Initialization List's value :" << *(param.begin()) << endl;
}

//template specify the initializer_list  
template<typename T>
void f1(std::initializer_list<T> param){
    cout << "Initialization List's value :" << *(param.begin()) << endl;
    cout << "Type in f1: " << type_name<T>() << endl;
    cout << "Param's type : " << type_name<decltype(param)>() << endl;
    cout << "Initialization List's value :" << *(param.begin()) << endl;    
}

// auto in a function return type or a lambda parameter implies template type
// deduction, not auto type deduction.
//error: cannot deduce return type from initializer list

// This is c++14, lambda function which will make the auto act as 
// some template deduction.
//auto createInitList(){
//    return {1,2,3};
//}

// returning address of local temporary object
std::initializer_list<int> createInitList1(){
    return {1,2,3};
}

int main(){
    // auto deduce x as initialize list.
    // Which the x is deduced as the initialize_list type.
    auto x = {1,2,3};
    // While the template function f can not deduce this stuff.
    f(x);
    // test the direct {}
    //f({1,2,3});
    f1({4,5,6});
    return 0;
}

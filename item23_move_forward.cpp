#include <iostream>
#include "type_name.hpp"
//#include <typeinfo>

/* user defined move typename */
template<typename T>
typename std::remove_reference<T>::type &&
move1 (T&& param){
    std::cout << "T's type : " << type_name<T>() << std::endl;
    using ReturnType = typename std::remove_reference<T>::type&&;
    return static_cast<ReturnType>(param);
}

/* user defined move auto */
template<typename T>
decltype(auto) move(T&& param){
    std::cout << "T's type : " << type_name<T>() << std::endl;
    using ReturnType = std::remove_reference_t<T>&&;
    return static_cast<ReturnType>(param);
}

// /* Test on move action of const rvalue */
// class String1{
// public:
//     String1
// private:

// };

class Widget{};
void process(const Widget& lvalArg){
    std::cout << "process via lvalue reference \n" ;
}
void process(Widget&& rvalArg){
    std::cout << "process via rvalue reference \n";
}

template<typename T>
void logAndProcess(T&& param){
    std::cout << "param's type :" << type_name<decltype(param)>() << std::endl;
    std::cout << "T's type : " << type_name<T>() << std::endl;
    auto now = std::chrono::system_clock::now();
    std::cout << "now's type : " << type_name<decltype(now)>() << std::endl;
    //std::cout <<  "Now : " << now;
    process(std::forward<T>(param));
    process(param);
    process(static_cast<std::remove_reference_t<decltype(param)>>(param));
}

int main(){
    /* Test on rvalue and lvalue */
    Widget w;
    logAndProcess(w);
    logAndProcess(std::move(w));

    /* Test rvalue reference type */
    int&& i = 3;
    auto _i = move(i);

    /* Test lvalue reference type */
    int& j = i;
    auto _j = move(j);

    /* Test rvalue */
    auto t = move(1);

    /* Test rvalue reference type */
    //int&& i = 3;
    auto x = move1(i);

    /* Test lvalue reference type */
    auto y = move1(j);

    /* Test rvalue */
    auto z = move1(1);
    std::cout << "type of t : " << type_name<decltype(t)>() << std::endl;

}
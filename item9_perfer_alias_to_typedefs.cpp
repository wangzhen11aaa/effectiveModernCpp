#include <iostream>
#include "type_name.hpp"
#include <list>
using namespace std;

typedef void(*_FP)(int, const std::string &);

using FP = void(*)(int, const std::string &);

void func(int i, const std::string &s){
    cout << "Calling func here" << endl;
}

template<class T>
using remove_const_t = typename remove_const<T>::type;

template<class T>
using remove_reference_t = typename remove_reference<T>::type;

template<class T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

/* Proof typename must precede the type */
template<typename T>
struct MyAllocList{
    typedef std::list<T> type;
private:
    enum class WineType
    {White, Red, Rose};
    WineType type;
};

template<typename T>
class Widget{
private:
    /* Usage of typename is right */
    // typename MyAllocList<T>::type list;     
    /* no-typename usage is right */
    MyAllocList<T>::type list;
};

int main(){
    _FP p_func = &func;
    p_func(1, string("1"));
    FP p_func1 = &func;
    p_func1(2, "2");

    const int t = 10;
    std::remove_const<const int>::type a = 10;
    cout << "a's type after remove_const: " << type_name<decltype(a)>() << endl;
    cout << "Before ++ operation, a : " << a << endl;
    a++;
    cout << "After ++ operation, a : " << a << endl;
    const int& r = t;
    cout << "r's type : " << type_name<decltype(r)>() << endl;

    std::remove_reference<std::remove_const<const int &>::type>::type _r = 20;
    //_r++;
    //cout << "_r's type : " << type_name<decltype(_r)>() << endl;
    Widget<int> w;

    return 0;
}

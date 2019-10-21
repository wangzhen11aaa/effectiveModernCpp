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

template <typename T>
void f(T &param)
{
    cout << type_name<decltype(param)>() << "\n";
}
template<typename T>
void f1(T&& param){
    cout << "Type: T: " << type_name<T>() << "\n";
    cout << type_name<decltype(param)>() << "\n";
}

template<typename T>
void f3(T param){
    cout << "Type: T: " << type_name<T>() << "\n";
    cout << type_name<decltype(param)>() << "\n";
    cout << "Add pointer \n";
    param++ ;//"Hello world";
    cout << *param << *(param+1) << endl;
}

void myFunc(int param[]){
    cout << "param :" << endl;
    cout << param[0] << endl;
}
void myFunc1(int* param){
    cout << "param pointer : " << endl;
    cout << param[0] << endl;
}

template<typename T>
void myFunc2(T param){
    cout << "param pointer in template : " << endl;
    cout << param[0] << endl;
}

// normal reference.
template<typename T>
void myFunc3(T& param){
    cout << "param pointer in template (reference)" << endl;
    cout << param[0] << endl;
    cout << "Type in template myFunc3 : " << type_name<T>() << endl;
    cout << type_name<decltype(param)>() << "\n";
    param[0] = 10;

}

// Universal reference
template<typename T>
void myFunc4(T&& param){
    cout << "Param pointer in template (double reference) " << endl;
    cout << param[0] << endl;
    cout << "Type in template myFunc4" << type_name<T>() << endl;
    cout <<  type_name<decltype(param)>() << endl;
}

// return size of array
template<typename T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept{
    return N;
}

void someFunc(int a, int b){
    cout << "a + b: " << a + b << endl;
}

//function arguments.
template<typename T>
void f2(T& param){
    cout << "T's type : " << type_name<T>() << endl;
    cout << "param's type : " << type_name<decltype(param)>() << endl;
    param(1,2);
}

// template function automatically deduction.
template<typename T>
T myMax(T x, T y){
    return (x > y)? x : y;
}

int main()
{
    int arr[2] = {1,2};
    int keyVals[] = {1,2,3,4,5,6,7,8};
    int mappedVal[arraySize(keyVals)];

    int twoDimesionalArray[][3] = {{1,2,3}, {4,5,6}};
    cout << "twoDimensionalArray : " << arraySize(twoDimesionalArray) << endl;
    std::array<int, arraySize(keyVals)> mappedVal1s;
    cout << "array 's size : " << mappedVal1s.size();
    myFunc(arr);
    myFunc1(arr);
    myFunc2(arr);
    myFunc3(arr);
    myFunc4(arr);
    cout << "arr[0]" << arr[0] << endl;
    int x = 27;
    const int cx = x;
    const int &rx = x;
    cout << "Parameter's type is  reference" << endl;
    f<int>(x);
    f(cx);
    f(rx);
    f1(x);
    cout << "Parameter's type is double reference" << endl;
    f1(cx);
    f1(rx);
    f1(1);

    const char* const ptr="Fun with pointers";
    f3(ptr);
    cout << *ptr;
    cout << "Function Arguments " << endl;
    f2(someFunc);

    cout << "template function automatically deduction" << endl;
    cout << myMax('a', 'b');
    cout << myMax (1, 2);
    return 0;

}
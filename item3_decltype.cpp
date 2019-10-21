#include <iostream>
#include <vector>
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

// auto is the function return type, acts like template deduction.
// But when deducing, the reference of initializing expression is ignored.

// *************************** HERE IS ORIGINAL *************************/

template<typename Container, typename Index>
auto authAndAccess(Container& c, Index i){
    return c[i]; // return type deduced from c[i];
}

// Prevent the reference-strip of initializing expression action.
template<typename Container, typename Index>
decltype(auto)
authAndAccessNew(Container& c, Index i){
    return c[i];
}
// Universal reference for lvalue and rvalue. c++14
template<typename Container, typename Index>
decltype(auto) 
authAndAccessUniversalCF(Container&& c, Index i){
    //auto _c =std::move(c);
    auto _c = c;
    return std::forward<Container>(_c)[i];
}

//Universal reference C++11 version.
template<typename Container, typename Index>
auto 
authAndAccessUniversalCE(Container&& c, Index i) ->decltype(std::forward<Container>(c)[i]){
    //auto _c = c;
    return std::forward<Container>(c)[i];
}

int main(){
    const int i = 0;
    int  j = 0;
    // if(decltype(j) == decltype(i)){
    //     cout << "Two types equal" << endl;
    // }
    vector<bool> v(5, true);
    auto _v = v[0];
    cout << "v[0]'s type : " << type_name<decltype(_v)>() << endl;

    vector<int> vi(5, 1);
    auto _vi = vi[0];
    cout << "vi[0]'s type : " << type_name<decltype(_vi)>() << endl;
    cout << "Before authAndAccessNew function: " << vi[0] << endl;
    authAndAccess(vi, 0);
    authAndAccessNew(vi, 0) = 100;
    cout << "After authAndAccessNew function : " << vi[0] << endl;
    authAndAccessUniversalCE(vi, 1) = 200;
    cout << "After authAndAccessCE function : " << vi[1] << endl;
    cout << "Before authAndAccessCE : " << vi[2] << endl; 
    authAndAccessUniversalCF(std::move(vi), 2) = 10000;
    // std::move makes vi into a unspecifed stage.
    // auto vi_new = std::move(vi);
    cout << "vi's size : " << vi.size() << endl;
    cout << "AFter authAndAccessCF : " << vi[2] << endl; 
    cout << "std::move(vi) 's type : " << type_name<decltype(std::move(vi))>() << endl;
    return 0;
}
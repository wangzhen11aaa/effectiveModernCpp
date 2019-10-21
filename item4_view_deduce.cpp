#include <iostream>
#include <boost/type_index.hpp>
using namespace std;


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

template<typename T>
class TD;

int main(){
    using boost::typeindex::type_id_with_cvr;
    const int theAnswer = 42;
    auto x = theAnswer;
    auto a = std::is_same<decltype(theAnswer), decltype(x)>::value;
    cout << "theAnswer's type and x's type is : " << a << endl;
    cout << "theAnswer's type : " << type_name<decltype(theAnswer)>() << endl;
    cout << "x's type : " << type_name<decltype(x)>() << endl;
    x = 43;
    cout << "After assigning x to 43 " << endl;
    cout << "theAnswer is : " << theAnswer << endl;
    cout << "x 's type from boost tool : " << type_id_with_cvr<decltype(x)>().pretty_name() << endl;

    //TD<decltype(x)> xType;
    return 0;
}
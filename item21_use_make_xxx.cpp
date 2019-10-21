#include <iostream>
#include "type_name.hpp"
//#include <memory>

using namespace std;

namespace u{
/* For c++11, we construct our-own make_unique function 
What's more auto return without trailing return type; deduced return types are
a C++14 extension.
*/
template<typename T, typename... Ts>
auto make_unique(Ts&&... params)-> std::unique_ptr<T>{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
}
int main(){
    auto t = u::make_unique<int>(1);
    return 0;
}
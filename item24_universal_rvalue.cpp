#include <iostream>
#include "type_name.hpp"

using namespace std;

class Widget{};

int main(){
    /* rvalue reference assignment to rvalue rvalue reference */
    Widget&& var1 = Widget();
    cout << "var1's type : " << type_name<decltype(var1)>() << endl;
    cout << "var1's address : " << &var1 << endl;
    auto&& var2 = var1;
    cout << "var2's type: " << type_name<decltype(var2)>() << endl;
    
    /* Let format var1 to rvalue */
    auto && var3 = std::forward<decltype(var1)>(var1);
    cout << "var3's type : "<< type_name<decltype(var3)>() << endl;
    cout << "var3's address : " << &var3 << endl;
    /* use lvalue reference  */
    auto&& var4 = var2;
    cout << "var4's type : " << type_name<decltype(var4)>() << endl;
    cout << "var4's value : " << &var4 << endl;
    
    int&& i = 5;
    cout << "i's address : " << &i << endl;
    int& _i = i;
    cout << "_i's address : " << &_i << endl;
    return 0;

}
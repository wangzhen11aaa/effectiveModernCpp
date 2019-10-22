#include <string>
#include <iostream>
#include "type_name.hpp"

using namespace std;
template<typename T>
/* When param is not rvalue, then T is deduced to T& 
,then the invocation of func becomes (T& &&param), But reference to reference is a bad idea.
*/
/* Reference collapsing: Rule :If either reference is a lvalue reference, the result is a lvalue reference. Others are 
rvalue referenc. So func (T& &&) is a lvalue reference */
void func(T&& param){
    cout << "param's type : " << type_name<decltype(param)>() << endl;
    cout << "T's type : " << type_name<T>() << endl;

}
class Widget{
    string value;
public:
    Widget()=default;
    Widget(string v):value(v) {};
};
template<typename T>
void f(T&& fparam){
    func(std::forward<T>(fparam));
}

template<typename T>
T WidgetFactry(){
    T t;
    return t;
}

int main(){
    Widget w("abc");
    func(w);
    func(WidgetFactry<Widget>());
    return 0;
}
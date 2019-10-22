#include <iostream>
#include <memory>

#include "type_name.hpp"

using namespace std;
class SomeDataStructure{
    SomeDataStructure(SomeDataStructure&& rhs){cout << "Invoke from SomeDataStructure via rvalue reference \n";};
};

class Widget{
public:
    Widget(string &s):name(s){}
    Widget(Widget&& rhs):name(std::move(rhs.name)), p(std::move(rhs.p)){ cout << "Invoke from Widget via rvalue reference \n";}
    string getName(){
        return name;
    }
private:
    string name;
    shared_ptr<SomeDataStructure> p;
};

Widget makeWidget(){
    string s("zzz");
    Widget w(s);
    cout << "std::move(w)'s type : " << type_name<decltype(std::move(w))>() << endl;
    return std::move(w);
}
int main(){
    string s("abc");
    Widget w(s);
    Widget w1(std::move(w));
    Widget w2 = makeWidget();
    cout << "w1.name : " << w2.getName() << endl;
    return 0;
}
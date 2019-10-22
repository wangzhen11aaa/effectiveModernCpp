#include <iostream>
#include <vector>
#include <functional>

#include "type_name.hpp"

using namespace std;

class Widget{
public:
    Widget(int v=0):divisor(v){};
    void addFilter() const;
private:
    int divisor;
};
using FilterContainer = std::vector<std::function<bool(int)>>;
FilterContainer filters;

void Widget::addFilter() const
{
    /* = will capture this pointer, which is not safe too.. */
    // filters.emplace_back(
    //     [=](int value) { cout << "divisor : " << divisor << endl; return value % divisor == 0;}
    // );
    /* Above code equals */
    //  auto currentObjectPtr = this;
    // filters.emplace_back(
    //     [currentObjectPtr](int value){return value % currentObjectPtr->divisor == 0;}
    // );
    /* The most safe version, get non-static divisor from this class */
    auto divisorCopy = divisor;
    filters.emplace_back(
        [=](int value){return value % divisorCopy == 0;}
    );
}   

/* default capture mode is reference */
int main(){

    filters.emplace_back(
        [](int value){return value % 5 == 0;}
    );

    vector<int> v{1,2,3,4,5};
    for(auto _v: v){
        cout << "Process : " << _v << endl;
        cout << "Process result " << filters.front()(_v) << endl;
    }
    Widget w(3);
    w.addFilter();
        for(auto _v: v){
        cout << "Process : " << _v << endl;
        cout << "Process result " << filters[1](_v) << endl;
    }
    return 0;
}
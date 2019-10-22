#include <iostream>
#include <vector>
#include <functional>

#include "type_name.hpp"

using namespace std;

class Widget{
public:
    Widget(int v=0):divisor(v){};
    void addFilter() const;
    void addDivisorFilter();
private:
    int divisor;
};
using FilterContainer = std::vector<std::function<bool(int)>>;
FilterContainer filters;

int computeSomeValue1(){
    return 1;
}
int computeSomeValue2(){
    return 2;
}

int computeDivisor(int x, int y){
    return x + y;
}
/* addDivisorFilter function with static value*/
void Widget::addDivisorFilter(){
    /* For capture part only captures non-static part, so the last divisor and the second to last
    divisor have the same value */
    static auto cal1 = computeSomeValue1();
    static auto cal2 = computeSomeValue2();
    static auto divisor = computeDivisor(cal1, cal2);

    filters.emplace_back(
        [=](int value){
            cout << "Current divisor : "<< divisor << endl;
            return value % divisor == 0;
        }
    );
    divisor++;
    //int length = filters.size();
    
    filters.emplace_back(
        [=](int value){
            cout << "After divisor++, Current divisor : "<< divisor << endl;
            return value % divisor == 0;
        }
    );
}
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
    /* C++14 version capture */
    filters.emplace_back(
        [divisor=divisor](int value){return value % (divisor+1) == 0;}
    );

}


/* default capture mode is reference */
int main(){

    filters.emplace_back(
        [](int value){return value % 5 == 0;}
    );

    vector<int> v{1,2,3,4,5};
    // for(auto _v: v){
    //     cout << "Process : " << _v << endl;
    //     cout << "Process result " << filters.front()(_v) << endl;
    // }
    Widget w(3);
    w.addFilter();
    // for(auto _v: v){
    //     cout << "Process : " << _v << endl;
    //     cout << "Process result " << filters[1](_v) << endl;
    // }
    w.addDivisorFilter();
    int size = filters.size();
    cout << "filters' size : " << size << endl;
    cout << "The last filter \n";
    for(auto _v: v){
        cout << "Process : " << _v << endl;
        cout << "Process result " << filters[size-1](_v) << endl;
    }
    cout << "The second to last filter \n";
        for(auto _v: v){
        cout << "Process : " << _v << endl;
        cout << "Process result: " << filters[size-2](_v) << endl;
    }
    return 0;
}
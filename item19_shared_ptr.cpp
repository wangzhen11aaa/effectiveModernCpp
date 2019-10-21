#include <iostream>
#include <new>
#include <vector>

#include "type_name.hpp"

/* std::shared_ptrs are twice the size of the raw pointer, because
they internally contain a raw pointer to the resource as well as a raw 
pointer to the resource's reference count. */
/* 
  Increments and decrements of the reference count must be atomic.
 */
using namespace std;

class Widget{};

auto loggingLevel = [](Widget* pw){
    cout << "In loggingLevel \n";
    delete pw;

};

std::unique_ptr<Widget, decltype(loggingLevel)> upw(new Widget(), loggingLevel);
std::shared_ptr<Widget> spw(new Widget(), loggingLevel);
auto customDeleter1 = [](Widget* pw){};
auto customDeleter2 = [](Widget* pw){};


/* Vector of shared pointers to Widget */
class Widget1: public std::enable_shared_from_this<Widget1>{
//class Widget1 {
public:
    Widget1(int m = 0):_m(m){}
    void setValue(int y){
        _m = y;
    }
    void process();
     
private:
    int _m;
};

std::vector<std::shared_ptr<Widget1>> processedWidget1s;

void Widget1::process(){
    cout << _m << endl;
    // Add std::shared_ptr to current object to processedWidgets.
    processedWidget1s.emplace_back(shared_from_this());
    //processedWidget1s.emplace_back(this);
}
int main(){
    /* Shared pointer template has one type argument */
    // std::shared_ptr<Widget> pw1(new Widget(), customDeleter1);
    // //std::shared_ptr<Widget> pw2(new Widget(), customDeleter2);

    // /* Unique pointer template has two type arguments */
    // std::unique_ptr<Widget, std::function<void(Widget*)>> upw1(new Widget(), customDeleter1);
    // std::unique_ptr<Widget, decltype(customDeleter2)> upw2(new Widget(), customDeleter2);
    //std::unique_ptr<Widget> upw2(new Widget(), customDeleter2);
   // Widget* w = new Widget();

    /* shared_ptrs and unique_ptrs are designed for 
       pointers to single objects.
     */
    Widget1* wArray = new Widget1[3];
    //auto wArrayWP = make_shared<Widget1>(wArray);
    for(int i = 0; i < 3; i++){
        wArray[i].setValue(i);
        //wArray[i].process();
    }
    std::shared_ptr<Widget1> w1 = std::make_shared<Widget1>();
    
    cout << "w1's use_count : " << w1.use_count() << endl;
    w1->process();
    //wArrayWP.process();
    cout << "After processing, size of  processWidget : " <<  processedWidget1s.size() << endl;

    auto pw = new Widget();
    /* Raw pointer pw is associated with two distinct shared pointers,
       echo shared_ptr will free the Raw pointer, which results that
       the pointer will be released more than noce, That sucks.
     */
    std::shared_ptr<Widget> spw1(pw, loggingLevel);
    std::shared_ptr<Widget> spw2(pw, loggingLevel);
    cout << "spw1's use_count : " << spw1.use_count() << endl;
    cout << "spw2's use_count : " << spw2.use_count() << endl;
    return 0;

}
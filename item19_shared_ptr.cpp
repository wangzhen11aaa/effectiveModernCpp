#include <iostream>
#include <new>
#include <vector>

#include "type_name.hpp"

/* std::shared_ptrs are twice the size of the raw pointer, because
they internally contain a raw pointer to the resource as well as a raw 
pointer to the resource's reference count. 

    **** std::shared_ptr object contains a pointer to the reference count for the object it points to. 
    **** That's true, but it's a bit misleading, because the reference count is part of a larger data structure
    **** known as control block. There is a control block for each object managed by std::shared_ptrs. The control block contains
    **** in addition to reference count, a copy of the custom deleter, if one has been specified. If a custom allocator was specified,
    **** the control block contains a copy of that, too. What's more a secondary reference count known as the weak count.
    &&&& std::make_shared always creates a control block.
    &&&& A control block is created when std::shared_ptr is constructed from a unique-ownership pointer(i.e a std::unique_ptr or std::auto_ptr).
    &&&& When a std::shared_ptr constructor is called with a raw pointer, it creates a control block.  std::shared_ptr constructors taking std::shared_ptrs
    &&&& or std::weak_ptrs as constructor arguments don't create new control blocks, because they can rely on the smart pointers passed to point to any necessary control
    &&&& block.
    #### As a consequence of these rules is that constructing more than one std::shared_ptr from a single raw pointer gives you a complimentary ride 
    #### on the particle accelerator of undefined behavior, because the pointed-to object will have multiple control blocks. Multiple control blocks mean 
    #### multiple reference counts, and multiple control blocks means the object will be destroyed multiple times (once for each reference count). That means that
    #### the code like this is bad, bad, bad.

    $$$$ Conclusion: std::shared_ptrs can't do is work with arrays. In yet another difference from std::unique_ptr, std::shared_ptr has an API that's designed only for pointers
    $$$$ to single objects. There's no std::shared_ptr<T[]>.
*/
/*
    The shared_ptr have the best of both worlds: a system that works automatically(like garbage collection), yet applies to all resources
    and has predictable timing(like destructor.)
*/
/*
    The 
*/
/* 
  Increments and decrements of the reference count must be atomic.
 */
using namespace std;

class Widget{};

auto loggingLevel = [](Widget* pw){
    cout << "In loggingLevel \n";
    delete pw;

};

/*
    Like std::unique_ptr,std::shared_ptr uses delete as its default resource-destructioin mechanism, but it also
    supports custom deleters. The design of this support differs from that for std::unique_ptr, however. For std::unique_ptr,
    the type of the deleter is part of the type of the smart pointer. For shared_ptr, it is not.
*/
std::unique_ptr<Widget, decltype(loggingLevel)> upw(new Widget(), loggingLevel);
std::shared_ptr<Widget> spw(new Widget(), loggingLevel);
auto customDeleter1 = [](Widget* pw){};
auto customDeleter2 = [](Widget* pw){};

/*
    Introduce enable_shared_from_this (this means the raw pointer)
    void Widget::process(){
        ....
        processedWidgets.emplace_back(this); 
    }
    #### This will pass a raw pointer (this) to a container of std::shared_ptrs. The std::shared_ptr thus constructed will create a new 
    #### control block for the pointed-to Widget(*this). That doesn't sound harmful until you realize that if there are std::shared_ptrs outside
    #### the member function that already point to that Widget, it's game, set, and match for undefined behavior.

    ^^^^ This std::enable_shared_from_this<> template (Which is Curiously Recurring Template Pattern <paradigm>)
*/

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

/*
    Internally, shared_from_this looks up the control block for the current object, and it creates a new std::shared_ptr that refers to that control 
    block. There must be an existing std::shared_ptr(e.g, one outside the member function calling shared_from_this) that points to the current object. If
    no such std::shared_ptr exists(i.e if the current object has no associated control block), behaviour is undefined, although shared_from_this typically throws an exception.
*/

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
       the pointer will be released more than noce, That sucks with core dump.(abort)
     */

    std::shared_ptr<Widget> spw1(pw, loggingLevel);
    std::shared_ptr<Widget> spw2(pw, loggingLevel);
    cout << "spw1's use_count : " << spw1.use_count() << endl;
    cout << "spw2's use_count : " << spw2.use_count() << endl;

    
    return 0;

}
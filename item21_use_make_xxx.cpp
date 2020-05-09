#include <iostream>
#include "type_name.hpp"
#include <memory>
#include <vector>

using namespace std;

/* Let's define self namespace u */
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
/*
    We prefer the make series function, because 
    1. We don't want to repeat the code like this std::shared_ptr<Widget> spw2(new Widget), in which
    Widget occurs two times and it needs not to be so.
    2.More secure with make series function. ** The make function can handle exception safely. **
*/
class Widget{
public:
    Widget(int _v = 0):v(_v){
        std::cout << "Construct Widget \n";
    }
    ~Widget(){
        std::cout << "Destructing Widget \n" << v << endl;
    }
    int getValue(){
        return v;
    }
private:
    int v;
};

/* Trivial function for computating priority */
int computePriority(){
    //throw "Exception";
    return 0;
}

template<typename T>
void processWidget(std::shared_ptr<T>, int){}

template<typename T>
void processWidget(std::unique_ptr<T>, int){}

/* shared_ptr to support custom-delete */
auto widgetDeleter = [](Widget* pw){
    cout << "Wil delete Widget : " << pw->getValue() << endl;
    delete pw;

};


int main(){
    auto t = u::make_unique<int>(1);
    //                              &
    /* make_unique come from c++14 & & */
    //                            & & & //

    auto upw(std::make_unique<Widget>());
    std::unique_ptr<Widget> upw1(new Widget);
    
    /* Potential resource leak... */
    try{
        //throw "Hello, World";
        processWidget(std::shared_ptr<Widget>(new Widget(3)), computePriority());
    }catch (char const* e){
        cout << e << endl;
    }
    try{
    /* Safe usage method */
        processWidget(std::make_unique<Widget>(), computePriority());
    } catch (char const* e){
        cout << e << endl;
    } 

    /* make_shared<> vs shared_ptr<> */
    /* shared_ptr will allocate twice: one for Widget and control block
       It's obvious that this code entails a memory allocation, but it actually performs tow.
       Every shared_ptr points to a control block containing, among other things, the reference count
       for pointed-to object. Memory for this control block is allocated in the std::shared_ptr constructor.
     */
    std::shared_ptr<Widget> sp(new Widget(3));
    /* make_shared method will aallocate a single chunk of memory to hold 
       both Widget object and control block.
     */
    auto sp1 = make_shared<Widget>(2);

    cout << "After processWidget" << endl;

    
    /* 
       Make series functions do not permit the specification of custom deleters, but both std::unique_ptr and 
       std::shared_ptr have constructors that do.
       Use shared_ptr with custom-delete 
       Reveal more information, quotes from the book 

    */
    std::shared_ptr<Widget> spc(new Widget(3), widgetDeleter);

    /* Use initializer_list object to construct the make_xxx series function */
    auto initList = {1,2};
    auto x = std::make_unique<vector<int>>(initList);
    cout << "x's type : " << type_name<decltype(x)>() << endl;
    cout << "Vector's element :" << endl;
    for(auto _x : *x){
        cout << _x <<endl;
    }
    auto x1 = std::make_unique<Widget>(3);

    cout << "Size of shared_ptr : " << sizeof(sp1) << ", Size of unique_ptr : " << sizeof(x1) << endl;
    /* exception-safe  normal way */
    std::shared_ptr<Widget> spw(new Widget, widgetDeleter);
    try{
    processWidget(spw, computePriority());

    /* exception-unsafe, but fast way 
           "
            The answer has to do with compilers' translation of source code into object code.
            At runtime, the arguments for a function must be evaluated before the function can be invoked,
            so in the call to processWidget, the following things must occur before processWidget can begin
            execution:
            1. The expression "new Widget" must be evaluated, i.e, a Widget must be created on the heap.
            2. The constructor for the std::shared_ptr<Widget> responsible for managing the pointer produced by
            new must be executed.
            3. computePriority must run.
            The compiler are not required to generate code that execute them in this order. "new Widget" must 
            be executed before the std::shared_ptr constructor may  be called, because the result of that new is 
            used as an argument to that constructor, but computePriority may be executed before those calls, after them 
            or curcially, between them. That is, compilers may emit code to execute the operations in this order:
            1. Perform "new Widget".
            2.Execute computePriority.
            3.Run std::shared_ptr constructor.
            If step2 throws an exception, then the memory leak happens, For you new something without delete it and what's more, it has no name,
            so you can not delete in anyway.
       "
    
    */
    processWidget(shared_ptr<Widget>(new Widget, widgetDeleter), computePriority());
    
    /* safe and fast */
    processWidget(std::move(spw), computePriority());
    
    /* Use makexxx function */
    processWidget(make_shared<Widget>(), computePriority());
    }
    catch (char const * e){
        cout << e << endl;
    }
    return 0;
}
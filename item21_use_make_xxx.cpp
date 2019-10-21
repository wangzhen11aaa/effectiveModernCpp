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
    /* shared_ptr will allocate twice: one for Widget and control block */
    std::shared_ptr<Widget> sp(new Widget(3));
    /* make_shared method will aallocate a single chunk of memory to hold 
       both Widget object and control block.
     */
    auto sp1 = make_shared<Widget>(2);

    cout << "After processWidget" << endl;
    
    /* Use shared_ptr with custom-delete */
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

    /* exception-unsafe, but fast way */
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
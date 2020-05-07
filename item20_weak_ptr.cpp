#include <iostream>
#include<boost/smart_ptr/shared_ptr.hpp>
#include <new>
#include <memory>
#include <unordered_map>

#include "type_name.hpp"

using namespace std;
using namespace boost;


class Widget{

};
using WidgetID = int;

/*
    This kind of smart pointer has to contend with a problem unknown to std::shared_ptrs: the possibility that what it points to 
    has been destroyed. A truly smart pointer would deal with this problem by tracking when it dangles, i.e, when the object it is 
    supposed to point to no longer exists. That's precisely the kind of smart pointer std::weak_ptr is.
    The std::weak_ptr looks anything but smart. std::weak_ptrs can't be dereferenced, nor can they be tested for nullness.
    That's because std::weak_ptr isn't a standalone smart pointer. It's an augmentation of std::shared_ptr.
*/
std::shared_ptr<const Widget> fastLoadWidget(WidgetID id){
    static std::unordered_map<WidgetID, std::weak_ptr<const Widget>> cache;
    auto objPtr = cache[id].lock();
    if(!objPtr){
        cout << "Cache miss \n";
        objPtr = make_shared<Widget>();
        cache[id] = objPtr;
    }else{
        cout << "Cache hit \n";
    }
    return objPtr;
}

int main(){
    auto spw = std::make_shared<Widget>();
    std::weak_ptr<Widget> wpw(spw);
    cout << "After constructing weak pointer, the use_count of spw: " << spw.use_count() << endl;
    auto spw1 = spw;
    cout << "After copy assignment of shared_ptr : " << spw.use_count() << endl;
    auto spw2(spw);
    cout << "After copy construction, the use_count of spw : " << spw.use_count() << endl; 
    if(!wpw.expired()){
        cout << "Weak pointer is not expired \n";
    }
    /* Deleting original shared_ptr is not good */
    // delete(spw.get());
    // delete(spw2.get());
    // delete(spw1.get());
    //cout << "After deleting all shared pointers, we get spw.use_count : " << spw.use_count() << endl;

    /* Let's try boost */

    auto sp  =new boost::shared_ptr<int>(new int(2));
    cout << "sp's use_count : " << sp->use_count() << endl;

    
    delete(sp);
    /* Use std::shared_ptr */
    auto sp1 = make_shared<Widget>();
    std::weak_ptr<Widget>wp1(sp1);
    auto sp2 = wp1.lock();
    cout << "After locking: the use_count of sp1: " << sp1.use_count() << endl;
    //std::weak_ptr<Widget> wp(sp);
    sp1.reset();
    sp2.reset();

    auto sp3 = wp1.lock();
    if(sp3 == nullptr){
        cout << "Has been empty, you're late \n";
    }
    /* Construct unique_ptr with shared_ptr 
       Can not do it.
    */
    // std::unique_ptr<Widget>up(wp1);
    //cout << "Whether the weak_pointer is valid: " << wp.expired() << endl;
    //cout << "After deleting sp, the valid of sp : " << wp.expired() << endl;
    auto a = fastLoadWidget(1);
    cout << "Something's use_count() : " << a.use_count() << endl;
    auto b = fastLoadWidget(1);
    cout << "Something's use_count() : " << b.use_count() << endl;

    /* Cycle pointer */
    auto c1 = make_shared<Widget>();
    auto c2(c1);
    c1 = c2;
    cout << "c1's use_count() : " << c1.use_count() << endl;
    cout << "c2's use_count() :" << c2.use_count() << endl;
    c1.reset();
    cout << "After reseting, c1's use_count() " << c1.use_count() << endl;\
    c2.reset();
    cout << "After reseting, c2's use_count() " << c2.use_count() << endl;
    return 0;
}
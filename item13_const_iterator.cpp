#include <iostream>
#include <vector>
#include <algorithm>
#include "type_name.hpp"

using namespace std;


// self-defined cbegin()
template<class C>
auto cbegin(const C& container)->decltype(std::begin(container)){
    return std::begin(container);
}
// self-defined cend()
template<class C>
auto cend(const C& container)->decltype(std::end(container)){
    return std::end(container);
}

// self-defined container 
template<typename C, typename V>
void findAndInsert(C& container, const V& targetVal, const V& insertVal){
    /* Use self defined cbegin and cend function */
    // using std::cbegin;
    // using std::cend;
    auto it = std::find(begin(container), end(container), targetVal);
    container.insert(it, insertVal);
}

int main(){
    using vi =  vector<int>::iterator;
    using vci = vector<int>::const_iterator;
    vector<int> v{1,2,3,4};
    cout << "type_name of v : " << type_name<decltype(v)>() << endl; 
    int x = 5;
    // We test the non-const iterator version, to see if this function works.
    vci _vci0 = std::find(v.begin(), v.end(), 1);
    cout << *_vci0 << endl;
    cout << "type of _vci0 : " << type_name<decltype(_vci0)>() << endl; 

    // In this function, we force the v.begin() non-const iterator converted to const iterator.
    vci _vci = std::find(static_cast<vci>(v.begin()), static_cast<vci>(v.end()), 1);
    //vci _vci_ = std::find(static_cast<vi>)
    cout << "type of _vci : " << type_name<decltype(_vci)>() << endl; 
    v.insert(_vci, 10);
    // No conversion from const reference to non-const reference.
    //v.insert(static_cast<vi>(_vci), 100);
    cout << "result fo _vci :" << *_vci << endl;
    // Insert after finding item in vector.
    auto it = std::find(v.cbegin(), v.cend(), 3);
    cout << "type_name of it is : " <<  type_name<decltype(it)>() << endl;
    v.insert(it, 100);
    cout << "After all insertation : " << endl;
    
    // Try findAndInsert.
    findAndInsert(v, 2, 5);

    for(auto _v: v){
        cout << _v << endl;
    }

    return 0;
}
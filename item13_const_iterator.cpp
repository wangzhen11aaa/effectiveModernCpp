#include <iostream>
#include <vector>
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
    auto it = std::find(cbegin(container), cend(container), targetVal);
    container.insert(it, insertVal);
}

int main(){
    using vi =  vector<int>::iterator;
    using vci = vector<int>::const_iterator;
    vector<int> v{1,2,3,4};
    cout << "type_name of v : " << type_name<decltype(v)>() << endl; 
    int x = 5;
    vci _vci = std::find(static_cast<vci>(v.begin()), static_cast<vci>(v.end()), 1);
    //vci _vci_ = std::find(static_cast<vi>)
    v.insert(_vci, 10);
    cout << "result fo _vci :" << *_vci;
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
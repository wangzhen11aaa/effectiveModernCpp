#include <iostream>
#include "type_name.hpp"
#include <map>

using namespace std;

/* C++98 has four special member functions :default construtor, destructor, 
copy constructor, copy assignment, General special member functions are
implicitly public and inline.*/
/* C++11 Two more inductees: the move construcotr, and move assignment operator.*/
/* The BigThree Rule : copy assignment, copy costructor, destructor (memory resource management) */
class Widget {
public:
    Widget(Widget&& rhs); // move constructor.
    Widget& operator=(Widget&& rhs); // move assignment operator.

};

class Base{
public:
    Base(int _m): m(_m){cout << "Constructing base "<< endl;}
    virtual ~Base(){cout << "Invoke destructor via Base class" << endl;}
private:
    int m;
};

class Derived :public Base{
public:
    Derived(int x):Base(x){cout << "Constructing derived " << endl;}
    ~Derived() {cout << "Invoke destructor via Derived class" << endl;};

};

class Derived1: public Derived{
public:
    Derived1(int x):Derived(x){cout << "Constructing derived1 class " << endl;}
    ~Derived1(){cout << "Derived1 destructor via Derived1 class" << endl;}
};

void f(Base& t){
    cout << "Invoke f, t's type: " << type_name<decltype(t)>() << endl;
}


class StringTable{
public:
    using dataType = map<int, string>;
    StringTable(){
        cout << "Creating StringTable object \n";
    }
    ~StringTable(){
        cout << "Using StringTable destructor \n";
    }
    /* move operation */
    // StringTable& operator = (StringTable&& st){
    //     values = std::move(st.values);
    //     return *this;
    // }
    StringTable(StringTable&&) = default;
    StringTable& operator = (StringTable&&) = default;
    StringTable(const StringTable&) = default;
    StringTable& operator= (StringTable&) = default;
    // StringTable& operator = (StringTable &st){
    //     values = st.values;
    //     return *this;
    // }
   // StringTable& operator = ()
    void setValue(pair<int, string>&& p){
        values.insert(p);
    }
    unsigned int size(){
        return values.size();
    }
private:
    dataType values; 
};


/** Three things to decide move action to be generated or not */
/* No copy operations are declared in the classs */
/* NO move operations are declared in the class */
/* No destructor is declared in the class */

/* Use c++11 key word default to force the compiler to generate the default 
copy functions */

int main(){
    // Derived d(1);
    // Base& b = d;
    //f(d);
    /* Derived1 class */
    //Derived1 d1(2);
    //f(d);
    // /* Delete parent class which does have a virtual destructor, May be normal */
    // Base* pb = new Derived(2);
    // delete(pb);
    Base* pb1 = new Derived1(3);
    delete(pb1);
    /* Delete parent class which does not have a virtual destructor, undefined behavour. */
    //Derived* d0 = &d1;
    //delete(d0);
    //f(d0);
    StringTable st;
    for(int i = 0; i < 10000000; i++){
        st.setValue(make_pair(i, "abc"));
    }
    cout << "st' values size : " << st.size() << endl;
    StringTable st1;
    /* Without user defined destructor, ./item17_special_member_function_generation  15.88s user 0.75s system 99% cpu 16.699 total */
    /* ./item17_special_member_function_generation  8.07s user 0.29s system 99% cpu 8.379 total use move version. */
    /* with user defined destructor ./item17_special_member_function_generation  8.61s user 0.29s system 99% cpu 8.909 total */
    st1 = std::move(st);
    /*  Use copy-constructor ./item17_special_member_function_generation  14.96s user 0.65s system 99% cpu 15.620 total */
    //st1 = st;
    /* With user-defined destructor,  ./item17_special_member_function_generation  15.05s user 0.67s system 99% cpu 15.736 total*/
    /* Without any assignment of values ./item17_special_member_function_generation  8.50s user 0.28s system 99% cpu 8.791 total */
    cout << "st1's value size: " << st1.size() << endl;
    return 0;
}
#include <iostream>
#include "type_name.hpp"
#include <map>

using namespace std;

// In official parlance, the special member functions are the ones that C++ is willing to
// generate on its own. What's more, the generated special member functions are implicitly public and 
// inline and they are nonvirtual unless the function in question is a destructor in a derived class inheriting from 
// base class with a virtual destructor.

/* C++98 has four special member functions :default construtor, destructor, 
copy constructor, copy assignment, General special member functions are
implicitly public and inline.*/
/* C++11 Two more inductees: the move construcotr, and move assignment operator.*/
/* The BigThree Rule : copy assignment, copy costructor, destructor (memory resource management) 
If you declare any of a copy constructor, copy assignment operator, or destructor, you should declare all three.

*/
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

/*
    The following class support all move && copy operations. For
    we have declared 
*/

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
    StringTable& operator = (StringTable&&) = default; // Support moving 

    // StringTable(const StringTable&) = default; // Support copying
    // StringTable& operator= (StringTable&) = default;
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

    /*
    st' values size : 10000000
    Creating StringTable object 
    st1's value size: 10000000
    st's value size :10000000 // This must not the move action generated by the compiler.
    Using StringTable destructor 
    Using StringTable destructor 
    ./item17_special_member_function_generation  26.46s user 0.72s system 99% cpu 27.266 total
                         */
    /*
     When we comment our declared destructor (we want the compiler generate the move operation), and we use it. It is fast.
    st1's value size: 10000000
    st's value size :0 // The move action is done. For the st's size becomes zero.
    ./item17_special_member_function_generation  16.19s user 0.41s system 95% cpu 17.459 total
     For we have defined destructor ourself, so the move operator will act like memberwise copy (copy assignment.), It is slow.
                         Creating StringTable object 

    */
    //st1 = std::move(st); 


    /* 
        For this copy assignment, it will copy all the pair from st to st1, so it will be slow.
        ? We have declared the destructor, so it will suppress the copy operator generation.
        Why does this code run?
        Generation of the copy operations in classes with an explicitly declared destructor is deprecated.
        不 建议 在具有用户声明的复制构造函数或析构函数的类中生成此函数。

    */
    st1 = st;

   
    cout << "st1's value size: " << st1.size() << endl;
    cout << "st's value size :" << st.size() << endl;
    return 0;
}
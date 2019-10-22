#include <iostream>
#include <set>
#include <string>
#include "type_name.hpp"

using namespace std;

std::multiset<std::string> names;

/* Overload shows up */
string nameFromIdx(int idx){
    return string("hello_world");
}

void logAndAdd1(int idx){
    cout << "Invoke logAndAdd1 via int idx \n";
    names.emplace(nameFromIdx(idx));
}

/* Only support const lvalue in first version */
void logAndAdd(const std::string& name){

    cout << "Invoke logAndAdd via lvalue reference \n";
    // << "T's type : " << type_name<T>() << endl;
    cout << "name's type : " << type_name<decltype(name)>() << endl;
    names.emplace(name);

}

/* Rewrite the function for efficiency */
template<typename T>
void logAndAdd1(T&& name){
    cout << "Invoke logAndAdd via rvalue reference \n";
    cout << "T's type : " << type_name<T>() << endl;
    cout << "name's type : " << type_name<decltype(name)>() << endl;
    names.emplace(forward<T>(name));
}

/* class Person for these conditions */
class Person{
public:
    /* Perfect forwarding ctor */
    template<typename T>
    explicit Person(T&& _name):name(forward<T>(_name)){        
    }
    /* int ctor */
    explicit Person(int idx):name(nameFromIdx(idx)){}
    string getName(){
        return name;
    }
private:
    string name;
};

/* Derived class copy and move constructor do not call base class's
   copy and move constructors.
 */

class SpecialPerson: public Person{
public:
    template<typename T>
    SpecialPerson(T&& __name):Person(__name){};
    /* The following line invoke the Person's universal-reference verson ctor */
    SpecialPerson(const SpecialPerson& rhs):Person(rhs){

    }
    SpecialPerson(SpecialPerson&& rhs):Person(std::move(rhs)){

    }
};

int main(){
    string petName("Darla");
    /* standard lvalue */
    logAndAdd(petName);
    /* standard rvalue  */
    logAndAdd(string("Persephone"));
    /* string literal to build the string,
       then 'Patty Dog' will be bound to a temporary 
       std::string created from string literal, and 
       which is the same case as the second.
     */
    logAndAdd("Patty Dog");

    /* Use new function */
    logAndAdd1(petName);
    logAndAdd1(string("Persephone"));
    logAndAdd1("Patty Dog");

    /* Invoke overload functions */
    logAndAdd1(1);  
    /* Invoke overload function by short  */
    short nameIdex = 3;
    /* Sorry this invoke will be sucked by universal reference */
    //logAndAdd1(nameIdex);

    Person p(2);
    cout << "p(2)'s name : " << p.getName() << endl;
    Person p1("XiaoDi");
    cout << "XiaoDi's name : " << p1.getName() << endl;

    /* short type shows up again */
    //Person p2(nameIdex);
    //cout << "p2's name: "<< p2.getName() << endl;
    /* Invoke copy constructor */
    const auto cp = p;
    Person p3(cp);

    /* Special person */
    //SpecialPerson sp("abed");
    //cout << "SpecialPerson's name : " << sp.getName() << endl;
    return 0;
}
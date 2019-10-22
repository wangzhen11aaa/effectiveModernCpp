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
/* logAndAddImpl for non-integral type */
template<typename T>
void logAndAddImpl(T&& name, std::false_type){
    cout << "In logAndAddImpl, T's type : "<< type_name<T>() << endl;
    cout << "In logAndAddImpl, name's type : " << type_name<decltype(name)>() << endl;
    names.emplace(std::forward<T>(name));
}

/* logAndAddImpl for integral type */
template<typename T>
void logAndAddImpl(T&& name, std::true_type){
    cout << "In logAndAddImpl, T's type " << type_name<T>() << endl;
    cout << "In logAndAddImpl, name's type: " << type_name<decltype(name)>() << endl;
    names.emplace(nameFromIdx(1));
}

/* Use Tag Dispatch to continue use perfect forward */
template<typename T>
void logAndAdd(T&& name){
    /* Universal reference parameter will bring T of lvalue refrece,
       So we should remove reference.
     */
    /* std::remove_reference<T> */
    logAndAddImpl(std::forward<T>(name),  std::is_integral<typename std::remove_reference<T>::type>());
    /* is_same structure only has one method which is ::value, return bool type. */
    //logAndAddImpl(std::forward<T>(name), std::is_same<int ,typename std::decay<T>::type>);
}


/* enable_if determines whether usse perfect_forward or not */
class Person2{
public:
    /* The following version does not consider the derived class */
    // template<typename T,
    // typename = 
    // typename std::enable_if<!std::is_same<Person2, typename std::decay<T>::type>::value>::type
    // >
    // explicit Person2(T&& n):name(n){
    //     cout << "Invoke tempalte Person2 method name: " << name;
    // } 
    /* More complex */
    //template<typename T,  typename = 
    //typename std::enable_if<!std::is_base_of<Person2, typename std::decay<T>::type>::value>::type>
    template<typename T, 
    typename = std::enable_if_t<
    !std::is_base_of<Person2,std::decay_t<T>>::value 
    && !std::is_integral<std::remove_reference_t<T>>::value>>
    explicit Person2(T&& n):name(std::forward<T>(n)){
        /* assert that a std::string can be created from a T object */
        static_assert(std::is_constructible<string, T>::value, "Parameter n can not be used to construct a std::string");
        cout << "n's type : " << type_name<decltype(n)>() << endl;
        cout << "std::forward<T>(n)'s type : " << type_name<decltype(std::forward<T>(n))>() << endl;
        cout << "Invoke template Person2 method name : " << name << endl;
    }
    explicit Person2(int idx):name(nameFromIdx(idx)){}
private:
    string name;
};


class SpecialPerson2: public Person2{
public:
    /* Also */
    SpecialPerson2(string&& n):Person2(n){}
    SpecialPerson2(const SpecialPerson2& rhs): Person2(rhs){}
    SpecialPerson2(SpecialPerson2&& rhs):Person2(std::move(rhs)){}
};


class string1{
public:
    string1() = default;
    template<typename T>
    string1(T&& _n):name(forward<T>(_n)){cout << "Invoke string1's constructor \n";}
    string getName(){
        return name;
    }
private:
    string name;
};

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

/* Pass by value version, could solve dervied class problem */
class Person1{
public:
    //Person1() = default;
    explicit Person1(string1 n){
        cout << "n's type : " << type_name<decltype(n)>() << endl;
        name = std::move(n);
        cout << "Invoke Person1 via string";
        }
    string getName(){
        return name.getName();
    }
private:
    string1 name;
};

class SpecialPerson1: public Person1{
public:
    SpecialPerson1(string n):Person1(n){}
    SpecialPerson1(const SpecialPerson1& rhs):Person1(rhs){}
    SpecialPerson1(SpecialPerson1&& rhs):Person1(rhs){}
};




/* Derived class copy and move constructor do not call base class's
   copy and move constructors.
 */

// class SpecialPerson: public Person{
// public:
//     template<typename T>
//     SpecialPerson(T&& __name):Person(__name){};
//     /* The following line invoke the Person's universal-reference verson ctor */
//     SpecialPerson(const SpecialPerson& rhs):Person(rhs){

//     }
//     SpecialPerson(SpecialPerson&& rhs):Person(std::move(rhs)){

//     }
// };

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
    Person1 p1_1("XiaoMing");
    cout << "p1_1's name : " << p1_1.getName() << endl;
    Person1 p1_2(string("XiaoHong"));
    cout << "p1_2's name : " << p1_2.getName() << endl;

    /* Test SpecialPerson1 */
    SpecialPerson1 sp1("XiaoM");
    cout << "sp1's name : " << sp1.getName() << endl;

    /* Tag version test */
    logAndAdd("Apple");

    /* Tag version test int */
    logAndAdd(1);
    
    /* Test on Person2 */
    Person2 p2_1(std::move("abc"));
    Person2 p2_2(p2_1);
    
    /* Test on SpecialPerson */
    SpecialPerson2 sp2_1("abc");
    SpecialPerson2 sp2_2(sp2_1);
    
    /* Test on char16_t */
    Person2 p2_3((char *)u"abc");
    return 0;
}
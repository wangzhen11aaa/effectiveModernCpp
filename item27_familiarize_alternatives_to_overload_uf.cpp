#include <iostream>
#include <set>
#include <string>
#include "type_name.hpp"

/*
    Item 26 explains that overloading on universal references can lead to a variety of problems, both for 
    freestanding and for member functions(especially constructors). Yet it also gives examples where such
    overloading could be useful. If only it would behave the way we'd like! This item explores ways to achieve 
    the desired behavior, either through designs that avoid overloading on universal references or by 
    employing them in ways that constrain the types of arguments that they can match.

    The discussion that follows builds on the examples introduced in Item26. If you haven't read that Item recently,
    you'll want to review it before continuing.

    Abandon overloading
    The logAndAdd in Item26 can be avoided by breaking logAndAdd into two functions named by logAndAddName and 
    logAndAddNameIdx. Alas, this approach won't work for the second example we considered, the Person constructor, 
    because constructor names are fixed by the language. 

    Pass by Const T&
    An alternative is to revert to C++98 and replace pass-by-universal-reference with pass-by-lvalue-reference-to-const.
    In fact, that's the first approach Item26 considers. This drawback is that the design isn't as efficient as
    we'd prefer. Knowing what we now know about the interaction of universal references and over-loading, giving
    up some efficiency to keep thing simple might be a more attractive trade-off than it initially appeared.

    Pass by value
    An approach that often allows you to dial up performance without any increase in complexity is to replace
    pass-by-reference parameters with, counterintuitively, pass by value. The design adheres to the advice in Item
    41 to consider passing objects by value when you konw you'll copy them, so I'll defer to that item for a
    detailed discussion of how things work and how efficient they are. Here I'll show how the technique could be used
    in Person exmples:
    class Person{
        public:
            explicit Person(std::string n) // replace T&& ctor; see
            :name(std::move(n)) {} // Item41 for use of std::move

            explicit Person(int idx)
            :name(nameForIdx(idx)) {}

            ...

        private:
            std::string name;
    };

    Because there's no std::string constructor taking only an integer, all int and int-like arguments to a 
    Person constructor(e.g, std::size_t, short, long) get funneled to the int overload. Similarly, all arguments
    of type std::string(and things from which std::strings can be created, e.g, literals such as "Ruth") get passed
    to the constructor taking a std::string. There are thus no surprises for callres. You could argue, I suppose
    ,that some people might be surprised that using 0 or NULL to indicate a null pointer would invoke the int overload,
    but such people should be referred to Item8 and required to read it repeatedly until the thought of using
    0 or NULL as a null pointer makes them recoil.

    Use Tag dispatch
    Neither pass by value-reference-to-const nor pass by value offers support for perfect forwarding. If the motivation for
    the use of a universal refernce is perfect forwarding, we have to use a universal reference; there's no other
    choice.Yet we don't want to adandon overloading. So if we don't give up overloading and we don't give up
    univseral references, how can we avoid overloading on universal references?

    It's actually not that hard. Calls to  overloaded functions are resolved by looking at all the parameters
    of all the overloads as well as all the arguments at the call site, then choosing the function with the 
    best overall match - taking into account all parameter/argument combinations. A universal reference parameter
    generally provides an exact match for whatever's passed in, but if the universal reference is part of a parameter 
    list containing other paramters that are not universal references, sufficiently poor matches on
    the non-universal reference parameters can knock an overload with a universal reference out of the running.
    That's the basis behind the tag dispatch approach, and an example will make the foregoing description easier
    to understand.
    
    We'll apply tag dispatch to logAndAdd example on page 177. Here's the code for that example, let you 
    get sidetracked looking it up:

    std::multiset<std::string> names; //global data structure
    template<typname T>
    void logAndAdd(T&& name) // name to data structure
    {
        auto now = std::chrono::system_clock::now();
        log(now, "logAndAdd");
        names.emplace_back(std::forward<T>(name));
    }
    By itself, this function works fine, but were we to introduce the overload taking an int that's used to 
    look up objects by index, we'd be back in the troubled land of Item26.The goal of this Item is to avoid that.
    Rather than adding the overload, we'll reimplement logAndAdd to delegate to two functions, one for integral
    values and one for everything else. logAndAdd itself will accept all arguments types, both integral and non-integral.

    The two functions doing the real work will be named logAndAddImpl, i.e, we'll use overloading.One of the 
    functions will take a universal reference. So we'll have both overloading and universal references. But 
    each function will also take a second parameter, one that indicates whether the argument being passed is 
    integral. This second parameter is what will prevent us from trumbling into the morass described in 
    Item26, because we'll arrange it so that the second parameter will be the factor that determines which overload 
    is selected.
    template<typname T>
    void logAndAdd(T&& name){
        logAndAdd(std::forward<T>(name), std::is_integral<T>()); //not quite correct
    }

    For integral arguments that are rvalues, it's also what it does.But, as Item 28 explains, if an lvalue argument
    is passed to the universal reference name, the type deduced for T will be an lvalue referecne.So if an lvalue
    of type int is passed to logAndAdd, T will be deduced to be int &。That's not an integral type, because 
    reference aren't integral type.That means that std::is_integral<T> will be false for any lvalue argument,
    even if the argument really does represent an integral value.

    Recognizing the problem is tantamount to solve it, because the ever-handy Standard C++ Library has a type
    trait, std::remove_refrence, that does both whtat its name suggests and what we need, remove the reference 
    qualifiers from a type. The proper way to write logAndAdd is therefore:
    template<typename T>
    void logAndAdd(T&& name){
        logAndAddImpl(
            std::forward<T>(name), std::is_integral<typename std::remove_reference<T>::type());
        )
    }
    This does the trick.(In C++14, you can save a few keystrokes b using std::remove_reference_t<T>) in place 
    of the typename std::remove_reference<T>::type.

    With that taken care of, we can shift our attention to the function being called, logAndAddImpl.There are 
    two overloads, and the first is applicable only to non-integral types(i.e, types where std::is_integral<typename std::
    remove_reference<T>::type> is false):
    template<typename T>
    void logAndAddImpl(T&& name, std::false_type)
    {
        auto now = std::chrono::system_clock::now();
        log(now, "logAndAddImpl");
        names.emplace(std::forward<T>(name));
    }

    This is straightforward code, once you understand t
    he mechanics. Conceptually, logAndAdd passes a boolean
    to logAndAddImpl indicating whether an integral type was passed to logAndAdd, but true and false are runtime
    values, and we need to use overload resolution - a compile-time phenomenon -to choose the correct logAndAddImpl
    overload. That means we need a type that corresponds to true and a different type that corresponds to false. This need
    is common enough that the Standard Library provides what is required under the names std::true_type and 
    std::false_type. The argument passed to logAndAddImpl by logAndAdd is an object of type that inherits from 
    std::truee_type if T is integral and from std::false_type if T is not integral. The net result is that this
    logAndAddImpl overload is a viable candidate for call in logAndAdd only if T is not an integral type.

    The second overload covers the opposite case: when T is an integral type. In that event, logAndAddImpl simply
    finds the name corresponding to the passed-in index and passes that name back to logAndAdd:
    std::string nameFromIdx(int idx); //as in Item 26
    void logAndAddImpl(int idx, std::true_type) //integral argument: look up name and call logAndAdd with it.
    {
        logAndAdd(nameFromIdx(idx));
    }

    BY having logAndAddImpl for an index look up the corresponding name and pass it to logAdd(from where it will
    be std::forwarded to other logAndAddImpl overload), we avoid the need to put the logging code in both logAndAddImpl
    overloads.

    In this design, the types std::ture_type and std::false_type are "tags" whose only purpose is to force overload
    resolution to go the way we want.Notice that we don't even name those parameters. They serve no purpose at runtime,
    and in fact we hope the compilers will recognized that tag parameters are unused and will optimize them
    out of the program's execution image.The call to the overloaded implementation functions inside logAndAdd
    "dispatches" the work to the correct overload by causing the proper tag object to be created. Hence the name for 
    this design: tag dispatch. It's a standard building block of template metaprogramming, and the more you look
    at code inside contemporary C++ libraries, the more often you'll encounter it.

    For our purposes, what's important about tag dispatch is less how it works and more how it permits us to 
    combine universal references and overloading without the problems described in Item 26. The dispatching function -
     logAndAdd - are overloaded, and one takes a universal reference parameter, but resolution of calls to these
     functions depends not just on the universal reference paramter, but also on the tag parameter, and the tag
     values are designed so that no more than one overload will be a viable match. As a result, it's the tag
     that determines which overload gets called. The fact that the universal reference parameter will always generate
     an exact match for its argument is immaterial.
     
     Constraing templates that takes universal references
     A keystone of tag dispatch is the existence of single(unoverloaded) function as the client API.
     Creating an unoverloaded dispatch function is usually easy, but the second problem case Item 26 considers, 
     that of a perfect-forwarding constructor for the Person class, is an exception. Compilers may generate copy and 
     move constructors themselves,so  even if you write only one constructor and use tag dispatch within it, some 
     constructor calls may be handled by compiler-generated functions that bypass the tag dispatch system.

     In truth, the real problem is not that the compiler-generated functions sometimes bypass the tag dispatch
     design, it's that they don't always pass it by. You virtually always want to copy constructor for a class to
     handle requests to copy lvalues of that type, but, as Item 26 demonstrates, providing a constructor taking a 
     universal reference causes the universal reference constructor(rather than the copy constructor) to be called 
     when called when copying non-const values. That item also explains that when a base class declares a perfect-
     forwarding constructor, that constructor will typically be called when derived calss implement their copy
     and move constructors in the conventional fashion, even though the correct behavior is for the base class's 
     copy and move constructors to be invoked.

    For situations like these, where an overloaded function taking a universal reference is greedier than you want,
    yet not greedy enough to act as a single dispatch function, tag dispatch is not the droid you're looking for. 
    You need a different technique, one that lets you rachet down the conditions under which the function template
    that the universal reference is part of is permitted to be employed. What you need, my friend, is std::enable_if.

    std::enable_if gives you a way to force compilers to behave as if a particular template didn't exist.By default,
    all templates are enabled, but a template using std::enable_if is enabled only if the condition specified by
    std::enable_if is satisfied. In our case, we'd like to enable the Person perfect-forwarding constructor only if 
    the type being passed isn't Person. If the type being passed is Person, we want to disable the perfect-forwarding
    constructor in Person, showing only as much of the std::enable_if as is required simply to use it. 

    class Person{
        public:
            template<typename T, typename = typename std::enable_if<condition>::type>
            explict Person(T&& n);
            ...

    };

    The condition we want to specify is that T isn't Person, i.e, that the templatized constructor should 
    be enabled only if T is a type other than Person.Thanks to a type trait that determines whether two 
    types are the same (std::is_same), it would seem that the condition we want is ! std::is_same<Person, T>::value
    This is close to what we need, but it's not quite correct, because, as Item 28 explains, the type deduced
    for a universal reference initialized with an lvalue is always an lvalue reference. That means that
    for code like this,

    Person p("Nancy");
    auto CloneP(p);// initialize from lvalue

    the type T in the universal constructor will be deduced to be Person&. The types Person and Person& are not
    the same, and the result of std::is_same will reflect that:std::is_same<Person, Person&>::value is false.

    If we think more precisely about what we mean when we say that the templatized constructor in Person should be enabled 
    only if T isn't Person, we'll realize that when we're looking at T, we want to ignore
    
    1. Whether it's a refernce. For the purpose of determing whether the universal reference constructor should be 
    enabled, the types Person, Person&, and Person&& are all the same as Person.
    
    2.Whether it's const or volatile. As far as we're concerned, a const Person and a volatile Person and a const 
    volatile Person are the same as a Person.

    This means that we need a way to strip any references, consts, and volatiles from T before checking to see
    if that type is the same as Person. Once again, the Standard Library gives us what we need in the form of
    a type trait. That trait is std::decay. std::decay<T>::type is the same as T, except that references and 
    cv-qualifiers(i.e, const or volatile qualifiers) are removed.(I'm fuding that the truth here, because 
    std::decay, as its name suggests, also turns array and function types into pointers, but for purposes of 
    this discussion, std::decay behaves as I've described.) The condition we want to control whether our 
    constructor is enabled, then ,is 
    ! std::is_same<Person, typename std::decay<T>::type>::value

    i.e, Person is not the same type of T, ignoring any references or cv-qualifiers. (As Item 9 explains, the
    "typename" in front the std::decay is required, because the type std::decay<T>::type depends on the template Parameter T.)

    Inserting this condition into the std::enable_if boilerplate above, plus formatting the result to make it
    easier to see how the pieces fits together, yields this declaration for Person's perfect-forwarding constructor:
    class Person{
        public:
            template<typename T, typename = typename std::enable_if<
                                                !std::is_same<Person, typename std::decay<T>::type>::value
                                            >::type
            >
            explicit Person(T&& n)
    };

    For derived class :
    class SpecialPerson: public Person{
        public:
            SpecialPerson(const SpecialPerson& rhs) // copy ctor; calls base class forwarding ctor!
            :Person(rhs)
            {...}

            SpecialPerson(SpecialPerson&& rhs)
            :Person(std::move(rhs))
            {...}
        ...
    };

    When we copy or move a SpecialPerson object, we except to copy or move its base class parts using the 
    base class's copy and move construtors, but in these functions, we're passing SpecialPerson objects to 
    the base class's constructors, and because SpecialPerson isn't the same as Person(not even after application
    of std::decay), the universal reference constructor in the base class is enabled, and it happily instantiates 
    to perform an exact match for SpecialPerson argument. This exact match is better than the derived-to-base
    conversions that would be necessary to bind the SpecialPerson objects to Person parameters in Person's copy
    and move constructors, so with the code we have now, copying and moving SpecialPerson objects would use
    the Person perfect-forwarding constructor to copy or move their base class parts!


    The derived class is just following the normal rules for implementing derived class copy and move constructors,
    so the fix for this problem is in the base class and , in particular, in the condition that controls whether
    Person's universal reference constructor is enabled. We now realize that we don't want enable the 
    templatized constructor for any argument type other than Person, We want to enable it for any argument
    type other than Person or a type derived from Person. Pesky inheritance.

    You should not be surprised to hear that among the standard type traits is one that determines whether one
    type is derived from another. It's called std::is_base_of. std::is_base_of<T1, T2>::value is true if T2
    is derived from T1. Types are considered to be derived from themselves, so std::is_base_of<T,T>::value is 
    true. This is handy, because we want to revise our condition Person's perfect-forwarding constructor such that 
    the constructor is enabled only if type T, after stripping it of references and cv-qualifiers, is neither Person
    nor a class derived from Person. Using std::is_base_of instead of std::is_same gives us what we need:

    public Person{
        public:
            template<typename T, typename = typename std::enable_if<!std::is_base_of<Person, typename std::decay<T>::type>::value>::type
            >
            explicit Person(T&& n);
            ...
    };

    Now we've finally done. Provided we're writing the code in C++11, that is. If we're using C++14, this code
    will still work, but we can employ alias templates for std::enable_if and std::decay to get ride of typename
    and ::type cruft, thus yielding this somewhat more palatable code:

    class Person{
        public:
            template< typename T, typename = std::enable_if<
                                            !std::is_base_of<Person, 
                                                            std::decay_t<T>
                                                            >::value
                                            >
            >
            explicit Person(T&& n);
    };

    All we need to do - and I really do mean that this is everything - is (1) add a Person constructor overload
    to handle integral arguments and (2) further constrain the templatized constructor so that it's disabled
    for such arguments. Pour these ingredients into the pot with everything else we've discussed, simmer 
    over a low frame, and savor the aroma of success.
    
    class Person{
        public:
            template<typename T, typename T=std::enable_if_t<!std::is_base_of<Person, std::decay_t<T>::value
                                                            && 
                                                            !std::is_integral<std::remove_reference_t<T>>::value
                                                            >
            >
            explicit Person<T&& n)
            :name(std::forward<T>(n)) //ctor for std::strings and args convertible to std::strings
            {...}

            explicit Person(int idx)
            :name(nameFromIdx(idx))
            {...}
            
            ...

            // other Person, or inherited Person class will use the default copy-construtor or move-constructor.

        private:
            std::string name;
            
    };

    A thing of beauty! Well, okay, the beauty is perhaps most pronounced for those with something of 
    a template metaprogramming fetish, but the fact remains that this approach not only gets the job done, it
    does it with unique aplomb.Because it uses perfect-forwarding, it offers maximal efficieny, and because
    it controls the combination of universal reference and overloading rather than forbidding it, this technique can be 
    applied in circumstances(such as constructors) where overloading is unavoidable.


    Trade-offs
    As a rule, perfect forwarding is more efficient, because it avoids the creation of temporary objects 
    solely for the purpose of conforming to the type of a parameter declaration. In the case of Person constructor, 
    perfect forwarding permits a string literal such "Nancy" to be forwarded to the constructor for the std::string
    inside Person, whereas the techniques not using perfect forwarding must create a temporary std::string obejct
    from string literal to satisfy the parameter spcification for the Person constructor.
    

*/
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
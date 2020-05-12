#include <iostream>
#include "type_name.hpp"

using namespace std;

class Widget{};

/*
    It's been said that the truth shall set you free, but under the right circumstances, a well-chosen lie can equally liberating.
    In fact, "T&&" has two different meanings. One is rvalue reference, of course.Such references behave exactly the way you expect: they 
    bind only to rvalues, and their primary raison d'etre identify objects that may be moved from.
    
    The other meaning for "T&&" is either rvalue reference or lvalue reference. Such reference look like rvalue references in the source code (i.e, T&&),
    but they can behave as if they were lvalue references (i.e, "T&"). Furthure more, they can bind to const or non-const objects, to volatile or non-volatile
    objects, even to objects that are both const and volatile. They can bind to virtually anything. Such unprecedentedly flexible references deserve a name of 
    their own. I call them universal references.

    Universal references arise in two contexts. The most common is function template parameters, such as this example from the sample code above:
    template<typename T>
    void f(T&& param); // param is a universal reference
    The second context is auto declarations, including this one from the sample code above:
    auto && var2 = var1; // var2 is a universal reference
    What these contexts have in common is the presence of type deduction. In the template f, the type of param is being deduced, and in the 
    declaration for var2, var2's type is being deduced.Compare that with the following examples(also from the sample code above), where type deduction
    is missing. If you see "T&&" without type deduction, you're looking at an rvalue referece:
    void f(Widget && param); // no type deduction, param is an rvalue reference.
    
    Because universal references are references, they must be initialized.The initializer for a universal refernce determines whether it represents an rvalue
    reference or an lvalue reference. If the initializer is an rvalue, the universal reference corresponds to an rvalue reference. If the initializer is an lvalue, 
    the universal reference corresponds to an lvalue reference. For universal references that are function parameters, the initializer is provided at the call 
    site:

    template<typename T>
    void f(std::vector<T> && param); //param is an rvalue reference.

    When f is invoked, the type T will be deduced. But the form of param's type declaration isn't "T&&", it's "std::vector<T> &&". That rules out the possibility that param
    is a universal refernce. Param is therefore an rvalue reference, something that your compilers will be happy to confirm for you if you try to pass an lvalue of f:
    std::vector<int> v;
    f(v); // error! can not bind lvalue to rvalue reference.

    Even the simple presence of const qualifier is enough to disqualify a referene from being universal.

    template<typename T>
    void f(const T&& param); param is rvalue reference.

    universal reference can be bound to some callable object too.

    auto timeFuncInvocation = [](auto && func, auto&&... params){
        start timer;
        std::forward<decltype(func)>(func)(
            std::forward<decltype(params)>(params)...);
            stop timer and record elapsed time;
        )
    };

    Explanation:
        func is a universal reference that can be bound to any callable object, lvalue or rvalue. args is zero or more universal references
        (i.e, a universal reference parameter pack) that can be bound to any number of objects of arbitrary types.
    Distinguishing between rvalue references and universal references will help you read source code more accurately("Does that T&& I'm looking at
    bind to rvalues only or to everything"), and it will avoid ambiguities when you communicate with your colleagues("I am using a universal reference here, 
    not an rvalue reference..."). Let's embrace abstraction. Revel in it.

*/
int main(){
    // int a = 1;
    // int &a1 = static_cast<int &>(a);
    //int &&b = static_cast<int &>(a);
    /* rvalue reference assignment to rvalue rvalue reference */
    Widget&& var1 = Widget();
    cout << "var1's type : " << type_name<decltype(var1)>() << endl;
    cout << "var1's address : " << &var1 << endl;
    auto&& var2 = var1;
    cout << "var2's type: " << type_name<decltype(var2)>() << endl;
    
    /* Let format var1 to rvalue */
    auto && var3 = std::forward<decltype(var1)>(var1);
    cout << "var3's type : "<< type_name<decltype(var3)>() << endl;
    cout << "var3's address : " << &var3 << endl;
    /* use lvalue reference  */
    auto&& var4 = var2;
    cout << "var4's type : " << type_name<decltype(var4)>() << endl;
    cout << "var4's value : " << &var4 << endl;
    
    int&& i = 5;
    cout << "i's address : " << &i << endl;
    int& _i = i;
    cout << "_i's address : " << &_i << endl;
    return 0;

}
#include <iostream>
#include <vector>
//#include <ctype.h>

#include "type_name.hpp"

using namespace std;

/* variadic templates */
/* Perfect forwarding fails if calling f with a particular argument does one thing,
   but fwd with the same argument does something different.

   One of the features most prominently emblazoned on the C++ 11 box is perfect forwarding. Perfect forwarding, It's 
   perfect! Alas, tear the box open, you'll find the there's "perfect" (the ideal), and then there's "perfect" (the reality). 
   C++ 11's perfect forwarding is very good, but it archieves true perfection only if you're willing to overlook an eplison or two.
   This Item is devoted to familiarizing you with the epsilons.

   Before embraking on our epsilon exploration, it's worthwhile to review what's meant by "perfect forwarding". "Forwarding" just
   means that one function passes-forwards-its parameters to another function. The goal is for the second function (the one being forwarding)
   received. That rules out by-value parameters, because they're copies of what the original caller passed in. We want the forwarded-to
   function to be able to work with the original caller passed in. Pointer parameters are also rules out, because we don't want to
   force callers to pass pointers. When it comes to general purpose forwarding, we'll be dealing with parameters that are references.

   Perfect forwarding means that we don't just forward objects, we also forward their salient characteristics:their types, whether they're
   lvalues or rvalues, and whether they're const or volatile. In conjunction with the observation that we'll be dealing with reference
   parameters, this implies that we'll be using universal references, because only universal reference parameters encode information
   about the lvalueness and rvalueness of the arguments that are passed to them.

   Let's assume we have some function f, and we'd like to write a function that forwards to it. The core of that we need looks like this:

   template<typename T>
   void fwd(T&& param)
   {
       f(std::forward<T>(param)); // forward it to f.
   }

   Forwarding functions are, by their nature, generic. The fwd template, for example, accepts any type of argument, and it forwards whatever
   it gets. A logical extension of this genericity is for forwarding functions to be not just templates, but variadic templates, thus accepting
   any number of arguments. The variadic form for fwd looks like this:
   tempalte<typename... Ts>
   void fwd(Ts&&... params) // accept any arguments
   {
       f(std::forward<Ts>(params)...); // forward them to f.
   }

    This is the form you'll see in, among other places, the standard containers's emplacement functions and the smart pointer factory
    functions, std::make_shared and std::make_unique.

    Given our target function f and our forwarding function fwd, perfect forwarding fails if calling f with 
    a particular argument does one thing, but calling fwd with the same argument does something different.
    f(expression); // if this does one thing,
    fwd(expression); this does something else, fwd fails to perfectly forward expression  to f.

    Several kinds of arguments lead to this kind of failure. Knowing what they are and how to work around them is important, 
    so let's tour the kinds of arguments that can't be perfect forwarding.

    1. Braced initializers

    f({1,2,3}); fine "{1,2,3}" implicitly converted to std::vector<int>

    fwd({1,2,3}); //error! doesn't compile

    In a direct call to f(such as f({1,2,3})), compilers see the arguments passed at the call site, and they see the types 
    of the parameters declared by f.They compare the arguments at the call site to the parameter declarations to see if they're 
    compatible, and, if necessary, they perform implicit conversions to make the call succeed. In the example above, they 
    generate a temporary std::<int> object from {1,2,3} so that f's parameters v has a std::vector<int> object to bind to.

    When calling f indirectly through the forwarding function template fwd, compilers no longer compare the arguements passed 
    at fwd's call site to the parameter declarations in f. Instead, they deduce the types of the arguments being passed to fwd,
    and they compare the deduced types to f's parameter declartions. Perferct forwarding fails when either of following occurs:
    
    1. Compilers are unable to deduce a type: for one or more of fwd's parameters. In this case, the code fails to compiles.
    2. Compilers deduce the "wrong" type: for one or more fwd's parameters. Here "wrong" could mean that fwd's instantiation would 
    compile with the types behaves differently from a direct call to f with the arguments that were passed to fwd. One source
    of such divergent behavior would be if f were an overloaded function name, and , due to "incorrect" type deduction, the 
    overload of f called inside fwd were different from the overload that would be invoked if were called directly.

    In the "fwd({1,2, 3})" call above, the problem is that passing a braced initilizer to a function template parameter that's not 
    declared to be a std::initializer_list is decreed to be, as the Standard puts it, a "no-deduced context". In plain English,
    that means that compilers are forbidden from deducing a type of the expression {1,2,3} in the call fwd, because fwd's parameter
    isn't declared to be a std::initializer_list. Being prevented from deducing a type for fwd's parameter, compilers must understandly
    reject the call.


     Declartion-only integral static const data members
     As a general rule, there's no need to define integral static const data members in classes; declarations along suffice. That's 
     because compilers perform const propagation on such members' values, thus eliminating the need to set aside memory for them. 
     For example, consider this code:
     class Widget{
         public:
            static const std::size_t MinVals = 28; // Minval's declaration
            ...
     };
     ... // no definition for MinVals
    std::vector<int> widgetData;
    widgetData.reserve(Widget::Minvals); // use of Minvals

    Here, we're using Widget::MinVals(henceforth simply MinVals) to specify widget Data's initial capacity, even though MinVals lacks 
    a definition. Compilers work around the missing definition(as they are required to do) by plopping the value 28 into all places
    where MinVals is mentioned. The fact that no storage has been set aside for MinVals' values is unproblematic. If MinVals' address
    were to be taken(e.g., if somebody created a pointer to MinVals), then MinVals's would require storage, and the code above, though
    it would compile, would fail at link-time until a definition for MinVals was provided.

    With that in mind, imagine that f (the function fwd forward its arguments to ) is declared like this:

    Calling f with MinVals is fine, because compilers will just replace MinVals with its value:
    f(Widget::MinVals); // fine, treated as "f(28)"
    Alas, thing may not go so smoothly if we try to call f through fwd:
    fwd(Wiget::MinVals); // error! shouldn't link

    This code will compile, but it should link. If that reminds you of what happens if we write code that takes MinVals' address, that's 
    good, because the underlying problem is the same.

    Although nothing in the source code takes MinVals' address, fwd's parameter is a universal reference, and references, in the code 
    generated by compilers, are usually treated like pointers. In the program's underlying binary code(and on the hardware), pointers
    and references are essentially the same thing. At this level, there's truth to the adage that references are simply pointers that
    are automatically dereferenced. That being the case, passing MinVals by reference is effectively the same as passing it by pointer, 
    and as such, there has to be some memory for the pointer to point to.Passing integral static const data members by reference, then,
    generally requires that they should be defined, and that requirement can cause code using perfect forwarding to fail where the 
    equivalent code without perfect forwarding successds.

    According to the Standard, passing MinVals by reference require that it be defined. BUT,not all implementations enforce this requirement.
    So depending on your compilers and linkers,you may find that you can perfect-forward integral static const data members that haven't
    defined. If you do, congratulations, but there is no reason to expect such code to port. To make it possible, simply provide a 
    definition for the integral static const data member in question. For MinVals, that'd look like this:
    
    const std::size_t Widget::MinVals; // in Widget's .cpp file.
    

 */
template<typename... Ts>
void f(Ts&&... params){
    cout << "Invoke f \n";
}

template<typename... Ts>
void fwd(Ts&&... params){
    f(std::forward<Ts>(params)...);
}


void f(const std::vector<int>& v){
    cout << "v";
}

void f(std::size_t val){
    cout << "val : " << val;
}

class Widget{
public:
    static const std::size_t MinVals = 29;
};

/* Give it a name */
const std::size_t Widget::MinVals;

/* function overloading */
int process(int value){
    return 0;
}
int process(int value, int priority){
    return 1;
}

void f(int pf(int)){
    cout << "f pf \n";
}
// template<typename... Ts>
// void fwd(Ts&&... args){
//     f(std::forward<Ts>(args)...);
// }

/* More complex function overload */
template<typename T>
T WorkOnValue(T param){
    return param;
}

/* On bitfield */
struct IPv4Header{
    std::uint32_t version:4,
                    IHL:4,
                    DSCP:6,
                    ECN:2,
                    totalLength:16;

};

void f1(std::size_t sz){
    cout << sz << endl;
}

int main(){
    f({1,2,3});
    /* fwd deduce the bracket-list failed */
    //fwd({1,2,3});
    auto il = {1,2,3};
    cout << "type of il : " << type_name<decltype(il)>() << endl;
    fwd(il);

    f(Widget::MinVals);
    /* Invoke on MinVals is not good , for 
    fwd's parameter is universal reference*/
    /* Define the variable */
    fwd(Widget::MinVals);
    /* Invoke on 20 is good */
    fwd(20);
    /* invoke on function pointer */
    f(process);
    using ProcessFuncType = int (*)(int);
    ProcessFuncType processValPtr = process;
    fwd(processValPtr);
    /* static_cast function template to function ptr */
    fwd(static_cast<ProcessFuncType>(WorkOnValue));

    /* on bit fields */
    IPv4Header h;
    f1(h.totalLength);
    /* Can not bind a non-const reference to bit field, for no pointer can be pointed to arbitrary bits */
    //f(h.totalLength);

    auto length = static_cast<std::uint16_t>(h.totalLength);
    f(length);
    return 0;
}
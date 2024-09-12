#include <iostream>
#include <vector>
#include <functional>

#include "type_name.hpp"

using namespace std;
/*
    A lambda expression is just that: an expression. It's part of source code. In
    std::find_if(container.begin(), container.end(), [](int val){return 0 < val && val < 10;})

    A clousure is the runtime object created by a lamdba.Depending on the capture mode, closures
    hlod copies of or references to the captured data.In the call to std::find_if above, the 
    closure is the object that's passed at runtime as the third argument to std::find_if.

    A closure class is a class from which a closure is instantiated.Each lambda causes compilers to
    generate a unique closure class. The statements inside a lambda become executable instructions
    in the member functions of its closure class.

    A lambda is often used to create a clousure that's used only as an argument to a function.
    That's the case in the call to std::find_if above. However, closures may generally be copieed, 
    so it's usually possible to have multiple closures of a closure type corresponding to a single 
    lambda. For example, in the following code,

    {
        int x; // x is local variable.
        ...
        auto c1 = [x](int y){return x * y > 55;} // c1 is copy of the closure produced by lambda

        auto c2 = c1; // c2 is copy of c1
        auto c3 = c2; // c3 is copy of c2


    }
    c1,c2,and c3 are all copies of the closure produced by the lambda.

    Informally, it's perfectly acceptable to blur the lines between lambdas, closures, and 
    closure classes.But in the Items that follow, it's often important to distinguish what 
    exists during compilation(lambdas and closure classes), what exits at runtime(closures),
    and how they relate to one another.
*/

/*
    A by-reference capture causes a closure to contain a reference to a local variable or 
    to a parameter that's available in the scope where lambda is defined. If the lifetime
    of a closure created from that lambda exceeds the lifetime of the local variable or 
    parameter, the reference in the closure will dangle.For example, suppose we have a container
    of filtering functions, each of which takes an int and return bool indicating whether a 
    passed-in value satisfies the filter:

    {
        using FilterContainer = std::vector<std::function<bool(int)>>;

        FilterContainer filters; //  filtering funcs

        void addDivisorFilter()
        {
            auto calc1 = computeSomeValue1(); 
            auto calc2 = computeSomeValue2(); //
            auto divisor = computeDivisor(calc1, calc2);
            filters.emplace_back([&](int value){return value % divisor == 0;})
        }
        This code is a problem waiting to happen. The lambda refers to the local variable
        divisor, but that variable ceases to exist when addDivisorFilter returns. That's 
        immediately after filters'emplace_back returuns, so the function that' added to filters
        is essentially dead on arrival. Using that filter yields undefined behavior from
        from virtually the moment it's created.

        Now, the same problem would exist if divisor's by-reference capture were explict,
        filters.emplace_back(
            [&divisor](int value){return value % divisor == 0;} // danger! ref to divisor will still dangle!
        )
        but with an explicit capture, it's easier to see that the viability of the lambda is depen-
        dent on divisor's lifetime. Also, writing out the name, "divisor", reminds us to ensure
        that divisor lives at least as long as the lambda's closures. That's more specific memory
        jog than general "make sure nothing dangles" admonition that "[&]" conveys.

        If you know that a closure will be used immediately(e.g, by being passed to an STL 
        algorithm) and won't be copied, there is no risk that references it holds will outlive
        the local variables and parameters in the environment where its lambda is created. In 
        that case, you might argue, there's no risk of dangling references, hence no reason to 
        avoid a default by-reference capture mode.For example, our filtering lambda might be 
        used only as an argument of C++11 std::all_of, which returns whether all elements in
        range satisfy a condition:

    }
*/
class Widget{
public:
    Widget(int v=0):divisor(v){};
    void addFilter() const;
    void addDivisorFilter();
private:
    int divisor;
};
using FilterContainer = std::vector<std::function<bool(int)>>;
FilterContainer filters;

int computeSomeValue1(){
    return 1;
}
int computeSomeValue2(){
    return 2;
}

int computeDivisor(int x, int y){
    return x + y;
}
/*
    class Widget{
        public:
            ...
            void addFilter() const; // ctors, etc, add an entry to filters.
        private:
            int divisor; //used in Widget's filter
    };

    Why do we get to know the [=] grammar.
    1. void Widget::addFilter() const{
        filters.emplace_back(){
            [](int value) {return value % divisor == 0;} // error divisor
        }
    }
    1's error: Captures apply only to non-static local variables(including parameters) visible in the scope where the 
    lambda is created.In the body of Widget::addFilter, divisor is not a local variable, it's data member of the Widget class.
    It cann't be captured. Yet if the default capture mode is eliminated, the code won't compile:

    2. void Widget::addFilter() const{
        filters.emplace_back([divisor](int value){return value % divisor == 0;}) // error! no local divisor to capture.
    }
    2's error: same as 1's.Although the explicitly capture divisor(either by value or by reference - it doesn't matter), the
    capture won't compile, because divisor isn't a local variable or a parameter:

    3. 
    void Widget::addFilter() const{
        filters.emplace_back(){
            [=](int value){return value % divisor == 0;}
        }
    }
    3's correct: The explanation hinges on the implicit use of a raw pointer: this. Every non-static member function has a this 
    pointer, and you use that pointer every time you mention a data member of the class. Inside any Widget member function, for 
    example, compilers internally replace uses of divisor with this->divisor. In the version of Widget::addFilter with a default 
    by-value capture.
    Equals:
    void Widget::addFilter() const{
        auto currentObjectPtr = this;

        filters.emplace_back({
            [currentObjectPtr](int value) {
                return value % currentObjectPtr->divisor == 0;
            }
        })
    }
    Understanding this is tantamount to understanding that the viability of the closures arising from this lambda is tied to the lifetime of the Widget whoes this pointer
    they contain a copy of.

    Some more complex example.

    using FilterContainer = std::vector<std::function<bool(int)>>; // as before
    FilterContainer filters;
    
    void doSomeWork(){
        auto pw = std::make_unique<Widget>(); // create Widget; See Item21 for std::make_unique
        pw->addFilter(); add Filter that uses Widget::divisor
        ...
    }// destory Widget; filters now holds dangling pointer!

    When a call is made to doSomeWork, a filter is created that depends on the Widget object produced by std::make_unique, i.e, a filter that contains a copy of pointer
    of that Widget- the Widget's this pointer. This filter is added to filters, but When doSomeWork finishes, the Widget is destroyed by the std::unique_ptr managing its 
    lifetime. From that point on, filters contains an entry with dangling  pointer.

    To avoid the pointer-dangling problem, we need the divisor to be a local copy of data member you want to capture and then capturing the copy:
    void Widget::addFilter() const{
        auto divisorCopy = divisor;
        filters.emplace_back(
            [divisorCopy](int value){return value % divisorCopy == 0;}
        )
    }

    To be honest, if you take this approach, default by-value capture will work, too.
    void Widget::addFilter() const{
        auto divisorCopy = divisor; // copy data member
        fitlers.emplace_back(
            [=](int value){
                return value % divisorCopy == 0;
            } // capture the copy and use the copy.
        )
    }

    More concise way in C++14.
    void Widget::addFilter() const{
        filters.emplace_back([divisor=divisor](int value){return value % divisor == 0;}) //C++14 copy divisor to closure use the copy.
    }

    void addDivisorFilter(){
        static auto calc1 = computeSomeValue1(); // now static
        static auto calc2 = computeSomeValue2(); // now static

        static auto divisor = computeDivisor(calc1, calc2);
        
        fitlers.emplace_back([=](int value){return value % divisor == 0;});// captures nothing! refers to above static

        ++divisor; // modify divisor.

    }

    A casual reader of this code could be forgiven for seeing "[=]" and thinking, "Okay, the lambda makes a copy of all objects it uses and it therefore self-contained".
    But it's not self-contained. This lambda doesn't use any non-static local variables, so nothing is captured. Rather, the code for the lambda refers to the static variable
    divisor.

*/
/* addDivisorFilter function with static value*/
void Widget::addDivisorFilter(){
    /* For capture part only captures non-static part, so the last divisor and the second to last
    divisor have the same value */
    static auto cal1 = computeSomeValue1();
    static auto cal2 = computeSomeValue2();
    static auto divisor = computeDivisor(cal1, cal2);

    filters.emplace_back(
        [=](int value){
            cout << "Current divisor : "<< divisor << endl;
            return value % divisor == 0;
        }
    );
    divisor++;
    //int length = filters.size();
    
    filters.emplace_back(
        [=](int value){
            cout << "After divisor++, Current divisor : "<< divisor << endl;
            return value % divisor == 0;
        }
    );
}
void Widget::addFilter() const
{
    /* = will capture this pointer, which is not safe too.. */
    // filters.emplace_back(
    //     [=](int value) { cout << "divisor : " << divisor << endl; return value % divisor == 0;}
    // );
    /* Above code equals */
    //  auto currentObjectPtr = this;
    // filters.emplace_back(
    //     [currentObjectPtr](int value){return value % currentObjectPtr->divisor == 0;}
    // );
	filters.emplace_back([this](int value)
						 { return value % this->divisor; });
	/* The most safe version, get non-static divisor from this class */
	auto divisorCopy = divisor;
	filters.emplace_back(
        [=](int value){return value % divisorCopy == 0;}
    );
    /* C++14 version capture */
    filters.emplace_back(
        [divisor=divisor](int value){return value % (divisor+1) == 0;}
    );

}


/* default capture mode is reference */
int main(){

    filters.emplace_back(
        [](int value){return value % 5 == 0;}
    );

    vector<int> v{1,2,3,4,5};
    // for(auto _v: v){
    //     cout << "Process : " << _v << endl;
    //     cout << "Process result " << filters.front()(_v) << endl;
    // }
    Widget w(3);
    w.addFilter();
    // for(auto _v: v){
    //     cout << "Process : " << _v << endl;
    //     cout << "Process result " << filters[1](_v) << endl;
    // }
    w.addDivisorFilter();
    int size = filters.size();
    cout << "filters' size : " << size << endl;
    cout << "The last filter \n";
    for(auto _v: v){
        cout << "Process : " << _v << endl;
        cout << "Process result " << filters[size-1](_v) << endl;
    }
    cout << "The second to last filter \n";
        for(auto _v: v){
        cout << "Process : " << _v << endl;
        cout << "Process result: " << filters[size-2](_v) << endl;
    }
    return 0;
}
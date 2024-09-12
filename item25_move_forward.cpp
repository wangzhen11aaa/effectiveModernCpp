#include <iostream>
#include <memory>

#include "type_name.hpp"

/*
    Difference between rvalue reference and universal reference is that: Rvalue
   reference bind only to objects that are candidates for moving.If you have an
   rvalue reference parameter, you know that the object it's bound to may be
   moved. The bad idea is that when using the std::move with universal
   references, because that can have the effect of unexpectedly modifying
    lvalues(local variables):
    Like this:
    class Widget{
        public:
            template<typename T>
            void setName(T&& newName) // universal reference
            {name =std::move(newName)}
        ...
        private:
            std::string name;
            std::shared_ptr<SomeDataStructure> p;
    };

    std::string getWidgetName(); // factory function
    Widget w;
    auto n = getWidgetName(); // n is a local variable
    w.setName(n); moves n into w!
    n.value now unkonwn.

    The local variable n is passed to w.setName, which the caller can be
   forgiven for assuming is a read-only operation on n. But because setName
   internally uses std::move to unconditionally cast its reference parameter to
   an rvalue, n's value will be moved into w.name, and n will come back from the
   call to setName with an unspecified value. That's the kind of behavior that
   can drive callers to despair - possibly to violence.

    You might argue that setName shouldn't have declared its parameter to be a
   universal reference. Such references can't be const, yet setName surely
   shouldn't modify its parameter. You might point out that if setName had
   simply been overloaded for const lvalues and for rvalues, the whole problem
   could have been avoided. Like this:

    class Widget{
        public:
            void setName(const std::string & newName)
            {name = newName;}
            void setName(std::string && name)
            {name = std::move(newName);} // set from rvalue
    }

    This would certainly work in this case, but there are drawbacks. First, it's
   more source code to write and maintain. Second, it can be less efficient. For
   example, consider this use of setName: w.setName("Adela Novak") With the
   version of setName taking a universal reference, the string literal "Adela
   Novak" would be passed to setName, where it would be conveyed to the
   assignment operator for std::string inside w. w's name member would thus be
   assigned directly from the string literal; no temporary std::string objects
   would arise. With the overloaded versons of setName, however, a temporary
   std::string object would be created for setName's parameter to bind to, and
   this temporary std::string would then be moved into w's data member. A call
   to setName would thus entail execution of one std::string constructor(to
   create the temporary), one std::string move assignment operator(to move
   newName to w.name), and one std::string destructor(to destroy the
   temporary).That's almost certainly a more expensive execution sequence than
   invoking only the std::string assignment operator taking a const char*
   pointer.

    The most serious problem with overloading on lvalues and rvalues, however,
   isn't the volume or idiomaticity of the source code. nor is it the code's
   runtime performance. It's the poor scalability of the design.Widget::setName
   takes only one parameter, so only two overloads are necessary, but for
   functions taking more parameters, each of which could be an lvalue or rvalue,
   the number of overloads grows geometrically: n parameters necessitates 2^n
   overloads.And that's not the worst of it.Some functions- function templates,
   actually-take a unlimited number of parameters, each of which could be lvalue
   or rvalue. The poster children for such functions are std::make_shared, and ,
   as of C++14, std::make_unique.

    template<class T, class... Args>
    shared_ptr<T> make_shared(Args&&... args); //from C++11 Standard.
    template<class T, class... Args>
    unique_ptr<T> make_unique(Args&&... args); from C++14 Standard.

    For functions like these, overloading on lvalues and rvalues is not an
   option:universal references are the only way to go. And aside such functions,
   I assure you, std::forward is applied to the universal references when
   they're passed to other functions.Which is exactly what you should do.

    If you're in a function that returns by value, and you're returning an
   object bound to an rvalue reference or a universal reference, you'll want to
   apply std::move or std::forward when you return the reference. To see why,
   consider an operator+ function to add two matrices together, where the
   left-hand matrix is known to be an rvlaue(that can hence have its storage
   reused to hold the sum of the matrics): Matrix // by-value return
    operator+(Matrix&& lhs, const Matrix& rhs)
    {
        lhs += rhs;
        return std::move(lhs); // move lhs into return value
    }

    By casting the lhs to rvalue in the return statement (via std::move), lhs
   will be-moved into the function's return value location. If the call to
   std::move were omitted, Matrix operator+(Matrix &&lhs, const Matrix& rhs){
        lhs += rhs;
        return lhs; // copy lhs into return value
    }

    Some programers take the information above and try to extend it to
   situations where it doesn't apply."If using std::move on an rvalue reference
   parameter being copied into a return value turns a copy construction into a
   move construction," they reason, "I can perform the same optimization on
   local variable that I'm returning." In other words, they figure that given a
   function returning a local variable by value, such as this.

    Widget makeWidget(){
        Widget w; // local variable
        ...
        return w; // "copy" w into return value.

    }

    they can "optimize" it by turning the  "copy" into a move:
    Widget makeWidget()
    {
        Widget w;
        ...
        return std::move(w); // move w into return value(don't do this!)
    }

    MY liberal use of quotation marks should tip you off that this line of
   reasoning is flawed. It's flawed, because the Standardization Committee is
   way ahead of such programers when it comes to this kind of optimization. It
   was recognized long ago that the "copying" version of makeWidget can avoid
   the need to copy the local variable w by constructing it in the memory
   allocated for the function's return value.This is known as the return value
   optimization(RVO), and it's been expressly blessed by C++ Standard for as
   long as there's been one.

    Wording such a blessing is finicky business, because you want to permit such
   copy elision only in places where it won't affect the observed behavior of
   the software. Paraphrasing the legalistic(arguably toxic) prose of the
   Standard, this particular blessing says that compilers may elide the copying
   of a local object in a function that returns by value if(1) the type of local
   object is the same as that returned by the function and (2) the local object
   is what's being returned. With that in mind, look again at the "copying"
   version of makeWidget:

    Widget makeWidget()
    {
        Widget w;
        ...
        return w;
    }

    Every decent C++ compiler will employ the RVO to avoid copying w.That means
   that the "copying" version of makeWidget doesn't, in fact, copy anything.

    But the RVO is an optimization. Compilers aren't requred to elide copy and
   move operations, even when they're permitted to.Maybe you're paranoid, and
   you worry that your compilers will punish you with copy operations, just
   because they can. Or perhaps you're insightful enough to recognize that there
   are cases where RVO is difficult for compilers to implement, e.g, when
   different control pahts in a function return different local variables.If so,
   you might be willing to pay the price of a move as  insurance against the
   cost of a copy. That is, you might still think it's reasonable to apply
   std::move to a local object you're returning, simply because you'd rest easy
   knowing you'd never pay for a copy.

*/
using namespace std;
class SomeDataStructure {
  SomeDataStructure(SomeDataStructure &&rhs) {
    cout << "Invoke from SomeDataStructure via rvalue reference \n";
  };
};

class Widget {
public:
  Widget(string &s) : name(s) {}
  Widget(const Widget &other) : name(other.name), p(other.p) {
    std::cout << "Copy constructor \n";
  }

  Widget(Widget &&rhs) : name(std::move(rhs.name)), p(std::move(rhs.p)) {
    cout << "Invoke from Widget via rvalue reference \n";
  }
  string getName() { return name; }

private:
  string name;
  shared_ptr<SomeDataStructure> p;
};

Widget makeWidget() {
  string s("zzz");
  Widget w(s);
  cout << "std::move(w)'s type : " << type_name<decltype(std::move(w))>()
       << endl;
  return std::move(w);
}
int main() {
  string s("abc");
  Widget w(s);
  Widget w0{w};
  Widget w1(std::move(w));
  Widget w2 = makeWidget();
  cout << "w1.name : " << w2.getName() << endl;
  return 0;
}
#include "type_name.hpp"
#include <chrono>
#include <iostream>
// #include <typeinfo>

/*
    When you first learn about move semantics and perfect forwarding, it seems
   pretty straight-forward:

    Rvalues reference are the glue that ties these two rather disparate features
   together. They're underlying language mechanism that makes both move
   semantics and perfect forwarding possible.

    The more expensive you have with these features, the more you realize that
   your initial impression was based on only the metaphorical tip of the
   proverbial iceberg.The world of move semantic, perfect forwarding, and rvalue
   references is more nuanced than it appears. Std::move doesn't move anything,
   for example, and perfect forwarding is imperfect. Move operations aren't
   always cheaper than copying; when they are, they're not always as cheap as
   you'd expect; and they are not always called in a context where moving is
   valid.The construct "type &&" doesn't always represent an rvalue reference.

    In the Items in this chapter, it's especially important to bear in mind that
   a parameter is always an lvalue, even if its type is an rvalue reference.
    That is, given void f(Widget && w); the parameter w is an lvalue, even
   through its type is rvalue-reference-to-Widget.

*/

/*
    It's useful to approach std::move and std::forward in terms of what they
   don't do. std::move doesn't move anything.Std::forward doesn't forward
   anything.At runtime, neither does anything at all.They generate no executable
   code. Not a single byte.

    std::move and std::forward are merely functions(actually function templates)
   that perform casts. std::move unconditionally casts its argument to an
   rvalue, while std::forward performs this cast only if a particular condition
   is fulfilled. That's it. The explanation leads to a new set of questions,
   but, fundamentally, that's the complete story.

    Here's a sample implementation of std::move in C++11. It's not fully
   conforming to the details of the Standard, but it's very close.
    template<typename T>
    typename remove_reference<T>::type &&
    move(T&& param){
        using ReturnType = typename remove_reference<T>::type &&;
        return static_cast<ReturnType>(param);
    }

    std::move is a function, that takes a reference to an object(a universal
   reference, to be precise) and it returns a reference to the same object. The
   "&&" part of the function's return type implies that std::move returns an
   rvalue reference, but, as Item28 explains, if the type T happens to be a
   lvalue reference, T&& would become an lvalue reference. To prevent this from
   being happening, the type trait std::remove_reference is applied to T, thus
   ensuring that "&&" is applied to a type that isn't a reference. That
   guarantees that std::move truely returns an rvalue reference, and that's
   important, because rvalue references returned from functions are rvalues.
   Thus std::move casts its argument to an rvalue, and that's all it does.

    As an asdie, std::move can be implemented with less fuss in C++14.Thanks to
   function return type deduction and the Standard Library's alias template
   std::remove_reference_t, std::move can be written this way: template<typename
   T> decltype(auto) move(T&& param){ using ReturnType = remove_reference<T>&&;
        return static_cast<ReturnType>(param);
    }

    version 1.0:
    class Annotation{
        public:
            explicit Annotation(const std::string text)
            ...
    };
    To avoid paying for a copy operation when copying test into a data member,
   you remain true to the advice of Item 41 and apply std::move to text, thus
   producing an ralvue.

    class Annotation{
        public:
            explicit Annotation(const std::string text): value(std::move(text))
   { ... }
            ...
        private:
            std::string value;
    }
    The codes compiles and looks good. The only thing seperating this code from
   a perfect realization of your vision is that text is not moved into value,
   it's copied. Sure, text is a cast to an rvalue by std::move, but text is
   declared to be const std::string, so before the cast, text is an lvalue const
   std::string, and the result of the cast is an rvalue const std::string, but
   throughout it all, the constness remains.

    Consider the effect that has when compilers have to determine which
   std::string constructor to call. There are two possibilites: class string{
        public:
            string(const string& rhs); //copy ctor
            string(string && rhs); // move ctor
            ...
    };

    So, that rvalue can't be passed to std::string's move constructor, because
   the move constructor takes an rvalue reference to non-const std::string. The
   rvalue can, however, be passed to the copy constructor, because an
   lvalue-reference-to-const is permitted to bind to a const rvalue. Such
   behavior is essential to maintaining the const-correctness.Moving a value out
   of an object generally modifies the object, so the language should not permit
   const objects to be passed to functions that could modify them.

    There are two lessons to be drawn from this example. First don't declare
   objects const if you want to be able to move from them.Move requests on const
    objects are silently transformed into copy operations. Second, std::move not
   only doesn't actually move anything, it doesn't even guarantee that the
   object it's casting will be eligible to be moved.The only thing you know for
   sure about the result of applying std::move to an object is that it's an
   rvalue. The story for std::forward is similar to that for std::move, but
   whereas std::move unconditionally cast its argument to an ravlue, std::move
   doest it only under certain conditions. std::forward is a conditional cast.
   To understand when it casts and when it doesn't, recall how std::forward is
   typically used.

    std::move attractions are convenience, reduced the likelihood of error, and
   greater clarity. Consider a class where we want to track how many times the
   move construtor is called. A static counter that's incremented during move
   construction is all we need. Assuming the only non-static data in the data is
   std::string, here's the conventional way(i.e, using std::move) to implement
   the move constructor: class Widget{ public: Widget(Widget && rhs):
   s(std::move(rhs.s))
            {++moveCtorCalls;}
            ...
        private:
            static std::size_t moveCtorCalls;
            std::string s;
    }

    To implement the same behavior with std::forward, the code would like this:
    class Widget{
        public:
            Widget(Widget && rhs):s(std::forward<std::string>(rhs.s)) //
   unconventional && undesirable implementation.
            {++moveCtorCalls;}
            ...


    }
    By comparision, the std::move requires less typing than std::forward, and it
   spares us the trouble of passing a type argument that encodes that the
   arguments we're passing is a rvalue. It also eliminates the possibility of
   our passing an incorrect type. More importantly, the use of std::move conveys
   an unconditional cast to an rvalue, while the use of std::forward indicates a
   cast to an rvalue only for references to which rvalues have been bound.
*/
/* user defined move typename */
template <typename T>
typename std::remove_reference<T>::type &&move1(T &&param) {
  std::cout << "T's type : " << type_name<T>() << std::endl;
  using ReturnType = typename std::remove_reference<T>::type &&;
  return static_cast<ReturnType>(param);
}

/* user defined move auto */
template <typename T> decltype(auto) move(T &&param) {
  std::cout << "T's type : " << type_name<T>() << std::endl;
  using ReturnType = std::remove_reference_t<T> &&;
  return static_cast<ReturnType>(param);
}

// /* Test on move action of const rvalue */
// class String1{
// public:
//     String1
// private:

// };

class Widget {
public:
  Widget() = default;
  Widget(const Widget &w) { std::cout << "Copy constructor is called \n"; }
  Widget(Widget &&other) { std::cout << "Move constructor is called \n"; }
};
void process(const Widget &lvalArg) {
  std::cout << "process via lvalue reference \n";
}
void process(Widget &&rvalArg) {
  std::cout << "process via rvalue reference \n";
}

template <typename T> void logAndProcess(T &&param) {
  std::cout << "param's type :" << type_name<decltype(param)>() << std::endl;
  std::cout << "T's type : " << type_name<T>() << std::endl;
  auto now = std::chrono::system_clock::now();
  std::cout << "now's type : " << type_name<decltype(now)>() << std::endl;
  // std::cout <<  "Now : " << now;
  process(std::forward<T>(param));
  process(param);
  process(static_cast<std::remove_reference_t<decltype(param)>>(param));
}

int main() {
  /* Test on rvalue and lvalue */
  Widget w;
  logAndProcess(w);
  logAndProcess(std::move(w));

  /* Test rvalue reference type */
  int &&i = 3;
  auto _i = move(i);

  /* Test lvalue reference type */
  int &j = i;
  auto _j = move(j);

  /* Test rvalue */
  auto t = move(1);

  /* Test rvalue reference type */
  // int&& i = 3;
  auto x = move1(i);

  /* Test lvalue reference type */
  auto y = move1(j);

  /* Test rvalue */
  auto z = move1(1);
  std::cout << "type of t : " << type_name<decltype(t)>() << std::endl;
}
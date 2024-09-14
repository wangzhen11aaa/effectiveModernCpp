#include "type_name.hpp"
#include <iostream>
#include <set>
#include <string>

using namespace std;

/*
    Suppose you need to write a function that takes a name as a parameter, logs
   the current date and time, then adds the name to a global data structure.
    std::multiset<std::string> names; // global data structure
    void lodAndAdd(const std::string& name){
        auto now = std::chrono::system_clock::now(); // get current time
        log(now, "logAndAdd"); //get log entry
        names.emplace(name); // add name to global data structure; see Item42
   for info on emplace.
    }

    This isn't unreasonable code, but it's not as efficient as it could
   be.Consider three potential calls: std::string petName("Darla");
    logAndAdd(petName); // pass lvalue std::string

    logAndAdd(std::string("Persephone")); // pass rvalues std::string

    logAndAdd("Patty Dog"); pass string literal

    In the first call, logAndAdd's parameter name is bound to the variable
   petName. Within logAndAdd, name is ultimately passed to names.emplace.
   Because name is an lvalue, it is copied into names.There's no way to avoid
   that copy, because an lvalue(petName) was passed into logAndAdd.

    In the second call,the parameter name is bound to an rvalue(the temporary
   std::string explicitly created from "Persephone").name itself is an lvalue,
    so it's copied into names, but we recognize that, in principle, its value
   could be moved into names. In this call, we pay for a copy, but we should be
   able to get by with only a move.

    In the third call, the parameter name is bound to an rvalue, but this time
   it's to a temporay std::string that's implicitly created from "Patty Dog". As
   in the second call, name is copied into names, but in this case, the argument
   originally passed to logAndAdd was a string literal.Had that string literal
    been passed directly to emplace, there would have been no need to create a
   temporary std::string at all.Indeed, emplace would have used the string
   literal to create a temporary std::string at all.Instead, emplace would have
   used the string literal to create the std::string object directly inside the
   std::multiset. In this third call, then, we're paying to copy a std::string,
   yet there's really no reason to pay even for a move, much less a copy.

    We can eliminate the inefficiencies in the second and third calls by
   rewriting logAddLog to take universal reference and , in accord with Item25,
   std::forward- ing this reference to emplace. The results speak for
   themselves: template<typename T> void logAndAdd(T&& name){
        auto now = std::chrono::system_clock::now();
        log(now, "logAndAdd");
        names.emplace(std::forward<T>(name));
    }


    logAndAdd(petName); // as before, copy
    // lvalue into multiset
    logAndAdd(std::string("Persephone")); // move rvalue instead // of copying
   it logAndAdd("Patty Dog");
    // create std::string
    // in multiset instead // of copying a temporary // std::string

    Were this the end of the story, we could stop here and proudly retire,but I
   haven't told you that clients don't always have direct access to the names
   that logAndAdd requires. Some clients have only an index that logAndAdd uses
   to look up the corresponding name in a table. To support such clients,
   logAndAdd is overloaded: string string nameFromIndex(int idx); // return name
   corresponding to idx

    void logAndAdd(int idx)
    {
        auto now = std::chrono::system_clock::now();
        log(now, "logAndAdd");
        names.emplace(nameFromIndex(idx));
    }

    Actually, resolution works as expected only if you don't expect too much.
   Suppose you invoke logAndAdd(22), then it will call logAndAdd(int) overload.

    But,
    short nameIdx; //give nameIdx a value
    ...
    logAndAdd(nameIdx); // error.

    Explanation: There are two logAndAdd overloads. The one taking a universal
   reference can deduce T to to be short, thus yielding an exact match. The
   overload with an int parameter can match the short argument only with an int
   parameter can match the short only with a promotion. Per the normal overload
   resolution rules, an exact match beats a match with a promotion, so the
   universal reference overload is invoked.

    Within that overload, the parameter name is bound to the short that's passed
   in, name is then std::forward to emplace member function on names(a
   multiset<std::string>), which, in turn, dutifully forwards it to the
    std::string constructor. THere is no constructor for std::string that takes
   a short, so the std::string constructor call inside the call to
   multiset::emplace inside the call to logAndAdd fails. All because the
    universal reference overload was a better match for a short argument than an
   int.

    Functions taking universal references are the greediest functions in C++.
   THey instantiate to create exact matches for almost any type of argument.
   This is why combining overloading and universal references is almost always a
   bad idea: the universal reference overload vacuums up far more argument types
   than the developer doing the overloading generally expects.

    An easy way to topple into this pit is to write a perfect forwarding
   constructor. A small modification to the logAndAdd example demonstrates the
   problem. Instead of writing a free function that can either a std::string or
   an index that can be used to look up a std::string, imagine a class Person
   with constructions that do the same thing: class Person{ public:
            template<typename T>
            explicit Person(T&& n)
            : name(std::forward<T>(n)) {} // perfect forwarding ctor;
   initializes data member.

            explicit Person(int idx) // int ctor
            :name (nameFromIdx(idx)) {}
            ...
        private:
            std::string name;
    };

    As was the case with logAndAdd, passing an integral type other than int(e.g,
   std::size_t, short, long, etc) will call universal reference constructor
   overload instead of the int overload, and that will lead to compilation
   failures. The problem here is much worse, however, because there's more
   overloading present in Person than meets the eye. Item17 explains that under
   the appropriate conditions, C++ will generate both the copy and move
   constructors, and this is true even if the class contains a templatized
   constructor that could be instantiated to produce the signature of the copy
   or move constructor. If the copy and move constructors for Person are thus
   generated, Person will effectively look like this:

    class Person{
        public:
            template<typename T>
            explicit Person(T&& n)  //perfect forwarding ctor
            :name(std::forward<T>(n)) {}

            explict Person(int idx); // int ctor

            Person(const Person& rhs); // copy ctor (compiler-generated)
            Person(Person&& rhs); // move ctor(compiler-generated)
            ...
    };
    This leads to behavior that's intuitive only if you've spent so much time
   around compilers and compiler-wirters, you've forgotten what it's like to be
   human:

    Person p("Nancy")
    auto cloneP(p); //create new Person from p; this would compile

    Here we're trying to create a Person from another Person, which seems like
   about as obvious a case for copy construction as one can get.(p's an lvalue,
   so we can banish any thoughts we might have about the "copying" being
   accomplished through a move operation.)But this code will then try to
   initialize Person's std::string data member with a Person object
   (p).std::string having no constructor taking a Person, your compilers will
   throw up their hands in exasperation, possibly punishing you with long and
   incomprehensive error messages as an expression of their displeasure.

    "Why", you might wonder,"does the perfect-forwarding constructor get called
   instead of the copy constructor? We're initializing a Person with another
   Person!". Indeed we are, but compilers are sworn to uphold the rules of C++,
   and the rules of relevance here are the ones governing the resolution of
   calls to overloaded functions.

    Compilers reason as follows. cloneP is being initialized with a non-const
   lvalue(p),and that means that the templatized constructor can be instantizetd
   to take a non-const lvalue of type Person. After such instantiation, the
   Person class looks like this:

    class Person {
        public:
            explicit Person(Person& n) // instantiated from perfect-forwarding
   template :name(std::forward<Person&>(n)) {}

            explicit Person(int idx); // as before

            Person(const Person& rhs); // copy ctor(compiler-construction)
            ...
    };
    In the statement,
    auto cloneOfP(p);
    p could be passed to either the copy constructor or the instantiated
   template. Calling the copy constructor would require adding const to p to
   match the copy constructor's parameter's type，but calling the instantiated
    template requires no such addition. The overload generated from the template
   is thus a better match, so compilers do what they're designed to do: generate
   a call to the better-matching function. "Copying non-const lvalues" of type
   Person is thus handled by the perfect-forwarding constructor, not the copy
   constructor.ß

    If we change the example slightly so that the object to be copied is const,
   we hear an entirely different tune: const Person cp("Nancy"); // object is
   now const auto cloneP(cp); // call copy constructor!

    Because the obect to be copied is now const, it's an exact match for the
   parameter taken by the copy constructor. The templatized constructor can be
   instantiated to haveing tghe same signature.

    class Person{
        public:
            explicit Person(const Person& n); //instantiated from template

            Person(const Person& rhs); // copy ctor (compiler-generated)
            ...
     };
     But this doesn't matter, because one of the overload-resolution rules in
   C++ is that in situations where a template instantiation and non-template
   function(i.e, a 'normal' function) are equally good matches for a function
   call, the normal function is perfered. The copy constructor(a normal
   function) thereby trumps an instantiated template with the same signature.

    The interactio among perfect-forwarding constructors and compiler-generated
   copy and move-operations develops even more wrinkles when inheritance enters
   the picture. In particular, the conventional implementationis of derived
   class copy and move operations behave quite surprisingly.Here, take a look:

    class SpecialPerson: public Person{
        public:
            SpecialPerson(const SpecialPerson & rhs)//
            :Person(rhs)
            {...} // copy ctor; calls base class forwarding ctor!

            SpecialPerson(SpecialPerson&& rhs)
            :Person(std::move(rhs))
            {...} //move ctor; calls base class forwarding ctor!
    };
    As the comments indicates, the derived class copy and move constructors
   don't call their base class's copy and move constructors, they call the base
   class's perfect-forwarding constructor! To understand why, note that the
    derived class functions are using arguemnts of type SpecialPerson to pass to
   their base class, then work through the template instantiation and
   overloaded-resolution consequences for the constructors in class Person.
    Ultimatedly, the code won't compile, because there's no std::string
   constructor taking a SpecialPerson.

    I hope that by now I've convinced you that overloading on universal
   reference parameters is something you should avoid if at all possible. But if
   overloading on universal reference is a bad idea, what do you do if you need
   a function that forwards most argument type, yet needs to treat some argument
   types in special fashion? The egg can be unscrambled in a number of ways. So
   many, in fact, taht I've devoted an entire Item to them.
*/

std::multiset<std::string> names;

/* Overload shows up */
string nameFromIdx(int idx) { return string("hello_world"); }

void logAndAdd1(int idx) {
  cout << "Invoke logAndAdd1 via int idx \n";
  names.emplace(nameFromIdx(idx));
}

/* Only support const lvalue in first version */
void logAndAdd(const std::string &name) {

  cout << "Invoke logAndAdd via lvalue reference \n";
  // << "T's type : " << type_name<T>() << endl;
  cout << "name's type : " << type_name<decltype(name)>() << endl;
  names.emplace(name);
}

/* Rewrite the function for efficiency */
template <typename T> void logAndAdd1(T &&name) {
  cout << "Invoke logAndAdd via rvalue reference \n";
  cout << "T's type : " << type_name<T>() << endl;
  cout << "name's type : " << type_name<decltype(name)>() << endl;
  names.emplace(forward<T>(name));
}

/* class Person for these conditions */
// class Person {
// public:
//   /* Perfect forwarding ctor */
//   template <typename T> explicit Person(T &&n) : name(forward(n)) {
//     std::cout << "perfect forwarding is called \n";
//   }
//   /* int ctor */
//   explicit Person(int idx) : name(nameFromIdx(idx)) {}
//   string getName() { return name; }

// private:
//   string name;
// };

/* Derived class copy and move constructor do not call base class's
   copy and move constructors.
 */

// class SpecialPerson : public Person {
// public:
//   template <typename T> SpecialPerson(T &&__name) : Person(__name){};
//   /* The following line invoke the Person's universal-reference verson ctor */
//   SpecialPerson(const SpecialPerson &rhs) : Person(rhs) {}
//   SpecialPerson(SpecialPerson &&rhs) : Person(std::move(rhs)) {}
// };

int main() {
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
  // logAndAdd1(nameIdex);

//   Person p(2);
//   cout << "p(2)'s name : " << p.getName() << endl;
//   Person p1("XiaoDi");
//   cout << "XiaoDi's name : " << p1.getName() << endl;

  /* short type shows up again */
  // Person p2(nameIdex);
  // cout << "p2's name: "<< p2.getName() << endl;
  /* Invoke copy constructor */
//   const auto cp = p;
//   Person p3(cp);

  /* Special person */
  // SpecialPerson sp("abed");
  // cout << "SpecialPerson's name : " << sp.getName() << endl;
  return 0;
}
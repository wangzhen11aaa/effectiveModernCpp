#include <iostream>
#include <vector>
#include "type_name.hpp"

using namespace std;

/*
    If you've ever had to combat excessive build times, you're familiar with Pimpl("Pointer to implementation") Idiom.
    That's the technique whereby you replace the data members of a class with a pointer to an implementation class (or struct), put
    the data members that used to be in the primary class into the implementation class, and access those data members indirectly through
    the pointer. For example, suppose Widget looks like this:

        class B{};

        class Widget{
        public:
            Widget();
            ~Widget();
        private:
            string name;
            vector<double> data;
            B b;
    Because Widget's data members are of types std::string, std::vector, and Gadget, headers for those types must be present for Widget to compile, and that 
    means that Widget clients must #include<string>, <vector> and gadget.h. Those headers increase compilation time for Widget clients, plus they make those
    clients dependent on those contents of the headers. If a header's content changes, Widget clients must recompile.The standard headers <string> and <vector>
    don't change very often, but it could be that gadget'h is subject to frequent revision.

    Applying the Pimpl Idiom in C++98 could have Widget replace its data members with a raw pointer to a struct that has been declared, but not defined:

    class Widget1{ // in header. file "widget.h"
    public:
        Widget1();

    private:
        struct Impl; // so-called incomplete type, from here (header file), we only see the declaration, but not defined here. We will implete it in cpp file.
        std::unique_ptr<Impl> pImpl;

    };

    Because Widget no longer mentions the types std::string, std::vector, and Gadget, Widget clients no longer need to #include the headers for these types.
    That speeds compilation, and it also means that if something in these headers changes, Widget clients are unaffected.
    A type that has been declared, but not defined, is known as an incomplete type. Widget::Impl is such a type. There are very few things you can do with an
    incomplete type, but declaring a pointer to it is one of them.

    Part 1 of the Pimpl Idiom is the declaration of a data member that's a pointer to an incomplete type. Part 2 is the dynamic allocation and deallocation of the 
    object that holds the data members that used to be in original class. The allocation and deallocation code goes in the implementation file. e.g for Widget, in widget.cpp:
    #include "widget.h" // in impl. file "widget.cpp"
    
    #include "gadget.h" // This header may be subject to frequent revision.
    #include <string>
    #include <vector>

    struct Widget::Impl{ //definition of Widget::Impl with data members formerly in Widget (declaration in widget.h)
        std::string name;
        std::vector<double> data;
        Gadget g1, g2, g3;
    };

    Widget::Widget(): pImpl(new Impl) {}; // allocate data members for this Widget object
    Widget::~Widget() {delete pImpl;} // destory data members for this object.

    But I've shown you C++98 code, and that reeks of a bygone millennium.It uses raw pointers and raw new and raw delete and it's all
    just so ... raw.This chapter is built on the idea that smart pointers are preferable to raw pointers, and if what we want is to 
    dynamically allocate a Widget::Impl object inside the Widget constructor and have it destroyed at the same time the Widget is, std::unique_ptr is 
    precisely the tool we need. Replacing the raw pImpl pointer with a std::unique_ptr yields this code for the heasder file,

    class Widget{
        public:
            Widget();
            ...
        private:
            struct Impl;
            std::unique_ptr<Impl> pImpl; // use smart pointer instead of raw pointer.
    }

    struct Widget::Impl{ //definition of Widget::Impl with data members formerly in Widget (declaration in widget.h)
        std::string name;
        std::vector<double> data;
        Gadget g1, g2, g3;
    };    
    Widget::Widget()
    : pImpl(std::make_unique<Impl>()){} // use the general make series function to create a smart pointer.
    
    We think we can ignore the destructor for smart pointer can do the resource management for us.
    ----------------------------------------------------------------------------------------------------------
    You'll note that the Widget destructor is no longer present. That's because we have no code to put into it.
    std::unique_ptr automatically deletes what it points to when it(std::unique_ptr) is destroyed, so we need not delete
    anything ourselves.That's one of the attractions of smart pointers: they eliminate the need for us to sully our
    hands with manual resource releases.

    Something interesting. You can not destroy incomplete type.
    Like this, some user use this
    #include "widget.h"
    Widget w; // error.

    The issue arises due to the code that's generated when w is destoyed(e.g goes out of scope).At that point, its destructor is called.In the class definition using
    std::unique_ptr, we didn't declare a destructor, because we didn't have any code to put into it. In accord with the usual rules for compiler-generated special member
    functions, the compiler generates destructor for us. Within that desctructor, the compiler inserts code to call the destructor for Widget's data member pImpl. pImpl is a
    std::unique_ptr<Widget::Impl>, i.e , a std::unique_ptr using the default deleter. The default deleter is a function that uses delete on the raw pointer inside the std::unique_ptr.
    Prior to using delete, however, implementations typically have the default deleter employ C++11's static_assert to ensure that the raw pointer doesn't point to a incomplete type.
    When the compiler generates code for the destruction of the Widget w, then, it generally encounters a static_assert that fails, and that’s usually what leads to the error message.

    SO.
    class Widget{
        public:
            Widget();
            ~Widget();
            ...
        private:
            struct Impl;
            std::unique_ptr<Impl> pImpl; // use smart pointer instead of raw pointer.
    }

    struct Widget::Impl{ //definition of Widget::Impl with data members formerly in Widget (declaration in widget.h)
        std::string name;
        std::vector<double> data;
        Gadget g1, g2, g3;
    };    
    Widget::Widget()
    : pImpl(std::make_unique<Impl>()){} // use the general make series function to create a smart pointer.
    Widget::~Widget() = default; // Tell the reader that this destructor will be generated by the compiler.

    By now, the code is ok now.

    Still, it is interesting to note that if we were to use std::shared_ptr instead of std::unique_ptr for pImpl, we'd find that 
    the advice of this item no longer applied. There'd be no need to declare a destructor in Widget, and without a user-declared
    destructor, compilers would happily generate the move operations, which would do exactly what we'd want them do.

    That is, given this code in widget.h,

    class Widget{
        public:
            Widget();
            .. // no declarations for dtor or move operations.
            private:
                struct Impl; 
                std::shared_ptr<Impl> pImpl; // std::shared_ptr instead of std::unique_ptr.

    }; 
    
    and this client code that #include widget.h,
    Widget w1;
    auto w2(std::move(w1)); move-construct w2;
    w1 = std::move(w1); move-assign w1
    
    everything would compile and run as we'd hope: w1 would be default constructed, its value would be moved into w2, that value 
    would be moved back into w1, and then both w1 and w2 would be destroyed(thus causing the pointer-to Widget::Impl object to be destroyed.)
    
    The difference in behavior between std::unique_ptr and std::shared_ptr for pImpl pointers stems from the differing ways these smart pointers
    support custom deleters.For std::unique_ptr,the type of the deleter is part of type of smart pointer, and this makes it possible for compilers to 
    generate smaller runtime data structers and faster runtime code.A consequence of this greater efficiency is taht pointed-to types must be complete when 
    compiler-generated special functions(e.g destructors and move operations) are used. For std::shared_ptr, the type of the deleter is not part of the type 
    of the smart poiter. This necessitates larger runtime data structures and somewhat slower code, but pointed-to types need not be complete when compiler-
    generated special functions are employed. 
    For the Pimpl Idiom, there's not really a trade-off between the characteristics of std::unique_ptr and std::shared_ptr, because the relationship between 
    classes like Widget and classes like Widget::Impl is exculsive ownership, and that makes std::unique_ptr the proper tool for the job. Nervertheless, it's
    worth knowing that in other situations-situations where shared ownership exists(and std::shared_ptr is hence a fitting design choice), there is no
    need to jump through fue function-definition hoops that use of std::unique_ptr entails.
    
};

*/
/* Bad designment, 
  This structure will be effected 
  by the change of B. If B is in another header,
  when B is changed, then this file will be recompiled. 
 */
class B{};

class Widget{
public:
    Widget();
    ~Widget();
private:
    string name;
    vector<double> data;
    B b;
};

/* Beautiful Version
   This class is in header.
 */
class Widget1{
    struct Impl{
		int a;
		double b;
	};

public:
	Widget1();
	void print() {
		cout << "pImpl->a : " << pImpl->a << "\npImpl->b: " << pImpl->b << endl;
	}

private:
    std::unique_ptr<Widget1::Impl> pImpl;

};
/* Implement in .cpp */
/* Additional work, which does not occur in mac environment.

 */
Widget1::Widget1():pImpl(std::make_unique<Widget1::Impl>()){}


int main(){
    Widget1 w;
	w.print();
	return 0;
}
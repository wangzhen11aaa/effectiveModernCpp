#include <iostream>
#include "type_name.hpp"

using namespace std;

/* For std::array, the data is stored in the array object */
/* For small string, characters are stored in the string object */

/*
    Move semantics is arguably the premier feature of C++11. "Moving containers is now as cheap as copying pointers!"
    you're likely to hear, and "Copying temporary objects is now so efficient, coding to avoid it is tantamount to 
    premature optimization!"

    Let's begin with observation that many types fail to support move semantics. The entire C++ 98 Standard 
    Library was overhauled for C++11 to add move operations for types where moving could be implemented faster than 
    copying, and the implementation of the library components was revised to take advantage of these operations, but
    chances are that you're working with a code base that has not been completely revised to take advantage of C++11.
    For types in your applications where no modification for C++11 have been made, the existence of move support
    in your compilers is likely to do you little good. Ture, C++11 is willing to generate move operations for classes
    that lack them, but that happens only for classes declaring no copy operations, move operations or destructos.

    Even types with explicit move support may not benefit as much as you'd done. All containers in standard C++11 library
    support moving, for example, but it would be a mistake to assume that moving all containers is cheap. For some containers
    this is because there's no truly cheap way to move their contents. For others, it's because the truly cheap move 
    operations the containers offer come with caveats the container elements can't satisfy.

    std::array<Widge, 10000> aw1;

    auto aw2 = std::move(aw1);

    Note that the elements in aw1 are moved into aw2. Assuming that Widget is a type where moving is faster than copying, moving a std::array
    of Widget will be faster than copying the same std::array. So std::array certainly offers move support. Yet both
    moving and copying a std::array have linear-time computational comlexity, because each element in the container must 
    be copied or moved.This is far from the "moving a container is now as cheap as assigning a couple of pointers" claim
    that one sometimes hears.

    On the other hand, std::string offers constant-time moves and linear-time copies. That makes it sound like moving is faster than
    copying, but that may not be the case. Many string implementations employ the small string optimization(SSO).With the SSO, "small"
    strings (e.g, those with a capacity of no more than 15 characters) are stored in a buffer within the std::string object; no heap-
    allocated storage is used.Moving small strings using an SSO-based implementation is no faster than copying them, because the 
    copy-only-a-pointer trick that generally underlies the performance advantage of moves over copies isn't applicable.

    The motivation for the SSO is extensive evidence that short strings are the norm for many applications. Using an internal buffer
    to store the contents of such strings eliminates the need to dynamically allocate memory for them, and that's typically an 
    efficiency win. An implication of the win, however, is that moves are no faster than copies, though one could just as well
    take a glasss-half-full approach and say that for such strings, copying is no slower than moving.

    Even for types supporting speedy move operations, some seemingly sure-fire move situation can end up making copies. 
    Item 14 explains that some container operations in the Standard Library offer the strong exception safety guarantee 
    isn't broken when upgrading to C++11, the underlying copy operations may be replaced with move operations only
    if the move operations are known to not throw.A consequence is that even if a type offers move operations that 
    are more efficient than corresponding copy operations, and even if, at a particular point in the code, a move 
    operation would generally be appropriate(e.g, if the source object is an rvalue),compilers might still be forced to 
    invoke a copy operation because the corresponding move operation isn't declared nonexcept.

    There are thus several scenarios in which C++ 11's move semantics do you no good:
    1.No move semantics: The object to be moved from fails to offer move operations. The move request therefore becomes a 
    copy request.
    2.Move not faster: The object to be moved from has move operations that are no faster than the copy operators.
    3.Move not usable: The context in which the moving would take place requires a move operation that emits no exceptions, but
    that operation isn't declared noexcept.

    It's worth mentioning, too, another scenario whre move semantics offers no efficiency gain:
    1. Source object is lvalue: With very few exceptions only rvalues may be used as the source of a move operation.

    But the title of this Item is to assume that move operations are not present, not cheap, and not used.This is typically
    the case in generic code, e.g, when writing templates, because you don't know all the types you're working with.In such 
    circumstances, you must be as conservative about copying objects as you were in C++98- before move semantics existed. 
    This is also the case for "unstable" code, i.e, code where the characteristics of the type being used subject to relatively
    frequent modification.

    Often, however, you know the types your code uses, and you can rely on their characteristics not changing(e.g, whether they support
    inexpensive move operations).When that's the case, you don't need to make assumptions. You can simply look up the move support 
    details for the types you're using. If those types offer cheap move operations will be invoked, you can safely rely on 
    move semantics to replace copy operations with their less expensive move counterparts.
    

*/

int main(){

}
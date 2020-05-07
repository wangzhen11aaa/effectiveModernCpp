#include <iostream>
#include "type_name.hpp"

using namespace std;

// In C++11, we can only have one return statement for a constexpr function.
// So we need to write the function like pow(n, e) = n^e
// only recursive version for pow.
constexpr int pow(int base, int exp) noexcept{
    return (exp == 0 ? 1: base * pow(base, exp-1));
}

// C++14 standard, we can have more statements in constexpr function,
// and we can write loop.

constexpr int pow(int base, int exp) noexcept{
    auto result = 1;
    for(int i = 0; i < exp; i++) result *= base;
    return result;
}

// Constexpr functions are limited to taking and returning literal types, which essentially means types that can have 
// values determined during compilation. Literal types can have user-defined types like class etc.

class Point{
public:
    constexpr Point(double xVal = 0, double yVal = 0) noexcept :x(xVal), y(yVal){}; 
    constexpr double xValue() const noexcept {return x;}
    constexpr double yValue() const noexcept {return y;}
    /* need C++14 or above version */
    /* For C++11, we need the function (wants to be constexpr) should const itself, which means that 
    constexpr ... setX(double newX) const nonexcept {x = newX} constradicts the const qualifier in the function
    it will set the const object private member x.
     Second, the return type is void.
    */
    constexpr void setX(double newX) noexcept {x = newX;}
    constexpr void setY(double newY) noexcept {y = newY;}
  
private:
    double x, y;
};

// Which means that the object mid, though its initialization involves calls to constructors, getters, and non-member function, can be
// created in read-only memory. The more codes migrate from runtime to compile-time, the faster the code may run. (Compilation may take 
// longer, however.)
constexpr Point middlePoint(const Point& lhs, const Point& rhs) noexcept {
   return {(lhs.xValue()+rhs.xValue())/2,(lhs.yValue() + rhs.yValue())/2};
}

/* refecltion function */
constexpr Point reflection(const Point& p) noexcept{
    Point result; // create non-const Point
    result.setX(p.xValue()); // set its x and y values.
    result.setY(p.yValue()); 
    return result; // return copy of it.
}


int main(){
    constexpr Point p(1.0, 2.0);
    constexpr double x = 2.0, y = 3.0;
    constexpr Point _p1(3.0, 4.0);
    Point p1(x, y);
    cout << "p1's x : " << p1.xValue() << "p1's y : " << p1.yValue() << endl;
    /* constexpr Point expression */
    constexpr Point pc(1.0, 2.0);
    /* Not right */
    /* rvalue reference  */
    auto&& p2 = middlePoint(p, p1);
    cout << "p's type : " << type_name<decltype(p2)>() << endl;
    //constexpr Point pr(x, y);
    auto mid = middlePoint(p, p1);
    constexpr auto mid1 = middlePoint(p, _p1);

    /* Reflection of type Point */
    constexpr auto mid_ref = reflection(mid1); /* mid_ref's values are known during compilation */

    cout << "mid's type : " << type_name<decltype(mid)>() << endl;
    cout << "mid's x : " << middlePoint(p, p1).xValue() << "mid's y : " << middlePoint(p, p1).yValue() << endl;
    //cout << "mid_ref's type :" << type_name<decltype(mid_ref)>() << endl;
    mid.setX(2.0);
    mid.setY(3.0);
    /* set _p1 values */
    // _p1.setX(2.0);
    // _p1.setY(2.0);
    return 0;
}
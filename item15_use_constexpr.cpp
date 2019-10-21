#include <iostream>
#include "type_name.hpp"

using namespace std;

class Point{
public:
    constexpr Point(double xVal = 0, double yVal = 0) noexcept :x(xVal), y(yVal){}; 
    constexpr double xValue() const noexcept {return x;}
    constexpr double yValue() const noexcept {return y;}
    /* need C++14 or above version */
    constexpr void setX(double newX) noexcept {x = newX;}
    constexpr void setY(double newY) noexcept {y = newY;}
  
private:
    double x, y;
};

constexpr Point middlePoint(const Point& lhs, const Point& rhs) noexcept {
   return {(lhs.xValue()+rhs.xValue())/2,(lhs.yValue() + rhs.yValue())/2};
}

/* refecltion function */
constexpr Point reflection(const Point& p) noexcept{
    Point result;
    result.setX(p.xValue());
    result.setY(p.yValue());
    return result;
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
    return 0;
}
#include <iostream>
#include <vector>
#include "type_name.hpp"

using namespace std;

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
public:
    Widget1();
    /* Explicit use destructor */
    ~Widget1() = default; 
    /* Support move action */
    Widget1(Widget1&& rhs) = default;
    Widget1& operator=(Widget1&& rhs) = default;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

};
/* Implement in .cpp */
/* Additional work, which does not occur in mac environment.

 */

struct Widget1::Impl{
    int a;
    double b;
    vector<int> v;
};

Widget1::Widget1():pImpl(std::make_unique<Impl>()){}


int main(){
    Widget1 w;
    return 0;
}
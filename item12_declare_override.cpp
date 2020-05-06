#include <iostream>
#include <vector>
#include <memory>

using namespace std;
using dataType = vector<int>; 

class Widget{
public:
    Widget(dataType& _data):data(_data){}
    dataType& get() &{
        cout << "return reference version" << endl;
        return data;
    }
    dataType get() &&{
        cout << "invoke move version" << endl;
        return std::move(data);
    }
private:
    dataType data;
};

class Base{
    public:
        virtual void mf1() const & { cout << "Invoke Base's mf1() function" << endl;}
        virtual void mf1() const && { cout << "Rvalue Invoke Base's mf1() function" << endl;}

};
// better with the override keyword.
class Derived0 : public Base{
    public:
        virtual void mf1() const & override {cout << "Invoke Derived0's mf1() function" << endl;}
        virtual void mf1() const && override { cout << "Rvalue Invoke Base's mf1() function" << endl;}
};

class Derived1: public Derived0{
    public:
        virtual void mf1()  const & override {cout << "Invoke Derived1's mf1() function" << endl;}
        virtual void mf1() const && override { cout << "Rvalue Invoke Base's mf1() function" << endl;}
};

Widget makeWidget(){
    dataType d{1,2};
    Widget w(d);
    return w;
}
int main(){
    dataType v{1,2,3};
    Widget w(v);
    auto t0 = w.get();
    auto t = std::move((*std::make_unique<Widget>(v))).get();
    for(auto _t :t){
        cout <<_t << endl; 
    }
    // use factory;
    auto _t = makeWidget().get();

    // Test the grandpa->father-son derivation.
    Derived1 d1;
    Base & b1 = d1;
    cout << "Invoke mf1 from Base reference to d1" << endl;
    b1.mf1();
    Derived0 &d0 = d1;
    cout << "Invoke mf1 from Derived0 reference to d1" << endl;
    d0.mf1();

    // I did not figure out why this rvalue can invoke
    // lvalue function type.
    Derived1().mf1();
    Derived0().mf1();
    Base().mf1();
    return 0;
}
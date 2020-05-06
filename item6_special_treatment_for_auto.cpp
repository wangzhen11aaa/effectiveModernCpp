#include <iostream>
#include <vector>
#include "type_name.hpp"

using namespace std;


class Widget{

};

// class Widget that return the vector<bool> in
// the widget.
class Widget_{
    public:
        Widget_():v({true, false}){};
        vector<bool> v;
};

// the _features_ function return the variable from 
// class object v;
std::vector<bool> _features_(const Widget_& w){
    return w.v;
}

std::vector<bool> features(const Widget& w){
    vector<bool> v{true, false};
    return v;
}

auto _features(const Widget& w){
    vector<bool> v{true, false};
    auto r = v[0];
    cout << "r's type : " << type_name<decltype(r)>() << endl;
    return r;
}

void processWidget(const Widget& w, bool& t){
    cout << "In processWidget : "<<  t << endl;
}

template<typename T>
void _processWidget(const Widget, T& t){
    cout << "type of t in _processWidget : " << type_name<decltype(t)>() << endl;
    cout << "In _processWidget : " << t << endl;
}

int main(){
    vector<bool> v_b(5, true);
    auto v = v_b[0];
    cout << "v's type : " << type_name<decltype(v)>() << endl;
    cout << "Before assigning v_b[0] to false, v_b[0]'s value :" << v_b[0] << endl;
    // Output the v's value.
    // We want to reassign the value
    v = false;
    cout << "v_b[0]'s new value :" << v_b[0] << endl;
    vector<int> v_i(5, 1);
    auto _v_i = v_i[0];
    cout << "_v_i's type : " << type_name<decltype(_v_i)>() << endl;

    Widget w;
    bool b = features(w)[1];
    processWidget(w, b);
    cout << "Use auto ";
    void* ptr;
    
    auto _b = features(w)[1];
    cout << _b << endl;
    cout << "_b's type : " << type_name<decltype(_b)>() << endl;
        //ptr = &(_b);

    //Reassign the bool variable b
    _b = true;
    _processWidget(w, _b);
    // Force a different type deduction. That way you do that is what
    // I call the explicitly typed initializer idiom.
    auto __b = static_cast<bool>(features(w)[0]);
    cout << "type of __b : " << type_name<decltype(__b)>() << endl;

    auto _c  = _features(w);
    cout << "features(w)'s type : " << type_name<decltype(_features(w))>() << endl; 
    cout << "_c : " << _c << endl;
    //cout << "*ptr : " << (char *)ptr;

    // Widget_ class
    Widget_ w_;
    auto x = _features_(w_)[0];
    cout << "w_.v[0]'s value : " << x << endl;
    // Reassign the variable pointed to a temp variable.
    // Which i mean that the 
    x = false;
    cout << "After assignment, the w_.v[0] :" << _features_(w_)[0] << endl;
    cout << "x's value : " << x << endl;
    x = true;
    cout << "x's value : " << x << endl;

    return 0;   
}
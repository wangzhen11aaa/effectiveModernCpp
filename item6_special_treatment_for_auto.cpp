#include <iostream>
#include <vector>
#include "type_name.hpp"

using namespace std;


class Widget{

};

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
    cout << "type of t in _processWidget : " << type_name<decltype(t)>();
    cout << "In _processWidget : " << t << endl;
}

int main(){
    vector<bool> v_b(5, true);
    auto v = v_b[0];
    cout << "v's type : " << type_name<decltype(v)>() << endl;
    
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


    _processWidget(w, _b);
    auto __b = static_cast<bool>(features(w)[0]);
    cout << "type of __b : " << type_name<decltype(__b)>() << endl;

    auto _c  = _features(w);
    cout << "features(w)'s type : " << type_name<decltype(_features(w))>() << endl; 
    cout << "_c : " << _c << endl;
    //cout << "*ptr : " << (char *)ptr;
    return 0;   
}
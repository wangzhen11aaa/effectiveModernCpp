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
    return 0;
}
#include <iostream>
#include "type_name.hpp"
#include <functional>
using namespace std;

/* C++14 supports the method to move object to closure */

class Widget{
public:
    bool isValidated() const;
    bool isProcessed() const;
    bool isArchived() const;
private:

};
bool Widget::isArchived() const{
    return false;
}
bool Widget::isProcessed() const{
    return false;
}
bool Widget::isValidated() const{
    return true;
}

/* Writen in c++11 */
class IsValAndArch{
public:
    using DataType = std::unique_ptr<Widget>;
    explicit IsValAndArch(DataType&& ptr):
    pw(std::move(ptr)){}

    bool operator()() const{
        cout << "Invoke operator() ";
        return pw->isArchived() && pw->isProcessed();
    }

private:
    DataType pw;
};

int main(){
    auto pw = make_unique<Widget>();
    auto func = [pw = std::move(pw)]{
        cout << "pw's type :" << type_name<decltype(pw)>() << endl;
        return pw->isArchived() && pw->isProcessed();
    };
    cout << boolalpha << "func: "<< func() << endl;
    /* More concise */
    auto func1 = [pw = make_unique<Widget>()]{
        return pw->isArchived() && pw->isValidated();
    };
    cout << "func1: " << func1() << endl;
    /* Test on c++11 version */
    auto func2 = IsValAndArch(std::make_unique<Widget>());
    //func2();
    cout << "func2: " << func2() << endl;
    
    /* C++11 emulation */
    auto func3 = std::bind(
        [](const std::unique_ptr<Widget>& pw){
            return pw->isArchived() && pw->isProcessed(); },
            std::make_unique<Widget>()
    );
    
    cout << "func3: " << func3() << endl;
    return 0;
}
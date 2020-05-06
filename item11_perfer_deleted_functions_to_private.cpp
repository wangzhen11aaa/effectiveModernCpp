#include <iostream>
#include "type_name.hpp"
using namespace std;

bool isLucky(int number){
    cout << "isLucky(int) : " << number;
    return false;
}

// Disable all other kinds fo argumet.
template <typename T>
bool isLucky(T t) = delete;
//bool isLucky(char b) = delete;

// template specialization should be written at the namespace scope.
class Widget{
  public:
    template<typename T>
    void processPointer(T* ptr){

    }
    private:
    // In C++98, you want this kind of function should not work.
    // But you will get the error :explicit specialization is not allowed in
    // the current scope. And quotes from  the Modern Effective C++ : 
    // the problem is that template specializations must be written at namespace scope,
    // not class scope. Which means that you can not use this to avoid invoking
    // void processPointer<void>(void *) {}. Because that, template specialization
    // only occurs outside of class, which means it can not define in the private part.

   // template<> void processPointer<void>(void *){}
};

// The delete keyword does not be affected. like this
template<>
void Widget::processPointer<void>(void*) = delete;

int main(){
    isLucky(1);
    //isLucky('1');
  //  isLucky('1');
    return 0;
}
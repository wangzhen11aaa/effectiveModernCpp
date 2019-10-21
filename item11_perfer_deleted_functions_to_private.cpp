#include <iostream>
#include "type_name.hpp"
using namespace std;

bool isLucky(int number){
    cout << "isLucky(int) : " << number;
}
template <typename T>
bool isLucky(T t) = delete;
//bool isLucky(char b) = delete;

int main(){
    isLucky(1);
    isLucky('1');
  //  isLucky('1');
    return 0;
}
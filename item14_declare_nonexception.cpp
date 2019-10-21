#include <iostream>
#include "type_name.hpp"

using namespace std;

void f() noexcept
{
    try{
    throw "abc";
    }catch(const char *e){
        cerr << e << endl;
        throw "abc";
    }
}
int main(){
    try{
        f();
    }catch (const char * e){
        cerr << e << endl;
    } 
    cout << "hello, world" << endl;
    return 0;
}
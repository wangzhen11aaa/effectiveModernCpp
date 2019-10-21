#include <iostream>
#include <memory>
#include "type_name.hpp"

using namespace std;
class Investment{
public:
    Investment(int _m):m(_m){};
    virtual ~Investment(){};
private:
    int m;
};
class Stock: public Investment {
public:
    Stock(int _m=0):Investment(_m){};
    //~Stock(){};
};
class Bond: public Investment {
public:
    Bond(int _m=0):Investment(_m){};
    //~Bond(){};
};
class RealEstate: public Investment{
public:
    RealEstate(int _m=0): Investment(_m){};
    //~RealEstate(){};
};

/* template return unique_ptr pointer */
template<typename... Ts>
std::unique_ptr<Investment> makeInvestment(Ts&&... parameters){
    cout << "Use simple makeInvestment \n";
    return make_unique<Stock>(std::forward<Ts>(parameters)...);
}

auto delInvmt = [](Investment* pInvestment){
    delete pInvestment;
};

/* Textbook example */

/* self defined delete function. */
template<typename... Ts>
/* Maybe can use auto */
//std::unique_ptr<Investment, decltype(delInvmt)> makeInvestment(int type_code, Ts&&... params){
    auto makeInvestment(int type_code, Ts&&... params){
    cout << "Use complex makeInvestment \n";
    std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);
    // Use type code to determine which investment.
    if (type_code == 1){
        pInv.reset(new Stock(std::forward<Ts>(params)...));
    }else
    {
        pInv.reset(new Bond(std::forward<Ts>(params)...));
    }
    return pInv;
}

/* Example add all variables together */
template<typename T>
T adder(T v){
    return v;
}
template<typename T, typename... Targs>
T adder(T first, Targs... Fargs){
    return first + adder(Fargs...);
}

int main(){
    Stock* s = new Stock(10);
    Investment* i= s;
    long sum = adder(1,2,3,4);
    cout << "Sum of 1,2,3,4 : " << sum << endl;
    auto pInvestment = makeInvestment(2, 3);
    auto pInvestment1 = makeInvestment(1);
    auto pInvestment2 = makeInvestment();
    auto pInvestment3 = makeInvestment(2.0);
    cout << "Type of pInvestment : " << type_name<decltype(pInvestment)>() << endl;
    return 0;
}

#include <iostream>
#include <vector>

#include "type_name.hpp"

using namespace std;


class Polynomial{
public:
    using RootsType = std::vector<double>;
    /* Two threads writeing and reading data in Polynomial object will be unpredictable. */
    RootsType roots() const{
        if(!rootsAreValid){
            //...
            rootsAreValid = true;
        }
    }
private:
    mutable bool rootsAreValid{false};
    mutable RootsType rootVals {};
};

/* Add mutex to synchronize threads */
class PolynomialMutex{
public:
    using RootsType = std::vector<double>;
    RootsType roots() const{
        std::lock_guard<std::mutex> g(m);
        if(!rootsAreValid){
            ++callCount;
            // ...
            rootsAreValid = true;
        }
    }

private:
    mutable std::mutex m;
    mutable bool rootsAreValid{false};
    mutable RootsType rootVals{};
    /* atomic type for preventing overkilling */
    mutable std::atomic<unsigned> callCount{0};
};

int main(){

}
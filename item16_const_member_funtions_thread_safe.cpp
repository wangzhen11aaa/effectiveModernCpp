#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include "type_name.hpp"

using namespace std;


/* We want to construct a function to compute the root(s) of a polynomial. i.e, values where polynomial evaluates to zero.
   For compute the roots of polynomial can be expensive, we certainly don't want to do it more than once. We'll thus cache
   the root(s) of polynomial if we have to compute them, and we'll implement roots to return the cache value.
*/
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
    //Add mutable qualifier, so we can modify these members in const roots member function.
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
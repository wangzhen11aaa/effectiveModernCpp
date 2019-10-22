#include <iostream>
#include "type_name.hpp"

using namespace std;

/* typedef for a point in time  */
using Time = std::chrono::steady_clock::time_point;

enum class Sound{Beep, Siren, Whistle};

// typedef for a length of time
using Duration = std::chrono::steady_clock::duration;

/* At time t, make sound s for duration d */
void setAlarm(Time t, Sound s, Duration d){
    cout << "hello, world \n";
};
enum class Volumn{Normal, Loud, LoudPlusPlus};
/* overloaded setAlarm*/
void setAlarm(Time t, Sound s, Duration d, Volumn v){
    cout<< "HELLO, WORLD \n";
}

class PolyWidget{
public:
    template<typename T>
    void operator() (const T& param){
        cout << param << endl;
    }
};

int main(){
    /* In lambda version, the steady_clock::now() + 1h is an argument to setAlarm
       It will be evaluated when setAlarm is called.
     */
    auto setSoundL = [](Sound s){
        using namespace std::chrono;
        setAlarm(steady_clock::now() + hours(1), s , seconds(30));
    };
    setSoundL(Sound::Beep);

    /* Use std::bind version */
    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::placeholders;
    /* In std::bind() version, steady_clock::now+1h is passed to std::bind, not to setAlarm.
       Which means that the time resulting from the expression will be evaluated when the std::bind
       is called, and the time resulting from the expression will be stored inside the resulting bind 
       object.
     */
    /* Fix overloaded function */
    using SetAlarm3ParamType = void(*)(Time t, Sound s, Duration d);
    auto setSoundB = std::bind(static_cast<SetAlarm3ParamType>(setAlarm), steady_clock::now() + 1h, _1, 30s);
    /* Fix bug above */
    auto  SetSoundB1 =std::bind(static_cast<SetAlarm3ParamType>(setAlarm), std::bind(std::plus<steady_clock::time_point>(), steady_clock::now(), 1h), _1, 30s);

    int lowVal = 1, highVal = 10;
    auto betweenL = [lowVal, highVal](const auto& val){
        return lowVal <= val && val <= highVal;
    };
    cout << "5 is between 1 and 10 : " << betweenL(5) << endl;

    /* bind version c++11 version*/
    auto betweenB = std::bind(std::logical_and<>(), std::bind(std::less_equal<int>(), lowVal, _1),
                                                    std::bind(std::less_equal<int>(), _1, highVal));
    cout << "5 is between 1 and 10 (bind version) :" << betweenB(5) << endl;

    PolyWidget pw;
    /* bind version */
    auto boundPW = std::bind(pw, _1);
    boundPW(2);

    /* Codes in book have bug */
    auto boundPWL = [pw]<typename T>(const auto& param) // C++14 
    { pw(param); };
    boundPWL(3);
    //boundPWL(3);
    //int x = 3;
    //boundPWL(&x);
    //const PolyWidget pw1;
   /* c++14 version lambda */
//    auto boundPWL = [pw](const auto& param){pw(param);};
//    boundPWL(3);
}
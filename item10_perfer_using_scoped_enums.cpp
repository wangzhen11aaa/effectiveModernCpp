#include <iostream>
#include <string>
#include "type_name.hpp"


using namespace std;

/* C++11 version */
template<typename E>
constexpr typename std::underlying_type<E>::type _toEmType(E enumerator){
    return static_cast<typename std::underlying_type<E>::type>(enumerator);
}


/* C++14 version */
template<typename E>
constexpr std::underlying_type_t<E> toEmType(E emulator) noexcept{
    return static_cast<std::underlying_type_t<E>>(emulator);
}

/* C++14 auto version */
template<typename E>
constexpr auto
__toEmType(E enumerator){
    return static_cast<std::underlying_type_t<E>>(enumerator);
}


int main(){
    // unscoped.
    // enum Color {black, white, red};
    // auto white = false;
    // scoped
    enum class _Color{black, white, red};
    auto white = false;
    cout << "white in enum outer's scope : " << white;
    cout << "white in enum's scope : " << static_cast<int>(_Color::white) << endl;
    
    // Declare underlying type for scoped enum.
    enum class Color:std::uint8_t{
        black, white, red
    };
    cout << "Directly output the enumator :" << static_cast<int>(Color::white) << endl;

    using UserInfo = std::tuple<std::string, std::string, std::size_t>;
    UserInfo userInfo{"abc", "wz" ,1};
    auto first_v = std::get<1>(userInfo);
    cout << "first_v : " << first_v << endl;
    auto _v = Color::black;
    // get underlying type of enum type.
    std::cout << "underlying type for enum Color : " << typeid(std::underlying_type<Color>::type).name() << endl;
    
    // use toEuType function to get element type in Emulator.
    std::tuple<string, string, string> profile{"trousers", "skirt"};
    auto t_b = std::get<toEmType(_Color::black)>(userInfo);
    cout << "trouser's brand : " << t_b << endl;
    auto s_b = std::get<toEmType(_Color::white)>(userInfo);
    cout << "skirt's brand : " << s_b << endl;
    auto h_b = std::get<toEmType(_Color::red)>(userInfo);
    cout << "hat's size : " << h_b << endl;

    t_b = std::get<__toEmType(_Color::black)>(userInfo);
    cout << "trouser's brand from __toEmType function : " << t_b << endl;
    return 0;

}
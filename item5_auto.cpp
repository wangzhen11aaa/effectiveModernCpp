#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

using namespace std;

class Widget{
    public:
        // Use universal initialization brace braket.
        Widget(int &&t):v{t}{}

        bool operator <(const Widget &right){
            return v < right.v;
        }
    private:
        int v;
};

template<typename It>
void dwim(It b, It e){
    while(b != e){
        typename std::iterator_traits<It>::value_type
        currValue = *b;
        cout << "CurrValue : " << currValue << endl;
        b++;
    }
}

template<typename It>
void dwimNew(It b, It e){
    while(b != e){
        auto currValue = *b;
        cout << "CurrValue : " << currValue << endl;
        b++;
    }
}
std::function<bool(const std::unique_ptr<Widget> &, const std::unique_ptr<Widget> &)> func;

int main(){
    // auto vs explictly declare type.
    vector<int> v{1,2,3,4};
    dwim(v.begin(), v.end());
    dwimNew(v.begin(), v.end());

    // some test on unique_ptr.
    int i = 5;
    std::unique_ptr<int> ui = make_unique<int>(11);
    cout << "ui' address : " << ui << endl;
    std::unique_ptr<int> ui1 = std::move(ui);
    cout << "ui1's address : " << ui1 << endl;
    cout << "After moving : " << *ui1 << endl;

    // clousre of function partt. 
    int t = 1;
    cout << "Initialize t = 1";
    Widget w1(1);
    Widget w2(2);
    std::function<bool(const std::unique_ptr<Widget>&, const std::unique_ptr<Widget>&)> 
    Compare =[&t](const std::unique_ptr<Widget>& p1, 
    const std::unique_ptr<Widget>& p2){
        cout << "t in closure: " << t << endl; 
        ++t;
        cout << "t's value in closure : " << t << endl;
        return *p1 < *p2;
    };
    auto r = Compare(make_unique<Widget>(1), make_unique<Widget>(2));
    cout << "After the clousre, t is : " << t << endl;

    cout << "w1 < w2 :" << r << endl; 

    // test on container's size container<xx>::size_type.

    for(auto i = 0; i < v.size(); i++){
        cout << v[i] << endl;
    }    
    
    // test on unordered_map structure.
    std::unordered_map<std::string, int> m;
    m["GeeksforGeeks"] = 10;
    m["Are"] = 20;
    m["Good"] = 30;
    
    // use another pointer to modity the value.
    for(const std::pair<std::string, int> &p: m){
        std::pair<std::string, int> tmp = p;
        tmp.first = "ab";
    }

    cout << "After modification: " << endl;
    for(const auto &p: m){
        cout << p.first << p.second << endl;
    }
    
    return 0;
}
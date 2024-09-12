#include <iostream>
#include "type_name.hpp"
#include <atomic>
#include <thread>

using namespace std;

const int CNT = 100000000;

void add(std::atomic<int>& _ac, volatile int& _vc){
	for (auto i = 0; i < CNT; i++){
		++_ac;
	    ++_vc;
	}
}

int main(){
    std::atomic<int> ac(0);
    volatile int vc(0);
    std::thread t(add, std::ref(ac), std::ref(vc));
    std::thread t1(add, std::ref(ac), std::ref(vc));
    t.join();
    t1.join();
    cout << "ac : " << ac << endl;
    cout << "vc : " << vc << endl;
    return 0;
}
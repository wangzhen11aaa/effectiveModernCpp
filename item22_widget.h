#pragma once
#include <memory>
#include <iostream>
using namespace std;

class Widget{
public:
    Widget();
    ~Widget();

    Widget(Widget&& rhs);
    Widget& operator=(const Widget& rhs);
    Widget(const Widget& rhs);
    Widget& operator=(Widget&& rhs);

	auto pImpl_empty() {
		if(pImpl.get() == nullptr) {
			std::cout << "empty";
		}
	}

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
#pragma once
#include <memory>
using namespace std;

class Widget{
public:
    Widget();
    ~Widget();

    Widget(Widget&& rhs);
    Widget& operator=(const Widget& rhs);
    Widget(const Widget& rhs);
    Widget& operator=(Widget&& rhs);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
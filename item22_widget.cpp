#include "item22_widget.h"
#include "item22_gadget.h"
#include <string>
#include <vector>

struct Widget::Impl{
    std::string name;
    std::vector<double> data;
    Gadget g1, g2, g3;
};

Widget::Widget():pImpl(std::make_unique<Impl>()){
}

Widget::~Widget() = default;

/* move series construct */
Widget::Widget(Widget&& rhs) = default;
Widget& Widget::operator=(Widget&& rhs) =default;
/* copy series construct */
Widget::Widget(const Widget& rhs):pImpl(std::make_unique<Impl>(*rhs.pImpl)) {}
Widget& Widget::operator=(const Widget& rhs){
    *pImpl = *rhs.pImpl;
    return *this;
}
int main(){
    Widget w;
    Widget w2 = std::move(w);
    return 0;
}
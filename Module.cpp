#include "Module.h"

Module::Block::Block(double w, double h, std::string n)
    : width(w)
    , height(h)
    , name(n)
{
}

Module::Module(double x_, double y_, double width_, double height_, std::string name_, Block* b)
    : rect(Rectangle(x_, y_, width_, height_))
    , name(name_)
    , block(b)
{
}

#ifndef MODULE_H
#define MODULE_H

#include "Geometry.h"

#include <string>

struct Module
{
    enum Sign {
        Pos,
        Neg,
        None
    };

    struct Block
    {
        Block(double w, double h, std::string n);

        double width;
        double height;
        std::string name;
    };

    Module(double x_, double y_, double width_,
           double height_, std::string name_,
           Sign sign_ = None, Block* b = 0);

    Rectangle rect;
    std::string name;
    Sign sign;
    Block* block;
};

#endif

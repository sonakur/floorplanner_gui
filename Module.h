#ifndef MODULE_H
#define MODULE_H

#include "Geometry.h"

#include <string>

struct Module
{
    struct Block
    {
        Block(double w, double h, std::string n);

        double width;
        double height;
        std::string name;
    };

    Module(double x_, double y_, double width_, double height_, std::string name, Block* b = 0);

    Rectangle rect;
    std::string name;
    Block* block; 
};

#endif

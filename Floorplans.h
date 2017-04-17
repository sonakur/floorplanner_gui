#ifndef FLOORPLANS_H
#define FLOORPLANS_H

#include "Module.h"
#include "Geometry.h"

struct BaseFloorplan
{
    Rectangle rect;    
    double weight;
    Point centerOfGravity;

    BaseFloorplan();
    BaseFloorplan(Rectangle r, double w, Point c);
    virtual ~BaseFloorplan() = 0;
    
    double vertDistance(const Point& p) const;
    double horizDistance(const Point& p) const;
};

struct LeafFloorplan
    : public BaseFloorplan
{
    Module* module;

    LeafFloorplan(Module* module);
    virtual ~LeafFloorplan(); 
};

struct Floorplan
    : public BaseFloorplan
{
    enum Type {
        H,
        V
    };

    BaseFloorplan* left;
    BaseFloorplan* right;
    Type type;
    bool swap;

    Floorplan(BaseFloorplan* l, BaseFloorplan* r, Type t);
    virtual ~Floorplan();

    bool swapCondition(const Floorplan* f, Point p) const;

    Rectangle mergedRect() const;
    Point mergedCenterOfGravity() const;

    void swapChildren();
	void recalculateTree();

private:
	void _recalculateTree(BaseFloorplan* root);
};

#endif

#ifndef FLOORPLANS_H
#define FLOORPLANS_H

#include "Module.h"
#include "Geometry.h"

struct BaseFloorplan
{
    Rectangle rect;
    Point centerOfGravity;
    double weight;

    BaseFloorplan();
    BaseFloorplan(const Rectangle& r, const Point& c, double w);
    virtual ~BaseFloorplan();

    // TODO: remove
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

//    bool swapCondition(const Floorplan* f, Point p) const;

    Rectangle mergedRect() const;
    Point mergedCenterOfGravity() const;

    void swapChildren();
    void swapCoordinates();
	void recalculateTree();

    // Used for fixing coords of children based on root coords
    // This is needed, because after upward optimiziation coords
    // of children need to be fixed if their roots are swapped
    void recalculateChildrenCoords();

private:
	void _recalculateTree(BaseFloorplan* root);
};

#endif

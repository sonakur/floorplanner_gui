#include <cassert>
#include <cmath>
#include <algorithm>

#include "Floorplans.h"

BaseFloorplan::BaseFloorplan(Rectangle r, double w, Point /* c */)
    : rect(r)
    , weight(w)
	, centerOfGravity(Point::undefined)
{
}

BaseFloorplan::BaseFloorplan()
{
}

BaseFloorplan::~BaseFloorplan()
{
}

double BaseFloorplan::vertDistance(const Point& p) const
{
    if (centerOfGravity == Point::undefined) {
        return fabs((rect.top() + rect.bottom()) / 2 - p.y);
    }
    return fabs(centerOfGravity.y - p.y);
}

double BaseFloorplan::horizDistance(const Point& p) const
{
    if (centerOfGravity == Point::undefined) {
        return fabs((rect.right() + rect.left()) / 2 - p.x);
    }
    return fabs(centerOfGravity.x - p.x);
}


LeafFloorplan::LeafFloorplan(Module* m)
	: BaseFloorplan(m->rect, 0, Point::undefined)
    , module(m)
{
}

LeafFloorplan::~LeafFloorplan()
{
}

Floorplan::Floorplan(BaseFloorplan* l, BaseFloorplan* r, Floorplan::Type t)
    : BaseFloorplan()
    , left(l)
    , right(r)
    , type(t)
    , swap(false)
{
	if (t == Floorplan::H) {
		assert(l->rect.width() == r->rect.width());
		rect = Rectangle(l->rect.x(), l->rect.y(), l->rect.width(), l->rect.height() + r->rect.height());    
	} else {
		assert(l->rect.height() == r->rect.height());
		rect = Rectangle(l->rect.x(), l->rect.y(), l->rect.width() + r->rect.width(), l->rect.height());
	}
	centerOfGravity = Point::undefined;
}

Floorplan::~Floorplan()
{
}

Rectangle Floorplan::mergedRect() const
{
    if (type == Floorplan::H) {
        assert(left->rect.width() == right->rect.width());
        return Rectangle(left->rect.x(), left->rect.y(), left->rect.width(), left->rect.height() + right->rect.height());
    } else {
        assert(left->rect.height() == right->rect.height());
        return Rectangle(left->rect.x(), left->rect.y(), left->rect.width() + right->rect.width(), left->rect.height());
    }
}

Point Floorplan::mergedCenterOfGravity() const
{
    Point p1 = left->centerOfGravity;
    Point p2 = right->centerOfGravity;
    double length = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    double d = (length * left->weight) / (left->weight + right->weight);
	double maxX = std::max(p1.x, p2.x);
	double maxY = std::max(p1.y, p2.y);
	double minX = p1.x + p2.x - maxX;
	double minY = p1.y + p2.y - maxY;
	double deltaX = maxX - minX; 
	double deltaY = maxY - minY;    
	double x =  maxX - d * deltaX / length;
	double y =  maxY - d * deltaY / length;
    return Point(x, y);
}

void Floorplan::swapChildren()
{
    // Swap pointers
    BaseFloorplan* tmp = left;
    left = right;
    right = tmp;

    // Fix coordinates
    swapCoordinates();
}

void Floorplan::swapCoordinates()
{
    // Shift rects and centers of swapped leafs
    if (type == Floorplan::V) {
        // Vertically splitted floorplans should have the same y coord.
        assert(left->rect.y() == right->rect.y());

        // Shift rects
        left->rect.setX(left->rect.x() - right->rect.width());
        right->rect.setX(right->rect.x() + left->rect.width());

        // Shift centers
        left->centerOfGravity.x -= right->rect.width();
        right->centerOfGravity.x += left->rect.width();
    } else {
        // Horizontally splitted floorplans should have the same x coord.
        assert(left->rect.x() == right->rect.x());

        // Shift rects
        left->rect.setY(left->rect.y() - right->rect.height());
        right->rect.setY(right->rect.y() + left->rect.height());

        // Shift centers
        left->centerOfGravity.y -= right->rect.height();
        right->centerOfGravity.y += left->rect.height();
    }
}

void Floorplan::recalculateTree()
{
	_recalculateTree(this);
}

void Floorplan::_recalculateTree(BaseFloorplan* root)
{
	Floorplan* f = dynamic_cast<Floorplan*>(root);
	if (0 != f) {
		f->left->rect.setX(f->rect.x());
		f->left->rect.setY(f->rect.y());
		if (f->type == Floorplan::V) {
			f->right->rect.setX(f->rect.x() + f->left->rect.width());
			f->right->rect.setY(f->rect.y());
		} else {
			f->right->rect.setX(f->rect.x());
			f->right->rect.setY(f->rect.y() + f->left->rect.height());
		}
		_recalculateTree(f->left);
		_recalculateTree(f->right);
	} else {
		LeafFloorplan* leaf;
		leaf = dynamic_cast<LeafFloorplan*>(root);
		assert(leaf);
	}
}

void Floorplan::recalculateChildrenCoords()
{
    // Coords of left child always match with coords of parent
    left->rect.setX(rect.x());
    left->rect.setY(rect.y());

    // Coords of right child differ by width/height of the left one
    if (type == Floorplan::V) {
        right->rect.setX(rect.x() + left->rect.width());
        right->rect.setY(rect.y());
    } else {
        right->rect.setX(rect.x());
        right->rect.setY(rect.y() + left->rect.height());
    }
}


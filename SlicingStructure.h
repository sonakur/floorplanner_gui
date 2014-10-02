#ifndef SLICING_STRUCTURE_H
#define SLICING_STRUCTURE_H

#include "Floorplans.h"

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include <set>

class CompareX;
class CompareY;

template<typename T, typename Container = std::vector<T>, typename Compare = std::less<typename Container::value_type> >
class iterable_priority_queue : public std::priority_queue<T, Container, Compare>
{
public:
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;

    iterator begin() 
    { 
        return this->c.begin(); 
    }
    
    iterator end() 
    { 
        return this->c.end(); 
    }
    
    const_iterator begin() const 
    { 
        return this->c.begin(); 
    }
    
    const_iterator end() const 
    { 
        return this->c.end(); 
    }
};



class SlicingStructure
{
public:
    enum Destination {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

    SlicingStructure();     // constructs an empty structure
    SlicingStructure(const std::vector<Module*>& ); // constructs a slicing structure form list of blocks
    SlicingStructure(const SlicingStructure& SlicingStructure);

    BaseFloorplan* floorplan() const;


    void applyNetMigration(const std::set<Module*>& netModules, const Point& target = Point(0, 0));
    void reduceDistnace(Module* module1, Module* module2);
    void moveToSide(BaseFloorplan* root, LeafFloorplan* f, Destination dest, Floorplan::Type);

private:
    typedef std::priority_queue<BaseFloorplan*, std::vector<BaseFloorplan*>, CompareY> XCoordFloorplans;
    typedef std::priority_queue<BaseFloorplan*, std::vector<BaseFloorplan*>, CompareX> YCoordFloorplans;

    void fillFloorplanMaps(std::vector<Module*>);
    void buildSlicingTree();
    void mergeXFloorplans();
    void mergeYFloorplans();
	    
    bool areVerticalSiblings(BaseFloorplan* f1, BaseFloorplan* f2) const;
    bool areHorizontalSiblings(BaseFloorplan* f1, BaseFloorplan* f2) const;

    BaseFloorplan* lowestCommonAncestor(BaseFloorplan* root, LeafFloorplan* f1, LeafFloorplan* f2) const;
    bool findPath(BaseFloorplan* root, LeafFloorplan* f, std::vector<Floorplan*>& path);
    
    BaseFloorplan* xMapPop(double x);
    BaseFloorplan* yMapPop(double y);

    BaseFloorplan* xMapPop(XCoordFloorplans* );
    BaseFloorplan* yMapPop(YCoordFloorplans* );

	void xMapPush(double x, BaseFloorplan* f);
	void yMapPush(double y, BaseFloorplan* f);

	void cleanUnusedXKeys();
	void cleanUnusedYKeys();

	void fillXMap();
	void fillYMap();

    void _applyNetMigration(BaseFloorplan*, const std::set<Module*>&, const Point&);
    void _applyNetMigrationDownward(BaseFloorplan*, const std::set<Module*>&, const Point&);
    void calculateWeights(BaseFloorplan* f, const std::set<Module*>& moduleNets, const Point& target);
	
    void print(); // remove

private:
    std::map<double, XCoordFloorplans* > m_xFlrp;
    std::map<double, YCoordFloorplans* > m_yFlrp;
    
    BaseFloorplan* m_floorplan;
};

#endif

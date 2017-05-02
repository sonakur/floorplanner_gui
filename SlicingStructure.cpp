#include "SlicingStructure.h"

#include <cassert>
#include <iostream>

namespace utils {

Point mergedCenterOfGravity(const BaseFloorplan* left, const BaseFloorplan* right)
{
    if (left->weight == 0) {
        return right->centerOfGravity;
    } else if(right->weight == 0) {
        return left->centerOfGravity;
    }

    // Merge center of gravites using the following formula:
    // c = c1 * w1 / (w1 + w2) + c2 * w2 / (w1 + w2)
    Point centerOfGravity;

    double coef1 = left->weight / (left->weight + right->weight);
    double coef2 = right->weight / (left->weight + right->weight);
    centerOfGravity.x = coef1 * left->centerOfGravity.x +
            coef2 * right->centerOfGravity.x;
    centerOfGravity.y = coef1 * left->centerOfGravity.y +
            coef2 * right->centerOfGravity.y;

    return centerOfGravity;
}

Point swappedCenterOfGravity(const Floorplan* f)
{
    // Create the left child of swapped floorplan, using the right leaf of original one
    BaseFloorplan left(f->right->rect, f->right->weight, f->right->centerOfGravity);

    // Create the right child of swapped floorplan, using the left leaf of original one
    BaseFloorplan right(f->left->rect, f->left->weight, f->left->centerOfGravity);

    Floorplan swappedFloorplan(&left, &right, f->type);

    // Shift coordinates of swapped children
    swappedFloorplan.swapCoordinates();

    return mergedCenterOfGravity(&left, &right);
}

bool swapCondition(const Point& center, const Point& swappedCenter, const Point& target)
{
    return (center.distance(target) > swappedCenter.distance(target));
}

}

struct CompareX
{
    bool operator()(const BaseFloorplan* f1, const BaseFloorplan* f2) const
    {
        return f1->rect.x() > f2->rect.x();
    }
};

struct CompareY
{
    bool operator()(const BaseFloorplan* f1, const BaseFloorplan* f2) const
    {
        return f1->rect.y() > f2->rect.y();
    }
};

SlicingStructure::SlicingStructure()
    : m_floorplan(0)
{
}

SlicingStructure::SlicingStructure(const std::vector<Module*>& modules)
{
    fillFloorplanMaps(modules);
    //print();
    buildSlicingTree();
}

SlicingStructure::SlicingStructure(const SlicingStructure& other)
    : m_floorplan(other.m_floorplan)
    , m_xFlrp(other.m_xFlrp)
    , m_yFlrp(other.m_yFlrp)
{

}

BaseFloorplan* SlicingStructure::floorplan() const
{
    return m_floorplan;
}

void SlicingStructure::reduceDistnace(Module* module1, Module* module2)
{
    LeafFloorplan* f1 = new LeafFloorplan(module1);
    LeafFloorplan* f2 = new LeafFloorplan(module2);
    BaseFloorplan* root = lowestCommonAncestor(m_floorplan, f1, f2);
    Floorplan* f = dynamic_cast<Floorplan*>(root);
    assert(0 != f);
    if (f->type == Floorplan::H) {
        if (f1->rect.y() < f2->rect.y()) {
            moveToSide(root, f1, SlicingStructure::TOP, f->type);
            moveToSide(root, f2, SlicingStructure::BOTTOM, f->type);
        } else if (f1->rect.y() > f2->rect.y()) {
            moveToSide(root, f1, SlicingStructure::BOTTOM, f->type);
            moveToSide(root, f2, SlicingStructure::TOP, f->type);
        }
        f->recalculateTree();
        moveToSide(root, f1, SlicingStructure::RIGHT, Floorplan::V);
        f->recalculateTree();
        moveToSide(root, f2, SlicingStructure::RIGHT, Floorplan::V);
        f->recalculateTree();
        // reduce dist in vert dir
    } else {
        if (f1->rect.x() < f2->rect.x()) {
            moveToSide(root, f1, SlicingStructure::RIGHT, f->type);
            moveToSide(root, f2, SlicingStructure::LEFT, f->type);
        } else if (f1->rect.x() > f2->rect.x()) {
            moveToSide(root, f1, SlicingStructure::LEFT, f->type);
            moveToSide(root, f2, SlicingStructure::RIGHT, f->type);
        }
        f->recalculateTree();
        moveToSide(root, f1, SlicingStructure::BOTTOM, Floorplan::H);
        f->recalculateTree();
        moveToSide(root, f2, SlicingStructure::BOTTOM, Floorplan::H);
        f->recalculateTree();
        // reduce dist in horiz dir
    }
}

void SlicingStructure::moveToSide(BaseFloorplan* root, LeafFloorplan* f, Destination dest, Floorplan::Type type)
{
    std::vector<Floorplan*> path;
    bool found;
    found = findPath(root, f, path);
    assert(found);
    std::vector<Floorplan*>::const_iterator it = path.begin();
    Floorplan* floorplan = dynamic_cast<Floorplan*>(root);
    assert(0 != floorplan);
    bool isRightChild = false;
    if (((*it)->type == Floorplan::V && (*it)->rect.right() == f->rect.right()) ||
        ((*it)->type == Floorplan::H && (*it)->rect.top() == f->rect.top())) {
            isRightChild = true;
    }
    for (; it != path.end() - 1; ++it) {
        if (type == (*it)->type) {
            if (isRightChild && (dest == SlicingStructure::LEFT || dest == SlicingStructure::BOTTOM)) {
                (*it)->swapChildren();
            } else if (!isRightChild && (dest == SlicingStructure::RIGHT || dest == SlicingStructure::TOP)) {
                (*it)->swapChildren();
            }
        }
        Floorplan* parent = *(it + 1);
        if ((parent->type == Floorplan::V && parent->rect.right() == (*it)->rect.right()) ||
            (parent->type == Floorplan::H && parent->rect.top() == (*it)->rect.top())) {
            isRightChild = true;
        } else {
            isRightChild = false;
        }
    }

}

bool SlicingStructure::findPath(BaseFloorplan* root, LeafFloorplan* f, std::vector<Floorplan*>& path) {
    if (!root) {
        return false;
    }

    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(root);
    if (leaf) {
        if (leaf->module->name == f->module->name) {
            return true;
         } else {
             return false;
         }
    }

    Floorplan* floorplan = dynamic_cast<Floorplan*>(root);
    assert(0 != floorplan);
    if (findPath(floorplan->left, f, path) || findPath(floorplan->right, f, path)) {
        path.push_back(floorplan);
        return true;
    }
    return false;
}

void SlicingStructure::applyNetMigration(const std::set<Module*>& moduleNets, const Point& target)
{
    _applyNetMigrationUpward(m_floorplan, moduleNets, target);
//    Floorplan* floorplan = dynamic_cast<Floorplan*>(m_floorplan);
//    assert(0 != floorplan);
//    floorplan->recalculateTree();
    _applyNetMigrationDownward(m_floorplan, moduleNets, target);
//    floorplan->recalculateTree();
}

void SlicingStructure::_applyNetMigrationUpward(BaseFloorplan* f, const std::set<Module*>& moduleNets, const Point& target)
{
    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(f);
    if (leaf != 0) {
        if (moduleNets.find(leaf->module) != moduleNets.end()) {
            f->centerOfGravity = Point((f->rect.right() + f->rect.left()) / 2, 
                                        (f->rect.top() + f->rect.bottom()) / 2);
            f->weight = f->rect.width() * f->rect.height();
        } else {
            f->centerOfGravity = Point::undefined;
            f->weight = 0;
        }
		return;
    }

    Floorplan* floorplan = dynamic_cast<Floorplan*>(f);
    assert(0 != floorplan);

    _applyNetMigrationUpward(floorplan->left, moduleNets, target);
    _applyNetMigrationUpward(floorplan->right, moduleNets, target);

    floorplan->rect = floorplan->mergedRect();
    floorplan->weight = floorplan->left->weight + floorplan->right->weight;

    // If floorplan has 0 weight, no need to optimize anything
    if (0 == floorplan->weight) {
        return;
    }

    const Point& mergedCenter = utils::mergedCenterOfGravity(floorplan->left, floorplan->right);
    const Point& swappedCenter = utils::swappedCenterOfGravity(floorplan);

    if (utils::swapCondition(mergedCenter, swappedCenter, target)) {
        floorplan->swapChildren();
        floorplan->centerOfGravity = swappedCenter;
    } else {
        floorplan->centerOfGravity = mergedCenter;
    }
}

void SlicingStructure::_applyNetMigrationDownward(BaseFloorplan* f, const std::set<Module*>& moduleNets, const Point& target)
{
    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(f);
    if (0 != leaf) {
        return;
    }

    Floorplan* floorplan = dynamic_cast<Floorplan*>(f);
    assert(0 != floorplan);

    // Fix coords of children
    floorplan->recalculateChildrenCoords();

    // If floorplan has 0 weight, no need to optimize anything
    if (0 == floorplan->weight) {
        return;
    }

    // Check if further swap will make any improvment
    const Point& swappedCenter = utils::swappedCenterOfGravity(floorplan);
    if (utils::swapCondition(floorplan->centerOfGravity, swappedCenter, target)) {
        floorplan->swapChildren();
        floorplan->centerOfGravity = swappedCenter;
    }

    // Go recursively down to children
    _applyNetMigrationDownward(floorplan->left, moduleNets, target);
    _applyNetMigrationDownward(floorplan->right, moduleNets, target);
}

void SlicingStructure::print()
{
    std::map<double, XCoordFloorplans* >::const_iterator xIt;
    for (xIt = m_xFlrp.begin(); xIt != m_xFlrp.end(); ++xIt) {
        std::cout<<xIt->first<<": ";
        while (!xIt->second->empty()) {
            Module* m = dynamic_cast<LeafFloorplan*>(xIt->second->top())->module;
            std::cout<<m->name<<" ";
            std::cout<<m->rect.x()<<","<<m->rect.y()<<","<<m->rect.width()<<","<<m->rect.height()<<" \t";
            xIt->second->pop();
        }
        std::cout<<std::endl;
    }
    std::map<double, YCoordFloorplans* >::const_iterator yIt;
    for (yIt = m_yFlrp.begin(); yIt != m_yFlrp.end(); ++yIt) {
        std::cout<<yIt->first<<": ";
        while (!yIt->second->empty()) {
            Module* m = dynamic_cast<LeafFloorplan*>(yIt->second->top())->module;
            std::cout<<m->name<<" ";
            std::cout<<m->rect.x()<<","<<m->rect.y()<<","<<m->rect.width()<<","<<m->rect.height()<<" \t";
            yIt->second->pop();
        }
        std::cout<<std::endl;
    }
}

void SlicingStructure::fillFloorplanMaps(std::vector<Module*> modules)
{
    std::vector<Module*>::const_iterator it;
    for (it = modules.begin(); it != modules.end(); ++it) {
        BaseFloorplan* floorplan = new LeafFloorplan(*it);
        double x = (*it)->rect.x();
        double y = (*it)->rect.y();
        xMapPush(x, floorplan);
        yMapPush(y, floorplan);
    }
}

void SlicingStructure::mergeXFloorplans()
{
    std::map<double, XCoordFloorplans* >::iterator it = m_xFlrp.begin();
    for (it = m_xFlrp.begin(); it != m_xFlrp.end(); it++) {
        double currentXCoord = (*it).first;
        XCoordFloorplans* xCurrentQueue = (*it).second;
        BaseFloorplan* currentX = xMapPop(xCurrentQueue);
        XCoordFloorplans* mergedXQueue = new XCoordFloorplans();
        while (!xCurrentQueue->empty()) {
            BaseFloorplan* nextX = xMapPop(xCurrentQueue);
//            std::cout<<nextX->rect.x()<<" "<<nextX->rect.y()<<" "<<nextX->rect.width()<<" "<<nextX->rect.height()<<"\n";
            if (areHorizontalSiblings(currentX, nextX)) {
                BaseFloorplan* mergedFloorplan = new Floorplan(currentX, nextX, Floorplan::H);
                currentX = mergedFloorplan;
            } else {
                mergedXQueue->push(currentX);
                currentX = nextX;
            }
        }
        mergedXQueue->push(currentX);
        m_xFlrp[currentXCoord] = mergedXQueue;
    }
}

void SlicingStructure::mergeYFloorplans()
{
    std::map<double, YCoordFloorplans* >::iterator it = m_yFlrp.begin();
    for (it = m_yFlrp.begin(); it != m_yFlrp.end(); it++) {
        double currentYCoord = (*it).first;
        YCoordFloorplans* yCurrentQueue = (*it).second;
        BaseFloorplan* currentY = yMapPop(yCurrentQueue);
        YCoordFloorplans* mergedYQueue = new YCoordFloorplans();
        while (!yCurrentQueue->empty()) {
            BaseFloorplan* nextY = yMapPop(yCurrentQueue);
  //          std::cout<<nextY->rect.x()<<" "<<nextY->rect.y()<<" "<<nextY->rect.width()<<" "<<nextY->rect.height()<<"\n";
            if (areVerticalSiblings(currentY, nextY)) {
                BaseFloorplan* mergedFloorplan = new Floorplan(currentY, nextY, Floorplan::V);
                currentY = mergedFloorplan;
            } else {
                mergedYQueue->push(currentY);
                currentY = nextY;
            }
        }
        mergedYQueue->push(currentY);
        m_yFlrp[currentYCoord] = mergedYQueue;
    }
}

BaseFloorplan* SlicingStructure::lowestCommonAncestor(BaseFloorplan* root, LeafFloorplan* f1, LeafFloorplan* f2) const
{
    if (root == 0) {
        return 0;
    }

    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(root);
    if (leaf) {
        if (f1->module->name == leaf->module->name || f2->module->name == leaf->module->name) {
            return root;
        } else {
            return 0;
        }
    }

    Floorplan* f = dynamic_cast<Floorplan*>(root);
    assert(0 != f);
    BaseFloorplan* leftAnc = lowestCommonAncestor(f->left, f1, f2);
    BaseFloorplan* rightAnc = lowestCommonAncestor(f->right, f1, f2);
    if (leftAnc && rightAnc) {
        return root;
    }

    if (leftAnc != 0) {
        return leftAnc;
    }
    return rightAnc;
}

void SlicingStructure::buildSlicingTree()
{
    while (!m_xFlrp.empty() && !m_yFlrp.empty()) {
        mergeXFloorplans();
        if (m_xFlrp.size() == 1) {
            std::map<double, XCoordFloorplans* >::const_iterator it = m_xFlrp.begin();
            if ((*it).second->size() == 1) {
                m_floorplan = (*it).second->top();
                return;
            }
        }
        cleanUnusedXKeys(); // TODO call these functions onnly in case of modificationns
        fillYMap();

        mergeYFloorplans();
        if (m_yFlrp.size() == 1) {
            std::map<double, YCoordFloorplans* >::const_iterator it = m_yFlrp.begin();
            if ((*it).second->size() == 1) {
                m_floorplan = (*it).second->top();
                return;
            }
        }
        cleanUnusedYKeys();
        fillXMap();

    }
}

bool SlicingStructure::areHorizontalSiblings(BaseFloorplan* f1, BaseFloorplan* f2) const
{
    if (f1->rect.left() == f2->rect.left() && f1->rect.right() == f2->rect.right() && f1->rect.top() == f2->rect.bottom()) {
        return true;
    }
    return false;
}

bool SlicingStructure::areVerticalSiblings(BaseFloorplan* f1, BaseFloorplan* f2) const
{
    if (f1->rect.bottom() == f2->rect.bottom() && f1->rect.top() == f2->rect.top() && f2->rect.left() == f1->rect.right()) {
        return true;
    }
    return false;
}

BaseFloorplan* SlicingStructure::xMapPop(double x)
{
    BaseFloorplan* top = m_xFlrp[x]->top();
    m_xFlrp[x]->pop();
    return top;
}

BaseFloorplan* SlicingStructure::yMapPop(double y)
{
    BaseFloorplan* top = m_yFlrp[y]->top();
    m_yFlrp[y]->pop();
    return top;
}

BaseFloorplan* SlicingStructure::xMapPop(XCoordFloorplans* xQueue)
{
    BaseFloorplan* top = xQueue->top();
    xQueue->pop();
    return top;
}

BaseFloorplan* SlicingStructure::yMapPop(YCoordFloorplans* yQueue)
{
    BaseFloorplan* top = yQueue->top();
    yQueue->pop();
    return top;
}

void SlicingStructure::xMapPush(double x, BaseFloorplan* f)
{
    if (m_xFlrp.find(x) == m_xFlrp.end()) {
        m_xFlrp[x] = new XCoordFloorplans();
    }
    m_xFlrp[x]->push(f);
}

void SlicingStructure::yMapPush(double y, BaseFloorplan* f)
{
    if (m_yFlrp.find(y) == m_yFlrp.end()) {
        m_yFlrp[y] = new YCoordFloorplans();
    }
    m_yFlrp[y]->push(f);
}

void SlicingStructure::cleanUnusedXKeys()
{
    std::map<double, XCoordFloorplans* >::iterator it = m_xFlrp.begin();
    while (it != m_xFlrp.end()) {
        if ((*it).second->empty()) {
            double keyToDelete = (*it).first;
            delete (*it).second;
            it++;
            m_xFlrp.erase(keyToDelete);
        } else {
            ++it;
        }
    }
}

void SlicingStructure::cleanUnusedYKeys()
{
    std::map<double, YCoordFloorplans* >::iterator it = m_yFlrp.begin();
    while (it != m_yFlrp.end()) {
        if ((*it).second->empty()) {
            double keyToDelete = (*it).first;
            delete (*it).second;
            it++;
            m_yFlrp.erase(keyToDelete);
        } else {
            ++it;
        }
    }
}

void SlicingStructure::fillXMap()
{
    m_xFlrp.clear();
    std::map<double, YCoordFloorplans* > copy = std::map<double, YCoordFloorplans* >(m_yFlrp);
    std::map<double, YCoordFloorplans* >::const_iterator it = copy.begin();
    for (; it != copy.end(); ++it) {
        YCoordFloorplans* queue = (*it).second;
        while (!queue->empty()) {
            BaseFloorplan* f = queue->top();
            queue->pop();
            xMapPush(f->rect.x(), f);
        }
    }
}

void SlicingStructure::fillYMap()
{
    m_yFlrp.clear();
    std::map<double, XCoordFloorplans* > copy = std::map<double, XCoordFloorplans* >(m_xFlrp);
    std::map<double, XCoordFloorplans* >::const_iterator it = copy.begin();
    for (; it != copy.end(); ++it) {
        XCoordFloorplans* queue = (*it).second;
        while (!queue->empty()) {
            BaseFloorplan* f = queue->top();
            queue->pop();
            yMapPush(f->rect.y(), f);
        }
    }
}



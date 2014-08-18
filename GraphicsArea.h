#ifndef GRAPHICSAREA_H
#define GRAPHICSAREA_H

#include "Floorplans.h"

#include <QWidget>
#include <QPixmap>

#include <set>

class GraphicsArea
    : public QWidget
{
public:
    GraphicsArea(QWidget* parent = 0);
    ~GraphicsArea();

    Point recalculatePoint(const QPoint& point) const;

    void reset();
    void draw();
    void setFloorplan(BaseFloorplan* floorplan);
    void setSelectedItems(std::set<Module*> modules);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void resizeEvent(QResizeEvent *);

private:
    void drawFloorplan(BaseFloorplan* root);
    void drawSelection(LeafFloorplan* leaf);
    void calculateScaleAndPosition();

private:
    QPixmap m_pixmap;
    BaseFloorplan* m_floorplan;
    std::set<Module*> m_selectedModules;
    double m_scale;
    double m_xShift;
    double m_yShift;

};

#endif // GRAPHICSAREA_H

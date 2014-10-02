#include "GraphicsArea.h"

#include <QDebug>
#include <QPainter>
#include <QColor>

#include <cassert>

GraphicsArea::GraphicsArea(QWidget* parent)
    : QWidget(parent)
    , m_floorplan(0)
{

}

GraphicsArea::~GraphicsArea()
{

}

void GraphicsArea::draw()
{
    update();
}

Point GraphicsArea::recalculatePoint(const QPoint& point) const
{
    if (point.x() == 0 && point.y() == 0) {
        return Point(point.x(), point.y());
    }
    QPointF result;
    result.setX((point.x() - m_xShift) / m_scale);
    result.setY((point.y() - m_yShift) / m_scale);
    return Point(result.x(), result.y());
}

void GraphicsArea::drawFloorplan(BaseFloorplan* root)
{
    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(root);
    if (0 != leaf) {
        QPainter painter(this);
        double x  = m_xShift + leaf->rect.x() * m_scale;
        double y = m_yShift + leaf->rect.y() * m_scale;
        QRect rect = QRect(x, y, leaf->rect.width() * m_scale, leaf->rect.height() * m_scale);
        QPen pen(QColor(Qt::black));
        pen.setWidth(2);
        painter.setPen(pen);
        if (m_selectedModules.find(leaf->module) != m_selectedModules.end()) {
            painter.setBrush(QBrush(QColor(Qt::red)));
        } else {
            painter.setBrush(QBrush(QColor(Qt::yellow)));
        }
        painter.drawRect(rect);
        painter.drawText(QPointF(x + 10, y + 15), QString::fromStdString(leaf->module->name));
        return;
    }

    Floorplan* floorplan = dynamic_cast<Floorplan*>(root);
    assert(0 != floorplan);
    drawFloorplan(floorplan->left);
    drawFloorplan(floorplan->right);
}

void GraphicsArea::reset()
{
    delete m_floorplan;
    m_floorplan = 0;
    draw();
}

void GraphicsArea::drawSelection(LeafFloorplan *leaf)
{
    QPainter painter(this);
    double x  = m_xShift + leaf->rect.x() * m_scale;
    double y = m_yShift + leaf->rect.y() * m_scale;
    QRect rect = QRect(x, y, leaf->rect.width() * m_scale, leaf->rect.height() * m_scale);
    QPen pen(QColor(Qt::black));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(QBrush(QColor(Qt::red)));
    painter.drawRect(rect);
    painter.drawText(QPointF(x + 10, y + 15), QString::fromStdString(leaf->module->name));
}

void GraphicsArea::setFloorplan(BaseFloorplan* floorplan)
{
    m_floorplan = floorplan;
    calculateScaleAndPosition();
}

void GraphicsArea::calculateScaleAndPosition()
{
    if (0 == m_floorplan) {
        return;
    }

    if (m_floorplan->rect.width() >= m_floorplan->rect.height()) {
        m_scale = (double)this->width() / m_floorplan->rect.width();
        m_yShift = (this->height() - m_scale * m_floorplan->rect.height()) / 2.0;
        m_xShift = 0;
    } else {
        m_scale = (double)this->height() / m_floorplan->rect.height();
        m_yShift = 0;
        m_xShift = (this->width() - m_scale * m_floorplan->rect.width()) / 2.0;
    }
}

void GraphicsArea::setSelectedItems(std::set<Module*> modules)
{
    m_selectedModules = modules;
}

void GraphicsArea::paintEvent(QPaintEvent* e)
{
    m_pixmap = QPixmap(this->size());
    m_pixmap.fill(QColor(Qt::black));

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
    if (m_floorplan != 0) {
        drawFloorplan(m_floorplan);
    }
}

void GraphicsArea::resizeEvent(QResizeEvent *)
{
    calculateScaleAndPosition();
    draw();
}

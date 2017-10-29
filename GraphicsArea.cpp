#include "GraphicsArea.h"

#include <QDebug>
#include <QPainter>
#include <QColor>

#include <cassert>

static const GraphicsArea::ColorType colors_data[] = {Qt::black, Qt::darkBlue, Qt::darkGreen};
const GraphicsArea::ColorType* GraphicsArea::colors = colors_data;

GraphicsArea::GraphicsArea(QWidget* parent)
    : QWidget(parent)
    , m_target(0)
    , m_floorplan(0)    
{

}

GraphicsArea::~GraphicsArea()
{
    if (m_target) {
        delete m_target;
        m_target = 0;
    }
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
    _drawFloorplan(root, 0);
}

void GraphicsArea::_drawFloorplan(BaseFloorplan* root, unsigned short colorIdx)
{
    Floorplan* floorplan = dynamic_cast<Floorplan*>(root);
    if (0 != floorplan)
    {
        // Pick a different color for the parent and children
        _drawFloorplan(floorplan->left, (colorIdx + 1) % 3);
        _drawFloorplan(floorplan->right, (colorIdx + 2) % 3);
    }

    QPainter painter(this);
    double x  = m_xShift + root->rect.x() * m_scale;
    double y = m_yShift + root->rect.y() * m_scale;

    QRect rect = QRect(x, y, root->rect.width() * m_scale, root->rect.height() * m_scale);
    QPen pen(colors[colorIdx]);
    pen.setWidth(2);
    painter.setPen(pen);

    LeafFloorplan* leaf = dynamic_cast<LeafFloorplan*>(root);
    if (0 != leaf) {
        const std::set<Module*>::const_iterator it = m_selectedModules.find(leaf->module);
        if (it != m_selectedModules.end()) {
            if ((*it)->sign == Module::Pos) {
                painter.setBrush(QBrush(QColor(Qt::green)));
            } else if ((*it)->sign == Module::Neg) {
                painter.setBrush(QBrush(QColor(Qt::red)));
            }
        } else {
            painter.setBrush(QBrush(QColor(Qt::white)));
        }
        painter.drawRect(rect);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawText(QPointF(x + 10, y + 15), QString::fromStdString(leaf->module->name));
        return;
    } else {
        painter.drawRect(rect);
    }
}

void GraphicsArea::reset()
{
    if (m_target) {
        delete m_target;
        m_target = 0;
    }

    delete m_floorplan;
    m_floorplan = 0;
    draw();
}

void GraphicsArea::drawTarget()
{
    if (!m_target) {
        return;
    }

    QPainter painter(this);
    QPen pen(QColor(Qt::black));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(QBrush(QColor(Qt::red)));

    double x  = m_xShift + m_target->x * m_scale;
    double y = m_yShift + m_target->y * m_scale;

    painter.drawEllipse(QPoint(x, y), 4, 4);
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

void GraphicsArea::setTargetPoint(const Point& point)
{
    m_target = new Point(point);
}

void GraphicsArea::paintEvent(QPaintEvent* /* e */)
{
    m_pixmap = QPixmap(this->size());
    m_pixmap.fill(QColor(Qt::black));

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
    if (m_floorplan != 0) {
        drawFloorplan(m_floorplan);
        drawTarget();
    }
}

void GraphicsArea::resizeEvent(QResizeEvent *)
{
    calculateScaleAndPosition();
    draw();
}

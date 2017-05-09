#include "Geometry.h"
#include <cmath>

Point Point::undefined = Point(-1, -1);

Point::Point()
    : x(0)
    , y(0)
{
}

Point::Point(double _x, double _y)
    : x(_x)
    , y(_y)
{
}

bool Point::operator == (const Point& p) const
{
    return x == p.x && y == p.y;
}

bool Point::operator != (const Point& p) const
{
    return (*this) == p;
}

double Point::distance(const Point& p) const
{
    double deltaX = x - p.x;
    double deltaY = y - p.y;
    return (std::sqrt(deltaX * deltaX + deltaY * deltaY));
}

bool Point::isNull() const
{
    return (*this == undefined);
}

void Point::shiftX(double deltaX)
{
    if (!isNull()) {
        x += deltaX;
    }
}

void Point::shiftY(double deltaY)
{
    if (!isNull()) {
        y += deltaY;
    }
}

Rectangle::Rectangle(double x, double y, double width, double height)
    : m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
}

Rectangle::Rectangle()
    : m_x(0)
    , m_y(0)
    , m_width(0)
    , m_height(0)
{
}
    
double Rectangle::top() const
{
    return m_y + m_height;
}
    
double Rectangle::bottom() const
{
    return m_y;
}

double Rectangle::left() const
{
    return m_x;
}
    
double Rectangle::right() const
{
    return m_x + m_width;
}
    
double Rectangle::x() const
{
    return m_x;
}

double Rectangle::y() const
{
    return m_y;
}

double Rectangle::height() const
{
    return m_height;
}

double Rectangle::width() const
{
    return m_width;
}

void Rectangle::setX(double x)
{
	m_x = x;
}

void Rectangle::setY(double y)
{
	m_y = y;
}

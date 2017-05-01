#ifndef GEOMETRY_H
#define GEOMETRY_H

struct Point
{
    static Point undefined;
    double x;
    double y;

    Point();
    Point(double _x, double _y);
    
    bool operator == (const Point& p) const;
    bool operator != (const Point& p) const;

    double distance(const Point& p) const;
};

class Rectangle
{
public:
    Rectangle();
    Rectangle(double x, double y, double height, double width);
    
    double top() const;
    double bottom() const;
    double left() const;
    double right() const;
    double x() const;
    double y() const;
    double width() const;
    double height() const;

	void setX(double x);
	void setY(double y);

private:
    double m_x;
    double m_y;
    double m_width;
    double m_height;
};

#endif

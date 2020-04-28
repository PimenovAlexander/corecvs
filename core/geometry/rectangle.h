#ifndef RECTANGLE_H_
#define RECTANGLE_H_
/**
 * \file rectangle.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author alexander
 */

#include <iostream>

#include "math/vector/vector2d.h"
#include "geometry/convexPolyhedron.h"

namespace corecvs {

/* This should be unifed with AxisAlignedBounding box*/
template<typename ElementType>
class Rectangle
{
public:
    Rectangle() {}
   ~Rectangle() {}

    Vector2d<ElementType> corner;
    Vector2d<ElementType> size;

    Rectangle(const Vector2d<ElementType> &_corner, const Vector2d<ElementType> &_size)
        : corner(_corner)
        , size(_size)
    {}

    Rectangle(ElementType _x, ElementType _y, ElementType _w, ElementType _h)
        : corner(_x, _y)
        , size  (_w, _h)
    {}

    void setWidth(ElementType width) 
    {
        size.x() = width;
    }

    void setHeight(ElementType height)
    {
        size.y() = height;
    }

    void extendToFit(const Vector2d<ElementType> &point)
    {
        if (isEmpty()) {
            size.x() = 0;
            size.y() = 0;
            corner = point;
        } else {
            if (point.x() < corner.x()) {
                size.x() += (corner.x() - point.x());
                corner.x() = point.x();
            }
            if (point.y() < corner.y()) {
                size.y() += (corner.y() - point.y());
                corner.y() = point.y();
            }


            if (point.x() > right())
                size.x() = point.x() - corner.x();
            if (point.y() > bottom())
                size.y() = point.y() - corner.y();
        };
    }

    void extendToFit(const std::vector<Vector2d<ElementType>> &points)
    {
        for (auto &p : points)
        {
            extendToFit(p);
        }
    }

    bool contains(const Vector2d<ElementType> &point) const
    {
        return point.isInHypercube(corner, corner + size);
    }

    template<typename OtherRectType>
    bool contains(const OtherRectType &rect) const
    {
        if (rect.left() < left()) return false;
        if (rect.top () < top ()) return false;

        if (rect.right() > right()) return false;
        if (rect.bottom() > bottom()) return false;
        return true;
    }

    void extend (const ElementType& value)
    {
        if (isEmpty())
            return;

        corner.x() -= value;
        corner.y() -= value;
        size.x() += 2 * value;
        size.y() += 2 * value;
    }

    Rectangle extendedBy(const ElementType& value)
    {
        Rectangle toReturn = *this;
        toReturn.extend(value);
        return toReturn;
    }

    void scale(const ElementType& value)
    {
        corner *= value;
        size   *= value;
    }

    bool isEmpty() const
    {
        return size.x() == 0 && size.y() == 0 && corner.x() == 0 && corner.y() == 0;
    }

    ElementType height() const
    {
        return size.y();
    }

    ElementType width() const
    {
        return size.x();
    }

    ElementType left() const 
    { 
        return corner.x(); 
    }

    ElementType top() const 
    { 
        return corner.y(); 
    }

    ElementType right() const 
    {
        return corner.x() + size.x();
    }

    ElementType bottom() const 
    {
        return corner.y() + size.y();
    }

    Vector2d<ElementType> ulCorner() const
    {
        return corner;
    }

    Vector2d<ElementType> urCorner() const
    {
       return Vector2d<ElementType>(corner.x() + size.x(), corner.y());
    }

    Vector2d<ElementType> llCorner() const
    {
        return Vector2d<ElementType>(corner.x(), corner.y() + size.y());
    }

    Vector2d<ElementType> lrCorner() const
    {
       return corner + size;
    }

    Vector2d<ElementType> center() const
    {
       return corner + (size / 2);
    }


    static Rectangle SquareFromCenter(const Vector2d<ElementType> &center, ElementType radius)
    {
        return Rectangle(center - Vector2d<ElementType>(radius, radius), Vector2d<ElementType>(2 * radius, 2 * radius));
    }

    static Rectangle SquareFromCenter(const Vector2d<ElementType> &center, const Vector2d<ElementType> &radius)
    {
        return Rectangle(center - radius, 2 * radius);
    }

    static Rectangle FromCorners(const Vector2d<ElementType> &corner1, const Vector2d<ElementType> &corner2)
    {
        return Rectangle(corner1, corner2 - corner1);
    }

    static Rectangle FromCorners(const ElementType &x1, const ElementType &y1,
                                 const ElementType &x2, const ElementType &y2)
    {
        return Rectangle(x1, y1, x2 - x1, y2 - y1);
    }

    static Rectangle FromSize(const Vector2d<ElementType> &_size)
    {
        return Rectangle(Vector2d<ElementType>::Zero(), _size);
    }

    static Rectangle FromSize(const ElementType &width, const ElementType &height)
    {
        return Rectangle(0, 0, width, height);
    }

    static Rectangle Empty()
    {
        return Rectangle(0,0,0,0);
    }


    Rectangle intersect (Rectangle &r1)
    {
        ElementType newLeft  = std::max(left() , r1.left());
        ElementType newRight = std::min(right(), r1.right());

        ElementType newTop    = std::max(top()   , r1.top());
        ElementType newBottom = std::min(bottom(), r1.bottom());

        if (newLeft > newRight)
            return Empty();

        if (newTop > newBottom)
            return Empty();

        return FromCorners(newLeft, newTop, newRight, newBottom);
    }

    vector<Vector2dd> getPoints() const {
        vector<Vector2dd> result;
        result.reserve(4);
        result.push_back(ulCorner());
        result.push_back(urCorner());
        result.push_back(lrCorner());
        result.push_back(llCorner());
        return result;
    }

    ConvexPolygon toConvexPolygon() const
    {
        ConvexPolygon toReturn;
        toReturn.faces.push_back(Line2d::FormNormalAndPoint( Vector2dd::OrtY(), ulCorner() ));
        toReturn.faces.push_back(Line2d::FormNormalAndPoint(-Vector2dd::OrtY(), lrCorner() ));

        toReturn.faces.push_back(Line2d::FormNormalAndPoint( Vector2dd::OrtX(), ulCorner() ));
        toReturn.faces.push_back(Line2d::FormNormalAndPoint(-Vector2dd::OrtX(), lrCorner() ));
        return toReturn;
    }

    friend std::ostream & operator <<(std::ostream &out, const Rectangle &rect)
    {
        out << rect.corner << " -> " << rect.size;
        return out;
    }

    friend bool operator==(const Rectangle &rect1, const Rectangle &rect2)
    {
        return rect1.corner == rect2.corner && rect1.size == rect2.size;
    }

    const static unsigned  int CLIP_NO_INTERSECTIONS    = 0x0;
    const static unsigned  int CLIP_HAD_INTERSECTIONS   = 0x1;

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(corner, Vector2d<ElementType>(0), "corner");
        visitor.visit(size,   Vector2d<ElementType>(0), "size"  );
    }

    int clipCohenSutherland(Vector2d32 &lineStart, Vector2d32 &lineEnd) const;

private:

    const static unsigned  int LEFT_CODE   = 0x1;
    const static unsigned  int RIGHT_CODE  = 0x2;
    const static unsigned  int TOP_CODE    = 0x4;
    const static unsigned  int BOTTOM_CODE = 0x8;

    inline int getPointCode (const Vector2d32 &p) const
    {
        return ((p.x() < corner.x()) ? LEFT_CODE   : 0)            |
               ((p.x() > corner.x() + size.x()) ? RIGHT_CODE  : 0) |
               ((p.y() < corner.y()) ? BOTTOM_CODE   : 0)          |
               ((p.y() > corner.y() + size.y()) ? TOP_CODE  : 0);
    }

};

typedef Rectangle<int32_t> Rectangle32;
typedef Rectangle<double>  Rectangled;
typedef Rectangle<float>   Rectanglef;

} //namespace corecvs

#endif /* RECTANGLE_H_ */

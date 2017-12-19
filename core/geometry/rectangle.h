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

#include "core/math/vector/vector2d.h"

namespace corecvs {

template<typename ElementType>
class Rectangle
{
public:
    Rectangle() {}
   ~Rectangle() {}

    Vector2d<ElementType> corner;
    Vector2d<ElementType> size;

    Rectangle(Vector2d<ElementType> _corner, Vector2d<ElementType> _size)
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

    void extendToFit(Vector2d<ElementType> &point)
    {
        if (isEmpty()) {
            size.x() = 0;
            size.y() = 0;
            corner = point;
        } else {
            if (point.x() < corner.x())
                corner.x() = point.x();
            if (point.y() < corner.y())
                corner.y() = point.y();


            if (point.x() > right())
                size.x() = point.x() - corner.x();
            if (point.y() > bottom())
                size.y() = point.y() - corner.y();
        };
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

    static Rectangle SquareFromCenter(const Vector2d<ElementType> &center, ElementType radius)
    {
        return Rectangle(center - Vector2d<ElementType>(radius, radius), Vector2d<ElementType>(2 * radius, 2 * radius));
    }

    static Rectangle FromCorners(const Vector2d<ElementType> &corner1, const Vector2d<ElementType> &corner2)
    {
        return Rectangle(corner1, corner2 - corner1);
    }

    static Rectangle Empty()
    {
        return Rectangle(0,0,0,0);
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
typedef Rectangle<double> Rectangled;

} //namespace corecvs

#endif /* RECTANGLE_H_ */

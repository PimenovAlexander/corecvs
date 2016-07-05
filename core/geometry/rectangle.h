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
#include "vector2d.h"
namespace corecvs {

template<typename ElementType>
class Rectangle
{
public:
    Rectangle() {}
    ~Rectangle() {}

    Vector2d<ElementType> corner;
    Vector2d<ElementType> size;
    Rectangle(const Vector2d<ElementType> &_corner, const Vector2d<ElementType> &_size) :
        corner(_corner),
        size(_size)
    {}

    Rectangle(ElementType _x, ElementType _y, ElementType _w, ElementType _h) :
        corner(_x,_y),
        size  (_w,_h)
    {}

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

    friend ostream & operator <<(ostream &out, const Rectangle &rect)
    {
        out << rect.corner << " -> " << rect.size;
        return out;
    }

    const static unsigned  int CLIP_NO_INTERSECTIONS    = 0x0;
    const static unsigned  int CLIP_HAD_INTERSECTIONS   = 0x1;

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit( corner, Vector2d<ElementType>(0), "corner");
        visitor.visit( size,   Vector2d<ElementType>(0), "size");
    }

    int clipCohenSutherland ( Vector2d32 &lineStart, Vector2d32 &lineEnd ) const;

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


#ifndef GEOMETRY_ITERATOR_H
#define GEOMETRY_ITERATOR_H

#include "core/math/mathUtils.h"
#include "core/math/matrix/matrix44.h"
#include "core/geometry/polygons.h"
#include "core/buffers/rgb24/lineSpan.h"

namespace corecvs {

/**
 * Trapezoid 2d iterator. This class is not designed to be directly used, it is more of the
 *  basic functionality for polygon and triangle iterators;
 *
 * For a sake of performance no input checks on the parameters are done. Caller must ensure that
 *
 *  y1 <= y2
 * x11 <= x12
 * x21 <= x22
 **/

class TrapezoidSpanIterator
{
public:
    int y1;
    int y2;

    double x11;
    double x12;

    double x21;
    double x22;

    int currentY;

    double x1, x2;
    double dx1;
    double dx2;

    /*Left attributes*/
    FragmentAttributes  a1;
    FragmentAttributes da1;

    FragmentAttributes a11;
    FragmentAttributes a12;


    /*Right attributes*/
    FragmentAttributes  a2;
    FragmentAttributes da2;

    FragmentAttributes a21;
    FragmentAttributes a22;


    TrapezoidSpanIterator() {}

    TrapezoidSpanIterator(int y1, int y2, int x11, int x12, int x21, int x22) :
        y1(y1) , y2(y2), x11(x11), x12(x12), x21(x21), x22(x22)
    {
        // SYNC_PRINT(("TrapezoidSpanIterator::TrapezoidSpanIterator(%d %d %d %d %d %d): called\n", y1, y2, x11, x12, x21, x22 ));
        currentY = y1;
        double dy = y2 - y1;
        dx1 = (x21 - x11) / dy;
        dx2 = (x22 - x12) / dy;
        x1 = x11;
        x2 = x12;
    }

    TrapezoidSpanIterator(int y1, int y2, double x11, double x12, double x21, double x22) :
        y1(y1) , y2(y2), x11(x11), x12(x12), x21(x21), x22(x22)
    {
        // SYNC_PRINT(("TrapezoidSpanIterator::TrapezoidSpanIterator(%d %d %d %d %d %d): called\n", y1, y2, x11, x12, x21, x22 ));
        currentY = y1;
        double dy = y2 - y1;
        dx1 = (x21 - x11) / dy;
        dx2 = (x22 - x12) / dy;
        x1 = x11;
        x2 = x12;
    }

    void initAttributes()
    {
        double dy = y2 - y1;

        int attributes = (int)a11.size();
        a1.resize(attributes);
        a2.resize(attributes);

        da1.resize(attributes);
        da2.resize(attributes);

        for (int i = 0; i < attributes; i++)
        {

            da1[i] = (a21[i] - a11[i]) / dy;
            da2[i] = (a22[i] - a12[i]) / dy;

            a1[i] = a11[i] /*- da1[i]*/;
            a2[i] = a12[i] /*- da2[i]*/;
        }

#if 0
        SYNC_PRINT(("Zone Attributes:\n"));
        SYNC_PRINT(("Top Left :"));
        for (int i = 0; i < attributes; i++) SYNC_PRINT(("%lf ", a11[i]));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("Top Right:"));
        for (int i = 0; i < attributes; i++) SYNC_PRINT(("%lf ", a12[i]));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("Bottom Left :"));
        for (int i = 0; i < attributes; i++) SYNC_PRINT(("%lf ", a21[i]));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("Bottom Right:"));
        for (int i = 0; i < attributes; i++) SYNC_PRINT(("%lf ", a22[i]));
        SYNC_PRINT(("\n\n"));

        SYNC_PRINT(("Left Delta:"));
        for (int i = 0; i < attributes; i++) SYNC_PRINT(("%lf ", da1[i]));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("Right Delta:"));
        for (int i = 0; i < attributes; i++) SYNC_PRINT(("%lf ", da2[i]));
        SYNC_PRINT(("\n\n"));
#endif
    }

    /**
     * Step to next line in iterator
     * if hasValue() returned false result of call to this function is undefined
     **/
    void step()
    {
        currentY++;
        x1 += dx1;
        x2 += dx2;

        int attributes = (int)a11.size();
        for (int i = 0; i < attributes; i++)
        {
            a1[i] += da1[i];
            a2[i] += da2[i];
        }
    }

    /**
     * Step to line Y in iterator
     * if hasValue() returned false or Y <= currentY result of call to this function is undefined
     **/
    void stepTo(int Y)
    {
        int stepY = (Y - currentY);
        currentY = Y;
        x1 += dx1 * stepY;
        x2 += dx2 * stepY;

        int attributes = (int)a11.size();
        for (int i = 0; i < attributes; i++)
        {
            a1[i] += da1[i] * stepY;
            a2[i] += da2[i] * stepY;
        }
    }

    bool hasValue() {
        return (currentY <= y2);
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        y = currentY;
        x1 = fround(this->x1);
        x2 = fround(this->x2);
    }

    void getSpan(int &y, double &x1, double &x2)
    {
        y = currentY;
        x1 = this->x1;
        x2 = this->x2;
    }

    HLineSpanInt getSpan()
    {
        HLineSpanInt span;
        getSpan(span.cy, span.x1, span.x2);
        return span;
    }

    HLineSpanDouble getDSpan()
    {
        HLineSpanDouble span;
        getSpan(span.cy, span.x1, span.x2);
        return span;
    }

    int getY()
    {
        return currentY;
    }

    int getX1()
    {
        return fround(x1);
    }

    int getX2()
    {
        return fround(x2);
    }

    /**
     * C++ style iteration
     **/
    TrapezoidSpanIterator &begin() {
        return *this;
    }

    TrapezoidSpanIterator & end() {
        return *this;
    }

    bool operator !=(const TrapezoidSpanIterator & other) {
        return this->currentY <= other.y2;
    }

    HLineSpanInt operator *() {
        return getSpan();
    }

    void operator ++() {
        step();
    }

};


template<class TriangleType>
class GenericTriangleSpanIterator
{
public:
    TriangleType sortedt;
    TrapezoidSpanIterator part;

    GenericTriangleSpanIterator() {}

    GenericTriangleSpanIterator(const TriangleType &triangle)
    {
        sortedt = triangle;
        for (int i = 0; i < sortedt.SIZE; i++) {
            sortedt.p[i] = Vector2dd(fround(sortedt.p[i].x()), fround(sortedt.p[i].y()));
        }
        sortedt.sortByY();

        double longslope = (sortedt.p3().x() - sortedt.p1().x()) / (sortedt.p3().y() - sortedt.p1().y());
        double centerx1 = sortedt.p1().x() + longslope * (sortedt.p2().y() - sortedt.p1().y());
        double centerx2 = sortedt.p2().x();

        if (centerx1 > centerx2)
            std::swap(centerx1, centerx2);
        part = TrapezoidSpanIterator(sortedt.p1().y(), sortedt.p2().y(), sortedt.p1().x(), sortedt.p1().x(), centerx1, centerx2);
    }

    void step()
    {
        part.step();

        if (!part.hasValue() && hasValue())
        {
//            SYNC_PRINT(("Changing iterator: %d %d\n", part.currentY >= sortedt.p3.y()));
            part = TrapezoidSpanIterator(sortedt.p2().y(), sortedt.p3().y(), part.x21, part.x22, sortedt.p3().x(), sortedt.p3().x());
            if (part.hasValue()) {
                part.step();
            }
            return;
        }

    }

    bool hasValue() {
        return part.currentY <= sortedt.p3().y();
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        part.getSpan(y, x1, x2);
    }

    HLineSpanInt getSpan()
    {
        return part.getSpan();
    }

    /**
     * C++ style iteration
     **/
    GenericTriangleSpanIterator &begin() {
        return *this;
    }

    GenericTriangleSpanIterator & end() {
        return *this;
    }

    bool operator !=(const GenericTriangleSpanIterator & other) {
        return this->currentY <= other.sortedt.p3().y();
    }

    HLineSpanInt operator *() {
        return getSpan();
    }

    void operator ++() {
        step();
    }


};

typedef GenericTriangleSpanIterator<Triangle2dd> TriangleSpanIterator;

class AttributedPoint : public Vector2dd
{
public:
    AttributedPoint() {}

    AttributedPoint(const Vector2dd &base) : Vector2dd(base) {}

    FragmentAttributes attributes;
};

typedef GenericTriangle<AttributedPoint> AttributedTriangle;




class TrianglePointIterator {
public:
    TriangleSpanIterator it;
    HLineSpanInt spanIt;

    TrianglePointIterator(const Triangle2dd &tr) :
        it(tr),
        spanIt(it.getSpan())
    {
    }

    TrianglePointIterator(const Vector2dd &p1, const Vector2dd &p2, const Vector2dd &p3) :
        it(Triangle2dd(p1,p2,p3)),
        spanIt(it.getSpan())
    {
    }

    void step() {
        spanIt.step();
        if (!spanIt.hasValue()) {
            it.step();
            if (it.hasValue()) {
                spanIt = it.getSpan();
            } else {
                spanIt = HLineSpanInt::Empty();
            }
        }
        //while(spanIt.step())
    }

    bool hasValue() {
        return spanIt.hasValue();
    }

    Vector2d<int> pos() {
        return spanIt.pos();
    }

    /**
     * C++ style iteration
     **/
    TrianglePointIterator &begin() {
        return *this;
    }

    TrianglePointIterator & end() {
        return *this;
    }

    bool operator !=(const TrianglePointIterator & /*other*/) {
        return this->hasValue();
    }

    Vector2d<int> operator *() {
        return pos();
    }

    void operator ++() {
        step();
    }

};

} // namespace

#endif

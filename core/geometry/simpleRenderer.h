#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

/**
 *  \file  simpleRenderer.h
 **/

#include "mathUtils.h"
#include "matrix44.h"
#include "polygons.h"
#include "abstractBuffer.h"
#include "abstractPainter.h"


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

    int x11;
    int x12;

    int x21;
    int x22;

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
        x1 = x11 - dx1;
        x2 = x12 - dx2;
    }

    void initAttributes()
    {
        double dy = y2 - y1;

        int attributes = a11.size();
        a1.resize(attributes);
        a2.resize(attributes);

        da1.resize(attributes);
        da2.resize(attributes);

        for (int i = 0; i < attributes; i++)
        {

            da1[i] = (a21[i] - a11[i]) / dy;
            da2[i] = (a22[i] - a12[i]) / dy;

            a1[i] = a11[i] - da1[i];
            a2[i] = a12[i] - da2[i];
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

    bool step()
    {
        currentY++;
        x1 += dx1;
        x2 += dx2;

        int attributes = a11.size();
        for (int i = 0; i < attributes; i++)
        {
            a1[i] += da1[i];
            a2[i] += da2[i];
        }
        return (currentY <= y2);
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        y = currentY;
        x1 = fround(this->x1);
        x2 = fround(this->x2);
    }

    LineSpanInt getSpan()
    {
        LineSpanInt span;
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
        sortedt.sortByY();

        double longslope = (sortedt.p3().x() - sortedt.p1().x()) / (sortedt.p3().y() - sortedt.p1().y());
        double centerx1 = sortedt.p1().x() + longslope * (sortedt.p2().y() - sortedt.p1().y());
        double centerx2 = sortedt.p2().x();

        if (centerx1 > centerx2)
            std::swap(centerx1, centerx2);
        part = TrapezoidSpanIterator(sortedt.p1().y(), sortedt.p2().y(), sortedt.p1().x(), sortedt.p1().x(), centerx1, centerx2);
    }

    bool step()
    {
        if (!part.step())
        {
//            SYNC_PRINT(("Changing iterator: %d %d\n", part.currentY >= sortedt.p3.y()));
            if (part.currentY >= sortedt.p3().y()) {
                return false;
            }
            part = TrapezoidSpanIterator(sortedt.p2().y(), sortedt.p3().y(), part.x21, part.x22, sortedt.p3().x(), sortedt.p3().x());
            return part.step();
        }
        return true;
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        part.getSpan(y, x1, x2);
    }

    LineSpanInt getSpan()
    {
        return part.getSpan();
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


class AttributedTriangleSpanIterator : public GenericTriangleSpanIterator<AttributedTriangle>
{
public:
    typedef GenericTriangleSpanIterator<AttributedTriangle> BaseClass;

    AttributedTriangleSpanIterator(const AttributedTriangle &triangle)
    {
        sortedt = triangle;
        sortedt.sortByY();

        double longslope = (sortedt.p3().x() - sortedt.p1().x()) / (sortedt.p3().y() - sortedt.p1().y());
        double centerx1 = sortedt.p1().x() + longslope * (sortedt.p2().y() - sortedt.p1().y());
        double centerx2 = sortedt.p2().x();

        FragmentAttributes att1(sortedt.p1().attributes.size());
        for(size_t i = 0; i < att1.size(); i++)
        {
            double da = (sortedt.p3().attributes[i] - sortedt.p1().attributes[i]) / (sortedt.p3().y() - sortedt.p1().y());
            att1[i] = sortedt.p1().attributes[i] + da * (sortedt.p2().y() - sortedt.p1().y());
        }

        FragmentAttributes att2 = sortedt.p2().attributes;

        if (centerx1 > centerx2) {
            std::swap(centerx1, centerx2);
            std::swap(    att1,     att2);
        }

        part = TrapezoidSpanIterator(sortedt.p1().y(), sortedt.p2().y(), sortedt.p1().x(), sortedt.p1().x(), centerx1, centerx2);
        part.a11 = sortedt.p1().attributes;
        part.a12 = part.a11;
        part.a21 = att1;
        part.a22 = att2;

        part.initAttributes();

    }

    bool step()
    {
        if (!part.step())
        {
//            SYNC_PRINT(("Changing iterator: %d %d\n", part.currentY >= sortedt.p3.y()));
            if (part.currentY >= sortedt.p3().y()) {
                return false;
            }
            TrapezoidSpanIterator newPart = TrapezoidSpanIterator(sortedt.p2().y(), sortedt.p3().y(), part.x21, part.x22, sortedt.p3().x(), sortedt.p3().x());
            newPart.a11 = part.a21;
            newPart.a12 = part.a22;
            newPart.a21 = sortedt.p3().attributes;
            newPart.a22 = newPart.a21;
            part = newPart;
            part.initAttributes();
            return part.step();
        }
        return true;
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        part.getSpan(y, x1, x2);
    }

    LineSpanInt getSpan()
    {
        return part.getSpan();
    }

    AttributedLineSpan getAttrSpan()
    {
        AttributedLineSpan span(part.getY(), part.getX1(), part.getX2(), part.a1, part.a2 );
#if 0
        SYNC_PRINT(("Span Attributes:\n"));
        SYNC_PRINT(("Left :"));
        for (size_t i = 0; i < span.att1.size(); i++) SYNC_PRINT(("%lf ", span.att1[i]));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("Right:"));
        for (size_t i = 0; i < span.att2.size(); i++) SYNC_PRINT(("%lf ", span.att2[i]));
        SYNC_PRINT(("\n"));
#endif

        return span;
    }


};


class AttributedTrianglePointIterator {

    AttributedTriangleSpanIterator it;
    AttributedLineSpan spanIt;

    AttributedTrianglePointIterator(const AttributedTriangle &tr) :
        it(tr)
    {

    }

    AttributedTrianglePointIterator(const Vector2dd &p1, const Vector2dd &p2, const Vector2dd &p3) :
        it(AttributedTriangle(p1,p2,p3))
    {

    }
/*
    bool step() {
        //while(spanIt.step())
    }

    Vector2d<int> pos() {

    }
*/
};

class Mesh3D;
class RGB24Buffer;

class SimpleRenderer
{
public:
    SimpleRenderer();
    Matrix44 modelviewMatrix;

    bool backfaceClip;
    bool drawFaces;
    bool drawEdges;
    bool drawVertexes;


    AbstractBuffer<double> *zBuffer;
    void render (Mesh3D *mesh, RGB24Buffer *buffer);

    /* Add support for face and vertex shaders */
    void fragmentShader(AttributedLineSpan & span);

    ~SimpleRenderer();
};

} // namespace corecvs

#endif // SIMPLERENDERER_H

#ifndef ATTRIBUTEDTRIANGLESPANITERATOR_H
#define ATTRIBUTEDTRIANGLESPANITERATOR_H

#include "geometryIterator.h"

namespace corecvs {

#if 0
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
private:
    void initLowerPart()
    {
        TrapezoidSpanIterator newPart = TrapezoidSpanIterator(sortedt.p2().y(), sortedt.p3().y(), part.x21, part.x22, sortedt.p3().x(), sortedt.p3().x());
        newPart.a11 = part.a21;
        newPart.a12 = part.a22;
        newPart.a21 = sortedt.p3().attributes;
        newPart.a22 = newPart.a21;
        part = newPart;
        part.initAttributes();
    }
public:
    void step()
    {
        if (!part.hasValue())
        {
            initLowerPart();
            return;
        }
        part.step();
    }

    void stepTo(int Y)
    {
        if (Y >= sortedt.p2().y())
        {
            initLowerPart();
            part.stepTo(Y);
            return;
        }
        part.stepTo(Y);
    }

    bool hasValue() {
        return (part.currentY <= sortedt.p3().y());
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        part.getSpan(y, x1, x2);
    }

    HLineSpanInt getSpan()
    {
        return part.getSpan();
    }

    AttributedHLineSpan getAttrSpan()
    {
        AttributedHLineSpan span(part.getY(), part.getX1(), part.getX2(), part.a1, part.a2 );
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
#endif


class AttributedTriangleSpanIterator : public GenericTriangleSpanIterator<AttributedTriangle>
{
public:
    typedef GenericTriangleSpanIterator<AttributedTriangle> BaseClass;

    AttributedTriangleSpanIterator(const AttributedTriangle &triangle)
    {
        sortedt = triangle;

        sortedt.p1().y() = fround(sortedt.p1().y());
        sortedt.p2().y() = fround(sortedt.p2().y());
        sortedt.p3().y() = fround(sortedt.p3().y());

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
private:
    void initLowerPart()
    {
        TrapezoidSpanIterator newPart = TrapezoidSpanIterator(sortedt.p2().y(), sortedt.p3().y(), part.x21, part.x22, sortedt.p3().x(), sortedt.p3().x());
        newPart.a11 = part.a21;
        newPart.a12 = part.a22;
        newPart.a21 = sortedt.p3().attributes;
        newPart.a22 = newPart.a21;
        part = newPart;
        part.initAttributes();
    }
public:
    void step()
    {
        part.step();
        if (!part.hasValue() && hasValue())
        {
            initLowerPart();
            if (part.hasValue()) {
                part.step();
            }
            return;
        }
    }

    void stepTo(int Y)
    {
        part.stepTo(Y);
        if (!part.hasValue() && hasValue())
        {
            initLowerPart();
            if (part.hasValue()) {
                part.stepTo(Y);
            }
            return;
        }
    }



    bool hasValue() {
        return (part.currentY <= sortedt.p3().y());
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        part.getSpan(y, x1, x2);
    }

    HLineSpanInt getSpan()
    {
        return part.getSpan();
    }

    HLineSpanDouble getDSpan()
    {
        return part.getDSpan();
    }

    AttributedHLineSpan getAttrSpan()
    {
        AttributedHLineSpan span(part.getY(), part.getX1(), part.getX2(), part.a1, part.a2 );
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


} // namespace corecvs


#endif // ATTRIBUTEDTRIANGLESPANITERATOR_H

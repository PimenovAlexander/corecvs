#ifndef POLYGONPOINTITERATOR_H
#define POLYGONPOINTITERATOR_H

#include "core/geometry/polygons.h"
#include "core/geometry/simpleRenderer.h"

namespace corecvs {

/**
 * So far we only support convex polygon
 **/
class PolygonSpanIterator
{
public:
    const Polygon &polygon;

    TrapezoidSpanIterator part;

    int indexDelta;

    int cand1;
    int cand2;

    int deep  ;
    int shallow;

    vector<int>  sortedIndex;
    vector<bool> side;

    Vector2dd getPoint(int i) {
        return polygon[sortedIndex[i]];
    }

    PolygonSpanIterator(const Polygon &polygon) : polygon(polygon)
    {
        /* Prepare the sorted array. We don't need is do far, we only need max and min. But non-convex polygon support */
        side.resize(polygon.size());
        sortedIndex.reserve(polygon.size());
        for (unsigned i = 0; i < polygon.size(); i++)
            sortedIndex.push_back(i);

        std::sort(sortedIndex.begin(), sortedIndex.end(), [=](int a, int b) { return polygon[a].y() < polygon[b].y(); });
        int idx = sortedIndex.front() + 1;
        while (idx != sortedIndex.back()) {
            side[idx] = true;
            idx = (idx + 1) % polygon.size();
        }

        /** So far only convex poligons are supported **/
        bool orientation = true;
        bool isConvex = polygon.isConvex(&orientation);
        if (!isConvex) {
           part.currentY =  polygon.y(sortedIndex.back()) + 1;
           return;
        }

        if (orientation)
            indexDelta = 1;
        else
            indexDelta = -1;

        /**/

        int currentIndex = sortedIndex[0];
        Vector2dd origin = polygon[currentIndex];

        cand1 = (int)((currentIndex + polygon.size() + indexDelta) % polygon.size()); /* Right slope driver */
        cand2 = (int)((currentIndex + polygon.size() - indexDelta) % polygon.size()); /* Left  slope driver */

        deep    = cand1;
        shallow = cand2;

#if 0
        cout << "Polygon size:"  << polygon.size() << endl;
        cout << "Top Point:"    << origin <<  endl;
        cout << "Bottom Point:" << polygon[sortedIndex.back()] << endl;
        cout << "Left  Point:"  << polygon[cand1] <<  endl;
        cout << "Right Point:"  << polygon[cand2] <<  endl;
        cout << "Indexes:" << currentIndex << " " << deep << " " << shallow << endl;
#endif

        if (polygon.y(deep) < polygon.y(shallow)) std::swap(deep, shallow);

        double longslope = (polygon.x(deep) - origin.x()) / (polygon.y(deep) - origin.y());
        double centerx1 = origin.x() + longslope * (polygon.y(shallow) - origin.y());
        double centerx2 = polygon.x(shallow);

        if (centerx2 < centerx1) std::swap(centerx2, centerx1);


        part = TrapezoidSpanIterator(origin.y(), polygon.y(shallow), origin.x(), origin.x(), centerx1, centerx2);

    }

    void step()
    {
        // SYNC_PRINT(("PolygonSpanIterator::step(): called\n"));
        part.step();

        while (!part.hasValue() && hasValue())
        {
            int cand1n = cand1;
            int cand2n = cand2;
            //bool leftStep = false;

            Vector2dd  leftBegin = Vector2dd(part.x21, part.y2);
            Vector2dd rightBegin = Vector2dd(part.x22, part.y2);

            if (cand1 == shallow) {
                /*Left end finished */
                //leftStep = true;
                cand1n = (int)((cand1 + polygon.size() + indexDelta) % polygon.size()); /* Right slope driver */
            } else {
                /* Right end finished*/
                cand2n = (int)((cand2 + polygon.size() - indexDelta) % polygon.size()); /* Left  slope driver */
            }

            deep    = cand1n;
            shallow = cand2n;
            Vector2dd deepBegin    = leftBegin;
            Vector2dd shallowBegin = rightBegin;
#if 0
            cout << "We need to advance: " << endl;
            cout << "Left  Begin: " << leftBegin << endl;
            cout << "Right Begin: " << rightBegin << endl;

            cout << "Left  End" << polygon[cand1n] << endl;
            cout << "Right End" << polygon[cand2n] << endl;
#endif
            if (polygon.y(deep) < polygon.y(shallow)) {
                std::swap(deep, shallow);
                std::swap(deepBegin, shallowBegin);
            }

#if 0
            if (polygon.y(deep) - deepBegin.y()) {
                SYNC_PRINT(("We are in prefail...\n"));
            }
#endif

            double longslope = (polygon.x(deep) - deepBegin.x()) / (polygon.y(deep) - deepBegin.y());

            double newx1 = deepBegin.x() + longslope * (polygon.y(shallow) - shallowBegin.y());
            double newx2 = polygon.x(shallow);

            if (newx2 < newx1) std::swap(newx2, newx1);

            /**/
            part = TrapezoidSpanIterator(part.y2, polygon.y(shallow), part.x21, part.x22, newx1, newx2);

            cand1 = cand1n;
            cand2 = cand2n;

            if (part.hasValue()) {
                // SYNC_PRINT(("Part has values. stepping to %d\n", part.currentY));
                part.step();
                // SYNC_PRINT(("Part has values. stepped to %d\n", part.currentY));
            } else {
                // SYNC_PRINT(("Part is Empty. Skipping...\n"));
                part.currentY++;
            }           
        }
    }

    void getSpan(int &y, int &x1, int &x2)
    {
        part.getSpan(y, x1, x2);
    }

    LineSpanInt getSpan()
    {
        LineSpanInt span;
        getSpan(span.cy, span.x1, span.x2);
        return span;
    }

    bool hasValue() {
        return (part.currentY <= polygon.y(sortedIndex.back()));
    }

    /**
     * C++ style iteration
     **/
    PolygonSpanIterator &begin() {
        return *this;
    }

    PolygonSpanIterator & end() {
        return *this;
    }

    bool operator !=(const PolygonSpanIterator & /*other*/) {
        return this->hasValue();
    }

    LineSpanInt operator *() {
        return getSpan();
    }

    void operator ++() {
        step();
    }

};

class PolygonFanPointIterator
{
public:
    const Polygon &polygon;
    size_t petle;
    Triangle2dd triangle;
    TrianglePointIterator it;


    Triangle2dd getTriangle(size_t petle) {
        return Triangle2dd(polygon[0], polygon[petle - 1], polygon[petle]);
    }

    PolygonFanPointIterator(const Polygon &polygon) :
        polygon(polygon),
        petle(2),
        triangle(getTriangle(petle)),
        it(triangle)
    {}

    void step() {
        it.step();
        if (!it.hasValue()) {
            petle++;
            if (hasValue()) {
                triangle = getTriangle(petle);
                it = TrianglePointIterator(triangle);
            }
        }
    }

    bool hasValue() {
        return petle < polygon.size();
    }

    Vector2d<int> pos() {
        return it.pos();
    }


    /**
     * C++ style iteration
     **/
    PolygonFanPointIterator &begin() {
        return *this;
    }

    PolygonFanPointIterator & end() {
        return *this;
    }

    bool operator !=(const PolygonFanPointIterator & /*other*/) {
        return this->hasValue();
    }

    Vector2d<int> operator *() {
        return pos();
    }

    void operator ++() {
        step();
    }

};

class PolygonPointIterator
{
public:
    const Polygon &polygon;
    PolygonSpanIterator it;
    LineSpanInt spanIt;

    PolygonPointIterator(const Polygon &polygon) :
        polygon(polygon),
        it(polygon),
        spanIt(it.getSpan())
    {}

    void step() {
        spanIt.step();
        if (!spanIt.hasValue()) {
            it.step();
            if (it.hasValue()) {
                spanIt = it.getSpan();
            } else {
                spanIt = LineSpanInt::Empty();
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
    PolygonPointIterator &begin() {
        return *this;
    }

    PolygonPointIterator & end() {
        return *this;
    }

    bool operator !=(const PolygonPointIterator & /*other*/) {
        return this->hasValue();
    }

    Vector2d<int> operator *() {
        return pos();
    }

    void operator ++() {
        step();
    }
};

} // namespace corecvs

#endif // POLYGONPOINTITERATOR_H

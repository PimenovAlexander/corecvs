#ifndef LINE_SPAN_H
#define LINE_SPAN_H

#include <algorithm>
#include <vector>
#include <ostream>

#include "core/math/vector/vector2d.h"
#include "core/geometry/line.h"


namespace corecvs {


class HLineSpanInt {
public:
    int cy;
    int x1;
    int x2;

    HLineSpanInt() {}

    HLineSpanInt(int y, int x1, int x2) :
        cy(y), x1(x1), x2(x2)
    {}

    static HLineSpanInt Empty() {
        return HLineSpanInt(0, 1, 0);
    }


    void clip(int w, int h) {
        if (x1 > x2) std::swap(x1, x2);
        if (x1 <  0) x1 = 0;
        if (x2 >= w) x2 = w - 1;

        if (x1 >= w || x2 < 0 || cy < 0 || cy >= h ) {
            x1 = 1;
            x2 = 0;
        }
    }

    void step() {
        x1++;
    }

    bool hasValue() {
        return (x1 <= x2);
    }

    int x() {
        return x1;
    }

    int y() {
        return cy;
    }

    Vector2d<int> pos() {
        return Vector2d<int>(x(), y());
    }

    /**
     * C++ style iteration
     **/
    HLineSpanInt &begin() {
        return *this;
    }

    HLineSpanInt & end() {
        return *this;
    }

    bool operator !=(const HLineSpanInt & other) {
        return this->x1 <= (other.x2);
    }

    Vector2d<int> operator *() {
        return pos();
    }

    void operator ++() {
        step();
    }

    /**
     * Utility functions
     **/

    friend std::ostream& operator << (std::ostream &out, HLineSpanInt &toSave)
    {
        out << toSave.y() << " [" << toSave.x1 << " -> " << toSave.x2 << "]" << std::endl;
        return out;
    }

};

typedef std::vector<double> FragmentAttributes;


inline static ostream & operator <<(ostream &out, const FragmentAttributes &vector)
{
    out << "[";
    for (size_t i = 0; i < vector.size(); i++)
       out << (i == 0 ? "" : ", ") << vector.at(i);
    out << "]";
    return out;
}

class AttributedHLineSpan : public HLineSpanInt {
public:
    FragmentAttributes catt;
    FragmentAttributes datt;

    AttributedHLineSpan() : HLineSpanInt(0,1,0) {}

    AttributedHLineSpan(int y, int x1, int x2, const FragmentAttributes &a1, const FragmentAttributes &a2) :
        HLineSpanInt(y, x1, x2),
        catt(a1)
    {
        datt.resize(catt.size());
        for (size_t i = 0; i < datt.size(); i++) {
            datt[i] = (a2[i] - a1[i]) / (x2 - x1);
        }
    }

    void step() {
        for (size_t i = 0; i < catt.size(); i++) {
            catt[i] += datt[i];
        }
        HLineSpanInt::step();
    }

    bool hasValue() {
        return HLineSpanInt::hasValue();
    }

    const FragmentAttributes &att() {
        return catt;
    }

    AttributedHLineSpan &begin() {
        return *this;
    }

    AttributedHLineSpan & end() {
        return *this;
    }

    void operator ++() {
        step();
    }
};

/* Simple itretrator. Not optimized for speed */

class LineSpanIterator {
public:
    int x1;
    int y1;

    int x2;
    int y2;

    double dx;
    double dy;
    double cx;
    double cy;

    int count = 0;

    LineSpanIterator(int x1, int y1, int x2, int y2) :
        x1(x1), y1(y1), x2(x2), y2(y2)
    {
        cx = x1;
        cy = y1;
        dx = x2 - x1;
        dy = y2 - y1;

        double adx = fabs(dx);
        double ady = fabs(dy);
        if (adx > ady)
        {
            count = adx;
            dy /= adx;
            dx /= adx;
        } else {
            count = ady;
            dx /= ady;
            dy /= ady;
        }
    }

    LineSpanIterator(const Segment<Vector2d32> &segment) :
        LineSpanIterator(
            segment.a.x(), segment.a.y(),
            segment.b.x(), segment.b.y())
    {
    }

    int x() {
        return cx;
    }

    int y() {
        return cy;
    }

    Vector2d<int> pos() {
        return Vector2d<int>(x(), y());
    }

    void step() {
        count--;
        cx += dx;
        cy += dy;
    }

    bool hasValue() {
        return (count > 0);
    }

    /**
     * C++ style iteration
     **/
    LineSpanIterator &begin() {
        return *this;
    }

    LineSpanIterator & end() {
        return *this;
    }

    bool operator !=(const LineSpanIterator & /*other*/) {
        return this->hasValue();
    }

    void operator ++() {
        step();
    }

    Vector2d<int> operator *() {
        return pos();
    }

};

class AttributedLineSpanIterator : public LineSpanIterator {
public:
    FragmentAttributes catt;
    FragmentAttributes datt;

    //AttributedLineSpanIterator() : LineSpanIterator(0,1,0) {}

    AttributedLineSpanIterator(int x1, int y1, int x2, int y2,
                               const FragmentAttributes &a1,
                               const FragmentAttributes &a2
                              ) :
        LineSpanIterator(x1, y1, x2, y2),
        catt(a1)
    {
        datt.resize(catt.size());
        if (count != 0) {
            for (size_t i = 0; i < datt.size(); i++) {
                datt[i] = (a2[i] - a1[i]) / count;
            }
        }

//        SYNC_PRINT(("AttributedLineSpanIterator::AttributedLineSpanIterator(): called\n"));
//        cout << "CATTR" << catt << std::endl;
//        cout << "DATTR" << datt << std::endl;

    }

    void step() {
//        SYNC_PRINT(("AttributedLineSpanIterator::step(): called\n"));
        for (size_t i = 0; i < catt.size(); i++) {
            catt[i] += datt[i];
        }
//        cout << "CATTR" << catt << std::endl;
        LineSpanIterator::step();
    }

    bool hasValue() {
        return LineSpanIterator::hasValue();
    }

    const FragmentAttributes &att() {
        return catt;
    }

    void operator ++() {
//        SYNC_PRINT(("AttributedLineSpanIterator::operator ++(): called\n"));
        AttributedLineSpanIterator::step();
    }

    AttributedLineSpanIterator &begin() {
        return *this;
    }

    AttributedLineSpanIterator & end() {
        return *this;
    }




};


} // namespace corecvs

#endif // LINE_SPAN_H

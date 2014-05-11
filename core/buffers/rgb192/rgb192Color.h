#ifndef RGB192COLOR_H
#define RGB192COLOR_H

#include "vector3d.h"
#include "rgbColor.h"

namespace corecvs {
/**
 *   I use inheritance because no new data members will be added
 **/

class RGB192Color : public Vector3dd
{
public:
    enum FieldId {
        FIELD_R = 0,
        FIELD_G = 1,
        FIELD_B = 2
    };

    RGB192Color() {}

    explicit RGB192Color(const Vector3dd &vector) :  Vector3dd(vector)
    {
    }

    RGB192Color(double _r, double _g, double _b)
    {
        this->r() = _r;
        this->g() = _g;
        this->b() = _b;
    }

    inline double &r()
    {
        return (*this)[2];
    }

    inline double &g()
    {
        return (*this)[1];
    }

    inline double &b()
    {
        return (*this)[0];
    }

    /* Constant versions for read-only form const colors */
    inline const double &r() const
    {
        return (*this)[2];
    }

    inline const double &g() const
    {
        return (*this)[1];
    }

    inline const double &b() const
    {
        return (*this)[0];
    }


    static Reflection reflect;

    static Reflection staticInit()
    {
        Reflection reflection;
        reflection.fields.push_back( new DoubleField(FIELD_R, 0, "r") );
        reflection.fields.push_back( new DoubleField(FIELD_G, 0, "g") );
        reflection.fields.push_back( new DoubleField(FIELD_B, 0, "b") );
        return reflection;
    }

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(r(), static_cast<const DoubleField *>(reflect.fields[FIELD_R]));
        visitor.visit(g(), static_cast<const DoubleField *>(reflect.fields[FIELD_G]));
        visitor.visit(b(), static_cast<const DoubleField *>(reflect.fields[FIELD_B]));
    }

    static RGB192Color FromRGBColor(const RGBColor &color)
    {
        return RGB192Color(color.toDouble());
    }

};

}

#endif // RGB192COLOR_H

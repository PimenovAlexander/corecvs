/**
* \file rgbColor.h
* \brief RGBColor template class definition
*
* \date Oct 15, 2015
* \author alexander
* \author pavel.vasilev
*/
#ifndef CRGBTCOLOR_H_
#define CRGBTCOLOR_H_

#include <stdint.h>

#include "core/math/vector/fixedVector.h"
#include "core/math/vector/vector3d.h"
#include "core/math/mathUtils.h"

#include "core/xml/generated/rgbColorParameters.h"

namespace corecvs {

/**
*   I use inheritance because no new data members will be added
**/
template<typename T>
class RGBTColor : public FixedVector<T, 4>
{
public:
    typedef FixedVector<T, 4> RGBTColorBase;

    enum FieldId
    {
        FIELD_R = 2,
        FIELD_G = 1,
        FIELD_B = 0,
        FIELD_A = 3
    };

    /**
    *
    *
    * */
    explicit RGBTColor(T color)
    {
        const int shift = sizeof(T);
        const uint64_t max = (1 << (shift * 8)) - 1;
        r() = color & max;
        g() = (color >> shift * 8) & max;
        b() = (color >> shift * 16) & max;
        a() = 0;
    }

    explicit RGBTColor(const Vector3d<T> &other)
    {
        r() = other[0];
        g() = other[1];
        b() = other[2];
        a() = 0;
    }

    RGBTColor(T _r, T _g, T _b)
    {
        this->r() = _r;
        this->g() = _g;
        this->b() = _b;
        this->a() = 0;
    }

    RGBTColor(T _r, T _g, T _b, T _a)
    {
        this->r() = _r;
        this->g() = _g;
        this->b() = _b;
        this->a() = _a;
    }

    RGBTColor(const RgbColorParameters &color)
    {
        this->r() = color.r();
        this->g() = color.g();
        this->b() = color.b();
        this->a() = 0;
    }

    RGBTColor(const RGBTColorBase& base) : RGBTColorBase(base) {}

    RGBTColor()
    {
    }

    inline T &r()
    {
        return (*this)[FIELD_R];
    }

    inline T &g()
    {
        return (*this)[FIELD_G];
    }

    inline T &b()
    {
        return (*this)[FIELD_B];
    }

    inline T &a()
    {
        return (*this)[FIELD_A];
    }

    inline uint64_t &color()
    {
        return (*(uint64_t *)this);
    }

    /* Constant versions for read-only form const colors */
    inline const T &r() const
    {
        return (*this)[FIELD_R];
    }

    inline const T &g() const
    {
        return (*this)[FIELD_G];
    }

    inline const T &b() const
    {
        return (*this)[FIELD_B];
    }

    inline const T &a() const
    {
        return (*this)[FIELD_A];
    }

    inline uint64_t &color() const
    {
        return (*(uint64_t *)this);
    }

    inline T brightness() const
    {
        /** We add 1 to the sum, because
        *     0,  1,  0 -> 0
        *     0,  1,  1 -> 1
        *   255,255,254 -> 255
        */
        return (sum() + 1) / 3;
    }

    inline uint32_t sum() const
    {
        return (r() + g() + b());
    }

    // Y = 0.2126 R + 0.7152 G + 0.0722 B
    inline T luma() const
    {
        return (11 * r() + 16 * g() + 5 * b()) >> 4;
    }

    // Y = 0.2126 R + 0.7152 G + 0.0722 B
    inline uint16_t luma12() const
    {
        return (5 * (int)r() + 8 * (int)g() + 2 * (int)b());
        //return (11 * r() + 16 * g() + 5 * b()) >> 1;
    }

    inline double yd() const 
    {
        return 0.299 * r() + 0.587 * g() + 0.114 * b();
    }

    inline double yh() const
    {
        return (116. * r() + 232. * g() + 116. * b()) / 464.;
    }

    inline int32_t y() const
    {
        return fround(yd());
    }

    inline int32_t cb() const
    {
        return (int)(-0.16874 * r() - 0.33126 * g() + 0.50000 * b());
    }

    inline int32_t cr() const
    {
        return (int)(0.50000 * r() - 0.41869 * g() - 0.08131 * b());
    }

    /**
    *  TODO: I should test the consitency of conversions
    *
    * Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16
    * U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128
    * V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128
    *
    *
    **/
    inline T u() const
    {
        return ((-38 * r() - 74 * g() + 112 * b() + 128) >> 8) + 128;
    }

    inline T v() const
    {
        return ((112 * r() - 94 * g() - 18 * b() + 128) >> 8) + 128;
    }

    /*TODO: Move out common code */

    inline T value() const
    {
        T m;
        T M;

        if (r() > g())
        {
            m = g();
            M = r();
        }
        else
        {
            M = g();
            m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();
        return M;
    }

    inline T chroma() const
    {
        T m;
        T M;

        if (r() > g())
        {
            m = g();
            M = r();
        }
        else
        {
            M = g();
            m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();
        return M - m;
    }

    inline uint8_t saturation() const
    {
        T m;
        T M;

        if (r() > g())
        {
            m = g();
            M = r();
        }
        else
        {
            M = g();
            m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();
        return M == 0 ? 0 : (M - m) * 255 / M;
    }

    inline uint16_t hue() const
    {
        T m;
        T M;

        int16_t hue;

        if (r() > g())
        {
            m = g();
            M = r();
        }
        else
        {
            M = g();
            m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();

        T c = M - m;
        if (c == 0) return 0;
        if (M == r())
        {
            hue = (g() - b()) * 60 / c;
            if (hue < 0) hue += 360;
            return hue;
        }
        if (M == g())
        {
            hue = (b() - r()) * 60 / c + 120;
            return hue;
        }
        /* M == b() */
        hue = (r() - g()) * 60 / c + 240;
        return hue;
    }

    static RGBTColor gray(T gray)
    {
        return RGBTColor(gray, gray, gray);
    }

    static RGBTColor gray12(uint16_t gray)
    {
        return RGBTColor(gray >> 4, gray >> 4, gray >> 4);
    }

    /**
    *  C = Y - 16
    *  D = U - 128
    *  E = V - 128
    *  Using the previous coefficients and noting that clamp() denotes clamping a value to the range of 0 to 255, the following formulae provide the conversion from YUV to RGB (NTSC version):
    *  R = clamp(( 298 \times C                + 409 \times E + 128) >> 8)
    *  G = clamp(( 298 \times C - 100 \times D - 208 \times E + 128) >> 8)
    *  B = clamp(( 298 \times C + 516 \times D                + 128) >> 8)
    **/
    // TODO: generalize this!
    static RGBTColor<uint8_t> FromYUV(int y, int u, int v)
    {
        int c = y - 16;
        int d = u - 128;
        int e = v - 128;

        int r = ((298 * c + 409 * e + 128) >> 8);
        int g = ((298 * c - 100 * d - 208 * e + 128) >> 8);
        int b = ((298 * c + 516 * d + 128) >> 8);

        /*        int r =  ((298 * c           + 409 * e + 128) / 256);
        int g =  ((298 * c - 100 * d - 208 * e + 128) / 256);
        int b =  ((298 * c + 516 * d           + 128) / 256);*/


        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;

        return RGBTColor<uint8_t>(r, g, b);
    }

    static RGBTColor Black()
    {
        return RGBTColor(0, 0, 0);
    }

    static RGBTColor White()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(max, max, max);
    }

    static RGBTColor Red()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(max, 0, 0);
    }

    static RGBTColor Orange()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(max, max >> 1, 0);
    }

    static RGBTColor Yellow()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(max, max, 0);
    }

    static RGBTColor Green()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(0, max, 0);
    }

    static RGBTColor Cyan()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(0, max, max);
    }

    static RGBTColor Magenta()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(max, 0, max);
    }

    static RGBTColor Blue()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(0, 0, max);
    }

    static RGBTColor Indigo()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(111 * max / 255, 0, max);
    }

    static RGBTColor Violet()
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        return RGBTColor(143 * max / 255, 0, max);
    }

    static RGBTColor lerpColor(const RGBTColor &first, const RGBTColor &second, double alpha)
    {
        T r = (T)lerp<double>(first.r(), second.r(), alpha);
        T g = (T)lerp<double>(first.g(), second.g(), alpha);
        T b = (T)lerp<double>(first.b(), second.b(), alpha);
        return RGBTColor(r, g, b);
    }

    static RGBTColor diff(const RGBTColor &first, const RGBTColor &second)
    {
        int64_t r = (int64_t)first.r() - (int64_t)second.r();
        int64_t g = (int64_t)first.g() - (int64_t)second.g();
        int64_t b = (int64_t)first.b() - (int64_t)second.b();

        return RGBTColor(CORE_ABS(r), CORE_ABS(g), CORE_ABS(b));
    }

    /**
    *  Helper method that allows to represent the double value in interval 0..1
    *  With a real rainbow color coding.
    *  <ul>
    <li>0.0    Red (web color) (Hex: \#FF0000) (RGB: 255, 0, 0)
    <li>1/6    Orange (color wheel Orange) (Hex: \#FF7F00) (RGB: 255, 127, 0)
    <li>2/6    Yellow (web color) (Hex: \#FFFF00) (RGB: 255, 255, 0)
    <li>3/6    Green (X11) (Electric Green) (HTML/CSS “Lime”) (Color wheel green) (Hex: \#00FF00) (RGB: 0, 255, 0)
    <li>4/6    Blue (web color) (Hex: \#0000FF) (RGB: 0, 0, 255)
    <li>5/6    Indigo (Electric Indigo) (Hex: \#6600FF) (RGB: 111, 0, 255)
    <li>1.0    Violet (Electric Violet) (Hex: \#8B00FF) (RGB: 143, 0, 255)
    </ul>
    **/
    static RGBTColor rainbow(double x)
    {
        x *= 6;
        if (x < 0.0) x = 0.0;
        if (x > 6.0) x = 6.0;

        switch ((int)x)
        {
        case 0:
            return lerpColor(Red(), Orange(), x);
        case 1:
            return lerpColor(Orange(), Yellow(), x - 1);
        case 2:
            return lerpColor(Yellow(), Green(), x - 2);
        case 3:
            return lerpColor(Green(), Blue(), x - 3);
        case 4:
            return lerpColor(Blue(), Indigo(), x - 4);
        case 5:
        default:
            return lerpColor(Indigo(), Violet(), x - 5);
        }
    }

    /**
    *  Helper method that allows to represent the double value in interval 0..1
    *  With a color coding.
    *  <ul>
    *    <li>0.0  blue
    *    <li>0.5  green
    *    <li>1.0  red
    *  </ul>
    **/
    static RGBTColor rainbow1(double x)
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        x *= 2;
        if (x < 0.0) x = 0.0;
        if (x > 2.0) x = 2.0;

        if (x < 1.0)
        {
            return RGBTColor(0, fround(x * max), fround((1.0 - x) * max));
        }
        x -= 1.0;
        return RGBTColor(fround(x * max), fround((1.0 - x) * max), 0);
    }

    static Reflection reflect;
    static int dummy;
    static int staticInit()
    {
        reflect.fields.push_back(new IntField(FIELD_R, 0, "r"));
        reflect.fields.push_back(new IntField(FIELD_G, 0, "g"));
        reflect.fields.push_back(new IntField(FIELD_B, 0, "b"));
        reflect.fields.push_back(new IntField(FIELD_A, 0, "a"));
        return 0;
    }

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(r(), static_cast<const IntField *>(reflect.fields[FIELD_R]));
        visitor.visit(g(), static_cast<const IntField *>(reflect.fields[FIELD_G]));
        visitor.visit(b(), static_cast<const IntField *>(reflect.fields[FIELD_B]));
        visitor.visit(a(), static_cast<const IntField *>(reflect.fields[FIELD_A]));
    }

    friend ostream & operator <<(ostream &out, const RGBTColor &color)
    {
        out << "[";
        out << (uint64_t)color.r() << ", " << (uint64_t)color.g() << ", " << (uint64_t)color.b() << " (" << (uint64_t)color.a() << ")";
        out << "]";
        return out;
    }

    friend istream & operator >>(istream &out, RGBTColor &color)
    {
        T v;
        out >> v;
        color.r() = v;
        out >> v;
        color.g() = v;
        out >> v;
        color.b() = v;
        return out;
    }

    Vector3dd toDouble() const
    {
        return Vector3dd(r(), g(), b());
    }

    uint64_t toRGBInt() const
    {
        const int shift = 8 * sizeof(T);
        return ((uint64_t)r() << (2 * shift)) | ((uint64_t)g() << shift) | ((uint64_t)b());
    }

    uint64_t toBRGInt() const
    {
        const int shift = 8 * sizeof(T);
        return ((uint64_t)b() << (2 * shift)) | ((uint64_t)g() << shift) | ((uint64_t)r());
    }

    static RGBTColor FromDouble(const Vector3dd &input)
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        Vector3dd input1 = input;
        input1.mapToHypercube(Vector3dd(0.0, 0.0, 0.0), Vector3dd(max, max, max));
        return RGBTColor(input.x(), input.y(), input.z());
    }

    static RGBTColor FromHSV(uint16_t h, T s, T v)
    {
        const uint64_t max = (1 << (sizeof(T) * 8)) - 1;
        int c = ((int)(s * v)) / max;
        int m = v - c;
        int r, g, b;

        int swh = h / 60;
        int dh = h - swh * 60.0;

        int x1 = c * dh / 60;
        int x2 = c - x1;


        switch (swh)
        {
        case 0:
            r = c;  g = x1; b = 0; break;
        case 1:
            r = x2; g = c;  b = 0; break;
        case 2:
            r = 0;  g = c;  b = x1; break;
        case 3:
            r = 0;  g = x2; b = c; break;
        case 4:
            r = x1; g = 0;  b = c; break;
        case 5:
        default:
            r = c;  g = 0;  b = x2; break;
        }

        return RGBTColor(r + m, g + m, b + m);
    }

};

typedef RGBTColor<uint16_t> RGBColor48;

#ifdef REFLECTION_IN_CORE
    template<typename T>
    int RGBTColor<T>::dummy = staticInit();
#else
    template<typename T>
    const Reflection RGBTColor<T>::reflect;
#endif

} //namespace corecvs


#endif // CRGBTCOLOR_H_

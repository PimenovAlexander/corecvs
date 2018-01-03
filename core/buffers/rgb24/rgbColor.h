#pragma once
/**
 * \file rgbColor.h
 * \brief a header for rgbColor.c
 *
 * \date Apr 19, 2011
 * \author alexander
 */

#include <stdint.h>
#include <cmath>

#include "core/math/vector/fixedVector.h"
#include "core/math/vector/vector3d.h"
#include "core/math/mathUtils.h"
#include "core/reflection/reflection.h"

#include "core/xml/generated/rgbColorParameters.h"

namespace corecvs {
/**
 * \file rgbColor.h
 * \brief a header for rgbColor.c
 *
 * \date Apr 19, 2011
 * \author alexander
 */
/**
 *   I use inheritance because no new data members will be added
 **/
//class RGBColor : public FixedVectorBase<RGBColor, uint8_t,4> {
class RGBColor : public FixedVector<uint8_t, 4> {
public:

    typedef FixedVector<uint8_t, 4> RGBColorBase;

    enum FieldId {
        FIELD_R = 2,
        FIELD_G = 1,
        FIELD_B = 0,
        FIELD_A = 3
    };

    /**
     *
     *
     * */
    explicit RGBColor(uint32_t color)
    {
        r() = color & 0xFF;
        g() = (color >> 8) & 0xFF;
        b() = (color >> 16) & 0xFF;
        a() = 0;
    }
    /*TODO: Make this a template*/
    explicit RGBColor(const Vector3d32 &other)
    {
        r() = other[0];
        g() = other[1];
        b() = other[2];
        a() = 0;
    }

    RGBColor(uint8_t _r, uint8_t _g, uint8_t _b)
    {
        this->r() = _r;
        this->g() = _g;
        this->b() = _b;
        this->a() = 0;
    }

    RGBColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
    {
        this->r() = _r;
        this->g() = _g;
        this->b() = _b;
        this->a() = _a;
    }

    RGBColor(const RgbColorParameters &color)
    {
        this->r() = color.r();
        this->g() = color.g();
        this->b() = color.b();
        this->a() = 0;
    }

    RGBColor(const RGBColorBase& base) : RGBColorBase(base) {}

    RGBColor()
    {
    }

    inline uint8_t &r()
    {
        return (*this)[FIELD_R];
    }

    inline uint8_t &g()
    {
        return (*this)[FIELD_G];
    }

    inline uint8_t &b()
    {
        return (*this)[FIELD_B];
    }

    inline uint8_t &a()
    {
        return (*this)[FIELD_A];
    }

    inline uint32_t &color()
    {
        return (*(uint32_t *)this);
    }

    /* Constant versions for read-only form const colors */
    inline const uint8_t &r() const
    {
        return (*this)[FIELD_R];
    }

    inline const uint8_t &g() const
    {
        return (*this)[FIELD_G];
    }

    inline const uint8_t &b() const
    {
        return (*this)[FIELD_B];
    }

    inline const uint8_t &a() const
    {
        return (*this)[FIELD_A];
    }

    inline uint32_t &color() const
    {
       return (*(uint32_t *)this);
    }


    inline uint8_t brightness() const
    {
        /** We add 1 to the sum, because
         *     0,  1,  0 -> 0
         *     0,  1,  1 -> 1
         *   255,255,254 -> 255
         */
        return (sum() + 1) / 3;
    }

    inline uint16_t sum() const
    {
        return ((uint16_t)r() + (uint16_t)g() + (uint16_t)b());
    }

    // Y = 0.2126 R + 0.7152 G + 0.0722 B
    inline uint8_t luma() const
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

    inline int16_t y() const
    {
        return fround(yd());
    }

    inline int16_t cb() const
    {
        return (int)(-0.16874 * r() - 0.33126 * g() + 0.50000 * b());
    }

    inline int16_t cr() const
    {
        return (int)( 0.50000 * r() - 0.41869 * g() - 0.08131 * b());
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

    inline int16_t u() const
    {
        return ( ( -38 * r() -  74 * g() + 112 * b() + 128) >> 8) + 128;
    }

    inline int16_t v() const
    {
        return ( ( 112 * r() -  94 * g() -  18 * b() + 128) >> 8) + 128;
    }


    /*TODO: Move out common code */

    inline uint8_t value() const
    {
       uint8_t m;
       uint8_t M;

       if (r() > g())
       {
         m = g();
         M = r();
       } else {
         M = g();
         m = r();
       }
       if (m > b()) m = b();
       if (M < b()) M = b();
       return M;
    }

    inline uint8_t chroma() const
    {
        uint8_t m;
        uint8_t M;

        if (r() > g())
        {
          m = g();
          M = r();
        } else {
          M = g();
          m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();
        return M - m;
    }

    inline uint8_t saturation() const
    {
        uint8_t m;
        uint8_t M;

        if (r() > g())
        {
          m = g();
          M = r();
        } else {
          M = g();
          m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();
        return M == 0 ? 0 : (M - m) * 255 / M;
    }


    inline uint16_t hue() const
    {
        uint8_t m;
        uint8_t M;

        int16_t hue;

        if (r() > g())
        {
            m = g();
            M = r();
        } else {
            M = g();
            m = r();
        }
        if (m > b()) m = b();
        if (M < b()) M = b();

        uint8_t c = M - m;
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

    /**
     * hue \in [0; 360)
     * saturation \in [0; 1)
     * value \in [0; 1)
     */
    static RGBColor fromHue(double hue, double saturation, double value)
    {
        hue        = std::min(std::max(0.0, hue), 360.0 - 1e-10);
        saturation = std::min(std::max(0.0, saturation), 1.0);
        value      = std::min(std::max(0.0, value), 1.0);

        double chroma = value * saturation;
        hue /= 60.0;
        double largest = chroma * (1.0 - std::abs(hue - 2.0 * floor(hue / 2.0) - 1.0));
        int h = (int)floor(hue);
        double min = value - chroma;
        double r = min, g = min, b = min;

        switch(h)
        {
            case 0:
                r += chroma;
                g += largest;
                break;
            case 1:
                r += largest;
                g += chroma;
                break;
            case 2:
                g += chroma;
                b += largest;
                break;
            case 3:
                g += largest;
                b += chroma;
                break;
            case 4:
                r += largest;
                b += chroma;
                break;
            case 5:
                r += chroma;
                b += largest;
                break;
        }

        return RGBColor((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
    }

    /**
     * Depricated
     **/
    static RGBColor gray(uint8_t gray)
    {
        return RGBColor(gray, gray, gray);
    }

    static RGBColor Gray(uint8_t gray = 127)
    {
        return RGBColor(gray, gray, gray);
    }


    static RGBColor gray12(uint16_t gray)
    {
        return RGBColor(gray >> 4, gray >> 4, gray >> 4);
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
    static RGBColor FromYUV(uint8_t y, uint8_t u, uint8_t v)
    {
        int c = y - 16;
        int d = u - 128;
        int e = v - 128;

        int r =  ((298 * c           + 409 * e + 128) >> 8);
        int g =  ((298 * c - 100 * d - 208 * e + 128) >> 8);
        int b =  ((298 * c + 516 * d           + 128) >> 8);

        return RGBColor(clamp(r, 0, 255), clamp(g, 0, 255), clamp(b, 0, 255));
    }

    /**
    *  Format "Y′UV420sp (NV21) to RGB conversion (Android)" of the article at:
    *  https://en.wikipedia.org/wiki/YUV#Y.E2.80.B2UV420p_.28and_Y.E2.80.B2V12_or_YV12.29_to_RGB888_conversion
    *
    **/
    static RGBColor FromYUV420sp(uint8_t y, uint8_t u, uint8_t v)
    {
        int d = u - 128;
        int e = v - 128;

        int r = y + roundShiftUp(          1404 * e, 10);
        int g = y - roundShiftUp( 346 * d + 715 * e, 10);
        int b = y + roundShiftUp(1774 * d          , 10);

        return RGBColor(clamp(r, 0, 255), clamp(g, 0, 255), clamp(b, 0, 255));
    }

    static RGBColor Black()
    {
        return RGBColor(0, 0, 0);
    }

    static RGBColor White()
    {
        return RGBColor(255, 255, 255);
    }

    static RGBColor Red()
    {
        return RGBColor(255, 0, 0);
    }

    static RGBColor Orange()
    {
        return RGBColor(255, 127, 0);
    }

    static RGBColor Yellow()
    {
        return RGBColor(255, 255, 0);
    }

    static RGBColor Goldenrod()
    {
        return RGBColor(218, 165, 32);
    }

    static RGBColor Khaki()
    {
        return RGBColor(195, 176, 145);
    }

    static RGBColor Green()
    {
        return RGBColor(0, 255, 0);
    }

    static RGBColor Cyan()
    {
        return RGBColor(0, 255, 255);
    }

    static RGBColor Magenta()
    {
        return RGBColor(255, 0, 255);
    }

    static RGBColor Blue()
    {
        return RGBColor(0, 0, 255);
    }

    static RGBColor TiffanyBlue()
    {
        return RGBColor(129, 216, 208);
    }

    static RGBColor Indigo()
    {
        return RGBColor(111, 0, 255);
    }

    static RGBColor Violet()
    {
        return RGBColor(143, 0, 255);
    }

    static RGBColor Pink()
    {
        return RGBColor(255, 192, 203);
    }

    static RGBColor Navy()
    {
        return RGBColor(0, 0, 127);
    }

    static RGBColor Brown()
    {
        return RGBColor(150, 75, 0);
    }

    static RGBColor Amber()
    {
        return RGBColor(255, 191, 0);
    }


    static RGBColor lerpColor(const RGBColor &first, const RGBColor &second, double alpha)
    {
        uint8_t r = (uint8_t)lerp<double>(first.r(), second.r(), alpha);
        uint8_t g = (uint8_t)lerp<double>(first.g(), second.g(), alpha);
        uint8_t b = (uint8_t)lerp<double>(first.b(), second.b(), alpha);
        return RGBColor(r, g, b);
    }

    /**
     *    This function is slow. Use for debugging and fancy effects only.
     **/
    void blendWith(const RGBColor &second, double alpha = 0.5)
    {
        r() = (uint8_t)lerp<double>(r(), second.r(), alpha);
        g() = (uint8_t)lerp<double>(g(), second.g(), alpha);
        b() = (uint8_t)lerp<double>(b(), second.b(), alpha);
    }

    static RGBColor diff(const RGBColor &first, const RGBColor &second)
    {
        int16_t r = (int16_t)first.r() - (int16_t)second.r();
        int16_t g = (int16_t)first.g() - (int16_t)second.g();
        int16_t b = (int16_t)first.b() - (int16_t)second.b();

        return RGBColor(CORE_ABS(r), CORE_ABS(g), CORE_ABS(b));
    }

    RGBColor operator *(float f) {
        return RGBColor(r() * f, g() * f, b() * f);
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
    static RGBColor rainbow(double x);

    /**
     *  Helper method that allows to represent the double value in interval 0..1
     *  With a color coding.
     *  <ul>
     *    <li>0.0  blue
     *    <li>0.5  green
     *    <li>1.0  red
     *  </ul>
     **/
    static RGBColor rainbow1(double x)
    {
        x *= 2;
        if (x < 0.0) x = 0.0;
        if (x > 2.0) x = 2.0;

        if (x < 1.0)
        {
            return RGBColor(0, fround(x * 255), fround((1.0 - x) * 255));
        }
        x -= 1.0;
        return RGBColor(fround(x * 255), fround((1.0 - x) * 255), 0);
    }

    static Reflection reflect;
    static int        dummy;
    static int        staticInit();

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        int byte;
        byte = r(); visitor.visit(byte, static_cast<const IntField *>(reflect.fields[FIELD_R])); r() = byte;
        byte = g(); visitor.visit(byte, static_cast<const IntField *>(reflect.fields[FIELD_G])); g() = byte;
        byte = b(); visitor.visit(byte, static_cast<const IntField *>(reflect.fields[FIELD_B])); b() = byte;
        byte = a(); visitor.visit(byte, static_cast<const IntField *>(reflect.fields[FIELD_A])); a() = byte;
    }

    friend ostream & operator <<(ostream &out, const RGBColor &color)
    {
        out << "[";
            out << (int)color.r() << ", " << (int)color.g() << ", " << (int)color.b() << " (" << (int)color.a() << ")";
        out << "]";
        return out;
    }

    friend std::istream & operator >>(std::istream &out, RGBColor &color)
    {
       int v;
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

    Vector3df toFloat() const
    {
        return Vector3df(r(), g(), b());
    }

    RgbColorParameters toRGBParameters() const
    {
        return RgbColorParameters(r(), g(), b());
    }

    uint32_t toRGBInt() const
    {
        return ((uint32_t)r() << 16) | ((uint32_t)g() << 8) | ((uint32_t)b());
    }
    uint32_t toBGRInt() const
    {
        return ((uint32_t)b() << 16) | ((uint32_t)g() << 8) | ((uint32_t)r());
    }
    uint32_t toBRGInt() const
    {
        return ((uint32_t)b() << 16) | ((uint32_t)g() << 8) | ((uint32_t)r());
    }

    static RGBColor FromDouble(const Vector3dd &input)
    {
        Vector3dd input1 = input;
        input1.mapToHypercube(Vector3dd(0.0, 0.0, 0.0), Vector3dd(255.0, 255.0, 255.0));
        return RGBColor((uint8_t)input1.x(), (uint8_t)input1.y(), (uint8_t)input1.z());
    }

    static RGBColor FromHSV(uint16_t h, uint8_t s, uint8_t v)
    {
        int c = ((int)(s * v)) / 255;
        int m = v - c;
        int r,g,b;

        int swh = h / 60;
        int dh = h - swh * 60;

        int x1 = c * dh / 60;
        int x2 = c - x1;


        switch (swh) {
        case 0:
            r =  c; g = x1; b =  0; break;
        case 1:
            r = x2; g =  c; b =  0; break;
        case 2:
            r =  0; g =  c; b = x1; break;
        case 3:
            r =  0; g = x2; b =  c; break;
        case 4:
            r = x1; g =  0; b =  c; break;
        case 5:
        default:
            r =  c; g =  0; b = x2; break;
        }

        return RGBColor(r + m, g + m, b + m);
    }

};

} //namespace corecvs

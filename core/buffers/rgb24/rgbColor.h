#ifndef RGBCOLOR_H_
#define RGBCOLOR_H_
/**
 * \file rgbColor.h
 * \brief a header for rgbColor.c
 *
 * \date Apr 19, 2011
 * \author alexander
 */

#include <stdint.h>

#include "fixedVector.h"
#include "vector3d.h"
#include "mathUtils.h"

#include "generated/rgbColorParameters.h"

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
class RGBColor : public FixedVectorBase<RGBColor, uint8_t,4> {
public:

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

    RGBColor(const RgbColorParameters &color)
    {
        this->r() = color.r();
        this->g() = color.g();
        this->b() = color.b();
        this->a() = 0;
    }


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
        return (*this)[2];
    }

    inline const uint8_t &g() const
    {
        return (*this)[1];
    }

    inline const uint8_t &b() const
    {
        return (*this)[0];
    }

    inline uint32_t &color() const
    {
       return (*(uint32_t *)this);
    }


    inline uint8_t brightness() const
    {
        /**
         * TODO: Should I add ~1.5 = 2 to the sum?
         * because
         *     0,  1,  1 -> 1
         *   255,255,254 -> 255;
         *
         * */
        return ((uint16_t)r() + (uint16_t)g() + (uint16_t)b()) / 3;
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


    inline int16_t y() const
    {
        return (int)( 0.299   * r() + 0.587   * g() + 0.114   * b());
    }

    inline int16_t cb() const
    {
        return (int)(-0.16874 * r() - 0.33126 * g() + 0.50000 * b());
    }

    inline int16_t cr() const
    {
        return (int)( 0.50000 * r() - 0.41869 * g() - 0.08131 * b());
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

    static RGBColor gray(uint8_t gray)
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

/*        int r =  ((298 * c           + 409 * e + 128) / 256);
        int g =  ((298 * c - 100 * d - 208 * e + 128) / 256);
        int b =  ((298 * c + 516 * d           + 128) / 256);*/


        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;

        return RGBColor(r,g,b);
    }

    static RGBColor Black()
    {
        return RGBColor(0, 0, 0);
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

    static RGBColor Green()
    {
        return RGBColor(0, 255, 0);
    }

    static RGBColor Cyan()
    {
        return RGBColor(0, 255, 255);
    }

    static RGBColor Blue()
    {
        return RGBColor(0, 0, 255);
    }

    static RGBColor Indigo()
    {
        return RGBColor(111, 0, 255);
    }

    static RGBColor Violet()
    {
        return RGBColor(143, 0, 255);
    }

    static RGBColor lerpColor(const RGBColor &first, const RGBColor &second, double alpha)
    {
        uint8_t r = (uint8_t)lerp<double>(first.r(), second.r(),alpha);
        uint8_t g = (uint8_t)lerp<double>(first.g(), second.g(),alpha);
        uint8_t b = (uint8_t)lerp<double>(first.b(), second.b(),alpha);
        return RGBColor(r, g, b);
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
            return RGBColor(0, fround(x * 255) , fround((1.0 - x) * 255));
        }
        x -= 1.0;
        return RGBColor(fround(x * 255) , fround((1.0 - x) * 255), 0 );
    }


    static Reflection reflect;

    static Reflection staticInit()
    {
        Reflection reflection;
        reflection.fields.push_back( new IntField(FIELD_R, 0, "r") );
        reflection.fields.push_back( new IntField(FIELD_G, 0, "g") );
        reflection.fields.push_back( new IntField(FIELD_B, 0, "b") );
        reflection.fields.push_back( new IntField(FIELD_A, 0, "a") );
        return reflection;
    }

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(r(), static_cast<const IntField *>(reflect.fields[FIELD_R]));
        visitor.visit(g(), static_cast<const IntField *>(reflect.fields[FIELD_G]));
        visitor.visit(b(), static_cast<const IntField *>(reflect.fields[FIELD_B]));
        visitor.visit(a(), static_cast<const IntField *>(reflect.fields[FIELD_A]));
    }

    Vector3dd toDouble() const
    {
        return Vector3dd(r(), g(), b());
    }

    static RGBColor FromDouble(const Vector3dd &input)
    {
        Vector3dd input1 = input;
        input1.mapToHypercube(Vector3dd(0.0,0.0,0.0), Vector3dd(255.0,255.0,255.0));
        return RGBColor(input.x(), input.y(), input.z());
    }

};


} //namespace corecvs
#endif  //RGBCOLOR_H_


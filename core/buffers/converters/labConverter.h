/**
* \file labConverter.h
* \brief RGB to Lab converter definitions
*
* \ingroup cppcorefiles
* \date    Nov 5, 2015
* \author  pavel.vasilev
*/

#ifndef LABCONVERTER_H_
#define LABCONVERTER_H_

#include "core/utils/global.h"
#include "core/buffers/rgb24/rgbTColor.h"
#include "core/buffers/rgb24/rgbColor.h"

namespace corecvs {

class LabConverter
{

private:
    static const float lab_eps;
    static const float lab_coeff;

    static const float srgb2xyz[3][3];

    static const float white[3];

    static inline float GetLabValueL(float Y, float fy)
    {
        if (Y <= lab_eps)                                       // eps = 0.008856451586
            return lab_coeff * Y;                               // if linear functionality was used
        return 116 * fy - 16;                                   // if it was used after the cubic root
    }

    static inline float GetCubeRoot(float t)
    {
        if (t <= lab_eps)
        {                                                 // = 0.008856451586
            return (lab_coeff * t + 16) / 116;            // = ( 903.3 * t + 16 ) / 116
        }
        else
        {
            return cbrt(t);
        }
    }

    static inline void xyz2Lab(float* xyz, float* Lab)
    {
        float fx = GetCubeRoot(xyz[0]);
        float fy = GetCubeRoot(xyz[1]);
        float fz = GetCubeRoot(xyz[2]);

        Lab[0] = GetLabValueL(xyz[1], fy);                  // L
        Lab[1] = float(500) * (fx - fy);                    // a
        Lab[2] = float(200) * (fy - fz);                    // b
    }


    static inline void Lab2xyz(float* Lab, float* xyz)
    {
        float y = (Lab[0] + 16.0) / 116.0;
        float x = Lab[1] / 500.0 + y;
        float z = y - Lab[2] / 200.0;

        float x3 = x * x * x;
        float z3 = z * z * z;
        xyz[0] = white[0] * (x3 > lab_eps ? x3 : (x - 16.0 / 116.0) / 7.787);
        xyz[1] = white[1] * (Lab[0] > (lab_coeff * lab_eps) ? pow(((Lab[0] + 16.0) / 116.0), 3) : Lab[0] / lab_coeff);
        xyz[2] = white[2] * (z3 > lab_eps ? z3 : (z - 16.0 / 116.0) / 7.787);
    }

    template<typename T>
    static inline T clip(double x)
    {
        const int mMaximum = (1 << sizeof(T) * 8) - 1;
        if (x < 0)
            return 0;
        if (x > mMaximum)
            return mMaximum;

        return (T)x;
    }

    template<typename T>
    static inline void xyz2rgb(float* xyz, T* rgb)
    {
        // (Observer = 2°, Illuminant = D65)
        float x = xyz[0] / 100.0;
        float y = xyz[1] / 100.0;
        float z = xyz[2] / 100.0;

        float r = x *  3.2406f + y * -1.5372f + z * -0.4986f;
        float g = x * -0.9689f + y *  1.8758f + z *  0.0415f;
        float b = x *  0.0557f + y * -0.2040f + z *  1.0570f;

        r = r > 0.0031308f ? 1.055f * pow(r, 1 / 2.4f) - 0.055f : 12.92f * r;
        g = g > 0.0031308f ? 1.055f * pow(g, 1 / 2.4f) - 0.055f : 12.92f * g;
        b = b > 0.0031308f ? 1.055f * pow(b, 1 / 2.4f) - 0.055f : 12.92f * b;

        rgb[0] = clip<T>(r * 255);
        rgb[1] = clip<T>(g * 255);
        rgb[2] = clip<T>(b * 255);
    }

public:

    /**
    * Convert RGB to CIELab.
    *
    * \param   RGB RGB pixel
    * \param   Lab CIELab pixel
    *
    */
    template <typename T>
    static inline void rgb2Lab(RGBTColor<T> RGB, float* Lab)
    {
        const float max = (1 << sizeof(T)) - 1;

        float xyz[3];

        for (int i = 0; i < 3; i++)
            xyz[i] = (srgb2xyz[i][0] * RGB.b() + srgb2xyz[i][1] * RGB.g() + srgb2xyz[i][2] * RGB.r()) / max;

        xyz2Lab(xyz, Lab);

    }

    static inline void rgb2Lab(RGBColor RGB, float* Lab)
    {
        const float max = 255;

        float xyz[3];

        for (int i = 0; i < 3; i++)
            xyz[i] = (srgb2xyz[i][0] * RGB.r() + srgb2xyz[i][1] * RGB.g() + srgb2xyz[i][2] * RGB.b()) / max;

        xyz2Lab(xyz, Lab);

    }

    static inline void Lab2rgb(float* Lab, uint16_t* RGB)
    {
        float xyz[3];
        Lab2xyz(Lab, xyz);
        xyz2rgb<uint16_t>(xyz, RGB);
    }
};


} // namespace corecvs

const float LabConverter::lab_eps(0.008856451586f);
const float LabConverter::lab_coeff(903.3f);
const float LabConverter::srgb2xyz[3][3] {
    { 0.412453f, 0.357585f, 0.180423f },
    { 0.212671f, 0.715160f, 0.072169f },
    { 0.019334f, 0.119193f, 0.950227f }
};
const float LabConverter::white[3]{ 95.047f, 100.000f, 108.883f };
#endif //LABCONVERTER_H_
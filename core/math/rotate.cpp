#include <cstdio>
#include <iostream>
#include <string>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/utils/global.h"
#include "core/math/rotate.h"
#include "core/math/matrix/matrixOperations.h"

namespace corecvs {

inline double sinc(double x) {
    x = (x * M_PI);
    return sin(x) / x;
}

inline float sinc(float x) {
    x = (x * M_PI);
    return sin(x) / x;
}

inline double LanczosFilter(double x, double lanczos_size) {
    if (std::abs(x) < 0.0001) {
        return 1.0;
    }
    if (std::abs(x) < lanczos_size) {
        return (sinc(x) * sinc(x / lanczos_size));
    }

    return 0.0;
}

inline float LanczosFilter(float x, float lanczos_size) {
    if (std::abs(x) < 0.0001) {
        return 1.0;
    }
    if (std::abs(x) < lanczos_size) {
        return (sinc(x) * sinc(x / lanczos_size));
    }

    return 0.0;
}

inline float LanczosFilterD(int value)
{

}

inline double maxOf4(double d1, double d2, double d3, double d4){
    return std::max(d1, std::max(d2, std::max(d3, d4)));
}

inline double minOf4(double d1, double d2, double d3, double d4){
    return std::min(d1, std::min(d2, std::min(d3, d4)));
}

RGB24Buffer* RotateHelper::rotateWithLancoz(double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size) {

    Matrix33 operation = // Matrix33::ShiftProj(rgb24buffer->w / 2, rgb24buffer->h / 2)
                         Matrix33::ShiftProj(newW / 2, newH / 2)
                       * Matrix33::RotationZ(angle)
                       * Matrix33::ShiftProj(-rgb24buffer->w / 2, -rgb24buffer->h / 2);
    operation.invert();
    RGB24Buffer *rotated = new RGB24Buffer(newH, newW);


    for (int i = 0; i < rotated->w; i++) {
        for (int j = 0; j < rotated->h; j++) {

            Vector2dd target = operation * Vector2dd(i, j);

            int x_src = target.x();
            int y_src = target.y();

            double weight = 0.0;
            Vector3dd colorSum(0.0);

            for (int i_sample = x_src - lanczos_size + 1; i_sample <= x_src + lanczos_size; ++i_sample) {
                for (int j_sample = y_src - lanczos_size + 1; j_sample <= y_src + lanczos_size; ++j_sample) {

                    double lanc_val = LanczosFilter(target.x() - i_sample, lanczos_size) * LanczosFilter(target.y() - j_sample, lanczos_size);
                    weight += lanc_val;

                    if (rgb24buffer->isValidCoord(j_sample, i_sample)) {
                        colorSum += rgb24buffer->element(j_sample, i_sample).toDouble() * lanc_val;
                    }
                }
            }

            colorSum /= weight;

            int red   = clamp(fround(colorSum.x()), 0, 255);
            int green = clamp(fround(colorSum.y()), 0, 255);
            int blue  = clamp(fround(colorSum.z()), 0, 255);

            rotated->element(j, i) = RGBColor(red, green, blue);
        }

    }

    return rotated;
}



RGB24Buffer* RotateHelper::rotateWithLancozVF(double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size) {

    Matrix33 operation = // Matrix33::ShiftProj(rgb24buffer->w / 2, rgb24buffer->h / 2)
                         Matrix33::ShiftProj(newW / 2, newH / 2)
                       * Matrix33::RotationZ(angle)
                       * Matrix33::ShiftProj(-rgb24buffer->w / 2, -rgb24buffer->h / 2);
    operation.invert();
    RGB24Buffer *rotated = new RGB24Buffer(newH, newW);

    /* fix this */
    float a00 = operation.a(0,0);
    float a01 = operation.a(0,1);
    float a02 = operation.a(0,2);

    float a10 = operation.a(1,0);
    float a11 = operation.a(1,1);
    float a12 = operation.a(1,2);

    float a20 = operation.a(2,0);
    float a21 = operation.a(2,1);
    float a22 = operation.a(2,2);

    float lanczos_size_f = lanczos_size;


    for (int i = 0; i < rotated->h; i++) {
        for (int j = 0; j < rotated->w; j++) {

            Vector2df target(
                a00 * j + a01 * i + a02,
                a10 * j + a11 * i + a12);

            int x_src = target.x();
            int y_src = target.y();

            float weight = 0.0;
            Vector3df colorSum(0.0);

            for (int di = y_src - lanczos_size + 1; di <= y_src + lanczos_size; ++di) {
                for (int dj = x_src - lanczos_size + 1; dj <= x_src + lanczos_size; ++dj) {

                    float lanc_val = LanczosFilter(target.x() - dj, lanczos_size_f) * LanczosFilter(target.y() - di, lanczos_size_f);
                    weight += lanc_val;

                    if (rgb24buffer->isValidCoord(di, dj)) {
                        colorSum += rgb24buffer->element(di, dj).toFloat() * lanc_val;
                    }
                }
            }

            colorSum /= weight;

            int red   = clamp(fround(colorSum.x()), 0, 255);
            int green = clamp(fround(colorSum.y()), 0, 255);
            int blue  = clamp(fround(colorSum.z()), 0, 255);

            rotated->element(i, j) = RGBColor(red, green, blue);
        }

    }

    return rotated;
}


RGB24Buffer* RotateHelper::rotateWithLancozVFP(double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size) {

    Matrix33 operation = // Matrix33::ShiftProj(rgb24buffer->w / 2, rgb24buffer->h / 2)
                         Matrix33::ShiftProj(newW / 2, newH / 2)
                       * Matrix33::RotationZ(angle)
                       * Matrix33::ShiftProj(-rgb24buffer->w / 2, -rgb24buffer->h / 2);
    operation.invert();
    RGB24Buffer *rotated = new RGB24Buffer(newH, newW);

    const int scale = 1024;

    float lut[(int)(lanczos_size + 1) * scale];
    for (size_t i = 0; i < CORE_COUNT_OF(lut); i++) {
        lut[i] = LanczosFilter((float) i / scale, (float)lanczos_size);
//        cout << lut[i] << ", ";
    }
//    cout << endl;


    /* fix this */
    float a00 = operation.a(0,0);
    float a01 = operation.a(0,1);
    float a02 = operation.a(0,2);

    float a10 = operation.a(1,0);
    float a11 = operation.a(1,1);
    float a12 = operation.a(1,2);

    float a20 = operation.a(2,0);
    float a21 = operation.a(2,1);
    float a22 = operation.a(2,2);

    float lanczos_size_f = lanczos_size;


    for (int i = 0; i < rotated->h; i++) {
        for (int j = 0; j < rotated->w; j++) {

            Vector2df target(
                a00 * j + a01 * i + a02,
                a10 * j + a11 * i + a12);

            int x_src = target.x();
            int y_src = target.y();

            float weight = 0.0;
            Vector3df colorSum(0.0);

            for (int di = y_src - lanczos_size + 1; di <= y_src + lanczos_size; di++) {
                for (int dj = x_src - lanczos_size + 1; dj <= x_src + lanczos_size; dj++)
                {
                    int sx = abs((target.x() - dj) * scale);
                    int sy = abs((target.y() - di) * scale);

                    float lanc_val = lut[sx] * lut[sy];
                    weight += lanc_val;

                    if (rgb24buffer->isValidCoord(di, dj)) {
                        colorSum += rgb24buffer->element(di, dj).toFloat() * lanc_val;
                    }
                }
            }

            colorSum /= weight;

            int red   = clamp(fround(colorSum.x()), 0, 255);
            int green = clamp(fround(colorSum.y()), 0, 255);
            int blue  = clamp(fround(colorSum.z()), 0, 255);

            rotated->element(i, j) = RGBColor(red, green, blue);
        }

    }

    return rotated;
}


RGB24Buffer* RotateHelper::rotateWithLancozVFPI(double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size) {

    Matrix33 operation = // Matrix33::ShiftProj(rgb24buffer->w / 2, rgb24buffer->h / 2)
                         Matrix33::ShiftProj(newW / 2, newH / 2)
                       * Matrix33::RotationZ(angle)
                       * Matrix33::ShiftProj(-rgb24buffer->w / 2, -rgb24buffer->h / 2);
    operation.invert();
    RGB24Buffer *rotated = new RGB24Buffer(newH, newW);

    const int scale = 1024;

    //const int pfpos   = 1024;
    const int pfpos   = 1048;

    float    lut [(int)(lanczos_size + 1) * scale];
    int32_t  luti[(int)(lanczos_size + 1) * scale];

    for (size_t i = 0; i < CORE_COUNT_OF(lut); i++) {
        lut [i] = LanczosFilter((double)i / scale, lanczos_size);
        luti[i] = lut[i] * pfpos;

    }

    /* fix this */
    float a00 = operation.a(0,0);
    float a01 = operation.a(0,1);
    float a02 = operation.a(0,2);

    float a10 = operation.a(1,0);
    float a11 = operation.a(1,1);
    float a12 = operation.a(1,2);

    for (int i = 0; i < rotated->h; i++) {
        for (int j = 0; j < rotated->w; j++) {

            Vector2df target(
                a00 * j + a01 * i + a02,
                a10 * j + a11 * i + a12);

            int x_src = target.x();
            int y_src = target.y();

            int32_t    weight = 0;
            Vector3d32 colorSum(0);

            for (int di = y_src - lanczos_size + 1; di <= y_src + lanczos_size; di++)
            {
                for (int dj = x_src - lanczos_size + 1; dj <= x_src + lanczos_size; dj++)
                {
                    int sx = abs((target.x() - dj) * scale);
                    int sy = abs((target.y() - di) * scale);

                    int32_t lanc_val = (luti[sx] * luti[sy]);
                    weight += lanc_val;
                    if (rgb24buffer->isValidCoord(di, dj)) {
                        colorSum += rgb24buffer->element(di, dj).toInt32() * lanc_val;

                    }
                }
            }

            Vector3d32 res(0);
            if (weight != 0) {
                res = (colorSum + Vector3d32(weight / 2)) / weight;
            }

            uint16_t red   = clamp(res.x(), 0, 255);
            uint16_t green = clamp(res.y(), 0, 255);
            uint16_t blue  = clamp(res.z(), 0, 255);

/*
            if (i == 1005 && j == 982) {
                cout << "Strange at " << i << " " << j << endl;
                cout << red << " " << green << " " << blue << endl;
                cout << colorSum << " " << weight << endl;
                cout << res << endl;
            }
*/

            rotated->element(i, j) = RGBColor(red, green, blue);
        }

    }

    return rotated;
}



Rectangled RotateHelper::getTargetSize(const Rectangled &input, const Matrix33 &forwardTransform)
{
    vector<Vector2dd> points = input.getPoints();
    Rectangled target;
    for (const Vector2dd &p : points)
    {
        target.extendToFit(p);
    }
    return target;
}

Rectangled RotateHelper::getTargetSize(const RGB24Buffer &input, const Matrix33 &forwardTransform)
{
    return getTargetSize(Rectangled(0,0, input.w, input.h), forwardTransform);
}

}




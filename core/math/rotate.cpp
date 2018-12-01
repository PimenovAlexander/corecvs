#include <cstdio>
#include <iostream>
#include <string>
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/utils/global.h"
#include "core/math/rotate.h"

namespace corecvs {

inline double sinc(double x) {
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




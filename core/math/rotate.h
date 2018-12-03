#ifndef ROTATE_H
#define ROTATE_H
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {

class RotateHelper {
public:
    static Rectangled getTargetSize(const Rectangled  &input, const Matrix33 &forwardTransform);
    static Rectangled getTargetSize(const RGB24Buffer &input, const Matrix33 &forwardTransform);


    static RGB24Buffer* rotateWithLancoz    (double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size = 3.0);
    static RGB24Buffer* rotateWithLancozVFPI(double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size = 3.0);
    static RGB24Buffer* rotateWithLancozVFP (double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size = 3.0);
    static RGB24Buffer* rotateWithLancozVF  (double angle, const RGB24Buffer *rgb24buffer, int newH, int newW, double lanczos_size = 3.0);
};
}
#endif // ROTATE_H

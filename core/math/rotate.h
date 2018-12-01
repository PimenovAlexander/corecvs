#ifndef ROTATE_H
#define ROTATE_H
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {
    RGB24Buffer* rotate_with_lanczoc_filter(double angle, RGB24Buffer rgb24buffer, double lanczos_size = 3.0);
}
#endif // ROTATE_H

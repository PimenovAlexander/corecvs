#ifndef RGB192BUFFER_H
#define RGB192BUFFER_H
/**
 * \file rgb24Buffer.h
 * \brief a header for RGB24buffer.cpp
 *
 * \ingroup cppcorefiles
 * \date Mar 1, 2010
 * \author alexander
 */


#include "rgb24Buffer.h"
#include "rgb192Color.h"
#include "abstractContiniousBuffer.h"

namespace corecvs {


#define FLAGS_INCLUDE_MARGIN 0x1


typedef AbstractContiniousBuffer<RGB192Color, int32_t> RGB192BufferBase;


class RGB192Buffer : public RGB192BufferBase
{
public:
    RGB192Buffer(RGB192Buffer &that) : RGB192BufferBase (that) {}
    RGB192Buffer(RGB192Buffer *that) : RGB192BufferBase (that) {}

    RGB192Buffer(RGB192Buffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        RGB192BufferBase(src, x1, y1, x2, y2) {}

    RGB192Buffer(int32_t h, int32_t w, bool shouldInit = true) : RGB192BufferBase (h, w, shouldInit) {}
    RGB192Buffer(Vector2d<int32_t> size, bool shouldInit = true) : RGB192BufferBase (size, shouldInit) {}


    RGB192Buffer(RGB24Buffer *that);

    RGB24Buffer *toRGB24Buffer();
    RGB192Buffer *FromRGB24Buffer(RGB24Buffer *that);

    void fillWith(RGB24Buffer *that);
    void copyTo(RGB24Buffer *that);





};

} //namespace corecvs

#endif // RGB192BUFFER_H

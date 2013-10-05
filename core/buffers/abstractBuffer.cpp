/**
 * \file abstractBuffer.cpp
 * \brief TODO
 *
 * \ingroup cppcorefiles
 * \date Dec 21, 2012
 * \author sf
 */
#include "abstractBuffer.h"

namespace corecvs {

static cchar* sFormatStr = "#dims:%d (%d) x %d x %u";

bool_t AbstractBufferParams::dump(ostream& s, int h, int w, int stride, uint elemSize, bool_t binaryMode)
{
    if (binaryMode)
    {
        write_integer_bin(s, w);
        write_integer_bin(s, stride);
        write_integer_bin(s, h);
        write_integer_bin(s, elemSize);
    }
    else
    {
        char buf[256];
        s.write(buf, snprintf2buf(buf, sFormatStr, w, stride, h, elemSize));
        s << endl;
    }
    return !s.bad();
}

bool_t AbstractBufferParams::load(istream& s, int &h, int &w, int &stride, uint elemSize, bool_t binaryMode)
{
    int width, stridePix, height, elSize;
    size_t pos = s.tellg();

    if (binaryMode)
    {
        read_integer_bin(s, width);
        read_integer_bin(s, stridePix);
        read_integer_bin(s, height);
        read_integer_bin(s, elSize);
    }
    else
    {
        char buf[256];
        read(s, buf, binaryMode);
        if (sscanf(buf, sFormatStr, &width, &stridePix, &height, &elSize) != 4)
            elSize = 0;
    }

    if ((uint)elSize != elemSize)
    {
        s.seekg(pos);                                       // restore pos
        return false;
    }

    h      = height;
    w      = width;
    stride = stridePix;
    return true;
}

bool_t AbstractBufferParams::write(ostream& s, int val, bool_t binaryMode)
{
    if (binaryMode)
    {
        write_integer_bin(s, val);
    }
    else
    {
        s << val << ' ';
    }
    return !s.bad();
}

bool_t AbstractBufferParams::read(istream& s, int &val, bool_t binaryMode)
{
    int value;
    size_t pos = s.tellg();

    if (binaryMode)
    {
        read_integer_bin(s, value);
    }
    else
    {
        s >> value;
    }

    if (s.bad())
    {
        s.seekg(pos);                                       // restore pos
        return false;
    }

    val = value;
    return true;
}

bool_t AbstractBufferParams::write(ostream& s, cchar* str, bool_t binaryMode)
{
    s << str << endl;
    CORE_UNUSED(binaryMode);
    return !s.bad();
}

bool_t AbstractBufferParams::read(istream& s, char* str, size_t size, bool_t binaryMode)
{
    if (str != NULL && size) {
        s.getline(str, size);                               // read one line to the output buffer
    }
    else {
        char buf[256];
        s.getline(buf, CORE_COUNT_OF(buf));                 // skip one line
    }
    CORE_UNUSED(binaryMode);
    return !s.bad();
}

} //namespace corecvs

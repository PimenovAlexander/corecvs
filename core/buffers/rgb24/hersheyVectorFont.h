#pragma once

/**
 * \file hersheyVectorFont.h
 *
 * \date Dec 8, 2012
 **/

#include <stdint.h>

namespace corecvs
{

/**
 *   This class contains vector font from here - http://paulbourke.net/dataformats/hershey/
 *   License is public domain, so we will use it.
 **/
class HersheyVectorFont
{
public:

    struct Glyph {
        int16_t len;
        int16_t width;
        int16_t data[110];
    };

    static Glyph data[];

    HersheyVectorFont();
    ~HersheyVectorFont();
};

} /* namespace corecvs */
/* EOF */

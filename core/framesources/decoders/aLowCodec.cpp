/**
 * \file aLowCodec.cpp
 * \brief Add Comment Here
 *
 * \date Sep 1, 2015
 * \author sfedorenko
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils/global.h"

#include "aLowCodec.h"
//#include "buffers/rgb24/rgb24Buffer.h"

using namespace corecvs;

RGB24Buffer *ALowCodec::code(const RGB24Buffer *rgb24buffer)
{
    RGB24Buffer *toReturn = new RGB24Buffer((RGB24Buffer *)rgb24buffer);

    toReturn->binaryOperationInPlace(*rgb24buffer, [](const RGBColor &/*a*/, const RGBColor &rgb) {
        uint8_t r = rgb.r();
        uint8_t g = rgb.g();
        uint8_t b = rgb.b();
        r = code12to8((uint16_t)r << 4);
        g = code12to8((uint16_t)g << 4);
        b = code12to8((uint16_t)b << 4);
        return RGBColor(r, g, b);
    });

    return toReturn;
}

RGB24Buffer *ALowCodec::decode(const RGB24Buffer *rgb24buffer)
{
    //int w = 2592;
    //int h = 1944;
    //RGB24Buffer *toReturn = new RGB24Buffer(h, w, false);

    RGB24Buffer *toReturn = new RGB24Buffer((RGB24Buffer *)rgb24buffer);

    toReturn->binaryOperationInPlace(*rgb24buffer, [](const RGBColor &/*a*/, const RGBColor &rgb) {
        uint8_t r = rgb.r();
        uint8_t g = rgb.g();
        uint8_t b = rgb.b();
        r = decode8to12(r) >> 4;
        g = decode8to12(g) >> 4;
        b = decode8to12(b) >> 4;
        return RGBColor(r, g, b);
    });

    return toReturn;
}

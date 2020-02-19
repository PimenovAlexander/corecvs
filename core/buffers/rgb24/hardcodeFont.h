/**
 * \file hardcodeFont.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#ifndef HARDCODEFONT_H_
#define HARDCODEFONT_H_
#include <stdint.h>

#include "utils/global.h"
namespace corecvs {


class HardcodeFont
{
public:
    static const int GLYPH_WIDTH  = 5;
    static const int GLYPH_HEIGHT = 5;

    static uint8_t num_glyphs[];
    static uint8_t symbols_glyphs[];
    static uint8_t alpha_glyphs[];
    static uint8_t cyrilic_glyphs[];

    HardcodeFont();
    virtual ~HardcodeFont();
};


} //namespace corecvs
#endif /* HARDCODEFONT_H_ */


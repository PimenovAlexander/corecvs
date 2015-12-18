#include <QtGui>
#include "abstractFlowColorer.h"
/**
 * \file abstractFlowColorer.cpp
 * \brief Add Comment Here
 *
 * \date Apr 8, 2011
 * \author alexander
 */

QColor   *AbstractFlowColorer::hsvColorCache;
uint32_t *AbstractFlowColorer::hsvPackedCache;

int AbstractFlowColorer::dummy = AbstractFlowColorer::hsvCacheInit();


int AbstractFlowColorer::hsvCacheInit()
{
    hsvColorCache  = new QColor[256];
    hsvPackedCache = new uint32_t[256];

    for (unsigned i = 0; i < 256; i++)
    {
        QColor color;
        color.setHsv(i, 255, 255, 255);
        uint32_t r = color.red();
        uint32_t g = color.green();
        uint32_t b = color.blue();

        hsvColorCache [i] = color;
        hsvPackedCache[i] = packRGB(r,g,b);
    }

    return 0;
}

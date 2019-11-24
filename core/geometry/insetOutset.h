//
// Created by Ivan Kylchik on 24.11.2019.
//

#ifndef CORECVS_INSETOUTSET_H
#define CORECVS_INSETOUTSET_H

#include <iostream>
#include "core/geometry/polygons.h"

using namespace corecvs;

/**
 *
 * @param p
 * @param offset positive if outset, negative if inset
 */
void shiftPolygon(Polygon *p, int offset) {
    std::cout << "TEST";
}

#endif //CORECVS_INSETOUTSET_H

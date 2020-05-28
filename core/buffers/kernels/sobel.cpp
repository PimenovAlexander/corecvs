/**
 * \file sobel.cpp
 * \brief This file contains Sobel filters
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 27, 2010
 * \author alexander
 */

#include "utils/global.h"

#include "buffers/kernels/sobel.h"

namespace corecvs {

/**
 * Obsolete! It belongs to the old style filters management.
 */
int32_t SobelHorInt::data[9] = {
        -1 , 0 , 1,
        -2 , 0 , 2,
        -1 , 0 , 1 };

int32_t SobelVertInt::data[9] = {
        -1 , 0 , 1,
        -2 , 0 , 2,
        -1 , 0 , 1 };

SobelVertInt *SobelVertInt::instance = new SobelVertInt();
SobelHorInt  *SobelHorInt::instance  = new SobelHorInt();

} //namespace corecvs

/**
 * \file gaussian.cpp
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 24, 2010
 * \author alexander
 */

#include "core/utils/global.h"

#include "core/buffers/kernels/gaussian.h"
namespace corecvs {

double Gaussian3x3::data[9] = {
        1 , 2 , 1,
        2 , 4 , 2,
        1 , 2 , 1 };

uint32_t Gaussian3x3int::data[9] = {
        1 , 2 , 1,
        2 , 4 , 2,
        1 , 2 , 1 };

Gaussian3x3 *Gaussian3x3::instance = new Gaussian3x3();

Gaussian3x3int *Gaussian3x3int::instance = new Gaussian3x3int();

} //namespace corecvs


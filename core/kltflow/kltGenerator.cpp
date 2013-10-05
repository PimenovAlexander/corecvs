/**
 * \file kltGenerator.cpp
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 23, 2010
 * \author Rastorguev Alexei
 * \author Alexander Pimenov
 */

#undef TRACE

#include "global.h"

#include "mathUtils.h"
#include "mipmapPyramid.h"
#include "kltGenerator.h"
#include "vector2d.h"

namespace corecvs {


/**
 * This function does the classical KLT step
 *
 *
 *  \f{eqnarray*}
 * G &=&
 *         \sum_{ x = p_x - w_x}^{p_x + w_x} \sum_{y = p_y - w_y}^{p_y + w_y}
 *         \pmatrix{
 *             I_x^2   &  I_x I_y \cr
 *          I_x I_y &  I_y^2   \cr
 *         } \\
 *
 * b &=&
 *         \sum_{ x = p_x - w_x}^{p_x + w_x} \sum_{y = p_y - w_y}^{p_y + w_y}
 *         \pmatrix{
 *             \delta I I_x \cr
 *          \delta I I_y \cr
 *         } \\
 *
 *  v_{opt} &=& G^{-1}b
 *
 * \f}
 *
 * */

} //namespace corecvs


/**
 * \file vector2d.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 20, 2015
 * \author sergeyfed
 */

#include "vector2d.h"

namespace corecvs {

//#ifdef REFLECTION_IN_CORE

template<> Reflection Vector2d<double>::reflection = Reflection();
template<> int        Vector2d<double>::dummy = Vector2d<double>::staticInit();

template<> Reflection Vector2d<uint32_t>::reflection = Reflection();
template<> int        Vector2d<uint32_t>::dummy = Vector2d<uint32_t>::staticInit();

template<> Reflection Vector2d<uint16_t>::reflection = Reflection();
template<> int        Vector2d<uint16_t>::dummy = Vector2d<uint16_t>::staticInit();

template<> Reflection Vector2d<int32_t>::reflection = Reflection();
template<> int        Vector2d<int32_t>::dummy = Vector2d<int32_t>::staticInit();

template<> Reflection Vector2d<int16_t>::reflection = Reflection();
template<> int        Vector2d<int16_t>::dummy = Vector2d<int16_t>::staticInit();

//#endif

} //namespace corecvs

/**
 * \file vector2d.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 20, 2015
 * \author sergeyfed
 */

#include "core/math/vector/vector2d.h"

namespace corecvs {

//#ifdef REFLECTION_IN_CORE

template<> Reflection Vector2d<double>::reflection = Reflection();
template<> int        Vector2d<double>::dummy = Vector2d<double>::staticInit("Vector2d<double>");

template<> Reflection Vector2d<uint32_t>::reflection = Reflection();
template<> int        Vector2d<uint32_t>::dummy = Vector2d<uint32_t>::staticInit("Vector2d<uint32_t>");

template<> Reflection Vector2d<uint16_t>::reflection = Reflection();
template<> int        Vector2d<uint16_t>::dummy = Vector2d<uint16_t>::staticInit("Vector2d<uint16_t>");

template<> Reflection Vector2d<int32_t>::reflection = Reflection();
template<> int        Vector2d<int32_t>::dummy = Vector2d<int32_t>::staticInit("Vector2d<int32_t>");

template<> Reflection Vector2d<int16_t>::reflection = Reflection();
template<> int        Vector2d<int16_t>::dummy = Vector2d<int16_t>::staticInit("Vector2d<int16_t>");

//#endif

} //namespace corecvs

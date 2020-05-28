/**
 * \file vector2d.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 20, 2015
 * \author sergeyfed
 */

#include "math/vector/vector2d.h"

namespace corecvs {

//#ifdef REFLECTION_IN_CORE

template<> const char * Vector2d<double>::className = "Vector2d<double>";
template<> Reflection Vector2d<double>::reflection = Reflection();
template<> int Vector2d<double>::dummy = Vector2d<double>::staticInit(&Vector2d<double>::reflection);

template<> const char *Vector2d<float>::className = "Vector2d<float>";
template<> Reflection Vector2d<float>::reflection = Reflection();
template<> int Vector2d<float>::dummy = Vector2d<double>::staticInit(&Vector2d<float>::reflection);

template<> const char * Vector2d<uint32_t>::className = "Vector2d<uint32_t>";
template<> Reflection Vector2d<uint32_t>::reflection = Reflection();
template<> int Vector2d<uint32_t>::dummy = Vector2d<double>::staticInit(&Vector2d<uint32_t>::reflection);

template<> const char * Vector2d<uint16_t>::className = "Vector2d<uint16_t>";
template<> Reflection Vector2d<uint16_t>::reflection = Reflection();
template<> int Vector2d<uint16_t>::dummy = Vector2d<double>::staticInit(&Vector2d<uint16_t>::reflection);

template<> const char * Vector2d<int32_t>::className = "Vector2d<int32_t>";
template<> Reflection Vector2d<int32_t>::reflection = Reflection();
template<> int Vector2d<int32_t>::dummy = Vector2d<double>::staticInit(&Vector2d<int32_t>::reflection);

template<> const char * Vector2d<int16_t>::className = "Vector2d<int16_t>";
template<> Reflection Vector2d<int16_t>::reflection = Reflection();
template<> int Vector2d<int16_t>::dummy = Vector2d<double>::staticInit(&Vector2d<int16_t>::reflection);

//#endif

} //namespace corecvs

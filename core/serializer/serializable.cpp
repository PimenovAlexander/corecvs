/**
 * \file serializable.cpp
 * \brief Add Comment Here
 *
 * \date May 22, 2011
 * \author alexander
 */

#include "core/serializer/serializable.h"
namespace corecvs {

template<>
int PropertyListSerializer::visit<int>(int &value, std::string name, const int defaultValue)
{
    return true;
}

template<>
int PropertyListSerializer::visit<double>(double &value, std::string name, const double defaultValue)
{
    return true;
}

} //namespace corecvs


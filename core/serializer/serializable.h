#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_
/**
 * \file serializable.h
 * \brief Add Comment Here
 *
 * \date May 22, 2011
 * \author alexander
 */

#include <string>
#include <vector>
namespace corecvs {

using std::string;

class Serializer
{

template<typename ValueType>
    int visit(ValueType &value, string name, const ValueType defaultValue);

    int startGroup(string name);
    int endGroup();
};

template<typename RealType>
class Serializable
{
public:
    void serialize();
};

class PropertyListSerializer : public PropertyList
{
template<>
    int visit<int>(int &value, string name, const int defaultValue);

template<>
    int visit<double>(double &value, string name, const double defaultValue);


};


} //namespace corecvs
#endif /* SERIALIZABLE_H_ */


/**
 * \file propertyListVisitor.cpp
 * \brief Add Comment Here
 *
 * \date Nov 27, 2011
 * \author alexander
 */

#include "propertyListVisitor.h"

namespace corecvs {

using std::string;
using std::endl;

/**
 * PropertyListWriterVisitor methods that process different types
 * @{
 */
template<>
    void PropertyListWriterVisitor::visit<int>(int &intField, int /*defaultValue*/, const char *fieldName)
    {
        output->setIntProperty(getChildPath(fieldName), intField);
    }

template<>
    void PropertyListWriterVisitor::visit<double>(double &doubleField, double /*defaultValue*/, const char *fieldName)
    {
        output->setDoubleProperty(getChildPath(fieldName), doubleField);
    }

template<>
    void PropertyListWriterVisitor::visit<bool>(bool &boolField, bool /*boolValue*/, const char *fieldName)
    {
        output->setIntProperty(getChildPath(fieldName), boolField);
    }

/* New style fields*/

template<>
    void PropertyListWriterVisitor::visit<int,    IntField>(int &intField, const IntField *fieldDescriptor)
    {
        output->setIntProperty(getChildPath(fieldDescriptor->getSimpleName()), intField);
    }

template<>
    void PropertyListWriterVisitor::visit<double, DoubleField>(double &doubleField, const DoubleField *fieldDescriptor)
    {
        output->setDoubleProperty(getChildPath(fieldDescriptor->getSimpleName()), doubleField);
    }

template<>
    void PropertyListWriterVisitor::visit<bool,   BoolField>(bool &boolField, const BoolField *fieldDescriptor)
    {
        output->setIntProperty(getChildPath(fieldDescriptor->getSimpleName()), boolField);
    }

/**
 *
 * @}
 */

/**
 * PropertyListReaderVisitor methods that process different types
 * @{
 */
template<>
    void PropertyListReaderVisitor::visit<int>(int &intField, int defaultValue, const char *fieldName)
    {
        intField = input->getIntProperty(getChildPath(fieldName), defaultValue);
    }

template<>
    void PropertyListReaderVisitor::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
    {
        doubleField = input->getDoubleProperty(getChildPath(fieldName), defaultValue);
    }

template<>
    void PropertyListReaderVisitor::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName)
    {
        boolField = input->getIntProperty(getChildPath(fieldName), defaultValue);
    }

/* New style fields*/

template<>
    void PropertyListReaderVisitor::visit<int,    IntField>(int &intField, const IntField *fieldDescriptor)
    {
        intField = input->getIntProperty(getChildPath(fieldDescriptor->getSimpleName()), fieldDescriptor->defaultValue);
    }

template<>
    void PropertyListReaderVisitor::visit<double, DoubleField>(double &doubleField, const DoubleField *fieldDescriptor)
    {
        doubleField = input->getDoubleProperty(getChildPath(fieldDescriptor->getSimpleName()), fieldDescriptor->defaultValue);
    }

template<>
    void PropertyListReaderVisitor::visit<bool,   BoolField>(bool &boolField, const BoolField *fieldDescriptor)
    {
        boolField = input->getIntProperty(getChildPath(fieldDescriptor->getSimpleName()), fieldDescriptor->defaultValue);
    }


/**
 *
 * @}
 */

} //namespace corecvs


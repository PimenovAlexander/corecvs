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

template<>
    void PropertyListWriterVisitor::visit<std::string>(std::string &stringField, std::string /*stringValue*/, const char *fieldName)
    {
        output->setStringProperty(getChildPath(fieldName), stringField);
    }

/* New style fields*/

template<>
    void PropertyListWriterVisitor::visit<int,    IntField>(int &intField, const IntField *fieldDescriptor)
    {
        output->setIntProperty(getChildPath(fieldDescriptor->getSimpleName()), intField);
    }

template<>
    void PropertyListWriterVisitor::visit<int,    EnumField>(int &enumField, const EnumField *fieldDescriptor)
    {
        output->setIntProperty(getChildPath(fieldDescriptor->getSimpleName()), enumField);
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

template <>
    void PropertyListWriterVisitor::visit<std::vector<double>, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
    {
        std::stringstream ss;
        ss << fieldDescriptor->getSimpleName();
        ss << ".size";

        output->setIntProperty(getChildPath(ss.str().c_str()), field.size());
        for (size_t i = 0; i < field.size(); i++ )
        {
            std::stringstream ss;
            ss << fieldDescriptor->getSimpleName() << "[" << i << "]";
            output->setDoubleProperty(getChildPath(getChildPath(ss.str().c_str())), field[i]);
        }
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

template<>
    void PropertyListReaderVisitor::visit<std::string>(std::string &stringField, std::string stringValue, const char *fieldName)
    {
        stringField = input->getStringProperty(getChildPath(fieldName), stringValue);
    }


/* New style fields*/

template<>
    void PropertyListReaderVisitor::visit<int,    IntField>(int &intField, const IntField *fieldDescriptor)
    {
        intField = input->getIntProperty(getChildPath(fieldDescriptor->getSimpleName()), fieldDescriptor->defaultValue);
    }

template<>
    void PropertyListReaderVisitor::visit<int,    EnumField>(int &enumField, const EnumField *fieldDescriptor)
    {
        enumField = input->getIntProperty(getChildPath(fieldDescriptor->getSimpleName()), fieldDescriptor->defaultValue);
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


template <>
void PropertyListReaderVisitor::visit<std::vector<double>, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    std::stringstream ss;
    ss << fieldDescriptor->getSimpleName();
    ss << ".size";
    int sizeraw = input->getIntProperty(getChildPath(ss.str().c_str()), 0);
    uint size = sizeraw > 0 ? sizeraw : 0;
    field.resize(size);
    for (size_t i = 0; i < size; i++ )
    {
        std::stringstream ss;
        ss << fieldDescriptor->getSimpleName() << "[" << i << "]";
        field[i] = input->getDoubleProperty(getChildPath(ss.str().c_str()), 0.0);
    }
}


/**
 *
 * @}
 */

} //namespace corecvs


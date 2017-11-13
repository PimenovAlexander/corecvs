#include "core/reflection/commandLineSetter.h"

namespace corecvs {

template <>
void CommandLineSetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor)
{
    field = getInt(fieldDescriptor->name.name, fieldDescriptor->defaultValue);
}

template <>
void CommandLineSetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    field = getDouble(fieldDescriptor->name.name, fieldDescriptor->defaultValue);
}

template <>
void CommandLineSetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor)
{
    field = (float)getDouble(fieldDescriptor->name.name, fieldDescriptor->defaultValue);
}

template <>
void CommandLineSetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    field = getBool(fieldDescriptor->name.name);
}

template <>
void CommandLineSetter::visit<void *, PointerField>(void * &field,  const PointerField * /*fieldDescriptor*/)
{
    field = NULL;
}

template <>
void CommandLineSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    field = getInt(fieldDescriptor->name.name, fieldDescriptor->defaultValue);
}

template <>
void CommandLineSetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    field = getString(fieldDescriptor->name.name, fieldDescriptor->defaultValue);
}

/* Old style */

template <>
void CommandLineSetter::visit<int>(int &intField, int defaultValue, const char * fieldName)
{
    intField = getInt(fieldName, defaultValue);
}

template <>
void CommandLineSetter::visit<double>(double &doubleField, double defaultValue, const char * fieldName)
{
    doubleField = getDouble(fieldName, defaultValue);
}

template <>
void CommandLineSetter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char * fieldName)
{
    boolField = getBool(fieldName);
}

template <>
void CommandLineSetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char * fieldName)
{
    stringField = getString(fieldName, defaultValue);
}


template<class Type>
void CommandLineSetter::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    /*pushChild(fieldName);*/
        field.accept(*this);
    /*popChild();*/
}

} //namespace corecvs

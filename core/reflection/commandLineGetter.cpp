#include "core/reflection/commandLineGetter.h"

namespace corecvs {

template <>
void CommandLineGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor)
{
    std::string value = mArgPrefix + fieldDescriptor->name.name + mArgSeparator + std::to_string(field);
    mArgs.push_back(value);
}

template <>
void CommandLineGetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    std::string value = mArgPrefix + fieldDescriptor->name.name + mArgSeparator + std::to_string(field);
    mArgs.push_back(value);
}

template <>
void CommandLineGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor)
{
    std::string value = mArgPrefix + fieldDescriptor->name.name + mArgSeparator + std::to_string(field);
    mArgs.push_back(value);
}

template <>
void CommandLineGetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    if (field)
    {
        std::string value = mArgPrefix + fieldDescriptor->name.name;
        mArgs.push_back(value);
    }
}

/*
template <>
void CommandLineGetter::visit<void *, PointerField>(void * &field,  const PointerField * fieldDescriptor)
{
    std::string value = mArgPrefix + fieldDescriptor->name.name + mArgSeparator + field;
    mArgs.push_back(value);
}
*/

template <>
void CommandLineGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    std::string value = mArgPrefix + fieldDescriptor->name.name + mArgSeparator + std::to_string(field);
    mArgs.push_back(value);
}

template <>
void CommandLineGetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    std::string value = mArgPrefix + fieldDescriptor->name.name + mArgSeparator + field;
    mArgs.push_back(value);
}

/* Old style */

template <>
void CommandLineGetter::visit<int>(int &intField, int /*defaultValue*/, const char * fieldName)
{
    std::string value = mArgPrefix + fieldName + mArgSeparator + std::to_string(intField);
    mArgs.push_back(value);
}

template <>
void CommandLineGetter::visit<double>(double &doubleField, double /*defaultValue*/, const char * fieldName)
{
    std::string value = mArgPrefix + fieldName + mArgSeparator + std::to_string(doubleField);
    mArgs.push_back(value);}

template <>
void CommandLineGetter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char * fieldName)
{
    if (boolField)
    {
        std::string value = mArgPrefix + fieldName;
        mArgs.push_back(value);
    }
}

template <>
void CommandLineGetter::visit<std::string>(std::string &stringField, std::string /*defaultValue*/, const char * fieldName)
{
    std::string value = mArgPrefix + fieldName + mArgSeparator + stringField;
    mArgs.push_back(value);
}


template<class Type>
void CommandLineGetter::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    /*pushChild(fieldName);*/
        field.accept(*this);
    /*popChild();*/
}

} // namespace corecvs


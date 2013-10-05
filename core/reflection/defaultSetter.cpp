#include "defaultSetter.h"

namespace corecvs {

template <>
void DefaultSetter::visit<int, IntField>(
        int &field,
        const IntField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetter::visit<double, DoubleField>(
        double &field,
        const DoubleField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetter::visit<float, FloatField>(
        float &field,
        const FloatField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetter::visit<bool, BoolField>(
        bool &field,
        const BoolField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetter::visit<void *, PointerField>(
        void * &field,
        const PointerField * /*fieldDescriptor*/)
{
    field = NULL;
}

template <>
void DefaultSetter::visit<int, EnumField>(
        int &field,
        const EnumField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetter::visit<std::string, StringField>(
        std::string &field,
        const StringField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

} //namespace corecvs

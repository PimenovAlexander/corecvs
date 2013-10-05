#include "defaultSetter.h"


template <>
void DefaultSetterOld::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/)
{
    intField = defaultValue;
}

template <>
void DefaultSetterOld::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/)
{
    doubleField = defaultValue;
}

template <>
void DefaultSetterOld::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/)
{
    boolField = defaultValue;
}

template<class Type>
void DefaultSetterOld::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    pushChild(fieldName);
        field.accept(*this);
    popChild();
}

template <>
void DefaultSetterOld::visit<IntField>(
        int &field,
        const IntField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetterOld::visit<DoubleField>(
        double &field,
        const DoubleField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetterOld::visit<BoolField>(
        bool &field,
        const BoolField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}

template <>
void DefaultSetterOld::visit<PointerField>(
        void * &field,
        const PointerField * /*fieldDescriptor*/)
{
    field = NULL;
}

template <>
void DefaultSetterOld::visit<EnumField>(
        EnumField::CPPType &field,
        const EnumField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}


template <typename FieldReflectionType>
void DefaultSetterOld::visit(typename FieldReflectionType::CPPType &field, const FieldReflectionType * /*fieldDescriptor*/)
{
    field.accept(*this);
}


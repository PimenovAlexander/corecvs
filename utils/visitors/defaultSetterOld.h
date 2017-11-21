#pragma once
#include "core/utils/visitors/basePathVisitor.h"
#include "core/reflection/reflection.h"

using namespace corecvs;

class DefaultSetterOld : public BasePathVisitor
{
public:
    template <class Type>
        void visit(Type &field, Type defaultValue, const char *fieldName);

    template <typename FieldReflectionType>
        void visit(typename FieldReflectionType::CPPType &field, const FieldReflectionType *fieldDescriptor);
};

template <>
void DefaultSetterOld::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void DefaultSetterOld::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void DefaultSetterOld::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);


template <>
void DefaultSetterOld::visit<IntField>(
        int &field,
        const IntField *fieldDescriptor);

template <>
void DefaultSetterOld::visit<DoubleField>(
        double &field,
        const DoubleField *fieldDescriptor);

template <>
void DefaultSetterOld::visit<BoolField>(
        bool &field,
        const BoolField *fieldDescriptor);

template <>
void DefaultSetterOld::visit<PointerField>(
        void * &field,
        const PointerField *fieldDescriptor);

template <>
void DefaultSetterOld::visit<EnumField>(
        EnumField::CPPType &field,
        const EnumField *fieldDescriptor);


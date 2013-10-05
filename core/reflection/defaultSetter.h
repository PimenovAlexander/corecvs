#ifndef DEFAULT_SETTER_H_
#define DEFAULT_SETTER_H_

#include <iostream>

#include "global.h"

#include "reflection.h"

namespace corecvs {

class DefaultSetter
{
public:

/*template <typename FieldReflectionType>
    void visit(typename FieldReflectionType::CPPType &field, const FieldReflectionType *fieldDescriptor);*/

    template <typename inputType, typename reflectionType>
        void visit(inputType &field, const reflectionType * /*fieldDescriptor*/)
    {
        field.accept(*this);
    }
};

template <>
void DefaultSetter::visit<int, IntField>(
        int &field,
        const IntField *fieldDescriptor);

template <>
void DefaultSetter::visit<double, DoubleField>(
        double &field,
        const DoubleField *fieldDescriptor);

template <>
void DefaultSetter::visit<float, FloatField>(
        float &field,
        const FloatField *fieldDescriptor);

template <>
void DefaultSetter::visit<bool, BoolField>(
        bool &field,
        const BoolField *fieldDescriptor);

template <>
void DefaultSetter::visit<void *, PointerField>(
        void * &field,
        const PointerField *fieldDescriptor);

template <>
void DefaultSetter::visit<int, EnumField>(
        int &field,
        const EnumField *fieldDescriptor);

template <>
void DefaultSetter::visit<std::string, StringField>(
        std::string &field,
        const StringField *fieldDescriptor);


} //namespace corecvs

#endif // DEFAULT_SETTER_H_

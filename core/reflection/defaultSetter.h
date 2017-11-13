#ifndef DEFAULT_SETTER_H_
#define DEFAULT_SETTER_H_

#include <iostream>

#include "core/utils/global.h"

#include "core/reflection/reflection.h"

namespace corecvs {

class DefaultSetter
{
public:

/*template <typename FieldReflectionType>
    void visit(typename FieldReflectionType::CPPType &field, const FieldReflectionType *fieldDescriptor);*/

    /* Oldstyle */
    template <class Type>
        void visit(Type &field, Type defaultValue, const char * /*fieldName*/)
    {
        field = defaultValue;
    }

    template <typename inputType, typename reflectionType>
        void visit(inputType &field, const reflectionType * /*fieldDescriptor*/)
    {
        field.accept(*this);
    }

    template <typename inputType, typename reflectionType>
        void visit(std::vector<inputType> &/*field*/, const reflectionType * /*fieldDescriptor*/)
    {
        // SYNC_PRINT(("visit(std::vector<inputType> &/*field*/, const reflectionType * /*fieldDescriptor*/): ignoring\n"));
    }

    template <typename inputType>
        void visit(std::vector<inputType> &/*field*/, const char * /*name*/)
    {
       // SYNC_PRINT(("visit(std::vector<inputType> &/*field*/, \"%s\""): ignoring\n", name));
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

template <>
void DefaultSetter::visit<std::wstring, WStringField>(
        std::wstring &field,
        const WStringField *fieldDescriptor);


/*
template <>
void DefaultSetter::visit< std::vector<double>, DoubleVectorField>(
        std::vector<double> &field,
        const DoubleVectorField *fieldDescriptor);
*/
template <>
void DefaultSetter::visit<double, DoubleVectorField>(
        std::vector<double> &field,
        const DoubleVectorField *fieldDescriptor);



/* Oldstyle setter */
template <>
void DefaultSetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void DefaultSetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void DefaultSetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);

template <>
void DefaultSetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char * /*fieldName*/);


} //namespace corecvs

#endif // DEFAULT_SETTER_H_

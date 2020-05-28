#include "reflection/defaultSetter.h"

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
//    SYNC_PRINT(("DefaultSetter::visit<double, DoubleField>: Called for %s\n", fieldDescriptor->getSimpleName()));

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

template <>
void DefaultSetter::visit<std::wstring, WStringField>(
        std::wstring &field,
        const WStringField *fieldDescriptor)
{
    field = fieldDescriptor->defaultValue;
}


template <>
void DefaultSetter::visit<double, DoubleVectorField>(
        std::vector<double> &field,
        const DoubleVectorField *fieldDescriptor)
{
//    SYNC_PRINT(("DefaultSetter::visit<std::vector<double>, DoubleVectorField>: Called for %s\n", fieldDescriptor->getSimpleName()));
    field = fieldDescriptor->defaultValue;
}

/* Old style */

template <>
void DefaultSetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/)
{
    intField = defaultValue;
}

template <>
void DefaultSetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/)
{
    doubleField = defaultValue;
}

template <>
void DefaultSetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/)
{
    boolField = defaultValue;
}

template <>
void DefaultSetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char * /*fieldName*/)
{
    stringField = defaultValue;
}

#if 0
template<class Type>
void DefaultSetter::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    /*pushChild(fieldName);*/
        field.accept(*this);
    /*popChild();*/
}
#endif

} //namespace corecvs

#ifndef COMMANDLINEGETTER_H
#define COMMANDLINEGETTER_H

#include <sstream>
#include <string>
#include <vector>
#include "core/reflection/reflection.h"

namespace corecvs {

class CommandLineGetter
{
public:

    std::string mArgPrefix;
    std::string mArgSeparator;

    std::vector<std::string> mArgs;

    CommandLineGetter() :
        mArgPrefix("--"),
        mArgSeparator("=")
    {}

    /* Oldstyle */
    template <class Type>
        void visit(Type &field, Type defaultValue, const char *fieldName);

    template <typename inputType, typename reflectionType>
        void visit(std::vector<inputType> &/*field*/, const reflectionType * /*fieldDescriptor*/)
    {
    }

    template <typename inputType, typename reflectionType>
        void visit(inputType &field, const reflectionType * /*fieldDescriptor*/)
    {
        field.accept(*this);
    }

    std::string str()
    {
        std::ostringstream ss;
        for (std::string s : mArgs)
        {
            /**/
            ss << "\"" << s << "\" ";
        }
        return ss.str();
    }

};

template <>
void CommandLineGetter::visit<int, IntField>(
        int &field,
        const IntField *fieldDescriptor);

template <>
void CommandLineGetter::visit<double, DoubleField>(
        double &field,
        const DoubleField *fieldDescriptor);

template <>
void CommandLineGetter::visit<float, FloatField>(
        float &field,
        const FloatField *fieldDescriptor);

template <>
void CommandLineGetter::visit<bool, BoolField>(
        bool &field,
        const BoolField *fieldDescriptor);

/*
template <>
void CommandLineGetter::visit<void *, PointerField>(
        void * &field,
        const PointerField *fieldDescriptor);
*/

template <>
void CommandLineGetter::visit<int, EnumField>(
        int &field,
        const EnumField *fieldDescriptor);

template <>
void CommandLineGetter::visit<std::string, StringField>(
        std::string &field,
        const StringField *fieldDescriptor);

/* Oldstyle setter */
template <>
void CommandLineGetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void CommandLineGetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void CommandLineGetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);

template <>
void CommandLineGetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char * /*fieldName*/);



} //namespace corecvs;

#endif // COMMANDLINEGETTER_H

#ifndef COMMANDLINESETTER_H
#define COMMANDLINESETTER_H

#include <iostream>
#include <vector>
#include <string>


#include "core/utils/global.h"

#include "core/utils/visitors/basePathVisitor.h"
#include "core/reflection/reflection.h"
#include "core/reflection/dynamicObject.h"

namespace corecvs {

using std::vector;
using std::string;

class CommandLineSetter : public BasePathVisitor
{
public:
    string mArgPrefix;
    string mArgSeparator;
    vector<string> mArgs;

    bool mPreserveValues;

    CommandLineSetter() : mArgPrefix("--"), mArgSeparator("="), mPreserveValues(false)
    {}

    CommandLineSetter(const vector<string> &args) : CommandLineSetter()
    {
        mArgs = args;
    }

    CommandLineSetter(int argc, const char **argv) : CommandLineSetter()
    {
        mArgs.assign(argv, argv + argc);
    }

    CommandLineSetter(int argc, char **argv) : CommandLineSetter(argc, (const char **)argv)
    {}

    /* Helper getters */
    bool hasOption(const std::string& option, unsigned* pos = NULL) const;

    const std::string getOption(const std::string& option, bool *found = NULL) const;

    vector<std::string> nonPrefix() const;

    /* Returns first parameter */
    std::string getNonPrefixParam(int id = 1) const;

    int          getInt    (const std::string & option, int defaultInf = 0) const;
    bool         getBool   (const std::string & option) const;
    double       getDouble (const std::string & option, double defaultDouble = 0.0) const;
    std::string  getString (const std::string & option, const std::string & defaultString = "") const;
    std::wstring getWString(const std::string & option, const std::wstring & defaultString = L"") const;

    /* Oldstyle */
    template <class Type>
        void visit(Type &field, Type defaultValue, const char *fieldName);

    template <class Type>
        void visit(Type &field, const char *fieldName)
    {
        pushChild(fieldName);
        field.accept(*this);
        popChild();
    }

    template <typename inputType, typename reflectionType>
        void visit(std::vector<inputType> &/*field*/, const reflectionType * /*fieldDescriptor*/)
    {
    }

    template <typename inputType, typename reflectionType>
        void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        pushChild(fieldDescriptor->name.name);
        field.accept(*this);
        popChild();
    }


};

template <>
void CommandLineSetter::visit<int, IntField>(
        int &field,
        const IntField *fieldDescriptor);

template <>
void CommandLineSetter::visit<double, DoubleField>(
        double &field,
        const DoubleField *fieldDescriptor);

template <>
void CommandLineSetter::visit<float, FloatField>(
        float &field,
        const FloatField *fieldDescriptor);

template <>
void CommandLineSetter::visit<bool, BoolField>(
        bool &field,
        const BoolField *fieldDescriptor);

template <>
void CommandLineSetter::visit<void *, PointerField>(
        void * &field,
        const PointerField *fieldDescriptor);

template <>
void CommandLineSetter::visit<int, EnumField>(
        int &field,
        const EnumField *fieldDescriptor);

template <>
void CommandLineSetter::visit<std::string, StringField>(
        std::string &field,
        const StringField *fieldDescriptor);


template <>
void CommandLineSetter::visit<std::wstring, WStringField>(
        std::wstring &field,
        const WStringField *fieldDescriptor);


/* Oldstyle setter */
template <>
void CommandLineSetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<DynamicObject>(DynamicObject &dynamicObject, DynamicObject defaultValue, const char *fieldName);


} //namespace corecvs

#endif // COMMANDLINESETTER_H

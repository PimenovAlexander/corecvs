#ifndef COMMANDLINESETTER_H
#define COMMANDLINESETTER_H

#include <iostream>
#include <vector>
#include <string>

#include "core/utils/global.h"

#include "core/reflection/reflection.h"

namespace corecvs {

using std::vector;
using std::string;

class CommandLineSetter
{
public:
    string mArgPrefix;
    string mArgSeparator;

    vector<string> mArgs;

    CommandLineSetter() :
        mArgPrefix("--"),
        mArgSeparator("=")
    {
    }

    CommandLineSetter(const vector<string> &args) :
        CommandLineSetter()
    {
        mArgs = args;
    }

    CommandLineSetter(int argc, const char **argv) :
        CommandLineSetter()
    {
        mArgs.assign(argv, argv + argc);
    }

    /* Helper getters */
    bool hasOption(const string& option, unsigned* pos = NULL)
    {
        string decorated = mArgPrefix + option;

        for (unsigned i = 0; i < mArgs.size(); i++)
        {
            if (mArgs[i].compare(0, decorated.length(), decorated) == 0)
            {
                if (pos != 0) {
                    *pos = i;
                }
                return true;
            }
        }
        return false;
    }

    const string getOption(const string& option, bool *found = NULL)
    {
        string decorated = mArgPrefix + option + mArgSeparator;

        if (found != NULL) *found = false;
        for (unsigned i = 0; i < mArgs.size(); i++)
        {
            if (mArgs[i].compare(0, decorated.length(), decorated) == 0)
            {
                if (found != NULL) *found = true;
                return mArgs[i].substr(decorated.length());
            }
        }
        return "";
    }


    int getInt(const string & option, int defaultInf = 0)
    {
        const string& argument = getOption(option);

        if (argument.empty())
        {
            return defaultInf;
        }

        std::size_t pos = 0;
        int result = stoi(argument, &pos);
        if (pos != 0) {
            return result;
        } else {
            return defaultInf;
        }
    }

    bool getBool(const string & option)
    {
        //const string& argument = getOption(option);
        return hasOption(option);
    }

    double getDouble(const string & option, double defaultDouble = 0.0)
    {
        const string& argument = getOption(option);

        if (argument.empty())
        {
            return defaultDouble;
        }

        std::size_t pos = 0;
        double result = stod(argument, &pos);
        if (pos != 0) {
            return result;
        } else {
            return defaultDouble;
        }
    }

    string getString(const string & option, const std::string & defaultString)
    {
        bool found = false;
        const string& argument = getOption(option, &found);

        if (!found)
        {
            return string(defaultString);
        }

        return string(argument);
    }

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

/* Oldstyle setter */
template <>
void CommandLineSetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);

template <>
void CommandLineSetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char * /*fieldName*/);


} //namespace corecvs

#endif // COMMANDLINESETTER_H

#ifndef JSON_PRINTER_H
#define JSON_PRINTER_H


#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <locale>

#include "core/reflection/reflection.h"
#include "core/utils/log.h"

namespace corecvs {

using std::string;
using std::ostream;
using std::cout;


/**
   Please note, json is finallised on destruction of the object.
**/
class JSONPrinter
{
public:
    bool isSaver () { return true;}
    bool isLoader() { return false;}


public:
    std::ostream *stream = nullptr;
    int indentation = 0;
    int dIndent = 1;
    bool isFirst = true;
    bool isFile = false;

    /** This flag allows to format vectors as JSON arrays **/
    bool isNewArray = false;

    static const std::string LF;

    static const std::string PROLOGUE;
    static const std::string EPILOGUE;

    static const std::string OBJECT_OPEN;
    static const std::string OBJECT_CLOSE;

    static const std::string ARRAY_OPEN;
    static const std::string ARRAY_CLOSE;

    static const std::string FIELD_SEPARATOR;
    static const std::string FIELD_VALUE_SEPARATOR;
    static const std::string NAME_DECORATOR;


    explicit  JSONPrinter(ostream *_stream) : stream(_stream)
    {
        prologue();
    }

    explicit  JSONPrinter(ostream &_stream = cout) : stream(&_stream)
    {
        prologue();
    }

    explicit  JSONPrinter(int indent, int dindent, ostream &_stream = cout)
        : stream(&_stream), indentation(indent), dIndent(dindent)
    {
        prologue();
    }

    explicit  JSONPrinter(const string &filepath)
        : stream(new std::ofstream(filepath.c_str(), std::ofstream::out))
        , isFile(true)
    {
        if (!(*stream))
        {
            L_ERROR_P("Couldn't open for writting the file <%s>", filepath.c_str());
            delete_safe(stream);
            isFile = false;
            return;
        }
        L_INFO_P("saving to <%s>", filepath.c_str());
        prologue();
    }

    ~JSONPrinter()
    {
        epilogue();
        if (isFile && stream)
            delete stream;
    }

    std::string indent() {
        return std::string(indentation, ' ');
    }

    /*json prologue*/
    void prologue() {
        if (stream == NULL) return;
        stream->imbue(std::locale("C"));
        (*stream) << std::setprecision(std::numeric_limits<double>::digits10 + 2);
        (*stream) << PROLOGUE;
    }

    void epilogue() {
        if (stream == NULL) return;
        (*stream) << LF << EPILOGUE << std::flush;
    }

    std::string separate()
    {
        std::string result = (!isFirst && (stream != NULL)) ? ",\n" : "\n";
        isFirst = false;
        return result;
    }

    /* */
    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, const char *arrayName)
    {
        *stream << separate() << indent() << decorateName(arrayName) << FIELD_VALUE_SEPARATOR << ARRAY_OPEN;
        indentation += dIndent;
        isFirst = true;
        for (size_t i = 0; i < fields.size(); i++)
        {
            *stream << separate() << indent() << ARRAY_OPEN;
            indentation += dIndent;
            isFirst = true;
            for (size_t j = 0; j < fields[i].size(); j++)
            {
                *stream << separate() << indent() << OBJECT_OPEN;            isFirst = true;
                isFirst = true;
                fields[i][j].accept(*this);
                *stream << LF << indent() << OBJECT_CLOSE;
            }
            indentation -= dIndent;
            *stream << LF << indent() << ARRAY_CLOSE;
        }
        indentation -= dIndent;
        *stream << LF << indent() << ARRAY_CLOSE;
    }

    /**
     * Generic Array support with new style reflection
     **/
    template <typename inputType, typename reflectionType>
    void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        indentation += dIndent;
        for (int i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
        indentation -= dIndent;
    }

    /**
     * Generic Array support with old style
     **/
    template <typename innerType>
    void visit(std::vector<innerType> &field, const char* arrayName)
    {
        if (isNewArray) {
            *stream << NAME_DECORATOR << arrayName << NAME_DECORATOR << ":[";
            for (size_t i = 0; i < field.size(); i++)
            {
                visit<innerType>(field[i], "");
                if (i != (field.size() - 1)) {
                    *stream << ",";
                }
            }
            *stream << "]";
        }
        else {
            indentation += dIndent;
            for (size_t i = 0; i < field.size(); i++)
            {
                std::ostringstream ss;
                ss << NAME_DECORATOR << arrayName << NAME_DECORATOR << "[" << i << "]";
                visit<innerType>(field[i], innerType(), ss.str().c_str());
            }
            indentation -= dIndent;
        }
    }

    /**
     * Complex objects support with new style reflection
     **/
template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        if (stream != NULL) {
            *stream << separate() << indent() << decorateName(fieldDescriptor) << FIELD_VALUE_SEPARATOR << OBJECT_OPEN;
        }
        indentation += dIndent;
        isFirst = true;
        field.accept(*this);
        indentation -= dIndent;

        if (stream != NULL) {
            *stream << LF << indent() << OBJECT_CLOSE;
        }
    }


    /**
     * Complex objects support with old style calls
     **/
template <class Type>
    void visit(Type &field, const char * fieldName)
    {
        if (isNewArray) {
            if (stream != NULL) {
                *stream << OBJECT_OPEN;
            }
            indentation += dIndent;
            isFirst = true;
            field.accept(*this);
            indentation -= dIndent;

            if (stream != NULL) {
                *stream << LF << indent() << OBJECT_CLOSE;
            }
        }
        else {
            if (stream != NULL) {
                *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR << OBJECT_OPEN;
            }
            indentation += dIndent;
            isFirst = true;
            field.accept(*this);
            indentation -= dIndent;

            if (stream != NULL) {
                *stream << LF << indent() << OBJECT_CLOSE;
            }
        }
    }

    /**
     *  Arithmetic types such as double, int, etc
     *  Except for enums and boolean
     *
     **/
    template <typename Type,
              typename std::enable_if<!(std::is_enum<Type>::value || (std::is_arithmetic<Type>::value && !std::is_same<bool, Type>::value)), int>::type foo = 0>
    void visit(Type &field, Type /*defaultValue*/, const char * fieldName)
    {
        visit<Type>(field, fieldName);
    }

    template <typename type, typename std::enable_if<std::is_arithmetic<type>::value && !std::is_same<bool, type>::value, int>::type foo = 0>
    void visit(type &field, type, const char *fieldName)
    {
        if (stream == NULL) return;
        *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR << field;
    }

    /**
     * Enums
     **/
    template <typename type, typename std::enable_if<std::is_enum<type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        using U = typename std::underlying_type<type>::type;
        U u = static_cast<U>(field);
        visit(u, static_cast<U>(defaultValue), fieldName);
        field = static_cast<type>(u);
    }

private:

    static std::string escapeString(const std::string &str);
    static std::string decorateName(const BaseField *field);
    static std::string decorateName(const char *field);
};


template <>
void JSONPrinter::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void JSONPrinter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void JSONPrinter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void JSONPrinter::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void JSONPrinter::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void JSONPrinter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void JSONPrinter::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void JSONPrinter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void JSONPrinter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);


/* Arrays */

template <>
void JSONPrinter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

/**
 * Old Style
 *
 * this methods can be made universal, but are separated to make it a bit more controllable
 **/
template <>
void JSONPrinter::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void JSONPrinter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void JSONPrinter::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);

template <>
void JSONPrinter::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName);


} //namespace corecvs

#endif // JSON_PRINTER_H

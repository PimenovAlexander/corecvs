#ifndef PRINTER_VISITOR_OLD_H_
#define PRINTER_VISITOR_OLD_H_

#include <stdint.h>
#include <iostream>
#include <sstream>

#include "core/reflection/reflection.h"

namespace corecvs {

using std::string;
using std::wstring;
using std::ostream;
using std::cout;


/* In c++17 this is done in more compact and typesafe way */

#define TR_ENABLE_IF(X)       typename std::enable_if<(X)>::type
#define TR_ENABLE_IF_T(X , Y) typename std::enable_if<(X), (Y)>::type
#define TR_ENABLE_IF_P(X)     typename std::enable_if<X,int>::type foo = 0


#define TR_IS_BOOL(X)         std::is_same<X, bool>::value
#define TR_IS_ENUM(X)         std::is_enum<X>::value

#define TR_IS_SAME(X, Y)      std::is_same<X, Y>::value
#define TR_IS_ARITHM(X)       std::is_arithmetic<(X), (Y)>::value


class PrinterVisitor
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return false;}


public:
    std::ostream *stream;
    int indentation;
    int dIndent;

    explicit  PrinterVisitor(ostream *_stream) :
        stream(_stream),
        indentation(0),
        dIndent(1)
    {}

    explicit  PrinterVisitor(ostream &_stream = cout) :
        stream(&_stream),
        indentation(0),
        dIndent(1)
    {}

    explicit  PrinterVisitor(int indent, int dindent, ostream &_stream = cout) :
        stream(&_stream),
        indentation(indent),
        dIndent(dindent)
    {}

    std::string indent() {
        return std::string(indentation, ' ');
    }


    /**
     * Generic Array support. New Style
     **/
    template <typename inputType, typename ReflectionType, TR_ENABLE_IF_P(!TR_IS_SAME(ReflectionType, char))>
    void visit(std::vector<inputType> &fields, const ReflectionType * /*fieldDescriptor*/)
    {
        indentation += dIndent;
        for (size_t i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
        indentation -= dIndent;
    }

    /**
     * Generic Array support. Old Style
     **/
#if 1
    template <typename InnerType>
    void visit(std::vector<InnerType> &field, const char* arrayName)
    {
        indentation += dIndent;
        for (size_t i = 0; i < field.size(); i++)
        {
            std::ostringstream ss;
            ss << arrayName << "[" <<  i << "]";
            visit<InnerType>(field[i], InnerType(), ss.str().c_str());
        }
        indentation -= dIndent;
    }
#endif

#if 0
    /*Ok there seem to be no easy way to check for beeing a desent contanier */
    template <template <typename> class ContainerType, typename ElementType>
    typename std::enable_if< std::is_object<typename ContainerType<ElementType>::iterator>::value , void>::type
    visit(ContainerType<ElementType> &field, const char* containerName)
    {
        indentation += dIndent;

        int i = 0;
        for (auto &element: field)
        {
            std::ostringstream ss;
            ss << containerName << "[" <<  i << "]";
            visit(element, ss.str().c_str());
            i++;
        }
        indentation -= dIndent;
    }
#endif

    template <typename Type, class ReflectionType, TR_ENABLE_IF_P(!TR_IS_SAME(ReflectionType, char))>
    void visit(Type &field, const ReflectionType * fieldDescriptor)
        {
            if (stream != NULL) {
                *stream << indent() << fieldDescriptor->getSimpleName() << ":" << std::endl;
            }
            indentation += dIndent;
            field.accept(*this);
            indentation -= dIndent;
        }

    template <class Type>
        void visit(Type &field, const char * fieldName)
        {
            if (stream != NULL) {
                *stream << indent() << fieldName << ":" << std::endl;
            }
            indentation += dIndent;
            field.accept(*this);
            indentation -= dIndent;
        }


    template <typename FirstType, typename SecondType>
    void visit(std::pair<FirstType, SecondType> &field, const char* name)
    {
        SYNC_PRINT(("void visit(std::pair<FirstType, SecondType> &field, const char* name): called"));
    }

    template <typename Type, typename std::enable_if<!(std::is_enum<Type>::value || (std::is_arithmetic<Type>::value && !std::is_same<bool, Type>::value)), int>::type foo = 0>
    void visit(Type &field, Type /*defaultValue*/, const char * fieldName)
    {
        visit<Type>(field, fieldName);
    }

    template <typename type, typename std::enable_if<std::is_arithmetic<type>::value && !std::is_same<bool, type>::value, int>::type foo = 0>
    void visit(type &field, type, const char *fieldName)
    {
        if (stream == NULL) return;
        *stream << indent() << fieldName << "=" << field << std::endl;
    }

    template <typename type, TR_ENABLE_IF_P(TR_IS_ENUM(type))>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        using U = typename std::underlying_type<type>::type;
        U u = static_cast<U>(field);
        visit(u, static_cast<U>(defaultValue), fieldName);
        field = static_cast<type>(u);
    }
};


template <>
void PrinterVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void PrinterVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void PrinterVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void PrinterVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void PrinterVisitor::visit<string, StringField>(string &field, const StringField *fieldDescriptor);

template <>
void PrinterVisitor::visit<wstring, WStringField>(wstring &field, const WStringField *fieldDescriptor);

template <>
void PrinterVisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void PrinterVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void PrinterVisitor::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

/* Arrays */

template <>
void PrinterVisitor::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

template <>
void PrinterVisitor::visit<int, IntVectorField>(std::vector<int> &field, const IntVectorField *fieldDescriptor);

/**
 * Old Style
 *
 * this methods can be made universal, but are separated to make it a bit more controllable
 **/

template <>
void PrinterVisitor::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);


} //namespace corecvs
#endif // PRINTER_VISITOR_OLD_H_

#ifndef PRINTER_VISITOR_OLD_H_
#define PRINTER_VISITOR_OLD_H_

#include <iostream>
#include "reflection.h"

namespace corecvs {

using std::string;
using std::ostream;
using std::cout;

class PrinterVisitor
{
public:
    std::ostream &stream;

    explicit  PrinterVisitor(ostream &_stream = cout) :
        stream(_stream)
    {}


template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * /*fieldDescriptor*/)
    {
        field.accept(*this);
    }

template <class Type>
    void visit(Type &field, Type defaultValue, const char *fieldName)
	{
		field.accept(*this);
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
void PrinterVisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void PrinterVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void PrinterVisitor::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

/* Old style visitor */
template <>
void PrinterVisitor::visit<int>(int &intField, int defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<float>(float &floatField, float defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);


} //namespace corecvs
#endif // PRINTER_VISITOR_OLD_H_

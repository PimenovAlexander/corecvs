#include "printerVisitor.h"

namespace corecvs {

using std::string;
using std::endl;

template <>
void PrinterVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    stream << fieldDescriptor->getSimpleName() << "=" << field << endl;
}


/* Old style visitor */
template <>
void PrinterVisitor::visit<int>(int &intField, int /*defaultValue*/, const char *fieldName)
{
    stream << fieldName << "=" << intField << endl;
}

template <>
void PrinterVisitor::visit<double>(double &doubleField, double /*defaultValue*/, const char *fieldName)
{
    stream << fieldName << "=" << doubleField << endl;
}

template <>
void PrinterVisitor::visit<float>(float &floatField, float /*defaultValue*/, const char *fieldName)
{
    stream << fieldName << "=" << floatField << endl;
}

template <>
void PrinterVisitor::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    stream << fieldName << "=" << boolField << endl;
}




} //namespace corecvs

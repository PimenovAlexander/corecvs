#include "printerVisitor.h"

namespace corecvs {

using std::string;
using std::endl;

template <>
void PrinterVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}

template <>
void PrinterVisitor::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "=" << field << endl;
}


/* Arrays block */

template <>
void PrinterVisitor::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    if (stream == NULL) return;
    *stream << indent() << fieldDescriptor->getSimpleName() << "= [";
    for (size_t i = 0; i < field.size(); i++ )
    {
        *stream << ( i == 0 ? " " : ", ") << field[i] ;
    }
    *stream << "]" << endl;
}

/* Old style visitor */

template <>
void PrinterVisitor::visit<int>(int &intField, int /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    *stream << indent() << fieldName << "=" << intField << endl;
}

template <>
void PrinterVisitor::visit<double>(double &doubleField, double /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    *stream << indent() << fieldName << "=" << doubleField << endl;
}

template <>
void PrinterVisitor::visit<float>(float &floatField, float /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    *stream << indent() << fieldName << "=" << floatField << endl;
}

template <>
void PrinterVisitor::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    *stream << indent() << fieldName << "=" << boolField << endl;
}

template <>
void PrinterVisitor::visit<std::string>(std::string &stringField, std::string /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    *stream << indent() << fieldName << "=" << stringField << endl;
}




} //namespace corecvs

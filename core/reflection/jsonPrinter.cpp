#include "reflection/jsonPrinter.h"
#include "utils/utils.h"

namespace corecvs {

using std::string;
using std::endl;

const std::string JSONPrinter::LF = "\n";

const std::string JSONPrinter::PROLOGUE = "{";
const std::string JSONPrinter::EPILOGUE = "}";

const std::string JSONPrinter::OBJECT_OPEN  = "{";
const std::string JSONPrinter::OBJECT_CLOSE = "}";

const std::string JSONPrinter::ARRAY_OPEN  = "[";
const std::string JSONPrinter::ARRAY_CLOSE = "]";

const std::string JSONPrinter::FIELD_SEPARATOR = ",";
const std::string JSONPrinter::FIELD_VALUE_SEPARATOR = ":";

const std::string JSONPrinter::NAME_DECORATOR = "\"";


std::string JSONPrinter::escapeString(const std::string &str)
{
    return HelperUtils::escapeString(str,
    {{'"', '"'},
     {'\\', '\\'},
     {'\b','b'},
     {'\f', 'f'},
     {'\r', 'r'},
     {'\n', 'n'},
     {'\t', 't'}},"\\");
}

template <>
void JSONPrinter::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  field;
}


template <>
void JSONPrinter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  field;
}

template <>
void JSONPrinter::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  field;
}

template <>
void JSONPrinter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  NAME_DECORATOR << field << "u64" << NAME_DECORATOR;
}

template <>
void JSONPrinter::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  (field ? "true" : "false");
}

template <>
void JSONPrinter::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  NAME_DECORATOR << escapeString(field) << NAME_DECORATOR;
}

template <>
void JSONPrinter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  NAME_DECORATOR << /*escapeString(field)*/ "Unsupported" << NAME_DECORATOR;
    CORE_UNUSED(field);
}

template <>
void JSONPrinter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  field;
}

template <>
void JSONPrinter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) <<  FIELD_VALUE_SEPARATOR <<  field;
}


/* Arrays block */

template <>
void JSONPrinter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldDescriptor) << FIELD_VALUE_SEPARATOR << " " << ARRAY_OPEN;
    for (size_t i = 0; i < field.size(); i++ )
    {
        *stream << ( i == 0 ? " " : ", ") << field[i];
    }
    *stream << ARRAY_CLOSE;
}

/* Old style visitor */

template <>
void JSONPrinter::visit<uint64_t>(uint64_t &intField, const uint64_t &/*defaultValue*/, const char *fieldName)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR <<  NAME_DECORATOR << intField << "u64" << NAME_DECORATOR;
}

template <>
void JSONPrinter::visit<bool>(bool &boolField, const bool &/*defaultValue*/, const char *fieldName)
{
    if (!stream) return;
 // *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR << NAME_DECORATOR << (boolField ? "true" : "false") << NAME_DECORATOR;
    *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR << (boolField ? "true" : "false");
}

template <>
void JSONPrinter::visit<std::string>(std::string &stringField, const std::string & /*defaultValue*/, const char *fieldName)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR << NAME_DECORATOR << escapeString(stringField) << NAME_DECORATOR;
}

template <>
void JSONPrinter::visit<std::wstring>(std::wstring &stringField, const std::wstring & /*defaultValue*/, const char *fieldName)
{
    if (!stream) return;
    *stream << separate() << indent() << decorateName(fieldName) << FIELD_VALUE_SEPARATOR << NAME_DECORATOR << /*escapeString(stringField)*/ "Unsupported" << NAME_DECORATOR;
    CORE_UNUSED(stringField);
}


} //namespace corecvs

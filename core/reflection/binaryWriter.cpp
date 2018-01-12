#include "core/reflection/binaryWriter.h"
#include "core/utils/utils.h"

namespace corecvs {

const char* BinaryVisitorBase::MAGIC_STRING1 = "MJKPACK1";
const char* BinaryVisitorBase::MAGIC_STRING2 = " EOF";


template <>
void BinaryWriter::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    SYNC_PRINT(("BinaryWriter::visit<int,IntField>(%d):called\n", field));
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}


template <>
void BinaryWriter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    SYNC_PRINT(("BinaryWriter::visit<double, DoubleField>(%lf):called\n", field));
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}

template <>
void BinaryWriter::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    SYNC_PRINT(("BinaryWriter::visit<float,  FloatField>(%f):called\n", field));
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}

template <>
void BinaryWriter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor)
{
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}


template <>
void BinaryWriter::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}

template <>
void BinaryWriter::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    if (stream == NULL) return;
    visit<string>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void BinaryWriter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void BinaryWriter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void BinaryWriter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}


/* Arrays block */

template <>
void BinaryWriter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

/* Old style visitor */

template <>
void BinaryWriter::visit<uint64_t>(uint64_t &intField, uint64_t /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    stream->write((char *) &intField, sizeof(intField));
}

template <>
void BinaryWriter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    stream->write((char *) &boolField, sizeof(boolField));
}

template <>
void BinaryWriter::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    if (stream == NULL) return;
    stream->write((char *) &intField, sizeof(intField));
}


template <>
void BinaryWriter::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
{
    SYNC_PRINT(("BinaryWriter::visit<double>(%lf):called\n", doubleField));
    if (stream == NULL) return;
    stream->write((char *) &doubleField, sizeof(doubleField));
}

template <>
void BinaryWriter::visit<std::string>(std::string &field, std::string /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    SYNC_PRINT(("BinaryWriter::visit<double>(%s):called\n", field.c_str()));
    uint32_t length = field.length();
    stream->write((char *)&length, sizeof(length));
    stream->write((char *)field.c_str(), length);

}

template <>
void BinaryWriter::visit<std::wstring>(std::wstring &stringField, std::wstring /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}




} //namespace corecvs

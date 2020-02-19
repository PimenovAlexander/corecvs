#include "reflection/binaryReader.h"
#include "utils/utils.h"

namespace corecvs {


template <>
void BinaryReader::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    stream->read((char *) &field, sizeof(field));
    SYNC_PRINT(("BinaryReader::visit<int,IntField>(): read %d\n", field));
}


template <>
void BinaryReader::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    stream->read((char *) &field, sizeof(field));
    SYNC_PRINT(("BinaryReader::visit<double, DoubleField>():read %lf\n", field));
}

template <>
void BinaryReader::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    stream->read((char *) &field, sizeof(field));
    SYNC_PRINT(("BinaryReader::visit<float, FloatField>():read %f\n", field));
}

template <>
void BinaryReader::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    stream->read((char *) &field, sizeof(field));
}

template <>
void BinaryReader::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    stream->read((char *) &field, sizeof(field));
}

template <>
void BinaryReader::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
    //stream->read((char *) &field, sizeof(field));
}

template <>
void BinaryReader::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void BinaryReader::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void BinaryReader::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    stream->read((char *) &field, sizeof(field));
}


/* Arrays block */

template <>
void BinaryReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

/* Old style visitor */

template <>
void BinaryReader::visit<uint64_t>(uint64_t &intField, uint64_t /*defaultValue*/, const char *fieldName)
{
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    stream->read((char *) &intField, sizeof(intField));
}

template <>
void BinaryReader::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    stream->read((char *) &boolField, sizeof(boolField));
}

template <>
void BinaryReader::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    CORE_UNUSED(defaultValue);
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    stream->read((char *) &intField, sizeof(intField));
    SYNC_PRINT(("BinaryReader::visit<int>(): read %d\n", intField));
}


template <>
void BinaryReader::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
{
    CORE_UNUSED(defaultValue);
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    stream->read((char *) &doubleField, sizeof(doubleField));
    SYNC_PRINT(("BinaryReader::visit<double>(): read %lf\n", doubleField));
}

template <>
void BinaryReader::visit<std::string>(std::string &stringField, std::string /*defaultValue*/, const char *fieldName)
{
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    uint32_t length = 0;
    stream->read((char *)&length, sizeof(length));
    char* data = new char[length + 1];
    stream->read((char *)data, length);
    data[length] = 0;
    stringField = data;
    SYNC_PRINT(("BinaryReader::visit<std::string>():read %s\n", stringField.c_str()));
}

template <>
void BinaryReader::visit<std::wstring>(std::wstring &stringField, std::wstring /*defaultValue*/, const char *fieldName)
{
    CORE_UNUSED(stringField);
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}


} //namespace corecvs

#include "reflection/advanced/advancedBinaryWriter.h"
#include "utils/utils.h"

namespace corecvs {

const char* AdvancedBinaryVisitorBase::MAGIC_STRING1 = "MJKPACK1";
const char* AdvancedBinaryVisitorBase::MAGIC_STRING2 = " EOF";


template <>
void AdvancedBinaryWriter::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    SYNC_PRINT(("AdvancedBinaryWriter::visit<int,IntField>(%d):called\n", field));
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldDescriptor->name.name);
    cout << "Writing int to " << stream->tellp() << std::endl;
    stream->write((char *) &field, sizeof(field));
    fieldClosing(pos);
}


template <>
void AdvancedBinaryWriter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    SYNC_PRINT(("AdvancedBinaryWriter::visit<double, DoubleField>(%lf):called\n", field));
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldDescriptor->name.name);
    stream->write((char *) &field, sizeof(field));
    fieldClosing(pos);
}

template <>
void AdvancedBinaryWriter::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    CORE_UNUSED(fieldDescriptor);
    SYNC_PRINT(("AdvancedBinaryWriter::visit<float,  FloatField>(%f):called\n", field));
    if (stream == NULL) return;
    stream->write((char *) &field, sizeof(field));
}

template <>
void AdvancedBinaryWriter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor)
{
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldDescriptor->name.name);
    stream->write((char *) &field, sizeof(field));
    fieldClosing(pos);
}


template <>
void AdvancedBinaryWriter::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldDescriptor->name.name);
    stream->write((char *) &field, sizeof(field));
    fieldClosing(pos);
}

template <>
void AdvancedBinaryWriter::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldDescriptor->name.name);
    visit<string>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
    fieldClosing(pos);
}

template <>
void AdvancedBinaryWriter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void AdvancedBinaryWriter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void AdvancedBinaryWriter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldDescriptor->name.name);
    stream->write((char *) &field, sizeof(field));
    fieldClosing(pos);
}


/* Arrays block */

template <>
void AdvancedBinaryWriter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    CORE_UNUSED(field);
    CORE_UNUSED(fieldDescriptor);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

/* Old style visitor */

template <>
void AdvancedBinaryWriter::visit<uint64_t>(uint64_t &intField, uint64_t /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldName);
    stream->write((char *) &intField, sizeof(intField));
    fieldClosing(pos);
}

template <>
void AdvancedBinaryWriter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldName);
    stream->write((char *) &boolField, sizeof(boolField));
    fieldClosing(pos);
}

template <>
void AdvancedBinaryWriter::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    CORE_UNUSED(defaultValue);
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldName);
    stream->write((char *) &intField, sizeof(intField));
    fieldClosing(pos);
}


template <>
void AdvancedBinaryWriter::visit<double>(double &doubleField, double /*defaultValue*/, const char *fieldName)
{
    SYNC_PRINT(("AdvancedBinaryWriter::visit<double>(%lf):called\n", doubleField));
    if (stream == NULL) return;
    size_t pos = fieldOpening(fieldName);
    stream->write((char *) &doubleField, sizeof(doubleField));
    fieldClosing(pos);
}

template <>
void AdvancedBinaryWriter::visit<std::string>(std::string &field, std::string /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    SYNC_PRINT(("AdvancedBinaryWriter::visit<std::string>(%s):called\n", field.c_str()));
    size_t pos = fieldOpening(fieldName);
    SYNC_PRINT(("AdvancedBinaryWriter::visit<std::string> pos %d\n", (int)stream->tellp()));
    writeString(stream, field);
    fieldClosing(pos);

}

template <>
void AdvancedBinaryWriter::visit<std::wstring>(std::wstring &stringField, std::wstring /*defaultValue*/, const char *fieldName)
{
    CORE_UNUSED(stringField);
    CORE_UNUSED(fieldName);
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}


} //namespace corecvs

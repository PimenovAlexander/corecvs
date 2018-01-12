#include "core/reflection/advanced/advancedBinaryReader.h"
#include "core/utils/utils.h"

namespace corecvs {

bool AdvancedBinaryReader::repositionToField(const char *name)
{
    if (stream == NULL) return false;

    auto it = dicts.back().find(name);
    if (it == dicts.back().end())
    {
        std::cout << "Name not found:" << name << std::endl;
        return false;
    }

    stream->seekg(it->second);
    return true;
}

template<typename Type>
bool  AdvancedBinaryReader::loadField(Type &field, const std::string &name)
{
    if (!repositionToField(name.c_str()))
        return false;

    uint32_t size;
    stream->read((char *) &size, sizeof(size));
    if (size != sizeof(Type)) {
        std::cout << "Internal format error" << std::endl;
    }

    cout << "Reading from" << stream->tellg() << std::endl;
    stream->read((char *) &field, sizeof(field));
    return true;
}


template <>
void AdvancedBinaryReader::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    if (stream == NULL) return;
    if (!loadField<int>(field, fieldDescriptor->name.name))
        field = fieldDescriptor->defaultValue;

    SYNC_PRINT(("AdvancedBinaryReader::visit<int,IntField>(): read %d\n", field));
}

template <>
void AdvancedBinaryReader::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    if (stream == NULL) return;
    if (!loadField<double>(field, fieldDescriptor->name.name))
    {
        field = fieldDescriptor->defaultValue;
    }
    SYNC_PRINT(("AdvancedBinaryReader::visit<double, DoubleField>():read %lf\n", field));
}

template <>
void AdvancedBinaryReader::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    if (stream == NULL) return;
    if (!loadField<float>(field, fieldDescriptor->name.name)) {
        field = fieldDescriptor->defaultValue;
    }
    SYNC_PRINT(("AdvancedBinaryReader::visit<float, FloatField>():read %f\n", field));

}

template <>
void AdvancedBinaryReader::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor)
{
    if (stream == NULL) return;
    if (!loadField<uint64_t>(field, fieldDescriptor->name.name))
    {
        field = fieldDescriptor->defaultValue;
    }
    stream->read((char *) &field, sizeof(field));
}


template <>
void AdvancedBinaryReader::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    if (stream == NULL) return;
    if (!loadField<bool>(field, fieldDescriptor->name.name))
        field = fieldDescriptor->defaultValue;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void AdvancedBinaryReader::visit<string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    if (!repositionToField(fieldDescriptor->name.name))
    {
        field = fieldDescriptor->defaultValue;
        return;
    }

    field = readString(stream);

    SYNC_PRINT(("AdvancedBinaryReader::visit<string, StringField> %s\n", field.c_str()));
}

template <>
void AdvancedBinaryReader::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void AdvancedBinaryReader::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

template <>
void AdvancedBinaryReader::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    if (stream == NULL) return;
    loadField<int>(field, fieldDescriptor->name.name);
}


/* Arrays block */

template <>
void AdvancedBinaryReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    if (stream == NULL) return;

    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}

/* Old style visitor */

template <>
void AdvancedBinaryReader::visit<uint64_t>(uint64_t &field, uint64_t defaultValue, const char *fieldName)
{
    if (stream == NULL) {
        field = defaultValue;
        return;
    }
    loadField<uint64_t>(field, fieldName);
}

template <>
void AdvancedBinaryReader::visit<bool>(bool &field, bool defaultValue, const char *fieldName)
{
    if (stream == NULL) {
        field = defaultValue;
        return;
    }
    loadField<bool>(field, fieldName);
}

template <>
void AdvancedBinaryReader::visit<int>(int &field, int defaultValue, const char *fieldName)
{
    if (stream == NULL) return;
    loadField<int>(field, fieldName);
    SYNC_PRINT(("AdvancedBinaryReader::visit<int>(): read %d\n", field));
}


template <>
void AdvancedBinaryReader::visit<double>(double &field, double defaultValue, const char *fieldName)
{
    if (stream == NULL) return;
    loadField<double>(field, fieldName);
    SYNC_PRINT(("AdvancedBinaryReader::visit<double>(): read %lf\n", field));
}

template <>
void AdvancedBinaryReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName)
{
    if (!repositionToField(fieldName))
    {
        stringField = defaultValue;
        return;
    }
    uint32_t size;
    stream->read((char *) &size, sizeof(size));

    SYNC_PRINT(("AdvancedBinaryReader::visit<std::string> pos %d\n", (int)stream->tellg()));
    stringField = readString(stream);
    SYNC_PRINT(("AdvancedBinaryReader::visit<std::string>():read %s\n", stringField.c_str()));
}

template <>
void AdvancedBinaryReader::visit<std::wstring>(std::wstring &stringField, std::wstring /*defaultValue*/, const char *fieldName)
{
    if (stream == NULL) return;
    SYNC_PRINT(("%s : NYI\n", __FUNCTION__));
}




} //namespace corecvs

#include <sstream>

#include "utils.h"
#include "rapidjson/filereadstream.h"
#include "rapidJSONReader.h"

using namespace corecvs;

#define CONDITIONAL_TRACE(X) \
    if (trace) {             \
        SYNC_PRINT(X);       \
    }                        \


void RapidJSONReader::init(const char *fileName)
{
    mFileName = fileName;

    FILE* fp = fopen(fileName, "rb"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    mDocument.ParseStream(is);
    //rapidjson::Value &value = mDocument.GetObject();
    mNodePath.push_back(&mDocument);
}

template <>
void RapidJSONReader::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName)
{
    rapidjson::Value &value = (*mNodePath.back())[fieldName];

    if (value.IsBool()) {
        boolField = value.GetBool();
    } else {
        boolField = defaultValue;
    }
}

/**
 *   Because uint64_t is not supported we convert the value to string manually
 **/
template <>
void RapidJSONReader::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName)
{
    CONDITIONAL_TRACE(("RapidJSONReader::visit<uint64_t>(_ , _ , %s)", fieldName));
    rapidjson::Value &value = (*mNodePath.back())[fieldName];

    intField = defaultValue;

    if (value.IsString()) {
        std::string string(value.GetString());
        const char *postfix = "u64";

        if (HelperUtils::endsWith(string, postfix)) {
            string = string.substr(0, string.length() - strlen(postfix));

            std::istringstream ss(string);
            uint64_t res = 0;
            ss >> res;
            if (!ss.bad()) {
                intField = res;
                CONDITIONAL_TRACE(( "RapidJSONReader::visit<uint64_t>() got %" PRIu64 "\n", intField ));
            } else {
                SYNC_PRINT(( "RapidJSONReader::visit<uint64_t>() unable to parse %s \n", string.c_str()));
            }
        }
    }

}


template <>
void RapidJSONReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName)
{
    rapidjson::Value &value = (*mNodePath.back())[fieldName];

    if (value.IsString()) {
        stringField = std::string(value.GetString());
    } else {
        stringField = defaultValue;
    }
}

/* And new style visitor method */

template <>
void RapidJSONReader::visit<unsigned char, IntField>(unsigned char &intField, const IntField * fieldDescriptor)
{
    int foo = intField;
    visit<int>(foo, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
    intField = foo;
}

template <>
void RapidJSONReader::visit<int, IntField>(int &intField, const IntField * fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<uint64_t, UInt64Field>(uint64_t &intField, const UInt64Field *fieldDescriptor)
{
    visit<uint64_t>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<double, DoubleField>(double &doubleField, const DoubleField * fieldDescriptor)
{    
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<float, FloatField>(float &floatField, const FloatField * fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<bool, BoolField>(bool &boolField, const BoolField * fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<std::string, StringField>(std::string &stringField, const StringField * fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{
    SYNC_PRINT(("RapidJSONReader::visit<int, PointerField>(void * &field, const PointerField * /*fieldDescriptor*/) NOT YET SUPPORTED"));
}

template <>
void RapidJSONReader::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    visit<int>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    field.clear();

    rapidjson::Value &arrayValue = (*mNodePath.back())[fieldDescriptor->name.name];
    if (arrayValue.IsArray())
    {
        for (rapidjson::SizeType i = 0; i < arrayValue.Size(); i++)
        {
            rapidjson::Value &value = arrayValue[i];
            if (value.IsNumber()) {
                field.push_back(value.GetDouble());
            }
            else {
                field.push_back(fieldDescriptor->getDefaultElement(i));
            }
        }
    }
    else
    {
        for (uint i = 0; i < fieldDescriptor->defaultSize; i++)
        {
            field.push_back(fieldDescriptor->getDefaultElement(i));
        }
    }
}

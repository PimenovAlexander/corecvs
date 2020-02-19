#include <sstream>
#include <fstream>

#include "utils/utils.h"
#include "jsonModernReader.h"

using namespace corecvs;

#define CONDITIONAL_TRACE(X) \
    if (trace) {             \
        SYNC_PRINT(X);       \
    }                        \


JSONModernReader::JSONModernReader(std::istream &is)
{
    is >> mDocument;
    mNodePath.push_back(&mDocument);
}

void JSONModernReader::init(const char *fileName)
{
    mFileName = fileName;

    try {
        std::ifstream is;
        is.open(fileName, std::ifstream::in);
        is >> mDocument;
        mNodePath.push_back(&mDocument);
    }
    catch (nlohmann::detail::exception &e)
    {
        SYNC_PRINT(("JSONModernReader::init(%s): json parsing had an exception <%s>\n", fileName, e.what()));
        mHasError = true;
    }
}

template <>
void JSONModernReader::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName)
{
    nlohmann::json &value = (*mNodePath.back())[fieldName];

    if (value.is_boolean()) {
        boolField = value;
    } else {
        boolField = defaultValue;
    }
}

/**
 *   Because uint64_t is not supported we convert the value to string manually
 **/
template <>
void JSONModernReader::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName)
{
    CONDITIONAL_TRACE(("JSONModernReader::visit<uint64_t>(_ , _ , %s)", fieldName));
    nlohmann::json &value = (*mNodePath.back())[fieldName];

    intField = defaultValue;

    if (value.is_string()) {
        std::string string = value;
        const char *postfix = "u64";

        if (HelperUtils::endsWith(string, postfix)) {
            string = string.substr(0, string.length() - strlen(postfix));

            std::istringstream ss(string);
            uint64_t res = 0;
            ss >> res;
            if (!ss.bad()) {
                intField = res;
                CONDITIONAL_TRACE(( "JSONModernReader::visit<uint64_t>() got %" PRIu64 "\n", intField));
            } else {
                SYNC_PRINT(( "JSONModernReader::visit<uint64_t>() unable to parse %s\n", string.c_str()));
            }
        }
    }

}


template <>
void JSONModernReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName)
{
    nlohmann::json &current = (*mNodePath.back());
    if (!current.is_object()) {
        SYNC_PRINT(( "JSONModernReader::visit<std::string>() unable to find object where expected\n" ));
        return;
    }
    nlohmann::json &value = (*mNodePath.back())[fieldName];

    if (value.is_string()) {
        stringField = value;
    } else {
        stringField = defaultValue;
    }
}


/* And new style visitor method */

template <>
void JSONModernReader::visit<unsigned char, IntField>(unsigned char &intField, const IntField * fieldDescriptor)
{
    int foo = intField;
    visit<int>(foo, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
    intField = foo;
}

template <>
void JSONModernReader::visit<int, IntField>(int &intField, const IntField * fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONModernReader::visit<uint64_t, UInt64Field>(uint64_t &intField, const UInt64Field *fieldDescriptor)
{
    visit<uint64_t>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONModernReader::visit<double, DoubleField>(double &doubleField, const DoubleField * fieldDescriptor)
{    
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONModernReader::visit<float, FloatField>(float &floatField, const FloatField * fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONModernReader::visit<bool, BoolField>(bool &boolField, const BoolField * fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONModernReader::visit<std::string, StringField>(std::string &stringField, const StringField * fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template<>
void JSONModernReader::visit<std::wstring, WStringField>(std::wstring &/*wstringField*/, const WStringField *fieldDescriptor)
{
    SYNC_PRINT(("JSONModernReader::visit<std::wstring, WStringField>(_, %s): Unsupported!\n", fieldDescriptor->name.name));
    //visit<std::wstring>(wstringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}


template <>
void JSONModernReader::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{
    SYNC_PRINT(("JSONModernReader::visit<int, PointerField>(void * &field, const PointerField * /*fieldDescriptor*/) NOT YET SUPPORTED"));
}

template <>
void JSONModernReader::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    visit<int>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONModernReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    field.clear();

    nlohmann::json &arrayValue = (*mNodePath.back())[fieldDescriptor->name.name];
    if (arrayValue.is_array())
    {
        for (nlohmann::json::size_type i = 0; i < arrayValue.size(); i++)
        {
            nlohmann::json &value = arrayValue[i];
            if (value.is_number()) {
                double d = value;
                field.push_back(d);
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



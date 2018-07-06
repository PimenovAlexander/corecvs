#ifndef RAPIDJSONREADER_H
#define RAPIDJSONREADER_H

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "reflection.h"

using corecvs::IntField;
using corecvs::Int64Field;
using corecvs::UInt64Field;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::PointerField;
using corecvs::EnumField;
using corecvs::DoubleVectorField;

#define CONDITIONAL_TRACE(X) \
    if (trace) {             \
        SYNC_PRINT(X);       \
    }                        \



/**
 *    This class allows to read form JSON file
 **/
class RapidJSONReader
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return true ;}

public:
    /**
    *  Create a getter object that will use data from a file with a specified name.
    *
    * @brief RapidJSONReader
    * @param fileName
    **/
    RapidJSONReader(const char * fileName) { init(fileName); }


    /**
     *  Create a getter object that will use data from a file with a specified name.
     *
     * @brief RapidJSONReader
     * @param fileName
     */
    explicit RapidJSONReader(std::string const & fileName) { init(fileName.c_str()); }

#if 1
    /**
     *  Create a getter object that will use data from a given JSON
     **/
    RapidJSONReader(rapidjson::Value &document)
    {
        mNodePath.push_back(&document);
    }
#endif

    /**
     *  Visitor method that will traverse json and object tree and fill object with data form xml
     *
     **/
    template <class Type>
    void visit(Type &field, const char *fieldName)
    {
        CONDITIONAL_TRACE(("RapidJSONReader::visit(Type &field, %s)\n", fieldName));
        if (!mNodePath.back()->HasMember(fieldName)) {
             SYNC_PRINT(("RapidJSONReader::visit(): member %s not found\n", fieldName));
             return;
        }
        pushChild(fieldName);
            field.accept(*this);
        popChild();
    }

    template <typename Type, typename std::enable_if<!(std::is_enum<Type>::value || (std::is_arithmetic<Type>::value && !(std::is_same<bool, Type>::value || std::is_same<uint64_t, Type>::value))), int>::type foo = 0>
    void visit(Type &field, Type, const char *fieldName)
    {
        visit<Type>(field, fieldName);
    }

    template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        if (!mNodePath.back()->HasMember(fieldDescriptor->getSimpleName())) {
             SYNC_PRINT(("RapidJSONReader::visit(): member %s not found\n", fieldDescriptor->getSimpleName()));
             return;
        }

        pushChild(fieldDescriptor->getSimpleName());
           field.accept(*this);
        popChild();
    }


    /**
     * Generic Array support.
     * Reflection style
     **/
    template <typename inputType, typename reflectionType>
    void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        for (size_t i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
    }

    /**
     * Generic Array support
     * String style
     **/
    template <typename inputType>
    void visit(std::vector<inputType> &fields, const char* arrayName)
    {
        fields.clear();
        rapidjson::Value &mainNode = mNodePath.back();
        if (mainNode[arrayName].IsArray())
        {
            rapidjson::Value array = mainNode[arrayName];
            for (auto &v: array.GetArray())
            {
                inputType el;
                mNodePath.push_back(&v);
                el.accept(*this);
                fields.push_back(el);
                mNodePath.pop_back();
            }
        }
    }

    void visit(std::vector<std::string> &fields, rapidjson::Value &array)
    {
        fields.clear();
        for (auto &v: array.GetArray())
        {
            fields.push_back(std::string(v.GetString()));
        }
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, const char* arrayName)
    {
        fields.clear();
        rapidjson::Value mainNode = mNodePath.back();

        if (mainNode[arrayName].IsArray())
        {
            rapidjson::Value array = mainNode[arrayName];

            for(auto& ai : array.GetArray())
            {
                if (!ai.IsArray())
                    continue;
                std::vector<innerType> inner;
                visit(inner, ai);
                fields.push_back(inner);
            }
        }
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, rapidjson::Value &array)
    {
        fields.clear();
        for (auto &v: array.GetArray())
        {
            if (!v.IsArray())
                continue;
            std::vector<innerType> inner;
            rapidjson::Value &array = v.GetArray();
            visit(inner, array);
            fields.push_back(inner);
        }
    }



    template <typename innerType, typename std::enable_if<!std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, rapidjson::Value &array)
    {
        fields.clear();
        for (auto &v: array.GetArray())
        {
            innerType el;
            mNodePath.push_back(&v);
            el.accept(*this);
            fields.push_back(el);
            mNodePath.pop_back();
        }
    }


    template <typename innerType, typename std::enable_if<std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, rapidjson::Value &array)
    {
        fields.clear();
        for (auto &v: array.GetArray())
        {
            if (v.IsDouble())
                fields.push_back(v.GetDouble());
        }
    }

    template <typename type, typename std::enable_if<std::is_arithmetic<type>::value && !std::is_same<bool, type>::value && !std::is_same<uint64_t, type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        CONDITIONAL_TRACE(("RapidJSONReader::visit(type &field, type defaultValue, %s) v1 \n", fieldName ));
        if (!mNodePath.back()->HasMember(fieldName)) {
             SYNC_PRINT(("RapidJSONReader::visit(): member not found\n"));
             return;
        }

        rapidjson::Value &value = (*mNodePath.back())[fieldName];
        if (value.IsNumber())
        {
            double rawValue = value.GetDouble();
            CONDITIONAL_TRACE(("RapidJSONReader::visit() raw = %lf\n", rawValue));
            field = static_cast<type>(rawValue);
        }
        else
        {
            CONDITIONAL_TRACE(("RapidJSONReader::visit(): not a number\n"));
            field = defaultValue;
        }
    }

    template <typename type, typename std::enable_if<std::is_enum<type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        CONDITIONAL_TRACE(("RapidJSONReader::visit(type &field, type defaultValue, %s) v2 \n", fieldName ));

        using U = typename std::underlying_type<type>::type;
        U u = static_cast<U>(field);
        visit(u, static_cast<U>(defaultValue), fieldName);
        field = static_cast<type>(u);
    }

    void pushChild(const char *childName)
    {
        CONDITIONAL_TRACE(("RapidJSONReader::pushChild(%s)\n", childName));
        rapidjson::Value &mainNode = *mNodePath.back();
        rapidjson::Value &value    = mainNode[childName];
        if (value.IsNull()) {
            CONDITIONAL_TRACE(("RapidJSONReader::pushChild(%s): no child\n", childName));
        } else {
            CONDITIONAL_TRACE(("RapidJSONReader::pushChild(): depth %u\n", (unsigned)mNodePath.size()));
        }
        mNodePath.push_back(&value);
    }

    void popChild()
    {
        CONDITIONAL_TRACE(("RapidJSONReader::popChild() from depth %u\n", (unsigned)mNodePath.size()));
        mNodePath.pop_back();

    }

private:
    void init(const char *fileName);


    std::vector<rapidjson::Value *> mNodePath;
    std::string         mFileName;
    rapidjson::Document mDocument;
    bool trace = false;
};

#undef CONDITIONAL_TRACE

template <>
void RapidJSONReader::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void RapidJSONReader::visit<bool>  (bool &boolField, bool defaultValue, const char * fieldName);

template <>
void RapidJSONReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char* fieldName);

/* New style visitor */

template <>
void RapidJSONReader::visit<unsigned char, IntField>(unsigned char &field, const IntField *fieldDescriptor);

template <>
void RapidJSONReader::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void RapidJSONReader::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void RapidJSONReader::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void RapidJSONReader::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void RapidJSONReader::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void RapidJSONReader::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void RapidJSONReader::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void RapidJSONReader::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void RapidJSONReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);




#endif // RAPIDJSONREADER_H

#ifndef JSON_MODERN_READER_H
#define JSON_MODERN_READER_H

#if defined(_MSC_VER) && (_MSC_VER < 1900)      // fix bug for older vs than msvc2015
# define constexpr const
# define noexcept
# define noexcept_if(pred)
#else
# define noexcept_if(pred)   noexcept((pred))
#endif

#include "json.hpp"

#include "core/reflection/reflection.h"

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

#define CONDITIONAL_DEEPTRACE(X) \
    if (deepTrace) {             \
        SYNC_PRINT(X);       \
    }


/**
 *    This class allows to read form JSON file
 **/
class JSONModernReader
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return true ;}

public:
    /**
    *  Create a getter object that will use data from a file with a specified name.
    *
    * @brief JSONModernReader
    * @param fileName
    **/
    JSONModernReader(const char * fileName) { init(fileName); }


    /**
     *  Create a getter object that will use data from a file with a specified name.
     *
     * @brief JSONModernReader
     * @param fileName
     */
    explicit JSONModernReader(std::string const & fileName) { init(fileName.c_str()); }


    explicit JSONModernReader(std::istream &is);

#if 1
    /**
     *  Create a getter object that will use data from a given JSON
     **/
    JSONModernReader(nlohmann::json &document)
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
        CONDITIONAL_TRACE(("JSONModernReader::visit(Type &field, %s)\n", fieldName));
        if (mNodePath.empty() || mNodePath.back() == NULL) {
            SYNC_PRINT(("JSONModernReader::visit(Type &field, %s) is impossible\n", fieldName));
            return;
        }
        if (mNodePath.back()->count(fieldName) == 0) {
            CONDITIONAL_TRACE(("JSONModernReader::visit(): member %s not found\n", fieldName));
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
        if (mNodePath.empty() || mNodePath.back() == NULL || !fieldDescriptor) {
            SYNC_PRINT(("JSONModernReader::visit(inputType &field, field:%s) is impossible\n", fieldDescriptor ? fieldDescriptor->getSimpleName() : "none"));
            return;
        }
        if (mNodePath.back()->count(fieldDescriptor->getSimpleName()) == 0) {
            CONDITIONAL_TRACE(("JSONModernReader::visit(): member field:%s not found\n", fieldDescriptor->getSimpleName()));
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
        nlohmann::json &mainNode = *mNodePath.back();
        if (mainNode[arrayName].is_array())
        {
            nlohmann::json array = mainNode[arrayName];
            for (auto &v: array)
            {
                inputType el;
                mNodePath.push_back(&v);
                el.accept(*this);
                fields.push_back(el);
                mNodePath.pop_back();
            }
        }
    }

    void visit(std::vector<std::string> &fields, nlohmann::json &array)
    {
        fields.clear();
        for (auto &v: array)
        {
            std::string s = v;
            fields.push_back(s);
        }
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, const char* arrayName)
    {
        fields.clear();
        nlohmann::json &mainNode = *mNodePath.back();

        if (mainNode[arrayName].is_array())
        {
            nlohmann::json array = mainNode[arrayName];

            for(auto& ai : array)
            {
                if (!ai.is_array())
                    continue;
                std::vector<innerType> inner;
                visit(inner, ai);
                fields.push_back(inner);
            }
        }
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, nlohmann::json &array)
    {
        fields.clear();
        for (auto &v: array)
        {
            if (!v.is_array())
                continue;
            std::vector<innerType> inner;
            visit(inner, v);
            fields.push_back(inner);
        }
    }



    template <typename innerType, typename std::enable_if<!std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, nlohmann::json &array)
    {
        fields.clear();
        for (auto &v: array)
        {
            innerType el;
            mNodePath.push_back(&v);
            el.accept(*this);
            fields.push_back(el);
            mNodePath.pop_back();
        }
    }


    template <typename innerType, typename std::enable_if<std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, nlohmann::json &array)
    {
        fields.clear();
        for (auto &v: array)
        {
            if (v.is_number()) {
                double d = v;
                fields.push_back(d);
            }
        }
    }

    template <typename type, typename std::enable_if<std::is_arithmetic<type>::value && !std::is_same<bool, type>::value && !std::is_same<uint64_t, type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        CONDITIONAL_TRACE(("JSONModernReader::visit(type &field, type defaultValue, %s) v1 \n", fieldName));
        if (mNodePath.back()->count(fieldName) == 0) {
            CONDITIONAL_TRACE(("JSONModernReader::visit v1(_,_,%s): member not found\n", fieldName));
            return;
        }

        nlohmann::json &value = (*mNodePath.back())[fieldName];
        if (value.is_number())
        {
            double rawValue = value;
            CONDITIONAL_TRACE(("JSONModernReader::visit() raw = %lf\n", rawValue));
            field = static_cast<type>(rawValue);
        }
        else
        {
            CONDITIONAL_TRACE(("JSONModernReader::visit(): not a number\n"));
            field = defaultValue;
        }
    }

    template <typename type, typename std::enable_if<std::is_enum<type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        CONDITIONAL_TRACE(("JSONModernReader::visit(type &field, type defaultValue, %s) v2 \n", fieldName));

        using U = typename std::underlying_type<type>::type;
        U u = static_cast<U>(field);
        visit(u, static_cast<U>(defaultValue), fieldName);
        field = static_cast<type>(u);
    }

    void pushChild(const char *childName)
    {
        CONDITIONAL_TRACE(("JSONModernReader::pushChild(%s)\n", childName));
        nlohmann::json &mainNode = *mNodePath.back();
        nlohmann::json &value    = mainNode[childName];
        if (value.is_null()) {
            CONDITIONAL_TRACE(("JSONModernReader::pushChild(%s): no child\n", childName));
        } else {
            CONDITIONAL_TRACE(("JSONModernReader::pushChild(): depth %" PRIi64 "\n", mNodePath.size()));
        }
        mNodePath.push_back(&value);
    }

    void popChild()
    {
        CONDITIONAL_TRACE(("JSONModernReader::popChild() from depth %" PRIi64 "\n", mNodePath.size()));
        mNodePath.pop_back();
    }

    bool hasError() const
    {
        return mHasError;
    }

private:
    void init(const char *fileName);


    std::vector<nlohmann::json *> mNodePath;
    std::string                   mFileName;
    nlohmann::json                mDocument;
   
    bool mHasError = false;
public:
    bool trace = false;
    bool deepTrace = false;
};

#undef CONDITIONAL_TRACE
#undef CONDITIONAL_DEEPTRACE


template <>
void JSONModernReader::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void JSONModernReader::visit<bool>  (bool &boolField, bool defaultValue, const char * fieldName);

template <>
void JSONModernReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char* fieldName);

/* New style visitor */

template <>
void JSONModernReader::visit<unsigned char, IntField>(unsigned char &field, const IntField *fieldDescriptor);

template <>
void JSONModernReader::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void JSONModernReader::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void JSONModernReader::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void JSONModernReader::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void JSONModernReader::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void JSONModernReader::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void JSONModernReader::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void JSONModernReader::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void JSONModernReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);




#endif // JSON_MODERN_READER_H

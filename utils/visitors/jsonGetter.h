#ifndef JSONGETTER_H
#define JSONGETTER_H

#include <string>
#include <vector>

#include <QtCore/QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "core/reflection/reflection.h"

using corecvs::IntField;
using corecvs::Int64Field;
using corecvs::UInt64Field;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::WStringField;
using corecvs::PointerField;
using corecvs::EnumField;
using corecvs::DoubleVectorField;
using corecvs::IntVectorField;

/**
 *    This class allows to read form JSON file
 **/
class JSONGetter
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return true; }

public:
    /**
    *  Create a getter object that will use data from a file with a specified name.
    *
    * @brief JSONGetter
    * @param fileName
    **/
    explicit JSONGetter(const char * fileName) { init(fileName); }

    /**
     *  Create a getter object that will use data from a file with a specified name.
     *
     * @brief JSONGetter
     * @param fileName
     **/
    explicit JSONGetter(QString const & fileName) { init(QSTR_DATA_PTR(fileName)); }

    /**
     *  Create a getter object that will use data from a file with a specified name.
     *
     * @brief JSONGetter
     * @param fileName
     */
    explicit JSONGetter(std::string const & fileName) { init(fileName.c_str()); }

    /**
     *  Create a getter object that will use data from a given XML
     **/
    explicit JSONGetter(QJsonObject &document) : mDocument(document)
    {
        mNodePath.push_back(mDocument);
    }

    explicit JSONGetter(const QByteArray &array)
    {
        init(array);
    }


    /**
     *  Visitor method that will traverse json and object tree and fill object with data form xml
     *
     **/
    template <class Type>
    void visit(Type &field, const char *fieldName)
    {
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
        QJsonObject mainNode = mNodePath.back();
        if (mainNode.value(arrayName).isArray())
        {
            QJsonArray array = mainNode.value(arrayName).toArray();
            // FIXME: using ugly Qt's foreach instead of fancy c++11 one since Qt containers are weird
            foreach (QJsonValue v, array)
            {
                inputType el;
                mNodePath.push_back(v.toObject());
                el.accept(*this);
                fields.push_back(el);
                mNodePath.pop_back();
            }
        }
    }

    void visit(std::vector<std::string> &fields, QJsonArray &array)
    {
        fields.clear();
        foreach (QJsonValue v, array)
        {
            fields.push_back(v.toString().toStdString());
        }
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, const char* arrayName)
    {
        fields.clear();
        QJsonObject mainNode = mNodePath.back();
        if (mainNode.value(arrayName).isArray())
        {
            QJsonArray array = mainNode.value(arrayName).toArray();

            foreach (QJsonValue ai, array)
            {
                if (!ai.isArray())
                    continue;
                std::vector<innerType> inner;
                QJsonArray array = ai.toArray();
                visit(inner, array);
                fields.push_back(inner);
            }
        }
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, QJsonArray &array)
    {
        fields.clear();
        foreach (QJsonValue v, array)
        {
            if (!v.isArray())
                continue;
            std::vector<innerType> inner;
            QJsonArray array = v.toArray();
            visit(inner, array);
            fields.push_back(inner);
        }
    }



    template <typename innerType, typename std::enable_if<!std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, QJsonArray &array)
    {
        fields.clear();
        foreach (QJsonValue v, array)
        {
            innerType el;
            mNodePath.push_back(v.toObject());
            el.accept(*this);
            fields.push_back(el);
            mNodePath.pop_back();
        }
    }

    template <typename innerType, typename std::enable_if<std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, QJsonArray &array)
    {
        fields.clear();
        foreach (QJsonValue v, array)
        {
            if (v.isDouble())
                fields.push_back(v.toDouble());
        }
    }

    template <typename type, typename std::enable_if<std::is_arithmetic<type>::value && !std::is_same<bool, type>::value && !std::is_same<uint64_t, type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        auto value = mNodePath.back().value(fieldName);
        if (value.isDouble())
        {
            field = static_cast<type>(value.toDouble());
        }
        else
        {
            field = defaultValue;
        }
    }

    template <typename type, typename std::enable_if<std::is_enum<type>::value, int>::type foo = 0>
    void visit(type &field, type defaultValue, const char *fieldName)
    {
        using U = typename std::underlying_type<type>::type;
        U u = static_cast<U>(field);
        visit(u, static_cast<U>(defaultValue), fieldName);
        field = static_cast<type>(u);
    }

    void pushChild(const char *childName)
    {
        QJsonObject mainNode = mNodePath.back();
        QJsonObject value    = mainNode.value(childName).toObject();
        mNodePath.push_back(value);
    }

    void popChild()
    {
        mNodePath.pop_back();
    }

    bool hasError() const
    {
        return mHasError;
    }

private:
    void init(const char *fileName);
    bool init(const QByteArray &array);

    std::vector<QJsonObject> mNodePath;
    QString                  mFileName;
    QJsonObject              mDocument;
    bool                     mHasError = false;
};

template <>
void JSONGetter::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void JSONGetter::visit<bool>  (bool &boolField, bool defaultValue, const char * fieldName);

template <>
void JSONGetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char* fieldName);

template <>
void JSONGetter::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName);

/* New style visitor */

template <>
void JSONGetter::visit<unsigned char, IntField>(unsigned char &field, const IntField *fieldDescriptor);

template <>
void JSONGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void JSONGetter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void JSONGetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void JSONGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void JSONGetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void JSONGetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void JSONGetter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);

template <>
void JSONGetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void JSONGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void JSONGetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

template <>
void JSONGetter::visit<int, IntVectorField>(std::vector<int> &field, const IntVectorField *fieldDescriptor);


#endif // JSONGETTER_H

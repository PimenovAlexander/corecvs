#ifndef JSONGETTER_H
#define JSONGETTER_H

#include <QtCore/QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "reflection.h"

using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::PointerField;
using corecvs::EnumField;
using corecvs::DoubleVectorField;

using namespace corecvs;

class JSONGetter
{
public:
    /**
     *  Create a getter object that will use data from a file with a specified name.
     *
     **/
    JSONGetter(QString const & fileName);

    /**
     *  Create a getter object that will use data from a given XML
     **/
    JSONGetter(QJsonObject &document) :
        mDocument(document)
    {
        mNodePath.push_back(mDocument);
    }

    /**
     *  Visitor method that will traverse xml and object tree and fill object with data form xml
     *
     **/
    template <class Type>
    void visit(Type &field, Type, const char *fieldName)
    {
        pushChild(fieldName);
            field.accept(*this);
        popChild();
    }

    template <class Type>
        void visit(Type &field, const char *fieldName)
        {
            pushChild(fieldName);
                field.accept(*this);
            popChild();
        }

    template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        pushChild(fieldDescriptor->getSimpleName());
           field.accept(*this);
        popChild();
    }


    /* Generic Array support */
    template <typename inputType, typename reflectionType>
    void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        for (size_t i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
    }

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

private:
    std::vector<QJsonObject> mNodePath;
    QString     mFileName;
    QJsonObject mDocument;
};

template <>
void JSONGetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void JSONGetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void JSONGetter::visit<float>(float &floatField, float defaultValue, const char * /*fieldName*/);

template <>
void JSONGetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);

template <>
void JSONGetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char* fieldName);

/* New style visitor */

template <>
void JSONGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void JSONGetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void JSONGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void JSONGetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void JSONGetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void JSONGetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void JSONGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void JSONGetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);


#endif // JSONGETTER_H

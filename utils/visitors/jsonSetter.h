#ifndef JSONSETTER_H
#define JSONSETTER_H

#include <QtCore/QtCore>
#include <QtCore/QString>
#include <QJsonDocument>
#include <QJsonObject>
#include "reflection.h"

using corecvs::IntField;
using corecvs::UInt64Field;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::PointerField;
using corecvs::EnumField;
using corecvs::DoubleVectorField;

class JSONSetter
{
public:
    bool isSaver () { return true ;}
    bool isLoader() { return false;}


public:



    /**
     *  Create a setter object that will store data to a file with a specified name.
     **/
    JSONSetter(QString const & filename);

    /**
     *  Create a setter object that will store data to a given XML
     **/
    JSONSetter(QJsonObject &document)
    {
        mNodePath.push_back(document);
    }

    virtual ~JSONSetter();

    template <class inputType>
    void visit(inputType &field, const char *fieldName)
    {
        pushChild(fieldName);
            field.accept(*this);
        popChild(fieldName);
    }

    template <class Type>
    void visit(Type &field, Type /*defaultValue*/, const char *fieldName)
    {
        visit(field, fieldName);
    }

    template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        visit(field, fieldDescriptor->getSimpleName());
    }

    /*
     * Generic Array support
     */
    template <typename inputType>
    void visit(std::vector<inputType> &fields, const char * arrayName)
    {
        QJsonArray array;
        for (size_t i = 0; i < fields.size(); i++)
        {
            mNodePath.push_back(QJsonObject());
            fields[i].accept(*this);
            array.append(mNodePath.back());
            mNodePath.pop_back();
        }

        mNodePath.back().insert(arrayName, array);
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, const char *arrayName)
    {
        QJsonArray arrayOuter;
        for (size_t i = 0; i < fields.size(); ++i)
        {
            QJsonArray arrayInner;
            visit(fields[i], arrayInner);
            arrayOuter.append(arrayInner);
        }

        mNodePath.back().insert(arrayName, arrayOuter);
    }

    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, QJsonArray &array)
    {
        for (size_t i = 0; i < fields.size(); ++i)
        {
            QJsonArray arrayInner;
            visit(fields[i], arrayInner);
            array.append(arrayInner);
        }
    }

    void visit(std::vector<std::string> &fields, QJsonArray &array)
    {
        for (size_t i = 0; i < fields.size(); ++i)
        {
           array.append(QString::fromStdString(fields[i]));
        }
    }

    // XXX: Here we hope that either stack is unwinded automatically or it is not too big
    template <typename innerType, typename std::enable_if<!std::is_arithmetic<innerType>::value,int>::type foo = 0>
    void visit(std::vector<innerType> &fields, QJsonArray &array)
    {
        for (size_t i = 0; i < fields.size(); ++i)
        {
            mNodePath.push_back(QJsonObject());
            fields[i].accept(*this);
            array.append(mNodePath.back());
            mNodePath.pop_back();
        }
    }

    template <typename innerType, typename std::enable_if<std::is_arithmetic<innerType>::value,int>::type foo=0>
    void visit(std::vector<innerType> &fields, QJsonArray &array)
    {
        for (size_t i = 0; i < fields.size(); ++i)
        {
            array.append((QJsonValue)(double)fields[i]);
        }
    }

    template <typename inputType, typename reflectionType>
    void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        for (size_t i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
    }

    void pushChild(const char *childName)
    {
        CORE_UNUSED(childName);
        mNodePath.push_back(QJsonObject());
    }

    void popChild(const char *childName)
    {
        QJsonObject mainNode = mNodePath.back();
        mNodePath.pop_back();
        mNodePath.back().insert(childName, mainNode);
    }

private:
    std::vector<QJsonObject> mNodePath;
    QString                  mFileName;
    QString                  mChildName;
};

template <>
void JSONSetter::visit<int>(int &intField, int defaultValue, const char *fieldName);

template <>
void JSONSetter::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void JSONSetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template <>
void JSONSetter::visit<float>(float &floatField, float defaultValue, const char *fieldName);

template <>
void JSONSetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void JSONSetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);


/* New style visitor */

template <>
void JSONSetter::visit<unsigned char, IntField>(unsigned char &field, const IntField *fieldDescriptor);

template <>
void JSONSetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void JSONSetter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void JSONSetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void JSONSetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void JSONSetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void JSONSetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void JSONSetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void JSONSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void JSONSetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

#endif // JSONSETTER_H

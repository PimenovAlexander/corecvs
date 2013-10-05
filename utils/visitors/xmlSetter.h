#pragma once

#include "QtCore/QString"
#include "basePathVisitor.h"
#include "reflection.h"

using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::PointerField;
using corecvs::EnumField;

using namespace corecvs;

class XmlSetter : public BasePathVisitor
{
public:
    XmlSetter(QString const & filename);
    template <class Type>
    	void visit(Type &field, Type defaultValue, const char *fieldName)
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
private:
    QString mFileName;
    void saveValue(const char *fieldName, QString value);
};

template <>
void XmlSetter::visit<int>(int &intField, int defaultValue, const char *fieldName);

template <>
void XmlSetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template <>
void XmlSetter::visit<float>(float &floatField, float defaultValue, const char *fieldName);

template <>
void XmlSetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

/* New style visitor */

template <>
void XmlSetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void XmlSetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void XmlSetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void XmlSetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void XmlSetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void XmlSetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void XmlSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

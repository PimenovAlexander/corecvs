#pragma once

#include <QtCore/QString>
#include <QtXml/QDomElement>
#include "core/reflection/reflection.h"

using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::WStringField;
using corecvs::PointerField;
using corecvs::EnumField;
using corecvs::DoubleVectorField;

#include "core/utils/visitors/basePathVisitor.h"
#include "baseXMLVisitor.h"

using namespace corecvs;

class XmlGetter : public BaseXMLVisitor
{
public:
    /**
     *  Create a getter object that will use data from a file with a specified name.
     *
     **/
    XmlGetter(QString const & fileName);

    /**
     *  Create a getter object that will use data from a given XML
     **/
    XmlGetter(QDomElement &documentElement) :
        mDocumentElement(documentElement)
    {
        mNodePath.push_back(mDocumentElement);
    }

    /**
     *  Visitor method that will traverse xml and object tree and fill object with data form xml
     *
     *
     **/
    template <class Type>
        void visit(Type &field, Type /*defaultValue*/, const char *fieldName)
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

    /** Generic Array support **/
    template <typename inputType, typename reflectionType>
        void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        for (int i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
    }

    template <typename inputType>
        void visit(std::vector<inputType> &/*fields*/, const char* /*arrayName*/)
    {
         qDebug() << "XmlGetter::visit(std::vector<inputType> &fields, const char* arrayName) NOT YET SUPPORTED";
    }

    void pushChild(const char *childName)
    {
        mNodePath.push_back(getChildByTag(childName));
    }

    void popChild()
    {
        mNodePath.pop_back();
    }

private:

    QString mFileName;
    QDomElement mDocumentElement;
};

template <>
void XmlGetter::visit<int>(int &intField, int defaultValue, const char * /*fieldName*/);

template <>
void XmlGetter::visit<double>(double &doubleField, double defaultValue, const char * /*fieldName*/);

template <>
void XmlGetter::visit<float>(float &floatField, float defaultValue, const char * /*fieldName*/);

template <>
void XmlGetter::visit<bool>(bool &boolField, bool defaultValue, const char * /*fieldName*/);

template <>
void XmlGetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);

template <>
void XmlGetter::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName);


/* New style visitor */

template <>
void XmlGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void XmlGetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void XmlGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void XmlGetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void XmlGetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void XmlGetter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);

template <>
void XmlGetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void XmlGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void XmlGetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

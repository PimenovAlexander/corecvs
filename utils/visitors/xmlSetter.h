#ifndef XMLSETTER_H
#define XMLSETTER_H

#include <QtCore/QtCore>
#include "QtCore/QString"

#include "core/utils/visitors/basePathVisitor.h"
#include "core/reflection/reflection.h"
#include "baseXMLVisitor.h"

using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::PointerField;
using corecvs::EnumField;

using namespace corecvs;

class XmlSetter : public BaseXMLVisitor
{
public:
    /**
     *  Create a setter object that will store data to a file with a specified name.
     *
     **/
    XmlSetter(QString const & filename);

    /**
     *  Create a setter object that will store data to a given XML
     **/
    XmlSetter(QDomElement &documentElement) :
        mDocumentElement(documentElement)
    {
        mNodePath.push_back(mDocumentElement);
    }

    virtual ~XmlSetter();

    void pushChild(const char *childName);
    void popChild();


template <class Type>
    void visit(Type &field, Type /*defaultValue*/, const char *fieldName)
    {
        pushChild(fieldName);
           field.accept(*this);
        popChild();
    }

template <class inputType>
    void visit(inputType &field, const char *fieldName)
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
        for (int i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
    }

    template <typename inputType>
        void visit(std::vector<inputType> &/*fields*/, const char* /*arrayName*/)
    {
         qDebug() << "XmlSetter::visit(std::vector<inputType> &fields, const char* arrayName) NOT YET SUPPORTED";
    }


private:

    QString mFileName;

    QDomDocument mDocument;
    QDomElement mDocumentElement;


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
void XmlSetter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);

template <>
void XmlSetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void XmlSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */
template <>
void XmlSetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);


#endif // XMLSETTER_H

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>

#include "xmlGetter.h"

XmlGetter::XmlGetter(const QString &fileName)
{
    mFileName = fileName;
    QFile file(mFileName);
    if (!file.open(QFile::ReadWrite)) {
        qDebug() << "Can't open file <" << mFileName << ">";
        return;
    }
    QDomDocument doc("document");
    doc.setContent(&file);
    //mDocumentElement = doc.documentElement();
    qDebug() << doc.toString();
    file.close();

    mNodePath.push_back(doc);
}

template <>
void XmlGetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName)
{

    QDomElement childElement = getChildByTag(fieldName);
    if (childElement.isNull())
    {
        boolField = defaultValue;
        return;
    }
    boolField = childElement.attribute("value", defaultValue ? "true" : "false") == "true";
}

template <>
void XmlGetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
{
    QDomElement childElement = getChildByTag(fieldName);
    if (childElement.isNull())
    {
        doubleField = defaultValue;
        return;
    }
    doubleField = childElement.attribute("value", QString::number(defaultValue)).toDouble();
}

template <>
void XmlGetter::visit<float>(float &floatField, float defaultValue, const char *fieldName)
{
    QDomElement childElement = getChildByTag(fieldName);
    if (childElement.isNull())
    {
        floatField = defaultValue;
        return;
    }
    floatField = childElement.attribute("value", QString::number(defaultValue)).toFloat();
}

template <>
void XmlGetter::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    QDomElement childElement = getChildByTag(fieldName);
    if (childElement.isNull())
    {
        intField = defaultValue;
        return;
    }

    intField = childElement.attribute("value", QString::number(defaultValue)).toInt();
}

template <>
void XmlGetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName)
{
    QDomElement childElement = getChildByTag(fieldName);
    if (childElement.isNull())
    {
        stringField = defaultValue;
        return;
    }

    stringField = childElement.attribute("value", QString::fromStdString(defaultValue)).toUtf8().constData();
}

/* And new style visitor method */

template <>
void XmlGetter::visit<int, IntField>(int &intField, const IntField * fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "XmlGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<double, DoubleField>(double &doubleField, const DoubleField * fieldDescriptor)
{
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "XmlGetter::visit<int, DoubleField>(double &field, const DoubleField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<float, FloatField>(float &floatField, const FloatField * fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "XmlGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<bool, BoolField>(bool &boolField, const BoolField * fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "XmlGetter::visit<int, BoolField>(bool &boolField, const BoolField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<std::string, StringField>(std::string &stringField, const StringField * fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "XmlGetter::visit<int, StringField>(std::string &field, const StringField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, PointerField>(void * &field, const PointerField * /*fieldDescriptor*/) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<int, EnumField>(int &/*field*/, const EnumField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<double, DoubleVectorField>(std::vector<double> &/*field*/, const DoubleVectorField * /*fieldDescriptor*/)
{
    qDebug() << "XmlGetter::visit<double, DoubleVectorField>() NOT YET SUPPORTED";
}

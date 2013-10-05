#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>

#include "xmlGetter.h"

XmlGetter::XmlGetter(const QString &fileName)
{
    mFileName = fileName;
    QFile file(mFileName);
    if (!file.open(QFile::ReadWrite))
        return;
    QDomDocument doc("document");
    doc.setContent(&file);
    mDocumentElement = doc.documentElement();
    file.close();
}

template <>
void XmlGetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName)
{
    QDomElement element = mDocumentElement.toDocument().elementById(fieldName);
    if (element.isNull())
        boolField = defaultValue;
    else
        boolField = element.attribute("value", defaultValue ? "true" : "false") == "true";
}

template <>
void XmlGetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
{
    QDomElement element = mDocumentElement.toDocument().elementById(fieldName);
    if (element.isNull())
        doubleField = defaultValue;
    else
        doubleField = element.attribute("value", QString::number(defaultValue)).toDouble();
}

template <>
void XmlGetter::visit<float>(float &floatField, float defaultValue, const char *fieldName)
{
    QDomElement element = mDocumentElement.toDocument().elementById(fieldName);
    if (element.isNull())
        floatField = defaultValue;
    else
        floatField = element.attribute("value", QString::number(defaultValue)).toFloat();
}

template <>
void XmlGetter::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    QDomElement element = mDocumentElement.toDocument().elementById(fieldName);
    if (element.isNull())
        intField = defaultValue;
    else
        intField = element.attribute("value", QString::number(defaultValue)).toDouble();
}

/* And new style visitor method */

template <>
void XmlGetter::visit<int, IntField>(int &/*field*/, const IntField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<double, DoubleField>(double &/*field*/, const DoubleField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, DoubleField>(double &field, const DoubleField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<float, FloatField>(float &/*field*/, const FloatField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, FloatField>(float &field, const FloatField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<bool, BoolField>(bool &/*field*/, const BoolField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, BoolField>(bool &field, const BoolField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void XmlGetter::visit<std::string, StringField>(std::string &/*field*/, const StringField * /*fieldDescriptor*/)
{
	qDebug() << "XmlGetter::visit<int, StringField>(std::string &field, const StringField *fieldDescriptor) NOT YET SUPPORTED";
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

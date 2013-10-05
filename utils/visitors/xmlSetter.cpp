#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>

#include "xmlSetter.h"

XmlSetter::XmlSetter(QString const & filename)
{
    mFileName = filename;
}

template <>
void XmlSetter::visit<int>(int &intField, int /*defaultValue*/, const char *fieldName)
{
    saveValue(fieldName, QString::number(intField));
}

template <>
void XmlSetter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    saveValue(fieldName, boolField ? "true" : "false");
}

template <>
void XmlSetter::visit<double>(double &doubleField, double /*defaultValue*/, const char *fieldName)
{
    saveValue(fieldName, QString::number(doubleField));
}

template <>
void XmlSetter::visit<float>(float &floatField, float /*defaultValue*/, const char *fieldName)
{
    saveValue(fieldName, QString::number(floatField));
}

void XmlSetter::saveValue(const char *fieldName, QString value)
{
    QFile file(mFileName);
    if (!file.open(QFile::ReadWrite))
        return;
    QDomDocument doc("document");
    doc.setContent(&file);
    QDomElement element = doc.elementById(fieldName);
    if (element.isNull())
    {
        QDomElement root = doc.documentElement();
        if (root.isNull())
        {
            root = doc.createElement("Parameters");
            doc.appendChild(root);
        }
        element = doc.createElement(fieldName);
        root.appendChild(element);
    }
    element.setAttribute("value", value);
    QTextStream textStream(&file);
    doc.save(textStream, 0);
    file.close();
}

/* And new style visitor method */

template <>
void XmlSetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor)
{
	saveValue(fieldDescriptor->name.name, QString::number(field));
}

template <>
void XmlSetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
	saveValue(fieldDescriptor->name.name, QString::number(field));
}

template <>
void XmlSetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor)
{
	saveValue(fieldDescriptor->name.name, QString::number(field));
}

template <>
void XmlSetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor)
{
	saveValue(fieldDescriptor->name.name, field ? "true" : "false");
}

template <>
void XmlSetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
	saveValue(fieldDescriptor->name.name, field.c_str());
}

template <>
void XmlSetter::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{

}

template <>
void XmlSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
	saveValue(fieldDescriptor->name.name, QString::number(field));
}


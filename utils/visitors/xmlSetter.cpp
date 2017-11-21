#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QtDebug>
#include <QtXml/QDomDocument>

#include "xmlSetter.h"


XmlSetter::XmlSetter(const QString &fileName) :
    mDocument("document")
{
    mFileName = fileName;
    /*QFile file(mFileName);
    if (!file.open(QFile::ReadWrite)) {
        qDebug() << "XmlSetter::XmlSetter(): Can't open file <" << fileName << ">" << endl;
        return;
    }

    //mDocument.setContent(&file);
    file.close();*/
    mNodePath.push_back(mDocument);
}

XmlSetter::~XmlSetter()
{
    qDebug() << "XmlSetter::~XmlSetter(): saving to <" << mFileName << ">" << endl;

    QFile file(mFileName);
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "XmlSetter::XmlSetter(): Can't open file <" << mFileName << ">" << endl;
        return;
    }

    QTextStream textStream(&file);
    mDocument.save(textStream, 2);
    textStream.flush();
    qDebug() << "Output" << mDocument.toString();
    file.close();
}

void XmlSetter::pushChild(const char *childName)
{
//    qDebug("XmlSetter::pushChild(%s): called", childName);

    QDomElement element = getChildByTag(childName);
    if (element.isNull())
    {
        QDomNode mainElement = mNodePath.back();

//        qDebug("  XmlSetter::pushChild(): adding new subelement");
        element = mDocument.createElement(childName);
        QDomNode node = mainElement.appendChild(element);
        if (node == QDomNode())
        {
            qDebug("  XmlSetter::pushChild(): failed");
        }
    }
    mNodePath.push_back(element);
}

void XmlSetter::popChild()
{
//    qDebug() << "Node seem to be finished" << mDocument.toString();
    mNodePath.pop_back();
}

void XmlSetter::saveValue(const char *fieldName, QString value)
{
    QDomElement element = getChildByTag(fieldName);

    qDebug() << "We arrived at DOM path of length " << mNodePath.size() << " adding " << fieldName << " with value:" << value;

    if (element.isNull())
    {
        pushChild(fieldName);

        QDomElement mainElement = mNodePath.back().toElement();
        mainElement.setAttribute("value", value);
        popChild();
    }
    else {
        element.setAttribute("value", value);
    }
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
    saveValue(fieldDescriptor->name.name, QString::fromStdString(field));
}


template <>
void XmlSetter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor)
{
//    SYNC_PRINT(("XML doesn't support wide strings so far"));
    saveValue(fieldDescriptor->name.name, QString::fromStdWString(field));
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

template <>
void XmlSetter::visit<double, DoubleVectorField>(std::vector<double> & /*field*/, const DoubleVectorField * /*fieldDescriptor*/)
{
    qDebug() << "XmlSetter::visit<double, DoubleVectorField>() NOT YET SUPPORTED";
}

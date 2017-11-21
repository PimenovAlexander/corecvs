#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QtDebug>
#include <QtXml/QDomDocument>

#include "jsonSetter.h"


JSONSetter::JSONSetter(const QString &fileName)
{
    mFileName = fileName;
    mNodePath.push_back(QJsonObject());
}

JSONSetter::~JSONSetter()
{
    qDebug() << "JSONSetter::~JSONSetter(): saving to <" << mFileName << ">" << endl;

    QFile file(mFileName);
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "JSONSetter::~JSONSetter(): Can't open file <" << mFileName << ">" << endl;
        return;
    }

    QJsonDocument doc;
    doc.setObject(mNodePath.back());
    QByteArray jsonData = doc.toJson();
    file.write(jsonData);
    file.close();
}


/*=================*/

template <>
void JSONSetter::visit<uint64_t>(uint64_t &intField, uint64_t /*defaultValue*/, const char *fieldName)
{
    QString packed = QString::number(intField) + "u64";
    mNodePath.back().insert(fieldName, packed);
}

template <>
void JSONSetter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    mNodePath.back().insert(fieldName, boolField);
}

template <>
void JSONSetter::visit<std::string>(std::string &stringField, std::string /*defaultValue*/, const char *fieldName)
{
    mNodePath.back().insert(fieldName, QString::fromStdString(stringField));
}

template <>
void JSONSetter::visit<std::wstring>(std::wstring &wstringField, std::wstring /*defaultValue*/, const char *fieldName)
{
    mNodePath.back().insert(fieldName, QString::fromStdWString(wstringField));
}


/* And new style visitor method */

template <>
void JSONSetter::visit<int, IntField>(int &intField, const IntField *fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}


template <>
void JSONSetter::visit<uint64_t, UInt64Field>(uint64_t &intField, const UInt64Field *fieldDescriptor)
{
    visit<uint64_t>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONSetter::visit<unsigned char, IntField>(unsigned char &intField, const IntField *fieldDescriptor)
{
    int foo = intField;
    visit<int>(foo, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
    intField = foo;
}

template <>
void JSONSetter::visit<double, DoubleField>(double &doubleField, const DoubleField *fieldDescriptor)
{
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONSetter::visit<float, FloatField>(float &floatField, const FloatField *fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONSetter::visit<bool, BoolField>(bool &boolField, const BoolField *fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONSetter::visit<std::string, StringField>(std::string &stringField, const StringField *fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONSetter::visit<std::wstring, WStringField>(std::wstring &wstringField, const WStringField *fieldDescriptor)
{
    visit<std::wstring>(wstringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}


template <>
void JSONSetter::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{
}

template <>
void JSONSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    visit<int>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONSetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{    
    QJsonArray array;
    for (size_t i = 0; i < field.size(); i++ )
    {
        QJsonValue value = field[i];
        array.append(value);
    }
    mNodePath.back().insert(fieldDescriptor->name.name, array);
}

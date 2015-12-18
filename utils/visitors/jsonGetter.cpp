#include "jsonGetter.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QJsonDocument>
#include <QJsonArray>

#include "jsonGetter.h"

JSONGetter::JSONGetter(const QString &fileName)
{
    mFileName = fileName;
    QFile file(mFileName);
    QJsonObject object;

    if (file.open(QFile::ReadOnly))
    {
        QByteArray array = file.readAll();

        QJsonDocument document = QJsonDocument::fromJson(array);
        if (document.isNull())
        {
            SYNC_PRINT(("Fail parsing the data from <%s>\n", QSTR_DATA_PTR(mFileName)));
        }
        object = document.object();
        file.close();
    }
    else {
        qDebug() << "Can't open file <" << mFileName << ">";
    }

    mNodePath.push_back(object);
}

template <>
void JSONGetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    if (value.isBool()) {
        boolField = value.toBool();
    } else {
        boolField = defaultValue;
    }
}

template <>
void JSONGetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    if (value.isDouble()) {
        doubleField = value.toDouble();
    } else {
        doubleField = defaultValue;
    }
}

template <>
void JSONGetter::visit<float>(float &floatField, float defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    if (value.isDouble()) {
        floatField = value.toDouble();
    } else {
        floatField = defaultValue;
    }
}

template <>
void JSONGetter::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    if (value.isDouble()) {
        intField = value.toDouble();
    } else {
        intField = defaultValue;
    }
}

template <>
void JSONGetter::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    if (value.isString()) {
        stringField = value.toString().toStdString();
    } else {
        stringField = defaultValue;
    }
}

/* And new style visitor method */

template <>
void JSONGetter::visit<int, IntField>(int &intField, const IntField * fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "JSONGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void JSONGetter::visit<double, DoubleField>(double &doubleField, const DoubleField * fieldDescriptor)
{
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "JSONGetter::visit<int, DoubleField>(double &field, const DoubleField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void JSONGetter::visit<float, FloatField>(float &floatField, const FloatField * fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "JSONGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void JSONGetter::visit<bool, BoolField>(bool &boolField, const BoolField * fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "JSONGetter::visit<int, BoolField>(bool &boolField, const BoolField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void JSONGetter::visit<std::string, StringField>(std::string &stringField, const StringField * fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
//	qDebug() << "JSONGetter::visit<int, StringField>(std::string &field, const StringField *fieldDescriptor) NOT YET SUPPORTED";
}

template <>
void JSONGetter::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{
    qDebug() << "JSONGetter::visit<int, PointerField>(void * &field, const PointerField * /*fieldDescriptor*/) NOT YET SUPPORTED";
}

template <>
void JSONGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    visit<int>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    QJsonArray array = mNodePath.back().value(fieldDescriptor->name.name).toArray();

    field.clear();
    for (int i = 0; i < array.size(); i++ )
    {
        QJsonValue value = array[i];
        if (value.isDouble()) {
            field.push_back(value.toDouble());
        } else {
            field.push_back(fieldDescriptor->defaultValue);
        }
    }
}

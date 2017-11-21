#include "jsonGetter.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QJsonDocument>
#include <QJsonArray>

void JSONGetter::init(const char *fileName)
{
    mFileName = fileName;
    QFile file(mFileName);    

    if (file.open(QFile::ReadOnly))
    {
        QByteArray array = file.readAll();

        if (!init(array))
        {
            SYNC_PRINT(("Fail parsing the data from <%s>", QSTR_DATA_PTR(mFileName)));
             mHasError = true;
            mHasError = true;
        }     
        file.close();
    }
    else {
        SYNC_PRINT(("JSONGetter: couldn't open file <%s>", QSTR_DATA_PTR(mFileName)));
        mHasError = true;
    }
}

bool JSONGetter::init(const QByteArray &array)
{
    QJsonObject object;
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(array, &parseError);
    if (document.isNull())
    {
        SYNC_PRINT(("Fail parsing the data from <%s> with error \"%s\"\n\n", QSTR_DATA_PTR(mFileName), QSTR_DATA_PTR(parseError.errorString())));
        mHasError = true;
        return false;
    }
    object = document.object();
    mNodePath.push_back(object);
    return true;
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

/**
 *   Because uint64_t is not supported we convert the value to string manually
 **/
template <>
void JSONGetter::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    intField = defaultValue;

    if (value.isString()) {
        QString string = value.toString();
        if (string.endsWith("u64")) {
            string = string.left(string.length() - 3);
//            qDebug() << string;
            bool ok = false;
            uint64_t res = string.toULongLong(&ok);
            if (ok) {
                intField = res;
            }
        }
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

template <>
void JSONGetter::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName)
{
    QJsonValue value = mNodePath.back().value(fieldName);

    if (value.isString()) {
        stringField = value.toString().toStdWString();
    } else {
        stringField = defaultValue;
    }
}

/* And new style visitor method */

template <>
void JSONGetter::visit<unsigned char, IntField>(unsigned char &intField, const IntField * fieldDescriptor)
{
    int foo = intField;
    visit<int>(foo, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
    intField = foo;
}

template <>
void JSONGetter::visit<int, IntField>(int &intField, const IntField * fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<uint64_t, UInt64Field>(uint64_t &intField, const UInt64Field *fieldDescriptor)
{
    visit<uint64_t>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<double, DoubleField>(double &doubleField, const DoubleField * fieldDescriptor)
{
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<float, FloatField>(float &floatField, const FloatField * fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<bool, BoolField>(bool &boolField, const BoolField * fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<std::string, StringField>(std::string &stringField, const StringField * fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void JSONGetter::visit<std::wstring, WStringField>(std::wstring &stringField, const WStringField *fieldDescriptor)
{
     visit<std::wstring>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
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
    field.clear();

    QJsonValue arrayValue = mNodePath.back().value(fieldDescriptor->name.name);
    if (arrayValue.isArray())
    {
        QJsonArray array = arrayValue.toArray();

        for (int i = 0; i < array.size(); i++)
        {
            QJsonValue value = array[i];
            if (value.isDouble()) {
                field.push_back(value.toDouble());
            }
            else {
                field.push_back(fieldDescriptor->getDefaultElement(i));
            }
        }
    }
    else
    {
        for (uint i = 0; i < fieldDescriptor->defaultSize; i++)
        {
            field.push_back(fieldDescriptor->getDefaultElement(i));
        }
    }
}

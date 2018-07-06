#include "rapidJSONWriter.h"

using namespace corecvs;

RapidJSONWriter::RapidJSONWriter(const QString &fileName)
{
    mFileName = fileName;
    mNodePath.push_back(QJsonObject());
}

RapidJSONWriter::~RapidJSONWriter()
{
    qDebug() << "RapidJSONWriter::~RapidJSONWriter(): saving to <" << mFileName << ">" << endl;

    QFile file(mFileName);
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "RapidJSONWriter::~RapidJSONWriter(): Can't open file <" << mFileName << ">" << endl;
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
void RapidJSONWriter::visit<uint64_t>(uint64_t &intField, uint64_t /*defaultValue*/, const char *fieldName)
{
    QString packed = QString::number(intField) + "u64";
    mNodePath.back().insert(fieldName, packed);
}

template <>
void RapidJSONWriter::visit<bool>(bool &boolField, bool /*defaultValue*/, const char *fieldName)
{
    mNodePath.back().insert(fieldName, boolField);
}

template <>
void RapidJSONWriter::visit<std::string>(std::string &stringField, std::string /*defaultValue*/, const char *fieldName)
{
    mNodePath.back().insert(fieldName, QString::fromStdString(stringField));
}


/* And new style visitor method */

template <>
void RapidJSONWriter::visit<int, IntField>(int &intField, const IntField *fieldDescriptor)
{
    visit<int>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}


template <>
void RapidJSONWriter::visit<uint64_t, UInt64Field>(uint64_t &intField, const UInt64Field *fieldDescriptor)
{
    visit<uint64_t>(intField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONWriter::visit<unsigned char, IntField>(unsigned char &intField, const IntField *fieldDescriptor)
{
    int foo = intField;
    visit<int>(foo, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
    intField = foo;
}

template <>
void RapidJSONWriter::visit<double, DoubleField>(double &doubleField, const DoubleField *fieldDescriptor)
{
    visit<double>(doubleField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONWriter::visit<float, FloatField>(float &floatField, const FloatField *fieldDescriptor)
{
    visit<float>(floatField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONWriter::visit<bool, BoolField>(bool &boolField, const BoolField *fieldDescriptor)
{
    visit<bool>(boolField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONWriter::visit<std::string, StringField>(std::string &stringField, const StringField *fieldDescriptor)
{
    visit<std::string>(stringField, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONWriter::visit<void *, PointerField>(void * &/*field*/, const PointerField * /*fieldDescriptor*/)
{
}

template <>
void RapidJSONWriter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    visit<int>(field, fieldDescriptor->defaultValue, fieldDescriptor->name.name);
}

template <>
void RapidJSONWriter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{
    QJsonArray array;
    for (size_t i = 0; i < field.size(); i++ )
    {
        QJsonValue value = field[i];
        array.append(value);
    }
    mNodePath.back().insert(fieldDescriptor->name.name, array);
}

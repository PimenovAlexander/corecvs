#include "qSettingsGetter.h"

SettingsGetter::SettingsGetter(const QString &fileName, QString _root) :
    mRoot(_root)
{
    mSettings = new QSettings(fileName, QSettings::IniFormat);
    if (!mRoot.isEmpty())
        mSettings->beginGroup(mRoot);
}

SettingsGetter::~SettingsGetter()
{
    if (!mRoot.isEmpty())
        mSettings->endGroup();
    delete mSettings;
}

template <>
void SettingsGetter::visit<bool>(bool &boolField, bool boolValue, const char *fieldName)
{
    boolField = mSettings->value(fieldName, boolValue).toBool();
}

template <>
void SettingsGetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName)
{
    doubleField = mSettings->value(fieldName, defaultValue).toDouble();
}

template <>
void SettingsGetter::visit<float>(float &floatField, float defaultValue, const char *fieldName)
{
    floatField = mSettings->value(fieldName, defaultValue).toFloat();
}

template <>
void SettingsGetter::visit<int>(int &intField, int defaultValue, const char *fieldName)
{
    intField = mSettings->value(fieldName, defaultValue).toInt();
}

/* New style visitor*/

template <>
void SettingsGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor)
{
    field = mSettings->value(fieldDescriptor->name.name, fieldDescriptor->defaultValue).toInt();
}

template <>
void SettingsGetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    field = mSettings->value(fieldDescriptor->name.name, fieldDescriptor->defaultValue).toDouble();
}

template <>
void SettingsGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor)
{
    field = mSettings->value(fieldDescriptor->name.name, fieldDescriptor->defaultValue).toFloat();
}

template <>
void SettingsGetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    field = mSettings->value(fieldDescriptor->name.name, fieldDescriptor->defaultValue).toBool();
}

template <>
void SettingsGetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    field = mSettings->value(fieldDescriptor->name.name, fieldDescriptor->defaultValue.c_str()).toString().toStdString();
}

template <>
void SettingsGetter::visit<void *, PointerField>(void * &field, const PointerField * /*fieldDescriptor*/)
{
    // not supported yet
    // FIXME

    field = NULL;
}

template <>
void SettingsGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    field = mSettings->value(fieldDescriptor->name.name, fieldDescriptor->defaultValue).toInt();
}

/* Arrays block */

template <>
void SettingsGetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{

    mSettings->beginGroup(fieldDescriptor->name.name);
    int size = mSettings->value("size", 0).toInt();
    for (int i = 0; i < size; i++ )
    {
        QVariant def(fieldDescriptor->getDefaultElement(i));
        field.push_back(mSettings->value(QString::number(i), def).toDouble());
    }
    mSettings->endGroup();
}

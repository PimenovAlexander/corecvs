#include "qSettingsSetter.h"

using namespace corecvs;

SettingsSetter::SettingsSetter(QString const & fileName, QString _root)
    : mRoot(_root)
    , mAllocated(true)
{
    mSettings = new QSettings(fileName, QSettings::IniFormat);
    if (!mRoot.isEmpty())
        mSettings->beginGroup(mRoot);
}

SettingsSetter::SettingsSetter(QSettings *settings, QString _root )
    : mSettings(settings)
    , mRoot(_root)
    , mAllocated(false)
{
    if (!mRoot.isEmpty())
        mSettings->beginGroup(mRoot);
}

SettingsSetter::~SettingsSetter()
{
    if (!mRoot.isEmpty()) {
        mSettings->endGroup();
    }
    delete_safe(mSettings);
}

template <>
void SettingsSetter::visit<bool>(bool &boolField, bool /*boolValue*/, const char *fieldName)
{
    mSettings->setValue(fieldName, boolField);
}

template <>
void SettingsSetter::visit<double>(double &doubleField, double /*defaultValue*/, const char *fieldName)
{
    mSettings->setValue(fieldName, doubleField);
}

template <>
void SettingsSetter::visit<float>(float &floatField, float /*defaultValue*/, const char *fieldName)
{
    mSettings->setValue(fieldName, floatField);
}

template <>
void SettingsSetter::visit<int>(int &intField, int /*defaultValue*/, const char *fieldName)
{
    mSettings->setValue(fieldName, intField);
}

/* And new style visitor method */

template <>
void SettingsSetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor)
{
    mSettings->setValue(fieldDescriptor->name.name, field);
}

template <>
void SettingsSetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    mSettings->setValue(fieldDescriptor->name.name, field);
}

template <>
void SettingsSetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor)
{
    mSettings->setValue(fieldDescriptor->name.name, field);
}

template <>
void SettingsSetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    mSettings->setValue(fieldDescriptor->name.name, field);
}

template <>
void SettingsSetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor)
{
    mSettings->setValue(fieldDescriptor->name.name, field.c_str());
}

template <>
void SettingsSetter::visit<void *, PointerField>(void * &field, const PointerField * /*fieldDescriptor*/)
{
    // not supported yet
    // FIXME

    field = NULL;
}

template <>
void SettingsSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
{
    mSettings->setValue(fieldDescriptor->name.name, field);
}


/* Arrays block */

template <>
void SettingsSetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor)
{

    mSettings->beginGroup(fieldDescriptor->name.name);
    mSettings->setValue("size", QVariant((unsigned)field.size()));
    for (unsigned i = 0; i < field.size(); i++ )
    {
        mSettings->setValue(QString::number(i), field[i]);
    }
    mSettings->endGroup();
}

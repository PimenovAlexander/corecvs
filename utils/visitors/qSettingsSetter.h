#pragma once
#include <QtCore/QSettings>
#include "basePathVisitor.h"
#include "reflection.h"

using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::PointerField;
using corecvs::EnumField;

class SettingsSetter
{
public:
    SettingsSetter(QString const & fileName, QString _root = QString());
    SettingsSetter(QSettings *settings, QString _root = QString());

    ~SettingsSetter();

    template <class Type>
        void visit(Type &field, Type /*defaultValue*/, const char *fieldName)
        {
            mSettings->beginGroup(fieldName);
                field.accept(*this);
            mSettings->endGroup();
        }

    template <class Type>
        void visit(Type &field, const char *fieldName)
        {
            mSettings->beginGroup(fieldName);
                field.accept(*this);
            mSettings->endGroup();
        }

    template <typename inputType, typename reflectionType>
        void visit(inputType &field, const reflectionType * fieldDescriptor)
        {
            mSettings->beginGroup(fieldDescriptor->getSimpleName());
                field.accept(*this);
            mSettings->endGroup();
        }

    QSettings *settings() {return mSettings;};

private:
    QSettings * mSettings;
    QString mRoot;
};

template <>
void SettingsSetter::visit<int>(int &intField, int defaultValue, const char *fieldName);

template <>
void SettingsSetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template <>
void SettingsSetter::visit<float>(float &floatField, float defaultValue, const char *fieldName);

template <>
void SettingsSetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

/* And new style visitor method */
template <>
void SettingsSetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void SettingsSetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void SettingsSetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void SettingsSetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void SettingsSetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void SettingsSetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void SettingsSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);



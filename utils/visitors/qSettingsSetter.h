#pragma once
#include <sstream>
#include <QtCore/QSettings>
#include "core/utils/visitors/basePathVisitor.h"
#include "core/reflection/reflection.h"

using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::FloatField;
using corecvs::BoolField;
using corecvs::StringField;
using corecvs::WStringField;
using corecvs::PointerField;
using corecvs::EnumField;
using corecvs::DoubleVectorField;

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

    /* Generic Array support */
    template <typename inputType, typename reflectionType>
        void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
        {
            for (int i = 0; i < fields.size(); i++)
            {
                fields[i].accept(*this);
            }
        }

    /**
     * Generic Array support
     * String style
     **/
    template <typename inputType>
        void visit(std::vector<inputType> &fields, const char* arrayName)
        {
            fields.clear();
            mSettings->beginGroup(arrayName);
            mSettings->setValue("size", QVariant((int)fields.size()));
            for (size_t i = 0; i < fields.size(); i++ )
            {
                std::stringstream ss;
                ss << arrayName << "[" << i << "]";
                visit(fields[i], ss.str().c_str());
            }
            mSettings->endGroup();
        }

    QSettings *settings() { return mSettings; }

private:
    bool        mAllocated;
    QSettings  *mSettings;
    QString     mRoot;
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
void SettingsSetter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);

template <>
void SettingsSetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void SettingsSetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);



/* Arrays */

template <>
void SettingsSetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

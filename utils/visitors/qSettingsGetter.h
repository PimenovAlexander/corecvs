#pragma once
#include <QSettings>
#include <sstream>
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

class SettingsGetter
{
public:
    SettingsGetter(QString const & fileName, QString _root = QString());
    SettingsGetter(QSettings *settings, QString _root = QString());

    ~SettingsGetter();

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
            int size = mSettings->value("size", 0).toInt();
            fields.resize(size);
            for (int i = 0; i < size; i++ )
            {
                std::stringstream ss;
                ss << arrayName << "[" << i << "]";

                visit(fields[i], ss.str().c_str());
            }
            mSettings->endGroup();
        }


    QSettings * settings() {
        return mSettings;
    }

private:
    QSettings * mSettings;
    QString mRoot;
};

template <>
void SettingsGetter::visit<int>(int &intField, int defaultValue, const char *fieldName);

template <>
void SettingsGetter::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template <>
void SettingsGetter::visit<float>(float &floatField, float defaultValue, const char *fieldName);

template <>
void SettingsGetter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

/* New style visitor */

template <>
void SettingsGetter::visit<int, IntField>(int &field, const IntField *fieldDescriptor);

template <>
void SettingsGetter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void SettingsGetter::visit<float, FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void SettingsGetter::visit<bool, BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void SettingsGetter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void SettingsGetter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);

template <>
void SettingsGetter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void SettingsGetter::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor);

/* Arrays */

template <>
void SettingsGetter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

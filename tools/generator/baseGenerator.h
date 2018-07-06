#ifndef BASEGENERATOR_H_
#define BASEGENERATOR_H_

#include <fstream>
#include <QtCore/QString>
#include "reflectionGenerator.h"

using namespace corecvs;

class BaseGenerator
{
public:
    BaseGenerator(const ReflectionGen *_clazz);

protected:
    QString getGenerateDir();


    enum CaseModifier {
        toUpper,
        toLower,
        unchanged
    };

    virtual void enterFieldContext(int i) = 0;

    QString toEnumName(QString  input);
    QString toCamelCase(QString  input, bool firstUpper = false);
    QString j(QString in, int width);
    QString getWidgetNameForName(QString name);

    QString getDefaultElementValue(const BaseField *field);
    QString getDefaultValue(const BaseField *field);

    QString getCppTypeForElementType(BaseField::FieldType field);
    QString getCppTypeForType(const BaseField *field);

    QString getFieldRefTypeForElementType(BaseField::FieldType field);
    QString getFieldRefTypeForType(BaseField::FieldType intype);

    QString getWidgetGetterMethodForType(BaseField::FieldType type);
    QString getWidgetSetterMethodForType(BaseField::FieldType type);
    QString getUiWidgetForType(BaseField::FieldType type);
    QString getWidgetSuffixForType(BaseField::FieldType type);
    QString getSignalForType(BaseField::FieldType type);


    std::ofstream out;
    const ReflectionGen *clazz;

    /* GeneratorState */
    const BaseField *field;
    BaseField::FieldType type;
    QString name;
    QString descr;
    QString comment;
    QString fieldPlaceholder;
};

#endif // BASEGENERATOR_H_

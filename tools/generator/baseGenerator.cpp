#include <QDebug>
#include "baseGenerator.h"

BaseGenerator::BaseGenerator(const Reflection *_clazz)
    : clazz(_clazz)
{
}

QString BaseGenerator::getGenerateDir()
{
    return "Generated";
}

QString BaseGenerator::toEnumName(QString  input)
{
    return input.toUpper().replace(' ','_');
}

QString BaseGenerator::toCamelCase(QString  input, bool firstUpper)
{
    QString result = input;
    CaseModifier modifier = firstUpper ? toUpper : toLower;
//    bool wasSpace = firstUpper;
    for (int i = 0; i < input.size(); i++)
    {
        if (modifier == toUpper)
            result[i] = result[i].toUpper();
        else if (modifier == toLower)
            result[i] = result[i].toLower();

        modifier = (input[i] == ' ') ? toUpper : unchanged;
    }
    return result.remove(' ');
}

QString BaseGenerator::j(QString in, int width)
{
    return in.leftJustified(width, ' ');
}

QString BaseGenerator::getCppTypeForElementType(const BaseField::FieldType type)
{   
    switch(type)
    {
        case BaseField::TYPE_INT:
            return "int";
        case BaseField::TYPE_TIMESTAMP:
            return "int64_t";
        case BaseField::TYPE_DOUBLE:
            return "double";
        case BaseField::TYPE_BOOL:
            return "bool";
        case BaseField::TYPE_STRING:
            return "std::string";
        case BaseField::TYPE_ENUM:
        {
            QString enumName = toCamelCase(QString(static_cast<const EnumField*>(field)->enumReflection->name.name), true);
            return enumName+"::"+enumName;
        }
        case BaseField::TYPE_POINTER:
            return QString(static_cast<const PointerField*>(field)->targetClass) + " *";
        case BaseField::TYPE_COMPOSITE:
            return toCamelCase(static_cast<const CompositeField *>(field)->typeName, true);
        case BaseField::TYPE_COMPOSITE_ARRAY:
            return QString("std::vector<") + toCamelCase(static_cast<const CompositeField *>(field)->typeName, true) + ">";
        default:
            return "/* Something is not supported by generator */";
    }
}

QString BaseGenerator::getCppTypeForType(const BaseField *field)
{

    BaseField::FieldType type = (BaseField::FieldType)(field->type & ~BaseField::TYPE_VECTOR_BIT);
    QString element = getCppTypeForElementType(type);

    if (field->type & BaseField::TYPE_VECTOR_BIT) {
        return QString("vector<%1>").arg(element);
    } else {
        return element;
    }

}


QString BaseGenerator::getWidgetGetterMethodForType(BaseField::FieldType type)
{
    if (field->type & BaseField::TYPE_VECTOR_BIT) {
       qDebug() << "Unsupported type for UI: vectors not supported so far";

       switch(type & ~BaseField::TYPE_VECTOR_BIT)
       {
           case BaseField::TYPE_INT:
           case BaseField::TYPE_DOUBLE:
               return "value()";
       }

    }

    switch(type)
    {
        case BaseField::TYPE_INT:
        case BaseField::TYPE_DOUBLE:
            return "value()";
        case BaseField::TYPE_BOOL:
            return "isChecked()";
        case BaseField::TYPE_ENUM:
            return "currentIndex()";
        case BaseField::TYPE_STRING:
            return "text().toStdString()";
        case BaseField::TYPE_COMPOSITE:
            return "createParameters()";
        default:
            return "";
    }
}


QString BaseGenerator::getWidgetNameForName(QString name)
{
    return toCamelCase(name, true) + getWidgetSuffixForType(BaseField::TYPE_COMPOSITE);
}

QString BaseGenerator::getWidgetSetterMethodForType(BaseField::FieldType type)
{
    if (field->type & BaseField::TYPE_VECTOR_BIT) {
       qDebug() << "Unsupported type for UI: vectors not supported so far";

       switch(type & ~BaseField::TYPE_VECTOR_BIT)
       {
           case BaseField::TYPE_INT:
           case BaseField::TYPE_DOUBLE:
               return "setValue";
       }
    }

    switch(type)
    {
        case BaseField::TYPE_INT:
        case BaseField::TYPE_DOUBLE:
            return "setValue";
        case BaseField::TYPE_BOOL:
            return "setChecked";
        case BaseField::TYPE_ENUM:
            return "setCurrentIndex";
        case BaseField::TYPE_STRING:
            return "setText";
        case BaseField::TYPE_COMPOSITE:
            return "setParameters";
        default:
            return "";
    }
}

QString BaseGenerator::getDefaultElementValue(const BaseField *field)
{    
    switch(type)
    {
        case  BaseField::TYPE_INT:
        case (BaseField::TYPE_INT | BaseField::TYPE_VECTOR_BIT):
            return QString::number(static_cast<const IntField *>(field)->defaultValue);
        case  BaseField::TYPE_DOUBLE:
        case (BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT):
            return QString::number(static_cast<const DoubleField *>(field)->defaultValue);
        case BaseField::TYPE_BOOL:
            return static_cast<const BoolField *>(field)->defaultValue ? "true" : "false";
        case BaseField::TYPE_ENUM:
            return QString::number(static_cast<const EnumField *>(field)->defaultValue);
        case BaseField::TYPE_STRING:
            return QString("\"") + QString(static_cast<const StringField *>(field)->defaultValue.c_str()) + QString("\"");
        case BaseField::TYPE_POINTER:
            return "NULL";
        case BaseField::TYPE_COMPOSITE:
        {
            const CompositeField *cfield = static_cast<const CompositeField *>(field);
            const Reflection *referent = cfield->reflection;
            return toCamelCase(referent->name.name, true) + "()";
        }
        case BaseField::TYPE_COMPOSITE_ARRAY:
        {
            const CompositeArrayField *cafield = static_cast<const CompositeArrayField *>(field);
            const Reflection *referent = cafield->reflection;
            return QString("std::vector<") + toCamelCase(referent->name.name, true) + ">()";
        }
        default:
            return "/* default value is not supported by generator */";
    }
}

QString BaseGenerator::getDefaultValue(const BaseField *field)
{
    /*if (field->type & BaseField::TYPE_VECTOR_BIT) {
        return QString("std::") + getCppTypeForType(field) + "()";
    } else {*/
        return getDefaultElementValue(field);
    /*}*/
}



QString BaseGenerator::getFieldRefTypeForElementType(BaseField::FieldType type)
{
    switch(type)
    {
        case BaseField::TYPE_INT:
            return "Int";
        case BaseField::TYPE_TIMESTAMP:
            return "Timestamp";
        case BaseField::TYPE_DOUBLE:
            return "Double";
        case BaseField::TYPE_BOOL:
            return "Bool";
        case BaseField::TYPE_ENUM:
            return "Enum";
        case BaseField::TYPE_STRING:
           return "String";
        case BaseField::TYPE_POINTER:
            return "Pointer";
        case BaseField::TYPE_COMPOSITE:
            return "Composite";
        case BaseField::TYPE_COMPOSITE_ARRAY:
             return "CompositeArray";
        default:
            return "/* Something is not supported by generator */";
    }
}

QString BaseGenerator::getFieldRefTypeForType(BaseField::FieldType intype)
{
    BaseField::FieldType type = (BaseField::FieldType)(intype & ~BaseField::TYPE_VECTOR_BIT);
    QString element = getFieldRefTypeForElementType(type);

    if (field->type & BaseField::TYPE_VECTOR_BIT) {
        return QString("%1VectorField").arg(element);
    } else {
        return QString("%1Field").arg(element);
    }

}



QString BaseGenerator::getWidgetSuffixForType(BaseField::FieldType type)
{
    switch(type)
    {
        case BaseField::TYPE_INT:
            return "SpinBox";
        case BaseField::TYPE_DOUBLE:
        {
            switch (static_cast<const DoubleFieldGen *>(field)->widgetType)
            {
                case doubleSpinBox:     return "SpinBox";
                case exponentialSlider: return "Slider";
            }
        }
        case BaseField::TYPE_BOOL:
        {
            switch (static_cast<const BoolFieldGen *>(field)->widgetType)
            {
            case checkBox:    return "CheckBox";
            case radioButton: return "Button";
            }
        }
        case BaseField::TYPE_ENUM:
            return "ComboBox";
        case BaseField::TYPE_STRING:
            return "Edit";
        case BaseField::TYPE_COMPOSITE:
            return "ControlWidget";
        default:
            return "";
    }
}

QString BaseGenerator::getUiWidgetForType(BaseField::FieldType type)
{
    switch(type)
    {
        case BaseField::TYPE_INT:
            return "QSpinBox";
        case BaseField::TYPE_DOUBLE:
        {
            switch (static_cast<const DoubleFieldGen *>(field)->widgetType)
            {
                case doubleSpinBox:     return "QDoubleSpinBox";
                case exponentialSlider: return "ExponentialSlider";
            }
        }
        case BaseField::TYPE_BOOL:
        {
            switch (static_cast<const BoolFieldGen *>(field)->widgetType)
            {
            case checkBox:    return "QCheckBox";
            case radioButton: return "QRadioButton";
            }
        }
        case BaseField::TYPE_ENUM:
            return "QComboBox";
        case BaseField::TYPE_STRING:
            return "QLineEdit";
        case BaseField::TYPE_COMPOSITE:
        {
            return getWidgetNameForName(static_cast<const CompositeField *>(field)->typeName);
        }

        case (BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT):
        {
            return "DoubleVectorWidget";
        }


        default:
            return "";
    }
}

QString BaseGenerator::getSignalForType(BaseField::FieldType type)
{
    switch(type)
    {
        case BaseField::TYPE_INT:
            return "valueChanged(int)";
        case BaseField::TYPE_DOUBLE:
            return "valueChanged(double)";
        case BaseField::TYPE_BOOL:
        {
            switch (static_cast<const BoolFieldGen *>(field)->widgetType)
            {
              case checkBox:    return "stateChanged(int)";
              case radioButton: return "clicked(bool)";
            }
        }
        case BaseField::TYPE_ENUM:
        {
            switch (static_cast<const EnumFieldGen *>(field)->widgetType)
            {
                case comboBox:    return "currentIndexChanged(int)";
                case tabWidget:   return "currentChanged(int)";
            }
        }
        case BaseField::TYPE_STRING:
            return "textChanged(QString)";
        case BaseField::TYPE_COMPOSITE:
            return "paramsChanged()";
        case (BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT):
            return "valueChanged()";
        default:
            return "";
    }
}

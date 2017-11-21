/**
 * \file widgetUIGenerator.cpp
 * \brief Add Comment Here
 *
 * \date Jan 3, 2012
 * \author alexander
 */

#include <QDir>
#include <iostream>

#include "widgetUIGenerator.h"

using std::ios;

namespace
{
    const QString HEADER_TEMPLATE =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<ui version=\"4.0\">\n"
        " <class>%1</class> \n" // class name
        " <widget class=\"QWidget\" name=\"%2\">\n" // class name
        "  <property name=\"geometry\">\n"
        "   <rect>\n"
        "    <x>0</x>\n"
        "    <y>0</y>\n"
        "    <width>400</width>\n"
        "    <height>300</height>\n"
        "   </rect>\n"
        "  </property>\n"
        "  <property name=\"windowTitle\">\n"
        "   <string>%3</string>\n" // base name
        "  </property>\n"
        "  <layout class=\"QGridLayout\" name=\"gridLayout\">\n"
        "   <property name=\"margin\">\n"
        "    <number>3</number>\n"
        "   </property>\n"
        "   <property name=\"spacing\">\n"
        "    <number>3</number>\n"
        "   </property>\n";

    const QString FOOTER_TEMPLATE =
        "   <item row=\"%1\" column=\"0\">\n" // "+ QString::number(fieldNumber) +"
        "    <spacer name=\"verticalSpacer\">\n"
        "     <property name=\"orientation\">\n"
        "      <enum>Qt::Vertical</enum>\n"
        "     </property>\n"
        "     <property name=\"sizeHint\" stdset=\"0\">\n"
        "      <size>\n"
        "       <width>0</width>\n"
        "       <height>0</height>\n"
        "      </size>\n"
        "     </property>\n"
        "    </spacer>\n"
        "   </item>\n"

        "   <item row=\"%2\" column=\"2\">\n" // + QString::number(fieldNumber) +
        "    <spacer name=\"horizontalSpacer\">\n"
        "     <property name=\"orientation\">\n"
        "      <enum>Qt::Horizontal</enum>\n"
        "     </property>\n"
        "     <property name=\"sizeHint\" stdset=\"0\">\n"
        "      <size>\n"
        "       <width>0</width>\n"
        "       <height>0</height>\n"
        "      </size>\n"
        "     </property>\n"
        "    </spacer>\n"
        "   </item>\n"

        "  </layout>\n"
        " </widget>\n"
        " <layoutdefault spacing=\"3\" margin=\"3\"/>\n"
        " <resources/>\n";

    const QString ADVANCED_MARKER =
        "       <property name=\"tags\" stdset=\"0\">\n"
        "           <string>advanced</string>\n"
        "       </property>\n";

    const QString LABEL_HEADER_TEMPLATE =
        "    <item row=\"%1\" column=\"0\">\n"
        "     <widget class=\"QLabel\" name=\"label_%2\">\n"
        "       <property name=\"text\">\n"
        "         <string>%3</string>\n"
        "       </property>\n";

    const QString LABEL_FOOTER =
        "     </widget>\n"
        "    </item>\n";

    const QString EDITOR_HEADER_TEMPLATE =
        "    <item row=\"%1\" column=\"1\">\n"
        "     <widget class=\"%2\" name=\"%3\">\n"
        "       <property name=\"toolTip\">\n"
        "         <string>%4</string>\n"
        "       </property>\n";

    const QString EDITOR_FOOTER =
        "     </widget>\n"
        "    </item>\n";
}

WidgetUiGenerator::WidgetUiGenerator(const Reflection *_clazz)
    : BaseGenerator(_clazz)
{
}

void WidgetUiGenerator::enterFieldContext(int i)
{
    field = clazz->fields[i];
    name = field->name.name;
    type = field->type;
    ui = getUiWidgetForType(type);
    boxSignal = getSignalForType(type);
    descr = field->name.decription;
    comment = field->name.comment;
    pos = QString::number(i);
    boxName = toCamelCase(name, false) + getWidgetSuffixForType(type);;
}

void WidgetUiGenerator::generateWidgetUi()
{
    QString baseName = toCamelCase(clazz->name.name, true);
    QString className = getWidgetNameForName(clazz->name.name);

    QString result;

    int fieldNumber = clazz->fields.size();

    out.close();
    out.open(QString(getGenerateDir() + QDir::separator() + toCamelCase(className) + ".ui").toLatin1(), ios::out);

    result += HEADER_TEMPLATE.arg(className).arg(className).arg(baseName);

    for (int i = 0; i < fieldNumber; i++ )
    {
        enterFieldContext(i);

        // generate label
        result += LABEL_HEADER_TEMPLATE.arg(pos).arg(pos).arg(descr);
        if (field->isAdvanced)
            result += ADVANCED_MARKER;
        result += LABEL_FOOTER;

        if (type == BaseField::TYPE_POINTER)
            continue;

        // generate edit widget
        result += EDITOR_HEADER_TEMPLATE
            .arg(pos)
            .arg(ui)
            .arg(boxName)
            .arg(comment);

        if (type == BaseField::TYPE_ENUM)
            result += getEnumWidgetProperties(field);

        if (type == BaseField::TYPE_INT)
            result += getIntWidgetProperties(field);

        if (type == BaseField::TYPE_STRING)
            result += getStringWidgetProperties(field);

        if (type == BaseField::TYPE_WSTRING)
            result += getWStringWidgetProperties(field);

        if (type == (BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT))
            result += getVectorBitWidgetProperties(field);

        if (type == BaseField::TYPE_DOUBLE)
            result += getDoubleWidgetProperties(field);

        if (type == BaseField::TYPE_BOOL)
            result += getBoolWidgetProperties(field);

        if (field->isAdvanced)
            result += ADVANCED_MARKER;

        result += EDITOR_FOOTER;

        // Take this to some more obvious place
        if (type == BaseField::TYPE_COMPOSITE)
        {
            const CompositeField* cField = static_cast<const CompositeField*>(field);
            WidgetUiGenerator generatorUI(cField->reflection);
            generatorUI.generateWidgetUi();
        }
    }

    result += FOOTER_TEMPLATE.arg(QString::number(fieldNumber)).arg(QString::number(fieldNumber));

    result += " <connections>\n";
    for (int i = 0; i < fieldNumber; i++ )
    {
        enterFieldContext(i);

        result +=
            "  <connection>\n"
            "   <sender>"+boxName+"</sender>\n"
            "   <signal>"+boxSignal+"</signal>\n"
            "   <receiver>"+className+"</receiver>\n"
            "   <slot>changeParameters()</slot>\n"
            "  </connection>\n";

    }

    result +=
        " </connections>\n"
        " <slots>\n"
        "  <slot>changeParameters()</slot>\n"
        " </slots>\n"
        " <customwidgets>\n";
    for (int i = 0; i < fieldNumber; i++ )
    {
        enterFieldContext(i);
        if (type != BaseField::TYPE_COMPOSITE)
            continue;
        const CompositeField *cfield = static_cast<const CompositeField *>(field);
        QString className = getWidgetNameForName(cfield->typeName);
        QString fileName  = toCamelCase(cfield->typeName, false) + "ControlWidget.h";
        result +=
            "  <customwidget>\n"
            "   <class>"+className+"</class>\n"
            "   <extends>QWidget</extends>\n"
            "   <header>"+fileName+"</header>\n"
            "   <container>1</container>\n"
            "  </customwidget>\n";
    }
    for (int i = 0; i < fieldNumber; i++ )
    {
        enterFieldContext(i);
        if (type == (BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT))
        {
            result +=
                "  <customwidget>\n"
                "   <class>DoubleVectorWidget</class>\n"
                "   <extends>QWidget</extends>\n"
                "   <header>vectorWidget.h</header>\n"
                "   <container>1</container>\n"
                "  </customwidget>\n";

            break;
        }
    }

    result +=
        " </customwidgets>\n"
        "</ui>\n";


    out << result.toLatin1().constData();
}

QString WidgetUiGenerator::getEnumWidgetProperties(const BaseField* field)
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString CURRENT_INDEX_TEMPLATE =
        "       <property name=\"currentIndex\">\n"
        "         <number>%1</number>\n"
        "       </property>\n";

    static const QString ITEM_HEADER_TEMPLATE =
        "       <item>\n"
        "        <property name=\"text\">\n"
        "         <string>%1</string>\n"
        "        </property>\n";

    static const QString ITEM_FOOTER_TEMPLATE = "       </item>\n";

    static const QString ICON_ITEM_TEMPLATE =
        "        <property name=\"icon\">\n"
        "        <iconset resource=\"%1\">\n"
        "         <normaloff>%2</normaloff>%3\n"
        "        </iconset>\n"
        "        </property>\n";

    QString result;

    const EnumField *efield = static_cast<const EnumField *>(field);
    const EnumReflection *eref = efield->enumReflection;
    result += CURRENT_INDEX_TEMPLATE.arg(QString::number(efield->defaultValue));

    for (int j = 0; j < eref->optionsNumber(); j ++)
    {
        const EnumOptionGen *option = static_cast<const EnumOptionGen *>(eref->options[j]);

        const QString name = option->name.name;
        const QString descr = option->name.decription;

        result += ITEM_HEADER_TEMPLATE.arg(descr);

        QString presentation(option->presentationHint);
        if (!presentation.isEmpty())
        {
            const QStringList& icon = presentation.split("@");
            const QString iconName = icon[0];
            const QString iconRes  = icon[1];

            result += ICON_ITEM_TEMPLATE
                .arg(iconRes)
                .arg(iconName)
                .arg(iconName);
        }

        result += ITEM_FOOTER_TEMPLATE;
    }

    return result;
}

QString WidgetUiGenerator::getIntWidgetProperties(const BaseField* field) // static
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString SETTING_TEMPLATE =
        "       <property name=\"minimum\">\n"
        "        <number>%1</number>\n"
        "       </property>\n"
        "       <property name=\"maximum\">\n"
        "        <number>%2</number>\n"
        "       </property>\n"
        "       <property name=\"singleStep\">\n"
        "        <number>%3</number>\n"
        "       </property>\n"
        "       <property name=\"value\">\n"
        "        <number>%4</number>\n"
        "       </property>\n";

    static const QString PREFIX_TEMPLATE =
        "       <property name=\"prefix\">\n"
        "        <string>%1</string>\n"
        "       </property>\n";

    static const QString SUFFIX_TEMPLATE =
        "       <property name=\"suffix\">\n"
        "        <string>%1</string>\n"
        "       </property>\n";

    QString result;

    const IntFieldGen *ifield = static_cast<const IntFieldGen *>(field);

    if (ifield->hasAdditionalValues)
        result += SETTING_TEMPLATE
            .arg(QString::number(ifield->min))
            .arg(QString::number(ifield->max))
            .arg(QString::number(ifield->step))
            .arg(QString::number(ifield->defaultValue));

    if (ifield->prefixHint != NULL)
        result += PREFIX_TEMPLATE.arg(ifield->prefixHint);

    if (ifield->suffixHint != NULL)
        result += SUFFIX_TEMPLATE.arg(ifield->suffixHint);

    return result;
}

QString WidgetUiGenerator::getStringWidgetProperties(const BaseField* field)
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString TEXT_TEMPLATE =
        "       <property name=\"text\">\n"
        "           <string>%1</string>\n"
        "       </property>\n";

    const StringField* sfield = static_cast<const StringField*>(field);

    return TEXT_TEMPLATE.arg(QString::fromStdString(sfield->defaultValue));
}

QString WidgetUiGenerator::getWStringWidgetProperties(const BaseField* field)
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString TEXT_TEMPLATE =
        "       <property name=\"text\">\n"
        "           <string>%1</string>\n"
        "       </property>\n";

    const WStringField* sfield = static_cast<const WStringField*>(field);

    return TEXT_TEMPLATE.arg(QString::fromStdWString(sfield->defaultValue));
}


QString WidgetUiGenerator::getBoolWidgetProperties(const BaseField* field)
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString CHECKED_TEMPLATE =
        "       <property name=\"checked\">\n"
        "           <bool>%1</bool>\n"
        "       </property>\n";

    const BoolField* bfield = static_cast<const BoolField*>(field);

    return CHECKED_TEMPLATE.arg(bfield->defaultValue);
}

QString WidgetUiGenerator::getVectorBitWidgetProperties(const BaseField* field)
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString SETTING_TEMPLATE =
        "       <property name=\"minimum\">\n"
        "        <double>%1</double>\n"
        "       </property>\n"
        "       <property name=\"maximum\">\n"
        "        <double>%2</double>\n"
        "       </property>\n" ;

    QString result;

    const DoubleVectorFieldGen *dfield = static_cast<const DoubleVectorFieldGen *>(field);

    if (dfield->hasAdditionalValues)
        result += SETTING_TEMPLATE
            .arg(QString::number(dfield->min))
            .arg(QString::number(dfield->max))
            ;

    return result;
}

QString WidgetUiGenerator::getDoubleWidgetProperties(const BaseField* field)
{
    CORE_ASSERT_TRUE(field != nullptr, "Invalid field");

    static const QString SETTING_TEMPLATE =
        "       <property name=\"minimum\">\n"
        "        <double>%1</double>\n"
        "       </property>\n"
        "       <property name=\"maximum\">\n"
        "        <double>%2</double>\n"
        "       </property>\n"
        "       <property name=\"singleStep\">\n"
        "        <double>%3</double>\n"
        "       </property>\n"
        "       <property name=\"value\">\n"
        "        <double>%4</double>\n"
        "       </property>\n";

    static const QString PREFIX_TEMPLATE =
        "       <property name=\"prefix\">\n"
        "        <string>%1</string>\n"
        "       </property>\n";

    static const QString SUFFIX_TEMPLATE =
        "       <property name=\"suffix\">\n"
        "        <string>%1</string>\n"
        "       </property>\n";

    static const QString DECIMALS_TEMPLATE =
        "       <property name=\"decimals\">\n"
        "        <number>%1</number>\n"
        "       </property>\n";

    QString result;

    const DoubleFieldGen *dfield = static_cast<const DoubleFieldGen *>(field);
    if (dfield->precision != 2)
        result += DECIMALS_TEMPLATE.arg(QString::number(dfield->precision));
    if (dfield->hasAdditionalValues)
        result += SETTING_TEMPLATE
            .arg(QString::number(dfield->min))
            .arg(QString::number(dfield->max))
            .arg(QString::number(dfield->step))
            .arg(QString::number(dfield->defaultValue))
            ;

    if (dfield->prefixHint != NULL)
        result += PREFIX_TEMPLATE.arg(dfield->prefixHint);

    if (dfield->suffixHint != NULL)
        result += SUFFIX_TEMPLATE.arg(dfield->suffixHint);

    return result;
}

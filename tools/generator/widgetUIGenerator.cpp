/**
 * \file widgetUIGenerator.cpp
 * \brief Add Comment Here
 *
 * \date Jan 3, 2012
 * \author alexander
 */

#include <iostream>

#include "widgetUIGenerator.h"

using std::ios;

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
    out.open(QString("Generated/" + toCamelCase(className) + ".ui").toLatin1(), ios::out);

    result +=
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<ui version=\"4.0\">\n"
    " <class>"+className+"</class>\n"
    " <widget class=\"QWidget\" name=\""+className+"\">\n"
    "  <property name=\"geometry\">\n"
    "   <rect>\n"
    "    <x>0</x>\n"
    "    <y>0</y>\n"
    "    <width>400</width>\n"
    "    <height>300</height>\n"
    "   </rect>\n"
    "  </property>\n"
    "  <property name=\"windowTitle\">\n"
    "   <string>"+baseName+"</string>\n"
    "  </property>\n"
    "   <layout class=\"QGridLayout\" name=\"gridLayout\">\n"
    "    <property name=\"margin\">\n"
    "     <number>3</number>\n"
    "    </property>\n"
    "    <property name=\"spacing\">\n"
    "     <number>3</number>\n"
    "    </property>\n";
    for (int i = 0; i < fieldNumber; i++ )
    {
        enterFieldContext(i);

     result +=
    "    <item row=\""+pos+"\" column=\"0\">\n"
    "     <widget class=\"QLabel\" name=\"label_"+pos+"\">\n"
    "       <property name=\"text\">\n"
    "         <string>"+descr+"</string>\n"
    "       </property>\n";

    if (field->isAdvanced)
    {
        result +=
    "       <property name=\"tags\" stdset=\"0\">\n"
    "           <string>advanced</string>\n"
    "       </property>\n";
    }

     result +=
    "     </widget>\n"
    "    </item>\n";


    if (type == BaseField::TYPE_POINTER)
        continue;

     result +=
    "    <item row=\""+pos+"\" column=\"1\">\n"
    "     <widget class=\""+ui+"\" name=\""+boxName+"\">\n";

    if (type == BaseField::TYPE_ENUM)
    {
        const EnumField *efield = static_cast<const EnumField *>(field);
        const EnumReflection *eref = efield->enumReflection;
     result +=
    "       <property name=\"currentIndex\">\n"
    "         <number>"+QString::number(efield->defaultValue)+"</number>\n"
    "       </property>\n";

        for (int j = 0; j < eref->optionsNumber(); j ++)
        {
            const EnumOption *option = eref->options[j];
            QString name = option->name.name;
            QString descr = option->name.decription;

      result +=
    "       <item>\n"
    "        <property name=\"text\">\n"
    "         <string>"+descr+"</string>\n"
    "        </property>\n"
    "       </item>\n";
        }
    }

    result +=
    "       <property name=\"toolTip\">\n"
    "         <string>"+comment+"</string>\n"
    "       </property>\n";

    /* Merge two if blocks below */
    if (type == BaseField::TYPE_INT)
    {
        const IntField *ifield = static_cast<const IntField *>(field);
        if (ifield->hasAdditionalValues)
            result +=
    "       <property name=\"minimum\">\n"
    "        <number>" + QString::number(ifield->min) + "</number>\n"
    "       </property>\n"
    "       <property name=\"maximum\">\n"
    "        <number>" + QString::number(ifield->max) + "</number>\n"
    "       </property>\n"
    "       <property name=\"singleStep\">\n"
    "        <number>" + QString::number(ifield->step) + "</number>\n"
    "       </property>\n"
    "       <property name=\"value\">\n"
    "        <number>" + QString::number(ifield->defaultValue) + "</number>\n"
    "       </property>\n";
    }

    if (type == BaseField::TYPE_DOUBLE)
    {
        const DoubleField *dfield = static_cast<const DoubleField *>(field);
        if (dfield->hasAdditionalValues)
            result +=
    "       <property name=\"minimum\">\n"
    "        <double>" + QString::number(dfield->min) + "</double>\n"
    "       </property>\n"
    "       <property name=\"maximum\">\n"
    "        <double>" + QString::number(dfield->max) + "</double>\n"
    "       </property>\n"
    "       <property name=\"singleStep\">\n"
    "        <double>" + QString::number(dfield->step) + "</double>\n"
    "       </property>\n"
    "       <property name=\"value\">\n"
    "        <double>" + QString::number(dfield->defaultValue) + "</double>\n"
    "       </property>\n";
    }

    if (type == BaseField::TYPE_BOOL)
    {
        const BoolField *bfield = static_cast<const BoolField *>(field);
//        printf("Bool field %d\n", bfield->defaultValue);

        if (bfield->defaultValue)
            result +=
    "       <property name=\"checked\">\n"
    "           <bool>true</bool>\n"
    "       </property>\n";
    }

    if (field->isAdvanced)
    {
        result +=
    "       <property name=\"tags\" stdset=\"0\">\n"
    "           <string>advanced</string>\n"
    "       </property>\n";
    }

    result +=
    "     </widget>\n"
    "    </item>\n";

     // Take this to some more obvious place
     if (type == BaseField::TYPE_COMPOSITE)
     {
         const CompositeField* cField = static_cast<const CompositeField*>(field);
         WidgetUiGenerator generatorUI(cField->reflection);
         generatorUI.generateWidgetUi();
     }
    }

    result +=
    "   <item row=\""+ QString::number(fieldNumber) +"\" column=\"0\">\n"
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

    "   <item row=\""+ QString::number(fieldNumber) +"\" column=\"2\">\n"
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
    " <resources/>\n"
    " <connections>\n";
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
    result +=
    " </customwidgets>\n"
    "</ui>\n";


    out << result.toLatin1().constData();
}



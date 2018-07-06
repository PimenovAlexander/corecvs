/**
 * \file documentationGenerator.cpp
 *
 * \date Aug 18, 2014
 **/

#include <iostream>

#include <QDir>
#include <QStringList>

#include "documentationGenerator.h"
#include "baseGenerator.h"

using std::ios;

DocumentationGenerator::DocumentationGenerator(const ReflectionGen *_clazz)
    : BaseGenerator(_clazz)
{


}

void DocumentationGenerator::enterFieldContext(int i)
{
    field = clazz->fields[i];
    name = field->name.name;
    type = field->type;
/*    ui = getUiWidgetForType(type);
    boxSignal = getSignalForType(type);*/
    descr = field->name.description;
    comment = field->name.comment;           
/*    pos = QString::number(i);
    boxName = toCamelCase(name, false) + getWidgetSuffixForType(type);*/
    iconName = "";
}

void DocumentationGenerator::parseComment(QString rawComment)
{
    QStringList helpers = rawComment.split("!", QString::SkipEmptyParts);
    if (helpers.count() > 1)
    {
        iconName = "!{width:24px}" +  helpers[0] + "!";
        helpers.removeAt(0);
        comment =  helpers.join("!");
    } else {
        comment = rawComment;
    }
}


QString DocumentationGenerator::printField()
{
    QString result;
    parseComment(comment);

    if (type == BaseField::TYPE_ENUM)
    {
        result +=
 "# " + iconName +  name + " - " + comment + "\n";

        iconName="";
        const EnumField *efield = static_cast<const EnumField *>(field);
        const EnumReflection *eref = efield->enumReflection;

        for (int j = 0; j < eref->optionsNumber(); j ++)
        {
            const EnumOption *option = eref->options[j];
            QString name = option->name.name;
            QString rawComment = option->name.comment;

            parseComment(rawComment);

            result += "## " + iconName + name + " - " + comment + "\n";
            iconName="";
        }
    } else {
        result +=
 "# " + name + " - " + comment + "\n";
    }

    return result;
}


void DocumentationGenerator::generateDocumentation()
{
    QString baseName  = toCamelCase(clazz->name.name, true);
    QString className = getWidgetNameForName(clazz->name.name);
    QString classComment   = clazz->name.comment;

    QString result;
    QString resultAdvanced;

    int fieldNumber = clazz->fields.size();

    out.close();
    out.open(QString(getGenerateDir() + QDir::separator() + toCamelCase(className) + ".redmine").toLatin1(), ios::out);

    result +=
    "" + classComment + "\n\n";

    for (int i = 0; i < fieldNumber; i++ )
    {
       enterFieldContext(i);
       if (!field->isAdvanced)
          result += printField();
    }

    for (int i = 0; i < fieldNumber; i++ )
    {
       enterFieldContext(i);
       if (field->isAdvanced)
          resultAdvanced += printField();
    }

    if (!resultAdvanced.isEmpty()) {
        result +="\nFollowing parameters are advanced and not recommended to change \n";
        result += resultAdvanced;
    }

    out << result.toLatin1().constData();
}

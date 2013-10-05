#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "configLoader.h"
#include "reflectionGenerator.h"


ConfigLoader::ConfigLoader()
{
    mEnums.clear();
    mReflections.clear();
}

ConfigLoader::~ConfigLoader()
{
    foreach (EnumReflection *ref, mEnums.values())
        delete ref;

    foreach (Reflection *ref, mReflections.values())
        delete ref;

}

const char *ConfigLoader::toCString(QString const &str)
{
    if (str.isEmpty())
        return "";
    else
        return strdup(str.toAscii());
}

/**
 *   We do this to support both
 *
 *   <pre>
 *   <class name="Test Name"> </class>
 *   </pre>
 *
 *   and
 *
 *   <pre>
 *   <class>
 *      <name>Test Name</name>
 *   </class>
 *   </pre>
 **/
QString ConfigLoader::getFromHierarcy(QDomElement const &classElement, QString const &name)
{
    QString value = classElement.attribute(name);
    if (!value.isEmpty())
        return value;
    QDomNodeList fields = classElement.elementsByTagName(name);
    if (fields.isEmpty())
        return QString();

    QDomElement subElement = fields.at(0).toElement();
    return subElement.text();
}


ReflectionNaming ConfigLoader::getNamingFromXML(QDomElement const &classElement)
{
    QString className   = getFromHierarcy(classElement, "name");
    QString description = getFromHierarcy(classElement, "description");
    QString comment     = getFromHierarcy(classElement, "comment");

    if (description.isEmpty())
       description = className;
    if (comment.isEmpty())
       comment = description;

    return ReflectionNaming(toCString(className), toCString(description), toCString(comment));
}

void ConfigLoader::loadEnums(QDomDocument const &config)
{
    Reflection *result = new ReflectionGen();
    result->name = ReflectionNaming("enums", NULL, NULL);

    QDomNodeList enums = config.elementsByTagName("enum");
    for (unsigned i = 0; i < enums.length(); i++)
    {
        QDomElement enumElement = enums.at(i).toElement();
        EnumReflection *enumReflection = new EnumReflection();
        enumReflection->name = getNamingFromXML(enumElement);

        QDomNodeList items = enumElement.elementsByTagName("item");
        for (unsigned j = 0; j < items.length(); j++)
        {
            QDomElement itemElement = items.at(j).toElement();
            int id = itemElement.attribute("id").toInt();
            enumReflection->options.push_back(new EnumOption(id, getNamingFromXML(itemElement)));
        }

        // TODO: add support for comment and description
        mEnums.insert(QString(enumReflection->name.name), enumReflection);
    }
}



void ConfigLoader::loadClasses(QDomDocument const &config)
{
    QDomNodeList classes = config.elementsByTagName("class");
    for (unsigned i = 0; i < classes.length(); i++)
    {
        QDomElement classElement = classes.at(i).toElement();
        ReflectionGen *result = new ReflectionGen();
        result->name = getNamingFromXML(classElement);
        QString uibase = classElement.attribute("uibase");
        result->uiBaseClass = toCString(uibase);

        QDomNodeList fields = classElement.elementsByTagName("field");
        for (unsigned j = 0; j < fields.length(); j++)
        {
            QDomElement fieldElement = fields.at(j).toElement();
            ReflectionNaming fieldNameing = getNamingFromXML(fieldElement);

            QDomAttr typeAttribute = fieldElement.attributeNode("type");
            QString type = typeAttribute.value();

//            qDebug() << "Field" << fieldNameing.name << " type " << type;

            QString defaultValue = fieldElement.attribute("defaultValue");
            QString minValue     = fieldElement.attribute("min");
            QString maxValue     = fieldElement.attribute("max");
            QString stepValue    = fieldElement.attribute("step");
            bool hasAdditionalParameters = (!minValue.isEmpty()) || (!maxValue.isEmpty()) || (!stepValue.isEmpty());

            BaseField *field = NULL;

            if (type == "int")
            {
                field = new IntFieldGen(
                          defaultValue.toInt()
                        , fieldNameing
                        , hasAdditionalParameters
                        , minValue.toInt()
                        , maxValue.toInt()
                        , stepValue.isEmpty() ? 1 : stepValue.toInt());
            }
            else if (type == "double")
            {
                DoubleWidgetType widgetType = fieldElement.attribute("widget") == "ExponentialSlider" ?
                                           exponentialSlider :
                                           doubleSpinBox;
                field = new DoubleFieldGen(
                          defaultValue.toDouble()
                        , widgetType
                        , fieldNameing
                        , hasAdditionalParameters
                        , minValue.toDouble()
                        , maxValue.toDouble()
                        , stepValue.isEmpty() ? 1.0 : stepValue.toDouble());
            }
            else if (type == "bool")
            {
                BoolWidgetType widgetType = fieldElement.attribute("widget") == "RadioButton" ?
                                            radioButton :
                                            checkBox;

                field = new BoolFieldGen(defaultValue == "true", widgetType, fieldNameing);
            }
            else if (type == "string")
            {
                const char *dValue = toCString(defaultValue);
                field = new StringFieldGen(dValue, fieldNameing);
            }
            else if (type == "Vector2dd" || type == "Vector3dd")
            {
                // not supported yet (and most probably won't be supported)
            }
            /* TODO: Use "Type *" syntax instead */
            else if (type == "pointer")
            {
                QString targetType = fieldElement.attribute("target");
                field = new PointerFieldGen(fieldNameing, toCString(targetType));

            }
            else // composite field or enum
            {
                Reflection *reflection = mReflections.value(type);
                EnumReflection *enumRef = mEnums.value(type);

                if (reflection) // is it a field with the type of some other class?
                {
                    field = new CompositeFieldGen(fieldNameing, toCString(type), reflection);
                }
                else if (enumRef) // then it should be a enum
                {
                    EnumWidgetType widgetType = fieldElement.attribute("widget") == "TabWidget" ?
                                                tabWidget :
                                                comboBox;

                    field = new EnumFieldGen(defaultValue.toInt(), widgetType, fieldNameing, enumRef);
                } else {
                    fprintf(stderr, "Error 12 (%s:%d,%d) : Type '%s' is unknown. Is neither enum, nor known type.\n",
                            result->name.name,
                            fieldElement.lineNumber(),
                            fieldElement.columnNumber(),
                            type.toAscii().constData()
                    );
                }
            }

            if (field)
            {
                bool isAdavnced = fieldElement.hasAttribute("adv") | fieldElement.hasAttribute("advanced");
                field->isAdvanced = isAdavnced;
                result->fields.push_back(field);
            }
        }

        QDomNodeList embeds = classElement.elementsByTagName("embed");
        for (unsigned j = 0; j < embeds.length(); j++)
        {
            // qDebug() << "processing tag embed N" << j << " of (" << embeds.length() << ")";
            QDomElement embeddedElement = embeds.at(j).toElement();
            QString type = embeddedElement.attribute("type");

            ReflectionNaming embedNameing = getNamingFromXML(embeddedElement);

            Reflection *reflection = mReflections.value(type);
            if (!reflection) {
                fprintf(stderr, "Error 12 (%s:%d,%d) : Type '%s' is unknown.\n",
                        result->name.name,
                        embeddedElement.lineNumber(),
                        embeddedElement.columnNumber(),
                        type.toAscii().constData()
                );
                continue;
            }

            EmbedSubclass *subclass = new EmbedSubclass();
            subclass->name = embedNameing;
            subclass->subclass = reflection;

            QDomNodeList prefix = embeddedElement.elementsByTagName("prefix");
            if (prefix.isEmpty())
            {
                // TODO: Partial renames
                QDomNodeList renames = embeddedElement.elementsByTagName("rename");
                for (unsigned j = 0; j < renames.length(); j++)
                {
                    QDomElement renaming = renames.at(j).toElement();
                    QString from = renaming.attribute("from");
                    QString to   = renaming.attribute("to");
//                    printf("  rename: %s -> %s\n", from.toAscii().constData(), to.toAscii().constData());
                    EmbedSubclass::EmbedMap map;
                    map.embeddedName = toCString(to);
                    map.originalName = toCString(from);
                    subclass->renameing.push_back(map);
                }
            } else
            {
                QString prefixString = prefix.at(0).toElement().attribute("name");
                vector<const BaseField *>::iterator it;
                for (it = reflection->fields.begin(); it != reflection->fields.end(); ++it)
                {
                    const BaseField *orignalField = (*it);
                    QString from = orignalField->getSimpleName();
                    QString to   = prefixString + from;
//                    printf("  rename: %s -> %s\n", from.toAscii().constData(), to.toAscii().constData());
                    EmbedSubclass::EmbedMap map;
                    map.embeddedName = toCString(to);
                    map.originalName = toCString(from);
                    subclass->renameing.push_back(map);
                }

            }

            result->embeds.push_back(subclass);
            // qDebug() << "Adding embed" << subclass->name.name;

            /* Now add fields */
            vector<const BaseField *>::iterator it;
            for (it = reflection->fields.begin(); it != reflection->fields.end(); ++it)
            {
                const BaseField *orignalField = (*it);
                const char *embeddedName = subclass->getEmbeddedName(orignalField->getSimpleName());
                BaseField *embeddedField = orignalField->clone();
                embeddedField->name.name = embeddedName;
                result->fields.push_back(embeddedField);
            }
        }

        mReflections.insert(result->name.name, result);
    }
}

void ConfigLoader::loadParamsMapper(QDomDocument const &config)
{
    QDomNodeList paramsMappers = config.elementsByTagName("parametersMapper");
    for (unsigned i = 0; i < paramsMappers.length(); i++)
    {
        QDomElement classElement = paramsMappers.at(i).toElement();
        QDomNodeList fields = classElement.elementsByTagName("field");
        mMapperPostfix = classElement.attribute("name");

        for (unsigned j = 0; j < fields.length(); j++)
        {
            QDomElement fieldElement = fields.at(j).toElement();
            QString fieldName = fieldElement.attribute("name");
            QString fieldType = fieldElement.attribute("type");

            mParamsMapper.append(qMakePair<QString, QString>(fieldName, fieldType));
        }
    }
}

void ConfigLoader::loadIncludes(QDomDocument const &config, QFileInfo const &currentFile)
{
    QDomNodeList includes = config.elementsByTagName("include");
    for (unsigned i = 0; i < includes.length(); i++) {
        QDomElement includeElement = includes.at(i).toElement();
        QString includeName = includeElement.attribute("name");
        QFileInfo included = QFileInfo(currentFile.dir(), includeName);
        if (included.exists()) {
            load(included.canonicalFilePath());
        } else {
            fprintf(stderr, "Error 13 (%s:%d,%d) : Include '%s' is unknown.\n",
                    currentFile.fileName().toAscii().constData(),
                    includeElement.lineNumber(),
                    includeElement.columnNumber(),
                    includeName.toAscii().constData()
            );
        }

    }
}

QList<QPair<QString, QString> > *ConfigLoader::paramsMapperFields()
{
    return &mParamsMapper;
}

QMap<QString, Reflection *> *ConfigLoader::load(QString const &fileName)
{
    QDomDocument config;
    QFile inputFile(fileName);
    config.setContent(&inputFile);
    inputFile.close();
    QFileInfo info(inputFile);

    // TODO: handle includes

    loadIncludes(config, info);
    loadEnums(config);
    loadClasses(config);
    loadParamsMapper(config);

    return &mReflections;


}

#include <fstream>
#include <stdio.h>

#include <QtCore/QCoreApplication>

#include "widgetUIGenerator.h"
#include "pdoGenerator.h"
#include "parametersMapperGenerator.h"
#include "documentationGenerator.h"

#include "configLoader.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString xmlFile = "xml/vigest.xml";

    if (argc >= 2)
        xmlFile = argv[1];

    ConfigLoader loader;
    QMap<QString, Reflection *> *reflections = loader.load(xmlFile);

    /* Generate all classes */
    foreach (Reflection *result, reflections->values())
    {
        WidgetUiGenerator generatorUI((ReflectionGen *)result);
        generatorUI.generateWidgetUi();

        PDOGenerator generator((ReflectionGen *)result);
        generator.generatePDOH();
        generator.generatePDOCpp();
        generator.generateControlWidgetCpp();

        DocumentationGenerator generatorDoc((ReflectionGen *)result);
        generatorDoc.generateDocumentation();
    }

    /* Generate all enums */
    QHash<QString, EnumReflection *> *enums = loader.enumReflections();
    foreach (EnumReflection *eref, enums->values())
    {
        PDOGenerator generator(NULL);
        generator.generatePDOEnumSubH((EnumReflectionGen *)eref);
    }

    ParametersMapperGenerator mapperGenerator(reflections, loader.paramsMapperFields(), loader.mapperPostfix());
    mapperGenerator.generateMapper();

    return 0;
}



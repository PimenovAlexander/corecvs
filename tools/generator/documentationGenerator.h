#pragma once

/**
 * \file documentationGenerator.h
 *
 * \date Aug 18, 2014
 **/

#include "baseGenerator.h"

class DocumentationGenerator : public BaseGenerator
{
public:
    DocumentationGenerator(const Reflection *_clazz);
    void generateDocumentation();

private:
    void enterFieldContext(int i);
    void parseComment(QString rawComment);

    QString printField();
    QString iconName;
};

/* EOF */

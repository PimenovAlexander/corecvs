/**
 * \file PODGenerator.h
 * \brief Add Comment Here
 *
 * \date Jan 2, 2012
 * \author alexander
 */

#ifndef PODGENERATOR_H_
#define PODGENERATOR_H_

#include "baseGenerator.h"

class PDOGenerator : public BaseGenerator
{
public:
    explicit PDOGenerator(const Reflection *_clazz);

    void generatePDOH();
    void generatePDOEnumSubH(const EnumReflection *eref);
    void generatePDOCpp();
    void generateControlWidgetCpp();

private:
    void enterFieldContext(int i);

    QString cppName;
    QString cppType;
    QString getterName;
    QString setterName;
    QString boxName;
    QString boxFileName;
    QString boxSignal;

    QString defaultValue;

    QString fieldEnumId;
    QString fieldRefType;
    QString simplifiedType;

    QString prefix;
    QString suffix;

    enum Flags {
        DYNAMIC_INITIALISATION_REFLECTION = 0x0,
        STATIC_INITIALISATION_REFLECTION = 0x1,
        CREATE_DEFAULT_CONSTRUCTOR = 0x2,
        CREATE_VISIT_METHOD = 0x4,
        CREATE_GETTERS = 0x8,
        CREATE_SETTERS = 0x10
    };
};

#endif /* PODGENERATOR_H_ */

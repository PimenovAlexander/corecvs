#ifndef WIDGETUIGENERATOR_H_
#define WIDGETUIGENERATOR_H_
/**
 * \file widgetUIGenerator.h
 * \brief Add Comment Here
 *
 * \date Jan 3, 2012
 * \author alexander
 */

#include "baseGenerator.h"

class WidgetUiGenerator : public BaseGenerator
{
public:
    WidgetUiGenerator(const ReflectionGen *_clazz);
    void generateWidgetUi();

private:
    void enterFieldContext(int i);

    static QString getEnumWidgetProperties(const BaseField* field);
    static QString getIntWidgetProperties(const BaseField* field);
    static QString getStringWidgetProperties(const BaseField* field);
    static QString getWStringWidgetProperties(const BaseField* field);
    static QString getBoolWidgetProperties(const BaseField* field);
    static QString getVectorBitWidgetProperties(const BaseField* field);
    static QString getDoubleWidgetProperties(const BaseField* field);

private:
    QString boxName;
    QString ui;
    QString boxSignal;

    QString pos;
};


#endif /* WIDGETUIGENERATOR_H_ */

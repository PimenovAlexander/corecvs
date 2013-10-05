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
    WidgetUiGenerator(const Reflection *_clazz);
    void generateWidgetUi();

private:
    void enterFieldContext(int i);

    QString boxName;
    QString ui;
    QString boxSignal;

    QString pos;
};


#endif /* WIDGETUIGENERATOR_H_ */

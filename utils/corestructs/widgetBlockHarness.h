#ifndef WIDGETBLOCKHARNESS_H
#define WIDGETBLOCKHARNESS_H

#include <QWidget>

#include "core/filters/newstyle/newStyleBlock.h"
#include "core/reflection/reflection.h"
#include "reflectionWidget.h"
#include "core/reflection/dynamicObject.h"

class WidgetBlockHarness : public QWidget
{
    Q_OBJECT
public:
    corecvs::DynamicObjectWrapper *blockReflection = NULL;
    NewStyleBlock *block = NULL;


    ReflectionWidget *inputs = NULL;
    ReflectionWidget *params = NULL;
    ReflectionWidget *outputs = NULL;


    WidgetBlockHarness(corecvs::DynamicObjectWrapper *blockReflection, NewStyleBlock *block, QWidget *parent = NULL);

public slots:
    void executeCalled();
public:
    virtual ~WidgetBlockHarness();

};

#endif // WIDGETBLOCKHARNESS_H

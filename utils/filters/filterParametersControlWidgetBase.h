#ifndef FILTER_PARAMETERS_CONTROL_WIDGET_BASE_H_
#define FILTER_PARAMETERS_CONTROL_WIDGET_BASE_H_
/**
 * \file filterParametersControlWidgetBase.h
 *
 *
 *
 * \date Jun 2, 2012
 * \author alexander
 */

#include <QWidget>

#include "core/utils/global.h"

#include "core/reflection/reflection.h"
#include "corestructs/parametersControlWidgetBase.h"

class FilterParametersControlWidgetBase : public QWidget
{
    Q_OBJECT

public:
    FilterParametersControlWidgetBase(QWidget *parent) : QWidget(parent) {}
    virtual void setParametersVirtual(void* /*input*/) {}

    virtual BaseReflectionStatic *createParametersVirtual() const
    {
        return NULL;
    }
signals:
    void paramsChanged();

};


#endif  /* #ifndef FILTER_PARAMETERS_CONTROL_WIDGET_BASE_H_ */



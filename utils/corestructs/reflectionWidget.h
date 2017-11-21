#ifndef REFLECTIONWIDGET_H
#define REFLECTIONWIDGET_H

#include <QGridLayout>
#include <vector>

#include "parametersControlWidgetBase.h"

class ReflectionWidget : public ParametersControlWidgetBase
{
protected:
    const corecvs::Reflection *reflection;

    std::vector<int>       positionToField;
    std::vector<int>       fieldToPosition;

    std::vector<QWidget *> positionToWidget;

public:
    enum FieldsType {
        TYPE_ALL,
        TYPE_PARAMS,
        TYPE_INPUT_PINS,
        TYPE_OUTPUT_PINS
    };

    enum {
        MARK_COLUMN,
        NAME_COLUMN,
        WIDGET_COLUMN
    };

    QGridLayout *gridLayout = NULL;

public:
    ReflectionWidget(const Reflection *reflection, FieldsType type = TYPE_ALL, QWidget *parent = NULL);


    // ParametersControlWidgetBase interface
public:
    /* This function is extreamly unsafe */
    virtual BaseReflectionStatic *createParametersVirtual() const;

    bool getParameters(void *param) const;
    bool setParameters(void *param) const;

public slots:
//    void executeTriggered();
    void inputTriggered(int inId);
    void outputTriggered(int outId);

    /* Block related. Most probably should be brought to ancestor */
signals:
    void outerFilterRequest(int filter);
//    void executeCalled();


};

#endif // REFLECTIONWIDGET_H

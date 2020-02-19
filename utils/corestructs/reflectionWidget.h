#ifndef REFLECTIONWIDGET_H
#define REFLECTIONWIDGET_H

#include <QGridLayout>
#include <vector>

#include "corestructs/parametersControlWidgetBase.h"

class ReflectionWidget : public ParametersControlWidgetBase
{
     Q_OBJECT
protected:    

    std::vector<int>       positionToField;
    std::vector<int>       fieldToPosition;

    std::vector<QWidget *> positionToWidget;

public:
    const corecvs::Reflection *reflection;

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

    bool trace = false;

public:
    ReflectionWidget(const Reflection *reflection, FieldsType type = TYPE_ALL, bool addButtons = false, QWidget *parent = NULL);
    virtual ~ReflectionWidget(){}


    // ParametersControlWidgetBase interface
public:
    /* This function is extreamly unsafe */
    virtual BaseReflectionStatic *createParametersVirtual() const;

    bool getParameters(void *param) const;
    bool setParameters(void *param) const;

public slots:
//    void executeTriggered();
    //void inputTriggered(int inId);
    //void outputTriggered(int outId);

    /* Block related. Most probably should be brought to ancestor */
    void loadCalled();
    void saveCalled();
    void resetCalled();

signals:
    void outerFilterRequest(int filter);
//    void executeCalled();


};

#endif // REFLECTIONWIDGET_H

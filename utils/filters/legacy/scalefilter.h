#ifndef SCALEFILTER_H
#define SCALEFILTER_H

#include <QtGui/QWidget>
#include "ui_scalefilter.h"
#include "basefilter.h"

using namespace corecvs;

class ScaleFilter : public BaseFilter
{
    Q_OBJECT
private:
    static QString name ;

public slots:
    /* Slots for gain-offset*/
     void updateXScale (double value);
     void updateYScale (double value);
     void updateHeight (int val);
     void updateWidth  (int val);

     void updateXShift (int val);
     void updateYShift (int val);

public:
    static QString getName()
     {
         return name;
     };

    virtual QString getInstanceName()
    {
        return name;
    };

    virtual G12Buffer *filter (G12Buffer *input);

    ScaleFilter(QWidget *parent = 0);
    ~ScaleFilter();

private:
    Ui_ScaleFilterClass ui;
};

#endif // SCALEFILTER_H

#ifndef GAINOFFSET_H
#define GAINOFFSET_H

#include <QtGui/QWidget>
#include "ui_gainoffset.h"
#include "basefilter.h"

using namespace corecvs;

class GainOffset : public BaseFilter
{
    Q_OBJECT

    typedef struct GainParameters_TAG
    {
        double gain;
        int offset;
    } GainParameters;


private:
    static QString name ;


public slots:
    /* Slots for gain-offset*/
     void updateOffsetSlider (double value);
     void updateOffsetText (int val);
     void updateGainSlider (double value);
     void updateGainText (int val);

public:
    GainOffset(QWidget *parent = 0);
    ~GainOffset();

    virtual G12Buffer *filter (G12Buffer *input);

    static QString getName()
     {
         return name;
     };

     virtual QString getInstanceName()
     {
         return name;
     };

private:
    Ui_GainOffsetClass ui;
    GainParameters gParams;
};

#endif // GAINOFFSET_H

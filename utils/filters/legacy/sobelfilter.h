#ifndef SOBLEFILTER_H
#define SOBLEFILTER_H

#include <QtGui/QWidget>
#include "ui_sobelfilter.h"
#include "core/buffers/g12Buffer.h"
#include "basefilter.h"

class SobelFilter : public BaseFilter
{
    Q_OBJECT
private:
    static QString name ;

public slots:
    void parametersChangedSlot( void );

public:
    SobelFilter(QWidget *parent = 0);
    ~SobelFilter();

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
    Ui_SobelFilterClass ui;
};

#endif // SOBLEFILTER_H

#ifndef BITSELECTOR_H
#define BITSELECTOR_H

#include <QtGui/QWidget>
#include "ui_bitselector.h"
#include <QtCore/qstring.h>
#include "basefilter.h"

using namespace corecvs;

class BitSelector : public BaseFilter
{
    Q_OBJECT

private:
    static QString name ;

public slots:
    void maskChanged(void);
    void shiftChanged (void);
public:
    BitSelector(QWidget *parent = 0);
    ~BitSelector();

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
    Ui_BitSelectorClass ui;
    QCheckBox *bitBoxes[12];

    uint32_t mask;
    int8_t shift;
};

#endif // BITSELECTOR_H

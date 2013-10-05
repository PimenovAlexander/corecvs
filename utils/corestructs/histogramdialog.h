#pragma once

#include <QtGui/QDialog>

#include <g12Buffer.h>
#include <histogram.h>

#include "ui_histogramdialog.h"


class HistogramDialog : public QDialog
{
    Q_OBJECT

public slots:
    void notifyBufferChangeSlot(G12Buffer *buf);
    void notifyHistogramChangeSlot(Histogram *buf);

    void doUpdate(void);

/*signals:
    void notifyBufferChange(G12Buffer *buf);*/

public:
    HistogramDialog(QWidget *parent = 0);
    ~HistogramDialog();

    bool isInternal;
    Histogram *histogram;

private:
    Ui_HistogramDialogClass ui;
};

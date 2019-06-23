#ifndef COPTERINPUTSWIDGETS_H
#define COPTERINPUTSWIDGETS_H

#include "copterInputs.h"

#include <QWidget>

namespace Ui {
class CopterInputsWidget;
}

class QSlider;

class CopterInputsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CopterInputsWidget(QWidget *parent = 0);
    ~CopterInputsWidget();
    QSlider *sliders[CopterInputs::CHANNEL_LAST];


public slots:
    void updateState(CopterInputs inputs);

    void uiSliderUpdated();

signals:
    void uiUpdated(CopterInputs inputs);

private:
    Ui::CopterInputsWidget *ui;
};

#endif // COPTERINPUTSWIDGETS_H

#ifndef COPTERINPUTSWIDGETS_H
#define COPTERINPUTSWIDGETS_H

#include <QWidget>

namespace Ui {
class CopterInputsWidget;
}


/** Separate this **/
class CopterInputs
{
public:
    enum ChannelID {
        CHANNEL_0,
        CHANNEL_THROTTLE = CHANNEL_0,
        CHANNEL_1,
        CHANNEL_ROLL     = CHANNEL_1,
        CHANNEL_2,
        CHANNEL_PITCH    = CHANNEL_2,
        CHANNEL_3,
        CHANNEL_YAW      = CHANNEL_3,

        CHANNEL_CONTROL_LAST,

        CHANNEL_4 = CHANNEL_CONTROL_LAST,
        CHANNEL_5,
        CHANNEL_6,
        CHANNEL_7,

        CHANNEL_LAST
    };

    int axis[CHANNEL_LAST];

    CopterInputs();

    static const char *getName(ChannelID value);
};



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

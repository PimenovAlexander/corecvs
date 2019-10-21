#ifndef JOYSTICKOPTIONSWIDGET_H
#define JOYSTICKOPTIONSWIDGET_H

#include "linuxJoystickInterface.h"
#include "mixerChannelOperationWidget.h"

#include <QWidget>

namespace Ui {
class JoystickOptionsWidget;
}

class JoystickOptionsWidget;

Q_DECLARE_METATYPE(corecvs::JoystickState);

class JoystickListener : public QObject, public LinuxJoystickInterface
{
    Q_OBJECT
public:
    JoystickOptionsWidget *mTarget = NULL;

    JoystickListener(const std::string &deviceName, JoystickOptionsWidget *target) :
        LinuxJoystickInterface(deviceName),
        mTarget(target)
    {
        qRegisterMetaType<corecvs::JoystickState>("JoystickState");
    }

    // JoystickInterface interface
public:
    virtual void newButtonEvent    (int /*button*/, int /*value*/, int /*timestamp*/) override {}
    virtual void newAxisEvent      (int /*axis  */, int /*value*/, int /*timestamp*/) override {}
    virtual void newJoystickState  (corecvs::JoystickState state) override;

signals:
    void joystickUpdated(corecvs::JoystickState state);

public:
    virtual ~JoystickListener(){}
};

class QPushButton;
class QSlider;


class JoystickOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JoystickOptionsWidget(QWidget *parent = 0);
    ~JoystickOptionsWidget();


public slots:
    void rereadDevices();
    void getProps();

    void openJoystick();
    void closeJoystick();

    void clearDialog();
    void reconfigure(corecvs::JoystickConfiguration &conf);

    void newData(corecvs::JoystickState state);

signals:
    void joystickUpdated(corecvs::JoystickState state);

private:
    Ui::JoystickOptionsWidget *ui;

    JoystickListener *mInterface = NULL;

    std::vector<QPushButton                 *> mButtonWidgets;
    std::vector<MixerChannelOperationWidget *> mAxisWidgets;

};

#endif // JOYSTICKOPTIONSWIDGET_H

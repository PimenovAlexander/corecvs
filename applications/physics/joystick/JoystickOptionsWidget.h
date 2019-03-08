#ifndef JOYSTICKOPTIONSWIDGET_H
#define JOYSTICKOPTIONSWIDGET_H

#include "joystickInterface.h"

#include <QWidget>

namespace Ui {
class JoystickOptionsWidget;
}

class JoystickOptionsWidget;

Q_DECLARE_METATYPE(JoystickState);

class JoystickListener : public JoystickInterface
{
public:
    JoystickOptionsWidget *mTarget = NULL;

    JoystickListener(const std::string &deviceName, JoystickOptionsWidget *target) :
        JoystickInterface(deviceName),
        mTarget(target)
    {
        qRegisterMetaType<JoystickState>("JoystickState");
    }


    // JoystickInterface interface
public:
    virtual void newButtonEvent    (int /*button*/, int /*value*/, int /*timestamp*/) override {}
    virtual void newAxisEvent      (int /*axis  */, int /*value*/, int /*timestamp*/) override {}
    virtual void newJoystickState  (JoystickState state) override;

    virtual ~JoystickListener(){}
};

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
    void reconfigure(JoystickConfiguration &conf);

    void newData(JoystickState state);

private:
    Ui::JoystickOptionsWidget *ui;

    JoystickListener *mInterface = NULL;
};

#endif // JOYSTICKOPTIONSWIDGET_H

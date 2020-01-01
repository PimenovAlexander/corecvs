#ifndef JOYSTICKOPTIONSWIDGET_H
#define JOYSTICKOPTIONSWIDGET_H

#include "linuxJoystickInterface.h"
#include "mixerChannelOperationWidget.h"

#include <QWidget>

#include <joystick/playbackJoystickInterface.h>

namespace Ui {
class JoystickOptionsWidget;
}

class JoystickOptionsWidget;

Q_DECLARE_METATYPE(corecvs::JoystickState);

class JoystickInterfaceQt : public QObject, public virtual corecvs::JoystickInterface
{
    Q_OBJECT




signals:
    void joystickUpdated(corecvs::JoystickState state);

public:
    virtual ~JoystickInterfaceQt(){}
};

template<class BaseObject>
class JoystickListener : public JoystickInterfaceQt, public BaseObject
{
public:
    JoystickOptionsWidget *mTarget = NULL;

    JoystickListener(const std::string &deviceName, JoystickOptionsWidget *target) :
        BaseObject(deviceName),
        mTarget(target)
    {
        qRegisterMetaType<corecvs::JoystickState>("JoystickState");
    }

    // JoystickInterface interface
public:
    virtual void newButtonEvent    (int /*button*/, int /*value*/, int /*timestamp*/) override {}
    virtual void newAxisEvent      (int /*axis  */, int /*value*/, int /*timestamp*/) override {}
    virtual void newJoystickState  (corecvs::JoystickState state) override
    {
        emit joystickUpdated(state);
    #if 0
        QMetaObject::invokeMethod( mTarget, "newData", Qt::QueuedConnection,
                                   Q_ARG( JoystickState, state ) );
    #endif

    }

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

    void recordJoystick();

    void clearDialog();
    void reconfigure(corecvs::JoystickConfiguration &conf);

    void newData(corecvs::JoystickState state);

signals:
    void joystickUpdated(corecvs::JoystickState state);

private:
    Ui::JoystickOptionsWidget *ui;

    JoystickInterfaceQt *mInterface = NULL;

    std::vector<QPushButton                 *> mButtonWidgets;
    std::vector<MixerChannelOperationWidget *> mAxisWidgets;

    corecvs::JoystickConfiguration currentConfiguation;
    bool recording = false;
    corecvs::JoystickFileFormat record;

};

#endif // JOYSTICKOPTIONSWIDGET_H

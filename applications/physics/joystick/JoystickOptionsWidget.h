#ifndef JOYSTICKOPTIONSWIDGET_H
#define JOYSTICKOPTIONSWIDGET_H

#include <QWidget>

namespace Ui {
class JoystickOptionsWidget;
}

class JoystickOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JoystickOptionsWidget(QWidget *parent = 0);
    ~JoystickOptionsWidget();

public slots:
    void rereadDevices();
    void getProps();

private:
    Ui::JoystickOptionsWidget *ui;
};

#endif // JOYSTICKOPTIONSWIDGET_H

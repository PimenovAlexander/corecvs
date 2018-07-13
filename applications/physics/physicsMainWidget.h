#ifndef PHYSICSMAINWIDGET_H
#define PHYSICSMAINWIDGET_H

#include <QWidget>

namespace Ui {
class PhysicsMainWidget;
}

class PhysicsMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhysicsMainWidget(QWidget *parent = 0);
    ~PhysicsMainWidget();

private slots:
    void on_pushButton_released();
    void myHandler();

private:
    Ui::PhysicsMainWidget *ui;
};

#endif // PHYSICSMAINWIDGET_H

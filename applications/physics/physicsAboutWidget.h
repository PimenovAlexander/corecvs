#ifndef PHYSICSABOUTWIDGET_H
#define PHYSICSABOUTWIDGET_H

#include <QWidget>

namespace Ui {
class PhysicsAboutWidget;
}

class PhysicsAboutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhysicsAboutWidget(QWidget *parent = 0);
    ~PhysicsAboutWidget();

private:
    Ui::PhysicsAboutWidget *ui;
};

#endif // PHYSICSABOUTWIDGET_H

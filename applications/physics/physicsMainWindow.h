#ifndef PHYSICSMAINWINDOW_H
#define PHYSICSMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class PhysicsMainWindow;
}

class PhysicsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PhysicsMainWindow(QWidget *parent = 0);
    ~PhysicsMainWindow();

private:
    Ui::PhysicsMainWindow *ui;
};

#endif // PHYSICSMAINWINDOW_H

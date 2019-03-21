#include "physicsMainWindow.h"
#include "ui_physicsMainWindow.h"

PhysicsMainWindow::PhysicsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PhysicsMainWindow)
{
    ui->setupUi(this);
}

PhysicsMainWindow::~PhysicsMainWindow()
{
    delete ui;
}

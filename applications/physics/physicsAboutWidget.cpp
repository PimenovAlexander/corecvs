#include "physicsAboutWidget.h"
#include "ui_physicsAboutWidget.h"

PhysicsAboutWidget::PhysicsAboutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhysicsAboutWidget)
{
    ui->setupUi(this);
}

PhysicsAboutWidget::~PhysicsAboutWidget()
{
    delete ui;
}

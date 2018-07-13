#include "core/utils/global.h"
#include "physicsMainWidget.h"
#include "ui_physicsMainWidget.h"

PhysicsMainWidget::PhysicsMainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhysicsMainWidget)
{
    ui->setupUi(this);
}

PhysicsMainWidget::~PhysicsMainWidget()
{
    delete ui;
}

void PhysicsMainWidget::on_pushButton_released()
{
    SYNC_PRINT(("Hello World\n"));
}

void PhysicsMainWidget::myHandler()
{

}

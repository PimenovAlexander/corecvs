#include "vectorWidget.h"
#include "ui_vectorWidget.h"
#include <QCheckBox>

VectorWidget::VectorWidget(QWidget *parent) :
    QWidget(parent),
    mFabric(NULL),
    ui(new Ui::VectorWidget)
{
    ui->setupUi(this);
}

VectorWidget::~VectorWidget()
{
    delete ui;
}

void VectorWidget::addEntry()
{
    QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget->insertItem(ui->listWidget->count(), item);

    if (mFabric != NULL) {
        ParametersControlWidgetBase *widget = mFabric->produce();
        widget->show();
        ui->listWidget->setItemWidget(item, widget);

    }
}

void VectorWidget::removeEntry()
{
    QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->count() - 1);
    delete item;
}

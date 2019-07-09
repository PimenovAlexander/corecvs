#include "calibrationWidget.h"
#include "ui_calibrationWidget.h"

CalibrationWidget::CalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget)
{
    ui->setupUi(this);

}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

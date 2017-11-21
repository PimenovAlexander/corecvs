#include "testNativeWidget.h"
#include "ui_testNativeWidget.h"

#include <QFileDialog>
#include <QDebug>

TestNativeWidget::TestNativeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestNativeWidget)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(released()), this, SLOT(testButtonPushed()));
}

TestNativeWidget::~TestNativeWidget()
{
    delete ui;
}

void TestNativeWidget::testButtonPushed()
{
    QString dir_path = QFileDialog::getExistingDirectory( this,
                                                          "Select target directory",
                                                          "."
                                                          );
    qDebug() << dir_path;

}

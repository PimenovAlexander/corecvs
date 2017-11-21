#include <QFileDialog>

#include "fixtureControlWidget.h"
#include "ui_fixtureControlWidget.h"

FixtureControlWidget::FixtureControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FixtureControlWidget)
{
    ui->setupUi(this);
    /* Fixtures should be in World coordinates */
    ui->locationWidget->setPresentationStyle(true);

    QObject::connect(ui->loadPushButton, SIGNAL(released()), this, SLOT(loadPressed()));
    QObject::connect(ui->savePushButton, SIGNAL(released()), this, SLOT(savePressed()));


    QObject::connect(ui->locationWidget, SIGNAL(paramsChanged())     , this, SIGNAL(paramsChanged()));
    QObject::connect(ui->nameEdit      , SIGNAL(textChanged(QString)), this, SIGNAL(paramsChanged()));

}

FixtureControlWidget::~FixtureControlWidget()
{
    delete ui;
}

Affine3dControlWidget *FixtureControlWidget::getLocationWidget()
{
    return ui->locationWidget;
}

QLineEdit *FixtureControlWidget::getNameWidget()
{
    return ui->nameEdit;
}

void FixtureControlWidget::loadPressed()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "LOAD: Choose an camera config name",
        ".",
        "Text (*.json)"
    );
    if (!filename.isEmpty()) {
        emit loadRequest(filename);
    }
}

void FixtureControlWidget::savePressed()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "SAVE: Choose an camera config name",
        ".",
        "Text (*.json)"
    );
    if (!filename.isEmpty()) {
        emit saveRequest(filename);
    }
}



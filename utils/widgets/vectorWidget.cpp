#include "vectorWidget.h"
#include "ui_vectorWidget.h"
#include <QCheckBox>

VectorWidget::VectorWidget(QWidget *parent) :
    QWidget(parent),
    mFabric(NULL),
    ui(new Ui::VectorWidget)
{
    ui->setupUi(this);
    // buttonsSide(true);
    ui->widgetLow->setVisible(false);
}

VectorWidget::~VectorWidget()
{
    delete ui;
}

void VectorWidget::buttonsSide(bool flag)
{
    ui->widgetLow ->setVisible(!flag);
    ui->widgetSide->setVisible(flag);
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


/*==============  DoubleVectorWidget ==============*/

DoubleVectorWidget::DoubleVectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VectorWidget)
{
    ui->setupUi(this);
    mMaximum    = 1000;
    mMinimum    = 0;
    mDecimals   = 2;
    mSingleStep = 1;
}

DoubleVectorWidget::~DoubleVectorWidget()
{
    delete_safe(ui);
}

void DoubleVectorWidget::addEntry()
{
    QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget->insertItem(ui->listWidget->count(), item);
    QDoubleSpinBox *widget = new QDoubleSpinBox;
    widget->show();
    widget->setMaximum(mMaximum);
    widget->setMinimum(mMinimum);
    widget->setDecimals(mDecimals);
    widget->setSingleStep(mSingleStep);

    ui->listWidget->setItemWidget(item, widget);
    connect(widget, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged()));

    emit valueChanged();
}

void DoubleVectorWidget::removeEntry()
{
    QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->count() - 1);
    delete item;
    emit valueChanged();
}

void DoubleVectorWidget::resize(int size)
{
    if (size < 0) size = 0;

    while ( ui->listWidget->count() > size)
        removeEntry();

    while ( ui->listWidget->count() < size)
        addEntry();

}

vector<double> DoubleVectorWidget::value()
{
    vector<double> toReturn;
    toReturn.resize(ui->listWidget->count());

    for (size_t i = 0; i < toReturn.size(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item((int)i);
        QDoubleSpinBox *dw = static_cast<QDoubleSpinBox *>(ui->listWidget->itemWidget(item));
        toReturn[i] = dw->value();
    }
    return toReturn;
}

void DoubleVectorWidget::setValue(const vector<double> &value)
{
    resize((int)value.size());
    for (size_t i = 0; i < value.size(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item((int)i);
        QDoubleSpinBox *dw = static_cast<QDoubleSpinBox *>(ui->listWidget->itemWidget(item));
        dw->setValue(value[i]);
    }
}

void DoubleVectorWidget::setMaximum(double value)
{
    mMaximum = value;
    for (size_t i = 0; i < (size_t)ui->listWidget->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item((int)i);
        QDoubleSpinBox *dw = static_cast<QDoubleSpinBox *>(ui->listWidget->itemWidget(item));
        dw->setMaximum(mMaximum);
    }
}

void DoubleVectorWidget::setMinimum(double value)
{
    mMinimum = value;
    for (size_t i = 0; i < (size_t)ui->listWidget->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item((int)i);
        QDoubleSpinBox *dw = static_cast<QDoubleSpinBox *>(ui->listWidget->itemWidget(item));
        dw->setMinimum(mMinimum);
    }
}

void DoubleVectorWidget::setDecimals(int value)
{
    mDecimals = value;
}

void DoubleVectorWidget::setSingleStep(double value)
{
    mSingleStep = value;
}

double DoubleVectorWidget::maximum()
{
    return mMaximum;
}

double DoubleVectorWidget::minimum()
{
    return mMinimum;
}

int DoubleVectorWidget::decimals()
{
    return mDecimals;
}

double DoubleVectorWidget::singleStep()
{
    return mSingleStep;
}




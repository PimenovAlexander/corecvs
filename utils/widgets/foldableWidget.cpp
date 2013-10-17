#include "foldableWidget.h"
#include "ui_foldableWidget.h"
#include "configManager.h"

#include <QtCore/QSettings>

FoldableWidget::FoldableWidget(
          QWidget *parent
        , const QString &name
        , QWidget *centralWidget
        , bool startFolded
        , bool isAdvanced) :
    QWidget(parent),
    ui(new Ui::Widget),
    mIsFolded(false),
    mName(name)
{
    ui->setupUi(this);
    ui->foldLabel->setText("Fold " + mName);

    connect(ui->foldButton, SIGNAL(clicked()), this, SLOT(changeFolding()));

    setMainWidget(centralWidget);

    QSettings settings(ConfigManager::configName(), QSettings::IniFormat);
    settings.beginGroup("foldedWidgets");
    startFolded = settings.value(name, true).toBool();
    settings.endGroup();

    if (startFolded)
    {
        changeFolding();
    }

    if (isAdvanced)
    {
        setProperty("tags", QString("advanced"));
    }
}

FoldableWidget::~FoldableWidget()
{
    QSettings settings(ConfigManager::configName(), QSettings::IniFormat);
    settings.beginGroup("foldedWidgets");
    settings.setValue(mName, mIsFolded);
    settings.endGroup();

    delete ui;
}

void FoldableWidget::changeFolding()
{
    mIsFolded = !mIsFolded;

    mCurrentSize = mPrevSize;
    mPrevSize = size();
    resize(mCurrentSize);

    if (mIsFolded) {
        ui->foldButton->setArrowType(Qt::RightArrow);
        ui->foldLabel->setText("Unfold " + mName);
    }
    else {
        ui->foldButton->setArrowType(Qt::DownArrow);
        ui->foldLabel->setText("Fold " + mName);
    }

    ui->placeholder->setVisible(!mIsFolded);
    ui->placeholder->setEnabled(!mIsFolded);
}

void FoldableWidget::setMainWidget(QWidget *widget)
{
    ui->placeholder->setLayout(new QVBoxLayout());
    ui->placeholder->layout()->addWidget(widget);
    ui->placeholder->layout()->setContentsMargins(0, 0, 0, 0);
    updateGeometry();
}

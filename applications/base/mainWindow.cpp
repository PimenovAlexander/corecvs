#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(BaseHostDialog *dialog, const QString &source,
        const MainWindowParameters &params,  QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::MainWindow)
    , mMainDialog(dialog)
{
    mUi->setupUi(this);
    Log::mLogDrains.push_back(mUi->loggingWidget);

    mMainDialog->init(mUi->widget);
//    mMainDialog->init(mUi->scrollAreaWidgetContents);
    mMainDialog->initCapture(source);
    mMainDialog->show();
    setCentralWidget(mMainDialog);

    showMaximized();
    setWindowTitle(mMainDialog->windowTitle());

    if (params.autoPlay)
    {
        QTimer::singleShot(600, mMainDialog, SLOT(play()));
    }

    if (params.startMini)
    {
        QTimer::singleShot(600, mMainDialog, SLOT(showMinimized()));
    }

    connectActions();

    createAdditionalMenu();

    // Hide log by default
    mUi->loggingDockWidget->setVisible(false);
}

MainWindow::~MainWindow()
{
    mMainDialog->deinit();
    delete_safe(mUi);
    delete_safe(mMainDialog);
}

void MainWindow::connectActions()
{
    connect(mUi->actionShowLog,         SIGNAL(triggered(bool)), mUi->loggingDockWidget, SLOT(setVisible(bool)));
    connect(mUi->actionToggleAdvanced,  SIGNAL(triggered(bool)), mMainDialog, SLOT(toggleAdvanced(bool)));

    connect(mUi->action_OpenInput,         SIGNAL(triggered()), mMainDialog, SLOT(showInputSelectorDialog()));
    connect(mUi->actionStatistics,         SIGNAL(triggered()), mMainDialog, SLOT(showStatistics()));
    connect(mUi->actionHistogram,          SIGNAL(triggered()), mMainDialog, SLOT(showHistogram()));
    connect(mUi->action3DHistogram,        SIGNAL(triggered()), mMainDialog, SLOT(show3DHistogram()));
    connect(mUi->actionCameraSettings,     SIGNAL(triggered()), mMainDialog, SLOT(showCaptureSettings()));
    connect(mUi->actionRectify,            SIGNAL(triggered()), mMainDialog, SLOT(showRectificationDialog()));
    connect(mUi->actionAbout,              SIGNAL(triggered()), mMainDialog, SLOT(showAboutDialog()));
    connect(mUi->actionDO,                 SIGNAL(triggered()), mMainDialog, SLOT(doRectify()));
    connect(mUi->actionResetRectification, SIGNAL(triggered()), mMainDialog, SLOT(resetRectification()));
    connect(mUi->actionSaveParameters,     SIGNAL(triggered()), mMainDialog, SLOT(doSaveParams()));
    connect(mUi->actionLoadParameters,     SIGNAL(triggered()), mMainDialog, SLOT(doLoadParams()));
    connect(mUi->actionCorrectDistortion,  SIGNAL(triggered()), mMainDialog, SLOT(showDistortionCorrectorDialog()));
    connect(mUi->actionCameraCalculator,   SIGNAL(triggered()), mMainDialog, SLOT(showCameraCalculatorDialog()));

    connect(mUi->actionAbout_Qt,           SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(mUi->actionStartCapture,       SIGNAL(triggered()), mMainDialog, SLOT(play()));
    connect(mUi->actionNextFrame,          SIGNAL(triggered()), mMainDialog, SLOT(nextFrame()));
    connect(mUi->actionStepMode,           SIGNAL(triggered()), mMainDialog, SLOT(stopCapture()));
    connect(mUi->actionPauseCalculation,   SIGNAL(triggered(bool)), this, SLOT(updateCalculationStatus(bool)));

    connect(mMainDialog, SIGNAL(captureStatusUpdated(bool)), this, SLOT(updateCaptureStatus(bool)));
    connect(mMainDialog, SIGNAL(singleStepStatusUpdated()), this, SLOT(updateSingleStateStatus()));
    connect(mMainDialog, SIGNAL(updatePauseCalculationAction(bool)), mUi->actionPauseCalculation, SLOT(setEnabled(bool)));

    connect(this, SIGNAL(pauseCalculation()),  mMainDialog, SIGNAL(pauseCalculator()));
    connect(this, SIGNAL(resumeCalculation()), mMainDialog, SIGNAL(resumeCalculator()));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qApp->quit();
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_X)
    {
        close();
    }

    event->ignore();
}

void MainWindow::createAdditionalMenu()
{
    QList<QMenu *> menuList = mMainDialog->additionalMenus();
    foreach (QMenu *menu, menuList)
    {
        menuBar()->insertMenu(mUi->menuHelp->menuAction(), menu);

        // create additional toolbar with actions from current menu
        mUi->toolBar->addSeparator();
        mUi->toolBar->addActions(menu->actions());
    }
}

void MainWindow::updateCaptureStatus(bool isOn)
{
    mUi->actionStartCapture->setEnabled(isOn);
}

void MainWindow::updateSingleStateStatus()
{
    bool isSingleStep = !mUi->actionStepMode->isChecked();
    mUi->actionStepMode->setText(isSingleStep ? "Run" : "Step Mode");
    mUi->actionNextFrame->setEnabled(isSingleStep);
}

void MainWindow::updateCalculationStatus(bool isOn)
{
    if (isOn)
        emit pauseCalculation();
    else
        emit resumeCalculation();
}

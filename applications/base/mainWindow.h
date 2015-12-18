#pragma once

#include "baseHostDialog.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindowParameters
{
public:
    bool autoPlay;
    bool startMini;
    bool autoStop;
    int  frameToStop;

    MainWindowParameters(
            bool _autoPlay,
            bool _startMini,
            bool _autoStop,
            int  _frameToStop
    ) :
        autoPlay(_autoPlay),
        startMini(_startMini),
        autoStop(_autoStop),
        frameToStop(_frameToStop)
    { }

    MainWindowParameters() :
        autoPlay(false),
        startMini(false),
        autoStop(false),
        frameToStop(0)
    { }

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
            BaseHostDialog *dialog,
            const QString &source,
            const MainWindowParameters &params,
            bool isRgb = false,
            QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void addMainPanelAction(QAction *action);

signals:
    void pauseCalculation();
    void resumeCalculation();

private slots:
    void updateCaptureStatus(bool isOn);
    void updateSingleStateStatus();
    void updateCalculationStatus(bool isOn);

private:
    void connectActions();
    void createAdditionalMenu();

    Ui::MainWindow *mUi;
    BaseHostDialog *mMainDialog; // has ownership
};


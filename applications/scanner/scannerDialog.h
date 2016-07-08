#ifndef _SCANNER_DIALOG_H_
#define _SCANNER_DIALOG_H_

/**
 * \file scannerDialog.h
 * \brief Defines a scanner dialog class derived from BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <QString>

#include "global.h"

#include "baseHostDialog.h"
#include "scannerThread.h"
#include "scannerControlWidget.h"

#define UI_NAME_SCANNER "scanner"

class ScannerDialog : public BaseHostDialog
{
    Q_OBJECT

public:

    ScannerDialog();
    ~ScannerDialog();


    virtual void initParameterWidgets();
    virtual void createCalculator();
    virtual void connectFinishedRecalculation();

signals:
    void recordingTriggered();
    void recordingReset();

public slots:
    void openPathSelectDialog();
    void toggleRecording();
    void resetRecording();
    void recordingStateChanged(ScannerThread::RecordingState );
    void scannerControlParametersChanged(QSharedPointer<Scanner> params);

    virtual void processResult();

    void errorMessage(QString message);
private:
    bool mIsRecording;

    ScannerControlWidget *mScannerControlWidget;

    QSharedPointer<Scanner> mScannerControlParams;

};

#endif // _SCANNER_DIALOG_H_

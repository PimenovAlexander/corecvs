#ifndef _RECORDER_DIALOG_H_
#define _RECORDER_DIALOG_H_

/**
 * \file recorderDialog.h
 * \brief Defines a recorder dialog class derived from BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <QString>

#include "global.h"

#include "baseHostDialog.h"
#include "recorderThread.h"
#include "recorderControlWidget.h"

#define UI_NAME_RECORDER "recorder"

class RecorderDialog : public BaseHostDialog
{
    Q_OBJECT

public:

    RecorderDialog();
    ~RecorderDialog();


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
    void recordingStateChanged(RecorderThread::RecordingState );
    void recorderControlParametersChanged(QSharedPointer<Recorder> params);

    virtual void processResult();

    void errorMessage(QString message);
private:
    bool mIsRecording;

    RecorderControlWidget *mRecorderControlWidget;

    QSharedPointer<Recorder> mRecorderControlParams;

    /* These slots are for silencing error, created due current to misdesign */
public slots:
    void showHistogram();
    void show3DHistogram();
    void showRectificationDialog();
    void doRectify();
    void resetRectification();

};

#endif // _RECORDER_DIALOG_H_

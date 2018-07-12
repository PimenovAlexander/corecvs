#ifndef _COPTER_DIALOG_H_
#define _COPTER_DIALOG_H_

/**
 * \file copterDialog.h
 * \brief Defines a copter dialog class derived from BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <QString>
#include <cloudViewDialog.h>

#include "core/utils/global.h"

#include "baseHostDialog.h"
#include "copterThread.h"
#include "copterControlWidget.h"

#define UI_NAME_COPTER "copter"

class CopterDialog : public BaseHostDialog
{
    Q_OBJECT

public:

    CopterDialog();
    ~CopterDialog();


    virtual void initParameterWidgets() override;
    virtual void createCalculator() override;
    virtual void connectFinishedRecalculation();

signals:
    void recordingTriggered();
    void recordingReset();

public slots:
    void copterControlParametersChanged(QSharedPointer<Copter> params);

    virtual void processResult() override;

    void errorMessage(QString message);
private:
    bool mIsRecording;

    CopterControlWidget *mCopterControlWidget;

    QSharedPointer<Copter> mCopterControlParams;

    CloudViewDialog *cloud = NULL;

    /* These slots are for silencing error, created due current to misdesign */
public slots:
    void showHistogram();
    void show3DHistogram();
    void showRectificationDialog();
    void doRectify();
    void resetRectification();

};

#endif // _COPTER_DIALOG_H_

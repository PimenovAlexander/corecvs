#ifndef _EGOMOTION_DIALOG_H_
#define _EGOMOTION_DIALOG_H_

/**
 * \file egomotionDialog.h
 * \brief Defines a egomotion dialog class derived from BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include <QString>

#include "global.h"

#include "baseHostDialog.h"
#include "egomotionThread.h"
#include "generated/ui/egomotionParametersControlWidget.h"

#define UI_NAME_RECORDER "egomotion"

class EgomotionDialog : public BaseHostDialog
{
    Q_OBJECT

public:

    EgomotionDialog();
    ~EgomotionDialog();


    virtual void initParameterWidgets();
    virtual void createCalculator();
    virtual void connectFinishedRecalculation();

signals:
    void recordingTriggered();
    void recordingReset();

public slots:   
    void toggleRecording();
    void resetRecording();
    void egomotionControlParametersChanged(QSharedPointer<EgomotionParameters> params);

    virtual void processResult();

    void errorMessage(QString message);
private:
    bool mIsRecording;

    EgomotionParametersControlWidget *mEgomotionParametersControlWidget;

    QSharedPointer<EgomotionParameters> mEgomotionControlParams;

    AdvancedImageWidget *addImage = NULL;
    GraphPlotDialog *graph = NULL;

};

#endif // _EGOMOTION_DIALOG_H_

/**
 * \file SaveFlowSettings.h
 *
 * \date Feb 27, 2012
 * \author: Olga Sapershteyn
 */

#ifndef SAVEFLOWSETTINGS_H
#define SAVEFLOWSETTINGS_H


#include <QWidget>

#include "ui_saveFlowSettings.h"

class SaveFlowSettings : public QWidget {

    Q_OBJECT


public:
    SaveFlowSettings();

    ~SaveFlowSettings();

    Ui_SaveFlowSettings* getUi();


public slots:
    void openPathSelectDialog();


protected:
    Ui_SaveFlowSettings *mUi;
};

#endif // SAVEFLOWSETTINGS_H

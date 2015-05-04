#include "saveFlowSettings.h"
#include <QFileDialog>

SaveFlowSettings::SaveFlowSettings():
    mUi(new Ui::SaveFlowSettings)
{
    mUi->setupUi(this);
    connect(mUi->choosePathButton, SIGNAL(clicked()), this, SLOT(openPathSelectDialog()));
}

SaveFlowSettings::~SaveFlowSettings()
{
    delete mUi;
}


Ui_SaveFlowSettings* SaveFlowSettings::getUi()
{
    return mUi;
}

void SaveFlowSettings::openPathSelectDialog()
{
    QString dir_path = QFileDialog::getExistingDirectory( this,
                                                          "Select target directory",
                                                          "."
                                                          );

    if (dir_path.length() > 0)
    {
        mUi->pathEdit->setText(dir_path);
    }
}

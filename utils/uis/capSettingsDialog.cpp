#include <QtGui/QMessageBox>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

#include "global.h"
#include "capSettingsDialog.h"

#include "parameterSelector.h"


CapSettingsDialog::CapSettingsDialog(QWidget *parent, ImageCaptureInterface *pInterface) :
        QWidget(parent),
        mUi(new Ui::CapSettingsDialog),
        mCaptureInterface(pInterface),
        signalMapper(NULL),
        resetSignalMapper(NULL)
{
    mUi->setupUi(this);

    setWindowFlags(windowFlags() ^ Qt::WindowMinimizeButtonHint);

    refreshDialog();
}

void CapSettingsDialog::clearDialog()
{
    delete_safe(signalMapper);
    delete_safe(resetSignalMapper);

    QLayout *layout = mUi->scrollAreaWidgetContents->layout();
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != 0)
    {
        delete item->widget();
        delete_safe(item);
    }
}

void CapSettingsDialog::refreshDialog()
{
   if (mCaptureInterface == NULL)
   {
       return;
   }

    //ImageCaptureInterface::CapErrorCode resLeft =
   mCaptureInterface->queryCameraParameters(mLeftCameraParameters);
   //ImageCaptureInterface::CapErrorCode resRight =
   mCaptureInterface->queryCameraParameters(mRightCameraParameters);

   QLayout *layout = mUi->scrollAreaWidgetContents->layout();

   signalMapper = new QSignalMapper(this);
   resetSignalMapper = new QSignalMapper(this);

   for (int i = CameraParameters::FIRST; i < CameraParameters::LAST; i++)
   {
       CaptureParameter &params = mLeftCameraParameters.mCameraControls[i];

       if (!params.active()) {
           continue;
       }

       ParameterEditorWidget *paramEditor = NULL;

       if (!params.isMenu() || !params.hasMenuItems())
       {
           ParameterSlider* paramSlider = new ParameterSlider(this);
           paramSlider->setMinimum(params.minimum());
           paramSlider->setMaximum(params.maximum());
           paramEditor = paramSlider;
       } else {
           ParameterSelector* paramSelector = new ParameterSelector(this);
           for (unsigned index = 0; index < params.getMenuItemNumber(); index++)
           {
               paramSelector->pushOption(
                   params.getMenuItem(index),
                   params.getMenuValue(index));
           }
           paramEditor = paramSelector;
       }

       paramEditor->setName(QString(CameraParameters::names[i]));
       paramEditor->setAutoSupported(params.autoSupported());
       paramEditor->setEnabled(params.active());

       int value;
       mCaptureInterface->getCaptureProperty(i, &value);
       paramEditor->setValue(value);

       layout->addWidget(paramEditor);
       sliders.insert(i, paramEditor);

       connect(paramEditor, SIGNAL(valueChanged(int)), signalMapper, SLOT(map()));
       signalMapper->setMapping(paramEditor, i);

       connect(paramEditor, SIGNAL(resetPressed()), resetSignalMapper, SLOT(map()));
       resetSignalMapper->setMapping(paramEditor, i);
   }

   layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));

   //layout->addChildLayout()(new QSpacerItem(0,0));
   connect(signalMapper,      SIGNAL(mapped(int)), this, SLOT(parameterChanged(int)));
   connect(resetSignalMapper, SIGNAL(mapped(int)), this, SLOT(resetPressed(int)));


}

void CapSettingsDialog::loadFromQSettings (const QString &fileName, const QString &_root)
{
    if (mCaptureInterface == NULL)
    {
        return;
    }

    QString interfaceName = mCaptureInterface->getInterfaceName();
    if (interfaceName.isEmpty())
    {
        qDebug("CapSettingsDialog::loadFromQSettings(): Loading won't happen. Interface is NULL");
        return;
    }

    qDebug() << "CapSettingsDialog::loadFromQSettings(): Interface name: " << interfaceName;
    QSettings *mSettings = new QSettings(fileName, QSettings::IniFormat);
    mSettings->beginGroup(_root + ":" + interfaceName + ":");


    QMapIterator<int, ParameterEditorWidget *> i(sliders);
    while (i.hasNext())
    {
        i.next();
        int id = i.key();
        const char *name = CameraParameters::names[id];
        ParameterEditorWidget *widget = i.value();
        double value = widget->value();
        double newValue = mSettings->value(name, value).toDouble();
        qDebug() << "   " << QString(name).leftJustified(16, ' ') << ": " << value << " -> " << newValue;
        widget->setValue(newValue);
    }

    mSettings->endGroup();
    delete mSettings;

}

void CapSettingsDialog::saveToQSettings   (const QString &fileName, const QString &_root)
{
    if (mCaptureInterface == NULL)
    {
        return;
    }

    QString interfaceName = mCaptureInterface->getInterfaceName();
    if (interfaceName.isEmpty())
    {
        return;
    }

    qDebug() << "CapSettingsDialog::saveToQSettings(): Interface name: " << interfaceName;
    QSettings *mSettings = new QSettings(fileName, QSettings::IniFormat);
    mSettings->beginGroup(_root + ":" + interfaceName + ":");


    QMapIterator<int, ParameterEditorWidget *> i(sliders);
    while (i.hasNext())
    {
        i.next();
        int id = i.key();
        const char *name = CameraParameters::names[id];
        ParameterEditorWidget *widget = i.value();
        double value = widget->value();
        mSettings->setValue(name, value);
        qDebug() << "   " << QString(name).leftJustified(16, ' ') << ": " << value;
    }

    mSettings->endGroup();
    delete mSettings;
}


void CapSettingsDialog::refreshLimitsPressed()
{
    clearDialog();
    refreshDialog();
}


CapSettingsDialog::~CapSettingsDialog()
{
    clearDialog();
    mCaptureInterface = NULL;
    delete_safe(mUi);
}

void CapSettingsDialog::parameterChanged(int id)
{
    //printf("Changed: %d\n", id);
    /*if (id == 0)
    {
        sliders[1]->setValue(0);
    }*/
    mCaptureInterface->setCaptureProperty(id, sliders[id]->value());
}

void CapSettingsDialog::resetPressed(int id)
{
    if (mCaptureInterface == NULL)
    {
        return;
    }

    //printf("Changed: %d\n", id);
    mCaptureInterface->setCaptureProperty(id, mLeftCameraParameters.mCameraControls[id].defaultValue());
    sliders[id]->setValue(mLeftCameraParameters.mCameraControls[id].defaultValue());
}


void CapSettingsDialog::resetAllPressed()
{
    for (int i = CameraParameters::FIRST; i < CameraParameters::LAST; i++)
    {
        CaptureParameter &params = mLeftCameraParameters.mCameraControls[i];
        if (!params.active())
            continue;
        resetPressed(i);
    }
}

void CapSettingsDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        mUi->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CapSettingsDialog::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
#if 0
    if (!disabledAlertShown  && !(mUi->aeEnabledCheckBox->isEnabled()))
    {
        disabledAlertShown = true;
        QMessageBox msgBox;
        msgBox.setText("Error querying camera exposure properties. You will not be able to modify exposure settings.\n"
                       "This may occur because you are using different camera devices or because your cameras do not support"
                       "exposure settings.");
        msgBox.exec();
    }
#endif
}

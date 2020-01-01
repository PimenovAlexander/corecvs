#include "capSettingsDialog.h"
#include "widgets/parameterSelector.h"
#include "utils/log.h"

#include <QMessageBox>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

CapSettingsDialog::CapSettingsDialog(QWidget *parent, QString rootPath)
    :   QWidget(parent)
    ,   mRootPath(rootPath)
    ,   mUi(new Ui::CapSettingsDialog)
    ,   mCaptureInterface(NULL)
    ,   signalMapper(NULL)
    ,   resetSignalMapper(NULL)
{
    mUi->setupUi(this);

    setWindowTitle(rootPath);
    setWindowFlags(windowFlags() ^ Qt::WindowMinimizeButtonHint);

    refreshDialog();
}

void CapSettingsDialog::setCaptureInterface(ImageCaptureInterface *pInterface, bool updateOnFly)
{
    mCaptureInterface = pInterface;
    mUpdateOnFly = updateOnFly;
    refreshDialog();
}

void CapSettingsDialog::clearDialog()
{
    delete_safe(signalMapper);
    delete_safe(resetSignalMapper);

    QLayout *layout = mUi->scrollAreaWidgetContents->layout();
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete_safe(item);
    }
}

void CapSettingsDialog::refreshDialog()
{
    clearDialog();
    if (mCaptureInterface == NULL)
        return;

    //ImageCaptureInterface::CapErrorCode res =
    mCaptureInterface->queryCameraParameters(mCameraParameters);

    mUi->updateOnFlyCheckBox->setChecked(mUpdateOnFly ? true : false);

    QLayout *layout = mUi->scrollAreaWidgetContents->layout();

    signalMapper      = new QSignalMapper(this);
    resetSignalMapper = new QSignalMapper(this);

    for (int id = CameraParameters::FIRST; id < CameraParameters::LAST; id++)
    {
        CaptureParameter &params = mCameraParameters.mCameraControls[id];
        if (!params.active())
            continue;

        ParameterEditorWidget *paramEditor = NULL;

        if (!params.isMenu() || !params.hasMenuItems())
        {
            ParameterSlider* paramSlider = new ParameterSlider(this);
            paramSlider->setMinimum(params.minimum());
            paramSlider->setMaximum(params.maximum());
            paramSlider->setStep   (params.step());
            paramEditor = paramSlider;
        }
        else
        {
            ParameterSelector* paramSelector = new ParameterSelector(this);
            for (unsigned index = 0; index < params.getMenuItemNumber(); index++)
            {
                paramSelector->pushOption(params.getMenuItem(index).c_str(),
                                          params.getMenuValue(index));
            }
            paramEditor = paramSelector;
        }

        paramEditor->setName(CameraParameters::names[id]);
        paramEditor->setAutoSupported(params.autoSupported());
        paramEditor->setEnabled(params.active());

        int value;
        mCaptureInterface->getCaptureProperty(id, &value);
        paramEditor->setValue(value);

        layout->addWidget(paramEditor);
        sliders.insert(id, paramEditor);

        connect(paramEditor, SIGNAL(valueChanged(int)), signalMapper, SLOT(map()));
        signalMapper->setMapping(paramEditor, id);

        connect(paramEditor, SIGNAL(resetPressed()), resetSignalMapper, SLOT(map()));
        resetSignalMapper->setMapping(paramEditor, id);
    }

    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));

    //layout->addChildLayout()(new QSpacerItem(0,0));
    connect(signalMapper     , SIGNAL(mapped(int)), this, SLOT(parameterChanged(int)));
    connect(resetSignalMapper, SIGNAL(mapped(int)), this, SLOT(resetPressed(int)));
}

void CapSettingsDialog::loadFromQSettings(const QString &fileName, const QString &_root, bool interfaceGroup)
{
    if (mCaptureInterface == NULL)
        return;

    QString interfaceName = QString::fromStdString(mCaptureInterface->getInterfaceName());
    if (interfaceName.isEmpty())
    {
        L_DDEBUG_P("loading won't happen - interfaceName is empty");
        return;
    }
    // L_DDEBUG_P("ifcName: %s", QSTR_DATA_PTR(interfaceName));

    QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
    settings->beginGroup(_root);
    settings->beginGroup(mRootPath);
    if (interfaceGroup)
        settings->beginGroup(interfaceName);

    QMapIterator<int, ParameterEditorWidget *> i(sliders);
    while (i.hasNext())
    {
        i.next();
        int id = i.key();
        const char *name = CameraParameters::names[id];
        ParameterEditorWidget *widget = i.value();
        double value = widget->value();
        double newValue = settings->value(name, value).toDouble();
        qDebug() << "   " << QString(name).leftJustified(16, ' ') << ": " << value << " -> " << newValue;
        widget->setValue(newValue);
    }

    if (interfaceGroup)
        settings->endGroup();
    settings->endGroup();
    settings->endGroup();

    delete_safe(settings);
}

void CapSettingsDialog::saveToQSettings(const QString &fileName, const QString &_root, bool interfaceGroup)
{
    L_DDEBUG_P("filename: <%s> root:<%s>", QSTR_DATA_PTR(fileName), QSTR_DATA_PTR(_root));
    if (mCaptureInterface == NULL)
    {
        L_DDEBUG_P("mCaptureInterface is null");
        return;
    }

    QString interfaceName = QString::fromStdString(mCaptureInterface->getInterfaceName());
    if (interfaceName.isEmpty())
    {
        L_DDEBUG_P("interfaceName is empty");
        return;
    }
    L_DDEBUG_P("ifcName: %s", QSTR_DATA_PTR(interfaceName));

    std::unique_ptr<QSettings> settings(new QSettings(fileName, QSettings::IniFormat));
    settings->beginGroup(_root);
    settings->beginGroup(mRootPath);
    if (interfaceGroup)
        settings->beginGroup(interfaceName);

    QMapIterator<int, ParameterEditorWidget *> i(sliders);
    while (i.hasNext())
    {
        i.next();
        int id = i.key();
        const char *name = CameraParameters::names[id];
        ParameterEditorWidget *widget = i.value();
        double value = widget->value();
        settings->setValue(name, value);
        qDebug() << "   " << QString(name).leftJustified(16) << ": " << value;
    }

    if (interfaceGroup)
        settings->endGroup();
    settings->endGroup();
    settings->endGroup();
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
    if (mCaptureInterface == NULL)
        return;

    bool updCam = mUi->updateOnFlyCheckBox->isChecked();
    int  v      = sliders[id]->value();

    L_DDEBUG_P("id:%d %s\t dlg=>%s\tv: %d", id, CameraParameters::names[id], (updCam ? "cam" : "NO-cam"), v);

    if (updCam)
        mCaptureInterface->setCaptureProperty(id, v);
}

void CapSettingsDialog::newCameraParamValue(int id)
{
    L_DDEBUG_P("id:%d %s", id, CameraParameters::names[id]);

    if (mCaptureInterface == NULL)
        return;

    int v;
    mCaptureInterface->getCaptureProperty(id, &v);

    //TODO: ??? parameterChanged will be called later, which actualizes camState
    //sliders[id]->blockSignals(true);
    sliders[id]->setValue(v);
    //sliders[id]->blockSignals(false);

    L_DDEBUG_P("id:%d %s\t cam=>dlg \t v: %d", id, CameraParameters::names[id], v);
}

void CapSettingsDialog::resetPressed(int id)
{
    bool updCam = mUi->updateOnFlyCheckBox->isChecked();
    int  v      = mCameraParameters.mCameraControls[id].defaultValue();

    L_DDEBUG_P("id:%d %s\t def=>dlf+%s\t v: %d", id, CameraParameters::names[id], (updCam ? "cam" : "NO-cam"), v);

    if (updCam && mCaptureInterface)
        mCaptureInterface->setCaptureProperty(id, v);

    //TODO: ??? parameterChanged will be called later, which actualizes camState
    sliders[id]->setValue(v);
}

void CapSettingsDialog::resetAllPressed()
{
    for (int id = CameraParameters::FIRST; id < CameraParameters::LAST; id++)
    {
        CaptureParameter &param = mCameraParameters.mCameraControls[id];
        if (!param.active())
            continue;

        resetPressed(id);
    }
}

void CapSettingsDialog::updateAllPressed()
{
    for (int id = CameraParameters::FIRST; id < CameraParameters::LAST; id++)
    {
        CaptureParameter &param = mCameraParameters.mCameraControls[id];
        if (!param.active())
            continue;

        int camv;
        mCaptureInterface->getCaptureProperty(id, &camv);

        int v = sliders[id]->value();

        if (camv != v)
        {
            L_DDEBUG_P("id:%d %s\t dlg=>cam\t prev: %d\t v: %d", id, CameraParameters::names[id], camv, v);

            mCaptureInterface->setCaptureProperty(id, v);
        }
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

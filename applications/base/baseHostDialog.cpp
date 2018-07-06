/**
 * \file baseHostDialog.cpp
 * \brief Implements a basic host dialog that uses BaseCalculationThread to process captured data
 *        Extend or reimplement this when designing specific host applications
 *
 * \date Aug 27, 2010
 * \author Sergey Levi
 */

#include <iostream>
#include <stdio.h>

#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include "core/utils/log.h"

#include "baseHostDialog.h"

#include "g12Image.h"
#include "graphPlotDialog.h"
#include "textLabelWidget.h"
#include "core/utils/visitors/propertyListVisitor.h"
#include "foldableWidget.h"
#include "parametersMapper/parametersMapperBase.h"

#define WITH_OPENGL
#ifdef WITH_OPENGL
#include "cloudViewDialog.h"
#endif

#ifdef Q_OS_WIN
#include "windowsMemoryUsageCalculator.h"
#elif MACX
#include "macMemoryUsageCalculator.h"
#else
#include "linuxMemoryUsageCalculator.h"
#endif

#include "qSettingsGetter.h"
#include "qSettingsSetter.h"
#include "../utils/serializer/widgetQtIterator.h"
#include "painterHelpers.h"
#include "configManager.h"

using namespace std;
const QString initialCameraString = "InitialCameraString";

BaseHostDialog::BaseHostDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
    , mParamsMapper(NULL)
    , mCalculator(NULL)
    , mImage(NULL)
    , mUseOneCaptureDevice(false)
    , mDistortionWidget(NULL)
    , mCameraStarted(false)
    , mCamera(NULL)
    , mCapSettings(NULL, "MainSource")
    , mFrames(NULL)
    , mBaseControlWidget(NULL)
    , mPresentationControlWidget(NULL)
    , mFilterGraphPresentation(NULL)
    , mAdditionalTools(new QMenu("Tools"))
    , mMemoryCalculator(NULL)
    , mDockWidget(NULL)
    , mLoggerWidget(NULL)
    , mScheduledSwap(false)
    , mDistortionTransform(NULL)
    , mCorrectionFrame(Frames::LEFT_FRAME)
    , collectingEvents(false)
    , autoExitCount(-1)
{
    mUi.setupUi(this);
}

void BaseHostDialog::init(bool isRGB, QWidget *parameterHolderWidget, QTextEdit * /*loggerWidget*/)
{
    mIsRGB = isRGB;
    mDockWidget = parameterHolderWidget;
    L_INFO << "ViMouse started" << "\n" << "Log will go here";

    initCommon();
    initParameterWidgets();

    mSaveableCameraWidgets.push_back(&mCapSettings);
    loadParams(ConfigManager::configName(), "");

    initMemoryUsageCalculator();
    initCalculator();

    initGraphPresentation();

    SettingsGetter visitor(ConfigManager::camConfigName(), UI_NAME_BASE_DIALOG);
    mRectifierData.accept<SettingsGetter>(visitor);
//    loadTransformFromFile(savePath);
}

void BaseHostDialog::initGraphPresentation()
{
    mFilterPresentationsCollection = new FilterPresentationsCollection(&mCalculator->mFiltersCollection);
    mFilterGraphPresentation = new FilterGraphPresentation(mFilterPresentationsCollection);
    connect(mBaseControlWidget->ui()->filterGraphButton, SIGNAL(released()), mFilterGraphPresentation, SLOT(show()));

    connect(mFilterGraphPresentation
            , SIGNAL(graphChanged(tinyxml2::XMLDocument*))
            , mCalculator
            , SLOT(graphChanged(tinyxml2::XMLDocument*)));

    cout << "Trying to start loading Graph" << std::endl;
    mFilterGraphPresentation->filterGraph->loadFromFile("graph.conf");
    mFilterGraphPresentation->restoreFromGraph();

    mFilterGraphPresentation->graphChangedSlot();
}

void BaseHostDialog::deinit()
{
    qDebug() << "void BaseHostDialog::deinit(): called";
    /* Stop all possible events sources */
    saveParams(ConfigManager::configName(), "");
    terminateCalculator();

    /* Cleanup the queue */
    cleanupEventQueue();

}

BaseHostDialog::~BaseHostDialog()
{
    foreach (QWidget *widget, mWidgets.values()) {
        delete_safe(widget);
    }

    delete_safe(mCamera);

    delete_safe (mMemoryCalculator);
    delete_safe (mAdditionalTools);

    delete_safe (mParamsMapper);

    delete_safe (mDistortionWidget);
    delete_safe (mFilterGraphPresentation);

    SettingsSetter visitor(ConfigManager::camConfigName(), UI_NAME_BASE_DIALOG);
    mRectifierData.accept<SettingsSetter>(visitor);
}

void BaseHostDialog::initMemoryUsageCalculator()
{
#ifdef Q_OS_WIN
    mMemoryCalculator = new WindowsMemoryUsageCalculator();
#elif MACX
    mMemoryCalculator = new MacMemoryUsageCalculator();
#else
    mMemoryCalculator = new LinuxMemoryUsageCalculator();
#endif
    connect(mMemoryCalculator, SIGNAL(memoryOverflow()), this, SLOT(handleMemoryOverflow()));

    connect(&mMemoryTimer, SIGNAL(timeout()), mMemoryCalculator, SLOT(getMemoryUsage()));
    mMemoryTimer.start(500); // check twice a second
}

void BaseHostDialog::initParameterWidgets()
{
//    qDebug() << "Entered BaseHostDialog::initParameterWidgets()";

    dockWidget()->setLayout(new QVBoxLayout());
//    mFilterSelectorL = new FilterSelector("Left" );
//    mFilterSelectorR = new FilterSelector("Right");


    mBaseControlWidget = new BaseParametersControlWidget(this, true, UI_NAME_BASE);
    dockWidget()->layout()->addWidget(mBaseControlWidget);
//    connect(mBaseControlWidget->ui()->filterButton , SIGNAL(released()), mFilterSelectorL, SLOT(show()));
//    connect(mBaseControlWidget->ui()->filter2Button, SIGNAL(released()), mFilterSelectorR, SLOT(show()));

    mSaveableWidgets.push_back(mBaseControlWidget);

    mPresentationControlWidget = new PresentationParametersControlWidget(this, true, UI_NAME_PRESENTATION);
    FoldableWidget *w1 = new FoldableWidget(this, "presentation", mPresentationControlWidget);
    dockWidget()->layout()->addWidget(w1);
    mSaveableWidgets.push_back(mPresentationControlWidget);

#ifndef WITH_HARDWARE
    disableComboBoxItem(mBaseControlWidget->ui()->interpolationTypeComboBox, InterpolationType::HARDWARE);
#endif
}

void BaseHostDialog::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
     SYNC_PRINT(("BaseHostDialog::baseControlParametersChanged"));
    // check if we should swap frames
    if (params.isNull()) {
        return;
    }

    if (((!mBaseParams.isNull() && params->swapCameras() != mBaseParams->swapCameras()) || mBaseParams.isNull())
    )
    {
        if (mFrames == NULL)
        {           
            mScheduledSwap = true; // should schedule swap if we can't perform it now
        }
        swapCameras(params->swapCameras());
    }

    mBaseParams = params;
    camerasParamsChanged(); // FIXME: should it be here?
}

void BaseHostDialog::presentationControlParametersChanged(QSharedPointer<PresentationParameters> params)
{
    mPresentationParams = params;
}

void BaseHostDialog::doOpenInput()
{
    //QMessageBox::information(this, "We will be opening the input", mInputSelectorDialog.getInputString());

    stopCapture();
    stopCalculation();

    initCapture(mInputSelectorDialog.getInputString());
}

void BaseHostDialog::stopCalculation()
{
    QMetaObject::invokeMethod(mCalculator, "pauseCalculation", Qt::BlockingQueuedConnection);

    mCalculator->setImageCaptureInterface(NULL);
}

void BaseHostDialog::initCommon()
{
    /* Main UI*/
    connect(&mInputSelectorDialog, SIGNAL(newInputString()), this, SLOT(doOpenInput()));

    /* Main output */
    mUi.widget->setSavingRoot("mainoutput");
    mSaveableWidgets.push_back(mUi.widget);

    /* Distortion corrector*/
    mDistortionWidget = new DistortionWidget();
    qRegisterMetaType<QSharedPointer<DisplacementBuffer> >("QSharedPointer<DisplacementBuffer>");
    connect(mDistortionWidget, SIGNAL(recalculationFinished(QSharedPointer<DisplacementBuffer>)), this, SLOT(distortionEstimationFinished()));

    /* Connect video sequence control */
    emit captureStatusUpdated(false);
}

void BaseHostDialog::showDistortionCorrectorDialog()
{
    mDistortionWidget->show();
    mDistortionWidget->raise();
    //TODO::make different corrections for left and right frame
    mDistortionWidget->setBuffer(mFrames->getCurrentFrame(mCorrectionFrame));
}

void BaseHostDialog::showCameraCalculatorDialog()
{
    mCameraCalculatorWidget.show();
    mCameraCalculatorWidget.raise();
}

void BaseHostDialog::showStatistics()
{
    mStatsDialog.show();
    mStatsDialog.raise();
}

void BaseHostDialog::showCaptureSettings()
{
    mCapSettings.show();
    mCapSettings.raise();
}

void BaseHostDialog::showColorHistogram()
{
    mColorHistogram.show();
    mColorHistogram.raise();
}


void BaseHostDialog::showAboutDialog()
{
    mAboutDialog.show();
    mAboutDialog.raise();
}

void BaseHostDialog::showInputSelectorDialog()
{
    mInputSelectorDialog.show();
    mInputSelectorDialog.raise();
}


void BaseHostDialog::distortionEstimationFinished()
{
    if (mFrames->getCurrentFrame(mCorrectionFrame) == NULL)
    {
        return;
    }
    mDistortionTransform = mDistortionWidget->distortionCorrectionTransform();
    camerasParamsChanged();
}

void BaseHostDialog::deinitCamera()
{
    mCapSettings.setCaptureInterface(NULL);
    delete_safe(mCamera);
    mCameraStarted = false;
    emit captureStatusUpdated(false);
}

/* ******************************************************************************
 *  Calculation thread
 */
void BaseHostDialog::initCapture(QString const &init/*, bool isRgb*/)
{
    SYNC_PRINT(("BaseHostDialog::initCapture(%s, rgb=%s): called\n", init.toLatin1().constData(), mIsRGB ? "true" : "false"));
    //TODO:: if mInputString is empty, but app params not empty wizard is shown. Maybe it's bad
    if (!init.isEmpty())
    {
        mInputString = init;
    }
    if (mInputString.isEmpty())
    {
        emit emptyString();
    }
    /* Deleting previous camera*/
    deinitCamera();

    mInputSelectorDialog.setInputString(mInputString);

    mCamera = ImageCaptureInterfaceQtFactory::fabric(mInputString.toStdString(), mIsRGB);

    if (mCamera == NULL)
    {
        cout << "BaseHostDialog::initCapture(): Error initializing capture device." << std::endl;
        return;
    }
    ImageCaptureInterface::CapErrorCode res = mCamera->initCapture();

    if (res == ImageCaptureInterface::FAILURE)
    {
        cout << "BaseHostDialog::initCapture(): Error: none of the capture devices started.\n" << std::endl;
        QMessageBox::warning(this, "Error: none of the capture devices started.","Error: none of the capture devices started.");
        return;
    }
    else if (res == ImageCaptureInterface::SUCCESS_1CAM)
    {
        cout << "BaseHostDialog::initCapture(): Will be using only one capture device.\n" << std::endl;
        mUseOneCaptureDevice = true;
        mPresentationControlWidget->ui()->outputComboBox->setCurrentIndex(0);
    }
    else
    {
        mUseOneCaptureDevice = false;
    }

    /* Now we need to connect the camera to widgets */
    mCapSettings.setCaptureInterface(mCamera);
    mCapSettings.loadFromQSettings(ConfigManager::camConfigName(), "");

    qRegisterMetaType<CaptureStatistics>("CaptureStatistics");
    connect(mCamera, SIGNAL(newStatisticsReady(CaptureStatistics)),
            this, SLOT(setCaptureStats(CaptureStatistics)), Qt::QueuedConnection);
    connect(mCamera, SIGNAL(streamPaused()), this, SIGNAL(singleStepStatusUpdated()));

    emit captureStatusUpdated(true);

 /*   bool isPauseable = mCamera->supportPause();
    mUi.stepModeButton ->setEnabled(isPauseable);
    mUi.nextFrameButton->setEnabled(isPauseable); */

    /*  Now connect mCamera to mCalculator */
    mCalculator->setImageCaptureInterface(mCamera);
    connect(mCamera,     SIGNAL(newFrameReady(frame_data_t)),
            mCalculator, SLOT  (newFrameReady(frame_data_t)), Qt::QueuedConnection);

    camerasParamsChanged();

    if (autoExitCount == -1)
    {
        stopCapture();
    }
}

void BaseHostDialog::createCalculator()
{
    ParametersMapperBase *mapper = new ParametersMapperBase();

    mapper->setBaseParametersControlWidget(mBaseControlWidget);
    mapper->setPresentationParametersControlWidget(mPresentationControlWidget);

    BaseCalculationThread *calculator = new BaseCalculationThread();
//    mFilterGraphPresentation->filterPresentations->fCollection = &calculator->mFiltersCollection;

    mCalculator = calculator;

    connect(mapper  , SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
          , calculator, SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>))
    );

    connect(mFilterGraphPresentation
            , SIGNAL(graphChanged(XMLDocument*))
            , calculator
            , SLOT(graphChanged(XMLDocument*)));

//    connect(mFilterSelectorL
//          , SIGNAL(parametersChanged())
//          , this
//          , SLOT(filterLParamsChanged())
//        );
//    connect(mFilterSelectorR
//          , SIGNAL(parametersChanged())
//          , this
//          , SLOT(filterRParamsChanged())
//        );

    qRegisterMetaType<Frames::FrameSourceId>("Frames::FrameSourceId");
    qRegisterMetaType<QSharedPointer<FilterSelectorParameters> >("QSharedPointer<FilterSelectorParameters>");

    connect(this    , SIGNAL(filterControlParametersChanged(Frames::FrameSourceId, QSharedPointer<FilterSelectorParameters>))
          , calculator, SLOT(filterControlParametersChanged(Frames::FrameSourceId, QSharedPointer<FilterSelectorParameters>))
        );

    connect(mapper
           , SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
           , calculator
           , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;
}

//void BaseHostDialog::filterLParamsChanged()
//{
//    qDebug("BaseHostDialog::filterParamsChanged(): Filter signal reached baseHostDialog");
//    QSharedPointer<FilterSelectorParameters> params(mFilterSelectorL->getParameters());
//    emit filterControlParametersChanged(Frames::LEFT_FRAME, params);
//}

//void BaseHostDialog::filterRParamsChanged()
//{
//    qDebug("BaseHostDialog::filter2ParamsChanged(): Filter signal reached baseHostDialog");
//    QSharedPointer<FilterSelectorParameters> params(mFilterSelectorR->getParameters());
//    emit filterControlParametersChanged(Frames::RIGHT_FRAME, params);
//    if (mBaseControlWidget->ui()->filterLockButton->isChecked())
//    {
//        emit filterControlParametersChanged(Frames::LEFT_FRAME, params);
//    }
//}

void BaseHostDialog::initCalculator()
{
    createCalculator();
    mFrames = mCalculator->getFramesHolder();
  //  swapCameras(mBaseControlWidget->ui()->swapCamerasPushButton->isChecked());
    connectFinishedRecalculation();

    qRegisterMetaType<QSharedPointer<CamerasConfigParameters> >("QSharedPointer<CamerasConfigParameters>");

    connect(this, SIGNAL(camerasParametersChanged(QSharedPointer<CamerasConfigParameters>)),
            static_cast<BaseCalculationThread *>(mCalculator), SLOT(camerasParametersChanged(QSharedPointer<CamerasConfigParameters>)),
            Qt::QueuedConnection);


    //qDebug() << "BaseHostDialog:camerasParametersChanged connected" << endl;

    /* Flow controls */
    emit updatePauseCalculationAction(true);
    connect(this, SIGNAL(pauseCalculator ()), mCalculator, SLOT(pauseCalculation()));
    connect(this, SIGNAL(resumeCalculator()), mCalculator, SLOT(resumeCalculation()));

    connect(mCalculator,
        SIGNAL(calculationStateChanged(AbstractCalculationThread::CalculationState)),
        this, SLOT(calculationStateChanged(AbstractCalculationThread::CalculationState)));


    /** Starting the thread */
    mCalculator->start();

}

void BaseHostDialog::terminateCalculator()
{
    if (mCalculator != NULL)
    {
        mCalculator->quit();
        mCalculator->wait();
        delete_safe (mCalculator);
    }
}

void BaseHostDialog::connectFinishedRecalculation() const
{
    connect(mCalculator, SIGNAL(processingFinished(AbstractOutputData *)),
            this, SLOT(finishedRecalculation(AbstractOutputData *)), Qt::QueuedConnection);
}


/**
 * Add all events to the list, and if we are not still collecting them -
 * process the whole list
 * */
void BaseHostDialog::finishedRecalculation(AbstractOutputData *abstractOutputData)
{
    if (autoExitCount != -1)
    {
        static int stopcount = autoExitCount;
        if (stopcount <= 0)
        {
            printf("Exiting because of auto-exit option\n");
            QApplication::quit();
        }
        stopcount--;
    }

    if (mScheduledSwap && !mBaseParams.isNull())
    {
        swapCameras(mBaseParams->swapCameras());
        mScheduledSwap = false;
    }

    if (abstractOutputData != NULL)
    {
        eventList.push_back(abstractOutputData);
    }

    if (!collectingEvents)
    {
        collectingEvents = true;
        QCoreApplication::processEvents();
        collectingEvents = false;
        processResult();
        eventList.clear();
    }
}

void BaseHostDialog::processResult()
{

    for (unsigned i = 0; i < eventList.size(); i++)
    {
        BaseOutputData *processedData = dynamic_cast<BaseOutputData*>(eventList[i]);

        if (processedData == NULL)
            continue;


        /**
        if (processedData->image() != NULL)
        {
            mImage = QSharedPointer<QImage>(new QImage(processedData->image()->width(), processedData->image()->height(),  QImage::Format_RGB32));
            QPainter painter(mImage.data());
            painter.drawImage(QPoint(0,0), *processedData->image());
        }
        */
        int h = processedData->mMainImage.height();
        int w = processedData->mMainImage.width();

        mImage = QSharedPointer<QImage>(new QImage(w, h,  QImage::Format_RGB32));
        processedData->mMainImage.drawImage(mImage.data());

        delete_safe(processedData);
        eventList[i] = NULL;
    }
    updateWidgets();
}


void BaseHostDialog::cleanupEventQueue( void )
{
    /**
    * Delete unprocessed results. Some can still be on the input queue...
    **/
    collectingEvents = true;
    QCoreApplication::processEvents();

    for (unsigned i = 0; i < eventList.size(); i++)
    {
        delete_safe(eventList[i]);
    }
    eventList.clear();

}


/* ***************************************************************************
 *   Sequence flow control
 **/
void BaseHostDialog::stopCapture()
{
    if (mCamera == NULL) {
        return;
    }
    emit singleStepStatusUpdated();
    mCamera->pauseCapture();
}

void BaseHostDialog::nextFrame()
{
    if (mCamera == NULL) {
        return;
    }
    mCamera->nextFrame();
}

/**
 *  Starts both capture and processing
 *
 **/
void BaseHostDialog::play()
{
    qDebug() << "BaseHostDialog::Play, calc state: " << AbstractCalculationThread::stateNames[mCalculator->getCalculationState()];
    if (!mCameraStarted)
    {
        if (!mCamera)
        {
            return;
        }
        mCamera->startCapture();
        mCamera->nextFrame();  // Get a first frame from file to show something, or do nothing when capturing from cam.
        mCameraStarted = true;
    }

    if (mCalculator->getCalculationState() != AbstractCalculationThread::CALCULATION_AWAITING_DATA)
    {
        QMetaObject::invokeMethod(mCalculator, "resumeCalculation", Qt::BlockingQueuedConnection);
    }

    if (mScheduledSwap && !mBaseParams.isNull())
    {
        swapCameras(mBaseParams->swapCameras());
        mScheduledSwap = false;
    }

}

void BaseHostDialog::calculationStateChanged(AbstractCalculationThread::CalculationState state)
{
    printf("Calculation state changed: %d (%s)\n", state, AbstractCalculationThread::stateNames[state]);
}

void BaseHostDialog::mouseChild(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void BaseHostDialog::camerasParamsChanged()
{
    qDebug("BaseHostDialog::camerasParamsChanged(): called");
    QSharedPointer<CamerasConfigParameters> parametersShPtr = QSharedPointer<CamerasConfigParameters>(getAdditionalParams());
    cout << parametersShPtr->rectifierData().F << std::endl;


    emit camerasParametersChanged(parametersShPtr);
}

CamerasConfigParameters * BaseHostDialog::getAdditionalParams() const
{
    CamerasConfigParameters  *fp = new CamerasConfigParameters ();
    fp->setInputsN(mUseOneCaptureDevice ? CamerasConfigParameters::OneCapDev : CamerasConfigParameters::TwoCapDev);

    fp->setRectifierData(mRectifierData);
    fp->setDistortionTransform(mDistortionTransform);

    return fp;
}

void BaseHostDialog::swapCameras(bool shouldSwap)
{
    if (mFrames == NULL)
    {
        qDebug() << "BaseHostDialog:swapping null frames!";
        return;
    }
    mFrames->swapFrameSources(shouldSwap);
}

void BaseHostDialog::setCaptureStats(CaptureStatistics stats)
{
    mStatsDialog.addCaptureStats(stats);

    foreach (ViAreaWidget *widget, mWidgets)
    {
        StatisticsDialog *dialog = dynamic_cast<StatisticsDialog *>(widget);
        if (dialog != NULL) {
            dialog->addCaptureStats(stats);
        }
    }
}




/* *****************************************************************************
 *  Additional widgets functions
 * */
void BaseHostDialog::updateWidgets()
{
    mUi.widget->setImage(mImage);

    mUi.widget->update();
    foreach (QWidget *widget, mWidgets.values()) {
        widget->update();
    }
}

ViAreaWidget *BaseHostDialog::addAdditionalWindow(QString const &name, ViAreaWidget *area, const QIcon &icon)
{
    if (area == NULL) {
        SYNC_PRINT(("BaseHostDialog::addAdditionalWindow(%s, NULL, _): Called with NULL", name.toLatin1().constData() ));
        return NULL;
    }

    mWidgets[name] = area;
    area->setAccessibleName(name);
    area->setWindowTitle(name);
    area->setWindowIcon(icon);
    area->setGeometry(0, 0, 800, 600);
    area->hide();

    QAction *action = mAdditionalTools->addAction(icon, name);
    connect(action, SIGNAL(triggered()), area, SLOT(show()));
    connect(action, SIGNAL(triggered()), area, SLOT(raise()));

    return area;
}

ViAreaWidget * BaseHostDialog::createAdditionalWindow(QString const &name, WindowType type, const QIcon &icon)
{
    ViAreaWidget *area = NULL;

    switch (type)
    {
        case ownerControlledWindow:
            area = new ViAreaWidget();
            break;
        case imageWindow:
        {
            AdvancedImageWidget *widget = new AdvancedImageWidget();
            widget->setSavingRoot(name);
            mSaveableWidgets.push_back(widget);
            area = widget;
            break;
        }
        case textWindow:
            area = new TextLabelWidget();
            break;
        case oglWindow:
#ifdef WITH_OPENGL
            area = new CloudViewDialog(NULL, "Open GL subwindow");
#endif
            break;
        case graphWindow:
        default:
        {
            GraphPlotDialog *widget = new GraphPlotDialog();
            widget->setSavingRoot(name);
            mSaveableWidgets.push_back(widget);
            area = widget;
        }
        break;
    }

    return addAdditionalWindow(name, area, icon);
}


/* ******************************************************************************
 * Base Dialog parameters loader/saver
 * */

void BaseHostDialog::doLoadParams()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Text (*.txt *.conf *.ini)"
        );

    loadParams(filename, "");
}

void BaseHostDialog::doSaveParams()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Text (*.txt *.conf *.ini)"
        );

    saveParams(filename, "");
}

void BaseHostDialog::loadParams(const QString &fileName, QString root)
{
    QSettings settings(fileName, QSettings::IniFormat);

    QSettings cameraSettings(ConfigManager::camConfigName(), QSettings::IniFormat);

    if (!root.isEmpty())
    {
        settings.beginGroup(root);
    }

    if (mInputString.isEmpty())
    {
        mInputString = cameraSettings.value(initialCameraString, "").toString();
    }

    if (settings.value("DockWidgetHide", false).toBool())
    {
        mDockWidget->hide();
    }

    if (!root.isEmpty())
    {
        settings.endGroup();
    }

    for (unsigned i = 0; i < mSaveableWidgets.size(); i++)
    {
        mSaveableWidgets[i]->loadFromQSettings(fileName, root);
    }

    for (unsigned i = 0; i < mSaveableCameraWidgets.size(); i++)
    {
        mSaveableCameraWidgets[i]->loadFromQSettings(ConfigManager::camConfigName(), root);
    }
}

void BaseHostDialog::saveParams(const QString &fileName, QString root)
{
    qDebug("BaseHostDialog::saveParam(\"%s\", \"%s\"): called",fileName.toLatin1().constData(), root.toLatin1().constData());
    QSettings settings(fileName, QSettings::IniFormat);

    QSettings cameraSettings(ConfigManager::camConfigName(), QSettings::IniFormat);

    if (!root.isEmpty())
    {
        cameraSettings.beginGroup(root);
    }

    cameraSettings.setValue(initialCameraString, mInputString);

    if (!root.isEmpty())
    {
        cameraSettings.endGroup();
    }

    qDebug("BaseHostDialog::saveParam(): We will be saving %lu widgets", mSaveableWidgets.size());
    for (unsigned i = 0; i < mSaveableWidgets.size(); i++)
    {
        mSaveableWidgets[i]->saveToQSettings(fileName, root);
    }

    qDebug("BaseHostDialog::saveParam(): We will be saving %lu camera widgets", mSaveableCameraWidgets.size());
    for (unsigned i = 0; i < mSaveableCameraWidgets.size(); i++)
    {
        mSaveableCameraWidgets[i]->saveToQSettings(ConfigManager::camConfigName(), root);
    }

}


/* ******************************************************************************
 * Rectification parameters loader/saver
 * */

void BaseHostDialog::loadTransformFromFile(QString const &filename)
{
    fstream f;
    f.open((char *)filename.toLatin1().constData(), fstream::in);
    mRectifierData = RectificationResult();
    if (f.is_open())
    {
        PropertyList list;
        list.load(f);
        PropertyListReaderVisitor readerVisitor(&list);
        RectificationResult defaultResult;
        readerVisitor.visit(mRectifierData, defaultResult, "rectification");
        f.close();
    }
    camerasParamsChanged();
}

void BaseHostDialog::saveTransformToFile(QString const &filename)
{
    qDebug() << "saving to" << filename;
    fstream f;
    f.open((char *)filename.toLatin1().constData(), fstream::out);
    PropertyList list;
    PropertyListWriterVisitor writerVisitor(&list);
    RectificationResult defaultResult;
    writerVisitor.visit(mRectifierData, defaultResult, "rectification");
    list.save(f);
    f.close();
}


void BaseHostDialog::doLoadTransform()
{
    /*Get file name*/
    QString filename = QFileDialog::getOpenFileName(
            this,
            "Choose an file name",
            ".",
            "Text (*.txt )"
            );

    loadTransformFromFile(filename);
}

void BaseHostDialog::doSaveTransform()
{
    /*Get file name*/
    QString filename = QFileDialog::getOpenFileName(
            this,
            "Choose an file name",
            ".",
            "Text (*.txt )"
            );

    saveTransformToFile(filename);
}

void BaseHostDialog::toggleAdvanced(bool on)
{
    WidgetQtIterator::advancedVisibility(dockWidget(), !on);
}


/* *
 *   Block of functions that handle memory overuse
 * */

void BaseHostDialog::handleMemoryOverflow()
{
    qDebug() << "Too much memory allocated. Check for memleaks!";
    qApp->exit(1);
}

QList<QMenu *> BaseHostDialog::additionalMenus()
{
    QList<QMenu *> menus;
    menus << mAdditionalTools;
    return menus;
}

QString BaseHostDialog::windowTitle() const
{
    return "Base Dialog";
}

QWidget *BaseHostDialog::dockWidget()
{
    return mDockWidget;
}

QtStatisticsCollector *BaseHostDialog::statisticsCollector()
{
    return &mStatsDialog.statsAccum;
}

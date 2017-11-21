/**
 * \file baseHostDialog.h
 * \see baseHostDialog.cpp
 *
 * \date Aug 27, 2010
 * \author Sergey Levi
 */

#pragma once

#include <deque>
#include <vector>

#include <QtCore/QSharedPointer>
#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>
#include <QDialog>
#include <QTextEdit>
#include <QAction>
#include <QKeyEvent>
#include <QTableView>

#include "ui_hostDialogStub.h"

#include "core/utils/global.h"
#include "core/math/matrix/matrix33.h"
#include "core/utils/propertyList.h"

#include "aboutDialog.h"

#include "capSettingsDialog.h"
#include "baseCalculationThread.h"
#include "statisticsDialog.h"

#include "baseParametersControlWidget.h"
#include "presentationParametersControlWidget.h"
#include "rectifyParametersControlWidget.h"
#include "pointsRectificationWidget.h"
#include "distortioncorrector/distortionWidget.h"
#include "cameraCalculatorWidget.h"
#include "core/alignment/distortionCorrectTransform.h"

#include "generatedParameters/baseParameters.h"
#include "camerasConfigParameters.h"
#include "g12Image.h"
#include "advancedImageWidget.h"
#include "histogramdialog.h"

#include "memoryUsageCalculator.h"
#include "inputSelectorWidget.h"
#include "filters/filterSelector.h"
#include "filterGraphPresentation.h"

#define UI_NAME_BASE           "base"
#define UI_NAME_BASE_DIALOG    "baseDialog"
#define UI_NAME_PRESENTATION   "presentation"

using std::deque;

QString const savePath = "rectification.txt";

/**
 *    BaseHost dialog is a class that is a base for ViTool dialogs
 *    It allows to have all the parameters related to 6D, handles rectification
 *
 *
 * */
class BaseHostDialog: public QDialog
{
Q_OBJECT

public:

    /**
     * Basic initialization. Most of the initialization procedures should be carried out
     * by the child class constructor
     *
     * Constructor can't call virtual methods.
     * Afterwards you will need to call
     *   init()
     *
     *   initCapture(const QString &)
     *
     */
    BaseHostDialog(QWidget *parent = NULL);

    /**
     * This method will do the following and need to be called once!
     * <ol>
     *   <li>Initialize common UI connect buttons etc (can be overloaded initCommon(QString)))
     *   <li>Initialize internal widgets (can be overloaded initParameterWidgets())
     *   <li>Call to initMemoryUsageCalculator
     *   <li>Initialize calculator (can be overloaded initCalculator())
     * </ol>
     *
     *  This dialog creates all internal window structure. Some of the dependant dialogs
     *  should have ability to dock, and are brought outside of BaseHostDialog.
     *  They should be transfered in this constructor
     **/
    void init(bool isRGB, QWidget *parameterHolderWidget, QTextEdit *loggerWidget = NULL);
    void initGraphPresentation();

    /**
     * Call to teardown the object. it stops all
     * child threads and flushes the event queue
     **/
    virtual void deinit();

    /**
     * This function interconnects all the objects realted to capturing
     * and initializes inputs
     *
     **/
    virtual void initCapture(QString const &initString = QString()/*, bool isRgb = false*/);



    virtual QList<QMenu *> additionalMenus();
    virtual QString windowTitle() const;
    virtual ~BaseHostDialog();

    void setAutoExit(int autoexit) {
        this->autoExitCount = autoexit;
    }

signals:
    void camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parametersShPtr);

    void pauseCalculator();
    void resumeCalculator();

    void captureStatusUpdated(bool isOn);
    void singleStepStatusUpdated();
    void updatePauseCalculationAction(bool);
    void emptyString();
    void filterControlParametersChanged(Frames::FrameSourceId id, QSharedPointer<FilterSelectorParameters> params);

public slots:
    void baseControlParametersChanged(QSharedPointer<BaseParameters> params);
    void presentationControlParametersChanged(QSharedPointer<PresentationParameters> params);
    void showStatistics();
    void showCaptureSettings();
    void showColorHistogram();
    void showAboutDialog();
    void showInputSelectorDialog();
    void showDistortionCorrectorDialog();
    void showCameraCalculatorDialog();

    void doOpenInput();
    virtual void doLoadParams();
    virtual void doSaveParams();

    void toggleAdvanced(bool off);

//    void filterRParamsChanged();
//    void filterLParamsChanged();

protected slots:
    /* Process the child repainting */
    void mouseChild(QMouseEvent *event);
    //void setChildImage(AdvancedImageWidget *who);

    virtual void camerasParamsChanged();

    virtual void play();
    virtual void calculationStateChanged(AbstractCalculationThread::CalculationState state);

    virtual void finishedRecalculation(AbstractOutputData *fod);

    virtual void doLoadTransform();
    virtual void doSaveTransform();

    void stopCapture();
    void nextFrame();

    void updateWidgets();

    void setCaptureStats(CaptureStatistics stats);

protected:
    /**
     *   Initialize widgets that should be in BaseHostDialog
     **/
    virtual void initParameterWidgets();

    virtual CamerasConfigParameters * getAdditionalParams() const;

    /**
     * In this function calculator is created and connected to parameter Widgets
     * Normally you should overload it.
     *
     **/
    virtual void createCalculator();
    virtual void connectFinishedRecalculation() const;

    /**
     * Initialization steps that are likely, however not mandatory to be used in specific
     * application dialogs using BaseHostDialog as their parent class.
     */
    virtual void initCommon();

    /**
     * Calculation initialization procedures. This function will call createCalculator.
     * Afterwards it makes flow settings and connections and launches the calculator
     *
     * \note The implementation provided by the base class should normally be used, unless you
     * want to change play/pause etc control flow
     *
     *
     */
    virtual void initCalculator();

    /**
     *  during the process of the destruction you need to make sure that the
     *  thread object is killed before deiniting all the objects that are
     *  referenced be it.
     *
     *  The other possible solution is to use QObject::deleteLater() on the
     *  objects that are referenced by the thread.
     **/
    virtual void terminateCalculator();

    virtual void loadParams(const QString &fileName, QString root);
    virtual void saveParams(const QString &fileName, QString root);

    void loadTransformFromFile(QString const &filename);
    void saveTransformToFile(QString const &filename);

    /*void loadParamsFromFile(QString const &filename, PropertyList &list);
    void saveParamsToFile(QString const &filename, PropertyList &list);*/

    virtual QWidget *dockWidget();

    QtStatisticsCollector *statisticsCollector();

    enum WindowType {
        ownerControlledWindow,
        imageWindow,
        textWindow,
        oglWindow,
        graphWindow
    };

    ViAreaWidget *createAdditionalWindow(QString const &name, WindowType type, const QIcon &icon = QIcon());
    ViAreaWidget *addAdditionalWindow(QString const &name, ViAreaWidget *area, const QIcon &icon);


    QObject *mParamsMapper;
    AbstractCalculationThread *mCalculator;

    RectificationResult mRectifierData;


    QSharedPointer<QImage> mImage;

    Ui::HostDialogStubClass mUi;

    bool mUseOneCaptureDevice;

    //RectifyControlWidget mRectifyGUI;
    DistortionWidget *mDistortionWidget;

    /* */
    CameraCalculatorWidget mCameraCalculatorWidget;

    /*TODO: Move this to ImageCaptureInterface*/
    bool mIsRGB;
    bool mCameraStarted;
    ImageCaptureInterfaceQt *mCamera;

    /*TODO: Rename this */
    CapSettingsDialog mCapSettings;
    HistogramDialog mColorHistogram;

    Frames *mFrames;
    StatisticsDialog mStatsDialog;

    BaseParametersControlWidget  *mBaseControlWidget;
    PresentationParametersControlWidget *mPresentationControlWidget;

    QSharedPointer<BaseParameters> mBaseParams;
    QSharedPointer<PresentationParameters> mPresentationParams;

    FilterGraphPresentation *mFilterGraphPresentation;

    /* Child widgets, usually these class are called by QActions in the menu */
    QHash<QString, ViAreaWidget *> mWidgets;
    QMenu *mAdditionalTools;

    /** Widgets that delegates BaseHostDialog right to save and load them*/
    vector<SaveableWidget *> mSaveableWidgets;
    /** Camera-dependent widgets that delegate BaseHostDialog right to save and load them */
    vector<SaveableWidget *> mSaveableCameraWidgets;

    /**
     *  Timer that will fire when it's time to check the memory consumption
     **/
    QTimer mMemoryTimer;
    /**
     *  Object that checks memory consumption
     **/
    MemoryUsageCalculator *mMemoryCalculator;

    QWidget *mDockWidget;
    QTextEdit *mLoggerWidget;

    /* Shows if we should swap frames the moment we get and start to process them */
    bool mScheduledSwap;

private slots:

    /**
     * Slot for swapping framesources --- right frame becomes left and left frame becomes right.
     * Does not restart flow.
     **/
    void swapCameras(bool shouldSwap = true);

    void handleMemoryOverflow();
    void distortionEstimationFinished();

protected:
    AboutDialog mAboutDialog;
    InputSelectorWidget mInputSelectorDialog;
    QSharedPointer<DisplacementBuffer> mDistortionTransform;
    Frames::FrameSourceId mCorrectionFrame;
    /// Creates mMemoryCalculator and connects mMemoryTimer signal to getMemoryUsage() slot
    void initMemoryUsageCalculator();


protected:

    QString mInputString;

    /**
     * Variables to handle overfill of the draw queue
     * collectingEvents is true when we are adding the result to
     * local queue to process, and false when we actually processing it
     **/
    int collectingEvents;
    /**
     *  Queue that holds draw requests.
     *  Should better use deque here
     **/
    vector<AbstractOutputData *> eventList;
    virtual void processResult();
    void cleanupEventQueue( void );

    void stopCalculation();
    void deinitCamera();

    int autoExitCount;
    FilterPresentationsCollection* mFilterPresentationsCollection;
};

#include "wrappers/jsonmodern/jsonModernReader.h"

#include "calibrationWidget.h"
#include "physicsMainWindow.h"
#include "ui_physicsMainWindow.h"

#include <g12Image.h>
#include <imageCaptureInterfaceQt.h>
#include <sceneShaded.h>

#include <reflection/jsonPrinter.h>

//#define LIBEVENT_WSERVER
#ifdef LIBEVENT_WSERVER
#include "applications/drone/drone-app/server/server.h"
#endif

PhysicsMainWindow::PhysicsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PhysicsMainWindow)
{

    throttleValueFromJS=1500;
    counter=0;

    ui->setupUi(this);
    Log::mLogDrains.add(ui->logWidget);


    ui->comboBox->addItem("Usual mode");
    ui->comboBox->addItem("Inertia mode");
    ui->comboBox->addItem("Casual mode");
    ui->comboBox->addItem("RT/LT Usual mode");
    ui->comboBox->addItem("RT/LT Full mode");
    ui->comboBox->addItem("RT/LT Extream mode");

    /*Camera*/
    mInputSelector.setInputString("v4l2:/dev/video0:1/30:mjpeg:800x600");

    PinholeCameraIntrinsics *intr = new PinholeCameraIntrinsics(Vector2dd(800, 600), degToRad(70.42));
    mCameraModel.intrinsics.reset(intr);
    mCameraModel.nameId = "Copter Main Camera";
    mCameraModel.setLocation(Affine3DQ::Identity());
    /* Connect parameters and send first version */
    connect(&mModelParametersWidget, SIGNAL(loadRequest(QString)), this, SLOT(loadCameraModel(QString)));
    connect(&mModelParametersWidget, SIGNAL(paramsChanged()), this, SLOT(cameraModelWidgetChanged()));
    mModelParametersWidget.setParameters(mCameraModel);


    for (int i=0;i<8;i++)
    {
        failSafeOutput.axis[i] = 1500;
    }
    failSafeOutput.axis[failSafeOutput.CHANNEL_THROTTLE] = 1300;

    connect(&mJoystickSettings, SIGNAL(joystickUpdated(JoystickState)), this, SLOT(joystickUpdated(JoystickState)));

    ui->actionShowLog->toggled(false);

    /* Creating main processing chain */
    mProcessor = new FrameProcessor();
    mProcessor->target = this;

    SYNC_PRINT(("PhysicsMainWindow::startCamera(): connecting parameters\n"));
    qRegisterMetaType<GeneralPatternDetectorParameters>("GeneralPatternDetectorParameters");
    connect(this      , SIGNAL(newPatternDetectionParameters(GeneralPatternDetectorParameters)),
            mProcessor, SLOT  (setPatternDetectorParameters(GeneralPatternDetectorParameters)));

    SYNC_PRINT(("PhysicsMainWindow::startCamera(): connecting camera model\n"));
    qRegisterMetaType<CameraModel>("CameraModel");
    connect(this      , SIGNAL(newCameraModel(CameraModel)),
            mProcessor, SLOT  (setCameraModel(CameraModel)));

    /* Setting put paramteres */

    flightControllerParametersWidget = new ReflectionWidget(FlightControllerParameters::getReflection());
    connect(flightControllerParametersWidget, SIGNAL(paramsChanged()), this, SLOT(flightControllerParametersChanged()));

    connect(&patternDetectorParametersWidget, SIGNAL(paramsChanged()), this, SLOT(patternDetectionParametersChanged()));

    toSave.push_back(flightControllerParametersWidget);
    toSave.push_back(&patternDetectorParametersWidget);
    toSave.push_back(&mInputSelector);
    toSave.push_back(&mModelParametersWidget);

    mInputSelector.loadFromQSettings("drone.ini", "");
    patternDetectorParametersWidget.loadFromQSettings("drone.ini", "");
    mModelParametersWidget.loadFromQSettings("drone.ini", "");


    connect(ui->actionDownloadModels, SIGNAL(triggered()), this , SLOT(downloadModels()));

#ifdef LIBEVENT_WSERVER
    SYNC_PRINT(("Starting web server...\n"));
    startWServer();
    web_server_timer = new QTimer();
    QObject::connect(web_server_timer, SIGNAL(timeout()), this, SLOT(callingWServer()));
    web_server_timer->start(100);
#endif

    /* Load world */
    world.load("models/world.json");
    connect(ui->actionWorldRedraw, SIGNAL(triggered()), this , SLOT(worldRedraw()));

}

PhysicsMainWindow::~PhysicsMainWindow()
{
    SYNC_PRINT(("PhysicsMainWindow::~PhysicsMainWindow(): called\n"));
    for (SaveableWidget *ts: toSave) {
        ts->saveToQSettings("drone.ini", "");
    }

    Log::mLogDrains.detach(ui->logWidget);
    delete ui;
}

void PhysicsMainWindow::showJoystickSettingsWidget()
{
    mJoystickSettings.show();
    mJoystickSettings.raise();
}

void PhysicsMainWindow::showAboutWidget()
{
    mAboutWidget.show();
    mAboutWidget.raise();
}

void PhysicsMainWindow::showFlightControllerParameters()
{
    SYNC_PRINT(("PhysicsMainWindow::showFlightControllerParameters():called\n"));
    if (flightControllerParametersWidget == NULL) {
        return;
    }
    flightControllerParametersWidget->show();
    flightControllerParametersWidget->raise();
}

void PhysicsMainWindow::flightControllerParametersChanged()
{
    SYNC_PRINT(("PhysicsMainWindow::flightControllerParametersChanged():called\n"));
    flightControllerParametersWidget->getParameters(&currentFlightControllerParameters);
    *static_cast<FlightControllerParameters *>(&copter) = currentFlightControllerParameters;

}

void PhysicsMainWindow::showPatternDetectionParameters()
{
    SYNC_PRINT(("PhysicsMainWindow::showPatternDetectionParameters():called\n"));   
    patternDetectorParametersWidget.show();
    patternDetectorParametersWidget.raise();
}

void PhysicsMainWindow::patternDetectionParametersChanged()
{
    SYNC_PRINT(("PhysicsMainWindow::patternDetectionParametersChanged():called\n"));

    GeneralPatternDetectorParameters params = patternDetectorParametersWidget.getParameters();
    SYNC_PRINT(("PhysicsMainWindow::patternDetectionParametersChanged():\n"));
    cout << params << endl;
    emit newPatternDetectionParameters(params);
}

void PhysicsMainWindow::showRadioControlWidget()
{
    radioWidget.show();
    radioWidget.raise();
}


void PhysicsMainWindow::onStartVirtualModeReleased()

//void PhysicsMainWindow::on_pushButton_released()

{
    SYNC_PRINT(("PhysicsMainWindow::onPushButtonReleased(): called\n"));

    /*if (!virtualModeActive & !RealModeActive)
        {
        VirtualSender.Client_connect();
        virtualModeActive = true;
        }
        */
    startVirtualMode();
}

///
/// \brief PhysicsMainWindow::sendJoyValues
///Sends to joystick emulator values of sticks with tcp
void PhysicsMainWindow::sendJoyValues()
{
#if 0
    if (virtualModeActive)
    {
        if (yawValue==0)
        {
            yawValue=1;
        }
        if (rollValue==0)
        {
            rollValue=1;
        }
        if (pitchValue==0)
        {
            pitchValue=1;
        }
        if (throttleValue==0)
        {
            throttleValue=1;
        }

        string ss="";
        std::string s = std::to_string(yawValue);

        ss+=s+" ";
        s = std::to_string(rollValue);
        ss+=s+" ";
        s = std::to_string(pitchValue);
        ss+=s+" ";
        s = std::to_string(throttleValue);
        ss+=s+"*";
        cout<<ss<<endl;
        VirtualSender.clientSend(ss);
    }
    else
    {
        cout<<"Please connect to the py file"<<endl;
    }
#endif
}



void PhysicsMainWindow::showValues()                                   //shows axis values to console
{
    for (int i = 0; i < CopterInputs::CHANNEL_CONTROL_LAST; i++)
    {
        SYNC_PRINT(("%s__%d   ", CopterInputs::getName((CopterInputs::ChannelID)i), copterInputs.axis[i]));
    }
}


void PhysicsMainWindow::startVirtualMode()
{
    QTimer::singleShot(8, this, SLOT(keepAlive()));
}

void PhysicsMainWindow::stopVirtualMode()
{
    bool oldbackend = !(ui->actionNewBackend->isChecked());

    Mesh3DScene *scene  = NULL;

    if (oldbackend)
    {
        scene = new Mesh3DScene;
        Mesh3D *mesh = new Mesh3D();
        mesh->switchColor();
        scene->setMesh(mesh);
    } else {
        if (mShadedScene == NULL) {
            mShadedScene = new SceneShaded;
            ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(mShadedScene), CloudViewDialog::DISP_CONTROL_ZONE);
        }
    }
    ui->cloud->update();
}

void PhysicsMainWindow::worldRedraw()
{
    SYNC_PRINT(("PhysicsMainWindow::worldRedraw(): called\n"));

    /* Temporary world draw */
    if (mShadedScene == NULL)
    {
        mShadedScene = new SceneShaded;
        Mesh3DDecorated *mesh = new Mesh3DDecorated;
        mesh->switchColor();
        mesh->switchTextures();
        for (size_t i = 0; i < world.meshes.size(); i++)
        {
            if (!world.meshes[i].show) {
                continue;
            }
            SYNC_PRINT(("Adding mesh %d\n", (int)i));
            world.meshes[i].mesh.dumpInfo();
            mesh->mulTransform(world.meshes[i].transform);
            mesh->add(world.meshes[i].mesh);
            mesh->popTransform();

            mesh->materials = world.meshes[i].mesh.materials;
        }

        mShadedScene->setMesh(mesh);
        mShadedScene->mParameters.point.type = ShaderPreset::NONE;
        mShadedScene->mParameters.edge.type  = ShaderPreset::NONE;

        mShadedScene->mParameters.face.load("obj_textured.vsh", QDir(":/new/shaders/shaders"));
        mShadedScene->mParameters.face.name = "obj_textured_custom";

        ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(mShadedScene), CloudViewDialog::CLUSTER_SWARM);
    }
}

void PhysicsMainWindow::keepAlive()
{
    if (virtualModeActive)
    {
        QTimer::singleShot(8, this, SLOT(keepAlive()));
    }

}




void PhysicsMainWindow::drawDrone()
{
    bool oldbackend = !(ui->actionNewBackend->isChecked());
    Mesh3DScene *scene  = NULL;

    if (oldbackend)
    {
        scene = new Mesh3DScene;
        Mesh3D *mesh = new Mesh3D();
        mesh->switchColor();
        scene->setMesh(mesh);
    }
    else
    {
        if (mShadedScene == NULL)
        {
            mShadedScene = new SceneShaded;
            ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(mShadedScene), CloudViewDialog::DISP_CONTROL_ZONE);
        }
    }

    if (oldbackend)
    {
        simSim.drone.drawMyself(*scene->owned);
    }
    else
    {
        Mesh3DDecorated *mesh = new Mesh3DDecorated();
        mesh->switchNormals();
        simSim.drone.drawMyself(*mesh);
        //mesh->dumpInfo();
        mShadedScene->setMesh(mesh);
    }

    mGraphDialog.addGraphPoint("X", simSim.drone.getPosCenter().x());
    mGraphDialog.addGraphPoint("Y", simSim.drone.getPosCenter().y());
    mGraphDialog.addGraphPoint("Z", simSim.drone.getPosCenter().z());

    mGraphDialog.update();

    if (oldbackend) {
        ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(scene), CloudViewDialog::CONTROL_ZONE);
    }
    ui->cloud->update();
}

void PhysicsMainWindow::showCameraInput()
{
    SYNC_PRINT(("PhysicsMainWindow::showCameraInput(): called\n"));
    mInputSelector.show();
    mInputSelector.raise();
}

void PhysicsMainWindow::startCamera()                                                 //how can I stop it??
{
    if (cameraActive)
    {
        mProcessor->exit();
    }
    cameraActive = true;
    /* We should prepare calculator in some other place */

    //std::string inputString = inputCameraPath;
    std::string inputString = mInputSelector.getInputString().toStdString();


    ImageCaptureInterfaceQt *rawInput = ImageCaptureInterfaceQtFactory::fabric(inputString, true);
    if (rawInput == NULL)
    {
        L_ERROR_P("Unable to fabricate camera grabber %s\n", inputString.c_str());
        return;
    }

    SYNC_PRINT(("main: initialising capture...\n"));
    ImageCaptureInterface::CapErrorCode returnCode = rawInput->initCapture();
    SYNC_PRINT(("main: initialising capture returing %d\n", returnCode));

    if (returnCode == ImageCaptureInterface::FAILURE)
    {
        SYNC_PRINT(("Can't open\n"));
        return;
    }

    mCameraParametersWidget.setCaptureInterface(rawInput);

    mProcessor->input = rawInput;
    qRegisterMetaType<CaptureStatistics>("CaptureStatistics");
    QObject::connect(
        rawInput    , SIGNAL(newStatisticsReady(CaptureStatistics)),
       &mStatsDialog,   SLOT(addCaptureStats(CaptureStatistics)), Qt::QueuedConnection);

    QObject::connect(
        rawInput  , SIGNAL(newFrameReady(ImageCaptureInterface::FrameMetadata)),
        mProcessor,   SLOT(processFrame(ImageCaptureInterface::FrameMetadata)), Qt::QueuedConnection);

    /* All ready. Let's rock */
    mProcessor->start();
    rawInput->startCapture();

}

void PhysicsMainWindow::pauseCamera()
{

}

void PhysicsMainWindow::stopCamera()
{

}

void PhysicsMainWindow::showCameraParametersWidget()
{
    mCameraParametersWidget.show();
    mCameraParametersWidget.raise();
}

void PhysicsMainWindow::showCameraModelWidget()
{
    mModelParametersWidget.show();
    mModelParametersWidget.raise();
}

void PhysicsMainWindow::loadCameraModel(QString filename)
{
    const char *name = filename.toLatin1().constData();
    SYNC_PRINT(("PhysicsMainWindow::loadCameraModel(<%s>):called\n", name));
    JSONModernReader reader(name);
    if (!reader.hasError())
    {
        CameraModel model;
        reader.visit(model, "camera");
        mModelParametersWidget.setParameters(model);
        cout << "Model:" << model << endl;
    } else {
        SYNC_PRINT(("Can't load <%s>\n", name));
    }
}

void PhysicsMainWindow::saveCameraModel(QString filename)
{
    const char *name = filename.toLatin1().constData();
    SYNC_PRINT(("PhysicsMainWindow::saveCameraModel(<%s>):called\n", name));
    JSONPrinter reader(name);
    CameraModel model;
    mModelParametersWidget.getParameters(model);
    reader.visit(model, "camera");
}

void PhysicsMainWindow::cameraModelWidgetChanged()
{
    SYNC_PRINT(("PhysicsMainWindow::cameraModelWidgetChanged()\n"));
    mModelParametersWidget.getParameters(mCameraModel);
    emit newCameraModel(mCameraModel);
}


void PhysicsMainWindow::showProcessingParametersWidget()
{
    mFlowFabricControlWidget.show();
    mFlowFabricControlWidget.raise();
}

void PhysicsMainWindow::showGraphDialog()
{
    mGraphDialog.show();
    mGraphDialog.raise();
}

void PhysicsMainWindow::showStatistics()
{
    mStatsDialog.show();
    mStatsDialog.raise();
}

void PhysicsMainWindow::startSimuation()
{
}

void PhysicsMainWindow::startRealMode()                                    //starts controlling the copter
{
    if (!virtualModeActive & !realModeActive)
    {
        //ComController.bindToRealDrone();
    }
}

void PhysicsMainWindow::stop()
{
    time_t Stop_time;
    time(&Stop_time);
    double seconds = difftime(Stop_time, startTime);
    printf("The time: %f seconds\n", seconds);
    cout<<counter<<" steeps done "<<endl;
    this->close();
}





void PhysicsMainWindow::startJoyStickMode()
{
    currentSendMode=0;
    QTimer::singleShot(8, this, SLOT(keepAliveJoyStick()));

}

void PhysicsMainWindow::keepAliveJoyStick()
{
#if MERGE
    if (joystick1.active)                             //Yes, indeed. We can not turn on autopiot without a joyStick.
    {
        if (!joystick1.mutexActive)
        {
            joystick1.mutexActive=true;
            joyStickOutput=joystick1.output;
            joystick1.mutexActive=false;
        }
        if (iiAutoPilot.active)
        {
            iiOutput=iiAutoPilot.output();
        }
        if (currentSendMode==0)
        {
            if (!ComController.mutexActive)
            {
                ComController.mutexActive=true;
                ComController.input = joyStickOutput;
                simSim.droneJoystick = joyStickOutput;
                ComController.mutexActive=false;
            }
        }
        if (currentSendMode==1)
        {
            if (!ComController.mutexActive)
            {
                ComController.mutexActive=true;
                ComController.input=iiOutput;
                ComController.mutexActive=false;
            }
        }
    if (frameCounter%4==0)
    {
        frameCounter=0;
        if (currentSendMode==0)
        {
            ui->inputsWidget->updateState(joyStickOutput);
        }
        if (currentSendMode==1)
        {
            ui->inputsWidget->updateState(iiOutput);

        }
    }
    frameCounter++;
    //ui->inputsWidget->updateState(joystick1.output);
    QTimer::singleShot(8, this, SLOT(keepAliveJoyStick()));

    }
#endif
}

void PhysicsMainWindow::downloadModels()
{
    system("wget http://calvrack.ru:3080/exposed-drone/data.tar.gz");
    system("tar -zxvf data.tar.gz");
}

void PhysicsMainWindow::joystickUpdated(JoystickState state)
{
    joystickState = state;
}



void PhysicsMainWindow::mainAction()
{
    //SYNC_PRINT(("Tick\n"));

    bool oldbackend = !(ui->actionNewBackend->isChecked());

    Mesh3DScene *scene  = NULL;

    if (oldbackend)
    {
        scene = new Mesh3DScene;
        Mesh3D *mesh = new Mesh3D();
        mesh->switchColor();
        scene->setMesh(mesh);
    } else {
        if (mShadedScene == NULL) {
            mShadedScene = new SceneShaded;
            ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(mShadedScene), CloudViewDialog::DISP_CONTROL_ZONE);
        }
    }



    //startJoyStickMode();
    /*

    if (mixer.mix(joystickState, inputs)) {
        copter.flightControllerTick(inputs);

        copter.physicsTick();
    }
    */

//    copter.flightControllerTick(joystick.output);
    copter.physicsTick();

    copter.visualTick();

    if (oldbackend) {
        copter.drawMyself(*scene->owned);
    } else {
        Mesh3DDecorated *mesh = new Mesh3DDecorated();
        mesh->switchNormals();
        copter.drawMyself(*mesh);
        //mesh->dumpInfo();
        mShadedScene->setMesh(mesh);
    }

    mGraphDialog.addGraphPoint("X", copter.position.x());
    mGraphDialog.addGraphPoint("Y", copter.position.y());
    mGraphDialog.addGraphPoint("Z", copter.position.z());

    mGraphDialog.update();


    //drone.flightControllerTick(joystick1.output);

    drone.physicsTick(0.1);


    if (oldbackend) {
        drone.drawMyself(*scene->owned);
    } else {
        Mesh3DDecorated *mesh = new Mesh3DDecorated();
        mesh->switchNormals();
        drone.drawMyself(*mesh);
        //mesh->dumpInfo();
        mShadedScene->setMesh(mesh);
    }

    mGraphDialog.addGraphPoint("X", drone.getPosCenter().x());
    mGraphDialog.addGraphPoint("Y", drone.getPosCenter().y());
    mGraphDialog.addGraphPoint("Z", drone.getPosCenter().z());

    mGraphDialog.update();

    if (oldbackend) {
        ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(scene), CloudViewDialog::CONTROL_ZONE);
    }
    ui->cloud->update();
}



void PhysicsMainWindow::onPushButton2Clicked()                      // it was proto^2 autipilot (only repeat)
{
    //       cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<endl;

    setlocale(LC_ALL, "rus");

    char buff[50];
    ifstream fin("cppstudio.txt");
    while (!fin.eof())
    {
        Message m;
        fin >> buff;
        m.pitch=std::stoi( buff);
        fin >> buff;
        m.roll=std::stoi( buff);
        fin >> buff;
        m.throttle=std::stoi( buff);
        fin >> buff;
        m.yaw=std::stoi( buff);
        fin >> buff;
        m.countOfRepeats=std::stoi( buff);
        cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.countOfRepeats<<endl;
        messages.push_front(m);
    }
    for (Message mm :messages)
    {
        autopilotStack.push(mm);
    }
    Message m;
    m=autopilotStack.top();
    cout<<"-_-_-"<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.countOfRepeats<<"-_-_-"<<endl;
    autopilotStack.pop();

    autopilotMode=true;

}

int PhysicsMainWindow::sign(int val)
{
    int result=0;
    if (val > 100) {result=1;}
    if (val < -100){result=-1;}
    return result;
}

void PhysicsMainWindow::updateUi()
{
    uiMutex.lock();
    /* We now could quickly scan for data and stats*/
    for (DrawRequestData *data : uiQueue)
    {
        mStatsDialog.addStats(data->stats);
    }

    /* But we would only draw last data */
    DrawRequestData *work = uiQueue.back();
    uiQueue.pop_back();
    /* Scan again cleaning the queue */
    for (DrawRequestData *data : uiQueue)
    {
        delete_safe(data);
    }
    uiQueue.clear();
    uiMutex.unlock();
    /**/
    if (work->mMesh != NULL) {
        Mesh3DScene* scene = new Mesh3DScene;
        Mesh3D *mesh = new Mesh3D();
        scene->setMesh(mesh);

        mesh->switchColor();
        mesh->add(*work->mMesh, true);
        ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(scene), CloudViewDialog::ADDITIONAL_SCENE);
    }
    if (work->mImage)
    {
        QSharedPointer<QImage> image(new RGB24Image(work->mImage));
        if (iiAutoPilot.active)
        {
            iiAutoPilot.makeStrategy(image);
            //cout<<"image changed"<<endl;
            image = iiAutoPilot.outputImage;
        }
        else
        {
            //cout<<"AutoPilot turned off"<<endl;
        }
        ui->imageView->setImage(image);
    }
    /* We made copies. Originals could be deleted */
    delete_safe(work);
}


void PhysicsMainWindow::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    inputCameraPath="v4l2:/dev/"+arg1.toStdString()+":mjpeg";
}

void PhysicsMainWindow::on_connetToVirtualButton_released()
{
    L_INFO << "PhysicsMainWindow::onPushButtonReleased(): called;";
    /*if (!virtualModeActive & !RealModeActive)
        {
        VirtualSender.Client_connect();
        virtualModeActive = true;
        }
        */
    virtualModeActive = !virtualModeActive;
    L_INFO << virtualModeActive;
    simSim.setIsAlive(virtualModeActive);
    if (virtualModeActive)
    {
        startVirtualMode();
    }
    else
    {
        stopVirtualMode();
    }
}


void PhysicsMainWindow::on_toolButton_3_released()
{
    startRealMode();
}

void PhysicsMainWindow::on_toolButton_2_released()
{

    if (!iiAutoPilot.active)
    {
        /*What is that??? */
        //iiAutoPilot = ProtoAutoPilot();
        iiAutoPilot.start();
    }
}

void PhysicsMainWindow::on_pushButton_released()
{
    iiAutoPilot.testImageVoid();
}

void PhysicsMainWindow::calibrateCamera()
{
    calibrationWidget.show();
    calibrationWidget.raise();
}


void PhysicsMainWindow::repositionCloudCamera()
{
    SYNC_PRINT(("PhysicsMainWindow::repositionCloudCamera(): called\n"));
    CameraModel model;
    mModelParametersWidget.getParameters(model); /* We get it from the UI just to be able to edit it. */
    cout << "Model to be set:" << endl;
    cout << model << endl;
    ui->cloud->setCamera(model);
}

void PhysicsMainWindow::callingWServer()
{
#ifdef LIBEVENT_WSERVER
    server->process_requests();
#else
    emit web_server_tick();
#endif
}


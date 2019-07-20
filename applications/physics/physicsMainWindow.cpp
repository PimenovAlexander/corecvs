#include "calibrationWidget.h"
#include "physicsMainWindow.h"
#include "ui_physicsMainWindow.h"

#include <g12Image.h>
#include <imageCaptureInterfaceQt.h>
#include <sceneShaded.h>


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


    on_updateCameraButton_clicked();
    //frameValuesUpdate();

    PinholeCameraIntrinsics *intr = new PinholeCameraIntrinsics(640, 480, degToRad(60));
    mCameraModel.intrinsics.reset(intr);
    mCameraModel.nameId = "Copter Main Camera";
    mModelParametersWidget.setParameters(mCameraModel);

    /* Move this to other thread completly */
    /**
    copterTimer.setInterval(30);
    copterTimer.setSingleShot(false);
    connect(&copterTimer, SIGNAL(timeout()), this, SLOT(mainAction()));
    copterTimer.start();
    **/
    connect(&mJoystickSettings, SIGNAL(joystickUpdated(JoystickState)), this, SLOT(joystickUpdated(JoystickState)));

    ui->actionShowLog->toggled(false);
}

PhysicsMainWindow::~PhysicsMainWindow()
{
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

#if 0
void PhysicsMainWindow::yawChange(int i)                                //i - current yaw value
{
    yawValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}


void PhysicsMainWindow::rollChange(int i)                                //i - current roll value
{
    rollValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWindow::pitchChange(int i)                                //i - current pitch value
{
    pitchValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWindow::throttleChange(int i)                              //i - current throttle value
{
    throttleValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }

}

void PhysicsMainWindow::CH5Change(int i)                              //i - current throttle value
{
    CH5Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWindow::CH6Change(int i)                              //i - current throttle value
{
    CH6Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWindow::CH7Change(int i)                              //i - current throttle value
{
    CH7Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWindow::CH8Change(int i)                              //i - current throttle value
{
    CH8Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}
#endif

void PhysicsMainWindow::startVirtualMode()
{
    /*
        SYNC_PRINT(("PhysicsMainWindow::startVirtualMode(): Adding new object to scene\n"));
        Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

        Mesh3DDecorated *mesh = new Mesh3DDecorated;

        mesh->switchColor();

        mesh->mulTransform(copterPos);

        mesh->setColor(RGBColor::Red());
        for (size_t i = 0; i < simSim.mainObjects.size(); ++i)
        {
            PhysMainObject &mainObj = simSim.mainObjects[i];
            for (size_t j = 0; j < mainObj.objects.size(); ++j)
            {
                mainObj.objects[j]->addToMesh(*mesh);
            }
        }

        Mesh3DScene *scene = new Mesh3DScene;
        scene->setMesh(mesh);

        ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(scene));
        ui->cloud->update();

        simSim.start();
        */


    simSim.startRealTimeSimulation();


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

void PhysicsMainWindow::keepAlive()
{
    if (virtualModeActive)
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

        if (oldbackend) {
            simSim.drone.drawMyself(*scene->owned);
        } else {
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

        QTimer::singleShot(8, this, SLOT(keepAlive()));
    }
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
    mProcessor = new FrameProcessor();
    mProcessor->target = this;

    std::string inputString = inputCameraPath;

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
    QObject::connect(
        rawInput  , SIGNAL(newFrameReady(ImageCaptureInterface::FrameMetadata)),
        mProcessor,   SLOT(processFrame (ImageCaptureInterface::FrameMetadata)), Qt::QueuedConnection);

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

void PhysicsMainWindow::startSimuation()
{
    //mJoystickSettings.openJoystick();    //counterrevolution (when i will understand how it works, i will swich joystickInput to it
    //joystick1.start();
    if (joystick1.active)
    {
        currentSendMode=0;
    }
}

void PhysicsMainWindow::frameValuesUpdate()
{
  /*  std::thread thr([this]()
    {
        while(true)
        {
            if (currentMode==1)
            {
#if 0
                throttleValue+=sign(throttleValueFromJS-1500);
                if (throttleValue>1800){throttleValue=1799;}
                if (throttleValue<900){throttleValue=901;}
#endif
            }

#if 0
            ui->Yaw->setValue(yawValue);
            ui->Throttle->setValue(throttleValue);

            ui->label->setText("Yaw-"+QString::number(yawValue));
            ui->label_4->setText("throttle-"+QString::number(throttleValue));

            ui->Pitch->setValue(pitchValue);
            ui->Roll->setValue(rollValue);

            ui->label_2->setText("Roll-"+QString::number(rollValue));
            ui->label_3->setText("pitch-"+QString::number(pitchValue));

            ui->CH5->setValue(CH5Value);
            ui->CH6->setValue(CH6Value);
            ui->CH7->setValue(CH7Value);
            ui->CH8->setValue(CH8Value);

            ui->CH5_label->setText("CH5-"+QString::number(CH5Value));
            ui->CH6_label->setText("CH6-"+QString::number(CH6Value));
            ui->CH7_label->setText("CH7-"+QString::number(CH7Value));
            ui->CH8_label->setText("CH8-"+QString::number(CH8Value));
#endif


            usleep(30000);

        }

    });
    thr.detach();
*/
}

void PhysicsMainWindow::startRealMode()                                    //starts controlling the copter
{
    if (!virtualModeActive & !realModeActive)
    {
        ComController.bindToRealDrone();
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





///
/// \brief PhysicsMainWindow::SendOurValues
/// \param OurValues
/// Sends our values to module (yes, it wants its own package for every byte)
void PhysicsMainWindow::sendOurValues(std::vector<uint8_t> OurValues)
{
    std::vector<uint8_t>  flyCommandOutput = OurValues;
    for (size_t i = 0; i < flyCommandOutput.size(); i++)
    {
        std::vector<uint8_t>  flyCom = { flyCommandOutput[i] };
        serialPort.write((const char *)flyCom.data(), flyCom.size());
        serialPort.flush();
    }
}

void PhysicsMainWindow::startJoyStickMode()
{
    joystick1.start();
    currentSendMode=0;
    QTimer::singleShot(8, this, SLOT(keepAliveJoyStick()));

}

void PhysicsMainWindow::keepAliveJoyStick()
{
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
/**
    copter.flightControllerTick(joystick1.output);
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
**/

    drone.flightControllerTick(joystick1.output);
    drone.physicsTick();

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

void PhysicsMainWindow::on_comboBox_currentTextChanged(const QString &arg1)                 //DO NOT TOUCH IT PLEASE
{
     if(arg1=="Usual mode")    //why qstring can not be in case?!
    {
        joystick1.setUsualCurrentMode();   //I dont want errors between qstring and string
    }
    if(arg1=="Inertia mode")
    {
        joystick1.setInertiaCurrentMode();
    }
    if(arg1=="Casual mode")
    {
        joystick1.setCasualCurrentMode();
    }
    if(arg1=="RT/LT Usual mode")
    {
        joystick1.setRTLTUsialMode();
    }
    if(arg1=="RT/LT Full mode")
    {
        joystick1.setRTLTFullMode();
    }
    if(arg1=="RT/LT Extream mode")
    {
        joystick1.setRTLTExtreamMode();
    }
 }

void PhysicsMainWindow::updateUi()
{
    uiMutex.lock();
    /* We now could quickly scan for data and stats*/
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
            cout<<"AutoPilot turned off"<<endl;
        }
        ui->imageView->setImage(image);
    }
    /* We made copies. Originals could be deleted */
    delete_safe(work);
}


void PhysicsMainWindow::on_updateCameraButton_clicked()
{

    QDir DevDir=*new QDir("/dev","video*",QDir::Name,QDir::System);
    ui->comboBox_2->clear();
    ui->comboBox_2->addItems(DevDir.entryList());

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

void PhysicsMainWindow::on_JoyButton_released()
{
    startJoyStickMode();
}

void PhysicsMainWindow::on_toolButton_3_released()
{
    startRealMode();
}

void PhysicsMainWindow::on_toolButton_2_released()
{

    if (!iiAutoPilot.active)
    {
        iiAutoPilot = ProtoAutoPilot();
        iiAutoPilot.start();
    }
}

void PhysicsMainWindow::on_pushButton_released()
{
    iiAutoPilot.testImageVoid();
}
void PhysicsMainWindow::on_connetToVirtualButton_pressed()
{

}

void PhysicsMainWindow::CalibrateCamera()
{
    calibrationWidget.show();
    calibrationWidget.raise();
}

void PhysicsMainWindow::checkForJoystick()               //auto connect
{
//   jReader->start();
}

void PhysicsMainWindow::LoadCalibrationSettings()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Open Calibration File"),"",tr("Calibration Parametrs (*.yml);; All Files (*)"));
    if (dir!=NULL)
    {
        std::cout<<dir.toStdString()<<std::endl;
        cv::FileStorage fs(dir.toStdString(),cv::FileStorage::READ);
        cv::Mat intrinsic, distCoeffs;
        fs["intrinsic"]>>intrinsic;
        fs["distCoeffs"]>>distCoeffs;
        calib.setIntrinsicMat(intrinsic);
        calib.setDistCoeffsMat(distCoeffs);
    }
}

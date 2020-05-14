#include <QApplication>
#include <QMainWindow>
#include <QVulkanInstance>
#include <QLoggingCategory>
#include <QtCore/QObject>

#include <QLCDNumber>
#include <QPushButton>
#include <QFileDialog>

#include <QVulkanWindow>
#include <QGridLayout>
#include <QWidget>

#include <vector>
#include <ios>
#include <functional>

// for .obj models loading
#include <world/simulationWorld.h>

#include "utils/vkrenderer/VkIbWindow.h"
#include "utils/vkrenderer/IbEngine.h"

// This is an example for a window that contains
// widget that is used for rendering in using Vulkan.
class TargetWindow : public QMainWindow {
public:
    // This function is the only necessary part for VkIbWindow.
    void init(vulkanwindow::VkIbWindow *w)
    {
        QWidget *wrapper = QWidget::createWindowContainer(w);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(wrapper, 0, 0, 7, 2);

        QWidget *widget = new QWidget();
        widget->setLayout(layout);
        setCentralWidget(widget);
    }
};

// "IbEngine::IUpdateable" is optional, in this example it is used
// for rotating light source, drawing lines in each frame and input for camera.
class ExampleApp : public TargetWindow, public vulkanwindow::IbEngine::IUpdateable {
public:
    void setEngine(vulkanwindow::IbEngine *engine) {
        this->engine = engine;

        // set the instance of current class to update light per-frame
        engine->setUpdateable(this);

        engine->setPerspectiveCamera(fov, camPosition, camRotation);
    }

    // vulkanwindow::IbEngine::IUpdateable
    void onUpdate() {
        lightRotation = Quaternion::Rotation({0,1,0}, 1.0f / 120.0f) ^ lightRotation;
        engine->setLightRotation(lightRotation);
        // draw light's axis
        const Vector3dd &dir = lightRotation.toMatrix() * Vector3dd(0,0,1);
        const Vector3dd &right = lightRotation.toMatrix() * Vector3dd(-1,0,0);
        const Vector3dd &up = lightRotation.toMatrix() * Vector3dd(0,1,0);
        engine->drawLine3d({0,0,0}, {(float)dir.x(),(float)dir.y(),(float)dir.z()} , RGBColor(255,255,0));
        engine->drawLine3d({0,0,0}, {(float)right.x(),(float)right.y(),(float)right.z()} , RGBColor(255,0,0));
        engine->drawLine3d({0,0,0}, {(float)up.x(),(float)up.y(),(float)up.z()} , RGBColor(0,255,0));
        // draw center dot
        engine->drawScreenPoint2d({0,0}, RGBColor(255,255,255), 3);
    }

    // QMainWindow
    void keyPressEvent(QKeyEvent *event) override {
        if (engine == nullptr) {
            return;
        }

        const float camSpeed = 15.0f / 60.0f;
        const float camAngleSpeed = 2.0f / 60.0f;

        if (event->key() == Qt::Key_W)
            camPosition += (camRotation.toMatrix() * Vector3dd(0,0,1)) * camSpeed;
        if (event->key() == Qt::Key_S)
            camPosition -= (camRotation.toMatrix() * Vector3dd(0,0,1)) * camSpeed;
        if (event->key() == Qt::Key_A)
            camPosition += (camRotation.toMatrix() * Vector3dd(1,0,0)) * camSpeed;
        if (event->key() == Qt::Key_D)
            camPosition -= (camRotation.toMatrix() * Vector3dd(1,0,0)) * camSpeed;
        if (event->key() == Qt::Key_E)
            camPosition.y() += camSpeed;
        if (event->key() == Qt::Key_Q)
            camPosition.y() -= camSpeed;
        if (event->key() == Qt::Key_Left)
            camRotation = Quaternion::Rotation({0,1,0}, camAngleSpeed) ^ camRotation;
        if (event->key() == Qt::Key_Right)
            camRotation = Quaternion::Rotation({0,1,0}, -camAngleSpeed) ^ camRotation;
        if (event->key() == Qt::Key_Up)
            camRotation = Quaternion::Rotation(camRotation.toMatrix() * Vector3dd(1,0,0), camAngleSpeed) ^ camRotation;
        if (event->key() == Qt::Key_Down)
            camRotation = Quaternion::Rotation(camRotation.toMatrix() * Vector3dd(1,0,0), -camAngleSpeed) ^ camRotation;

        // update camera rotation in engine
        engine->setPerspectiveCamera(fov, camPosition, camRotation);
    }

private:
    vulkanwindow::IbEngine *engine = nullptr;

    // camera properties
    float fov = 3.14f / 2;
    Vector3d<float> camPosition = {0, 1, -2};
    Quaternion camRotation = {0, -0.008333, 0, 0.999965};
    // light properties
    Quaternion lightRotation = Quaternion::Rotation({1,0,0}, 3.14f / 180.0f * 50.0f);
};

int main(int argc, char *argv[])
{
    QApplication qtApp(argc, argv);

    // enable logging
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    // create main window, which holds vulkan surface, buttons, etc
    ExampleApp mainTestWindow;

    // init vulkan widget
    vulkanwindow::VkIbWindow vulkanWindow;
    vulkanWindow.resize(100, 100);

    // vulkan widget is created, assign it to the main window
    mainTestWindow.init(&vulkanWindow);
    mainTestWindow.resize(1024, 768);
    mainTestWindow.show();

    // main window is now shown, init VkIbWindow
    vulkanWindow.initSurface();

        // load models from .json; assets can be downloaded through:
        //   "wget http://calvrack.ru:3080/exposed-drone/data.tar.gz"
        //   "tar -zxvf data.tar.gz"
        SimulationWorld world;
        world.load("models/world.json");

        // init corecvs' mesh class
        Mesh3DDecorated *mesh = new Mesh3DDecorated;
        mesh->switchColor();
        mesh->switchTextures();
        mesh->switchNormals();
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


    // create IbEngine that manages meshes, textures, shaders, ..;
    // it also implements IVkIbApp interface which is used
    // for frame drawing and releasing resources associated with render device
    vulkanwindow::IbEngine engine(&vulkanWindow);
    engine.setMesh3dDecorated(mesh);

    // set this application to window
    vulkanWindow.setVkIbApp(&engine);

    // for input / light rotation
    mainTestWindow.setEngine(&engine);

    return qtApp.exec();
}

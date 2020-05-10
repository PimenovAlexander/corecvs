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
class TargetWindow : public QMainWindow
{
public:
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

int main(int argc, char *argv[])
{
    QApplication qtApp(argc, argv);

    // enable logging
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    // create main window, which holds
    // vulkan surface, buttons, etc
    TargetWindow mainTestWindow;

    // init vulkan widget
    vulkanwindow::VkIbWindow vulkanWindow;
    vulkanWindow.resize(100, 100);

    // vulkan widget is created,
    // assign it to the main window
    mainTestWindow.init(&vulkanWindow);
    mainTestWindow.resize(1024, 768);
    mainTestWindow.show();

    // main window is now shown, init VkIbWindow
    vulkanWindow.initSurface();

    // create application that manages meshes, textures, shaders, ..;
    // it also implements IVkIbApp interface which is used
    // for frame drawing and releasing resources associated with render device
    vulkanwindow::IbEngine app(&vulkanWindow);

    app.setPerspectiveCamera(3.14f / 2, -12, 3, 12, 0, 1, 0, -3.14f / 6);

    // load models from .json; assets can be downloaded through:
    //   "wget http://calvrack.ru:3080/exposed-drone/data.tar.gz"
    //   "tar -zxvf data.tar.gz"
    SimulationWorld world;
    world.load("models/world.json");

    // init corecvs' mesh class
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

    app.setMesh3dDecorated(mesh);

    // set this application to window
    vulkanWindow.setVkIbApp(&app);

    return qtApp.exec();
}

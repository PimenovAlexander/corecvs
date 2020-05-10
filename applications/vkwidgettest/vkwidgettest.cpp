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

    app.setPerspectiveCamera(3.0f, 0, 0, 0, 0, 1, 0, 0);

    // set this application to window
    vulkanWindow.setVkIbApp(&app);

    return qtApp.exec();
}















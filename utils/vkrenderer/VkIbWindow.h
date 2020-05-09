#ifndef VKIBWINDOW_H
#define VKIBWINDOW_H

// Qt
#include <QWindow>
#include <QVulkanInstance>
#include <QLoggingCategory>
#include <qevent.h>

// std
#include <functional>

// ignimbrite
#include <IRenderDevice.h>

namespace vulkanwindow {

class IVkIbApp {
public:
    virtual void onUpdate() = 0;
    virtual void onDestroy() = 0;
};

class VkIbWindow : public QWindow {
public:
    // Create render device
    VkIbWindow();
    // Destroy device, surface and release app's resources (if exists)
    ~VkIbWindow();

    // Init surface for rendering using Vulkan.
    // Note: must be called after showing main window
    void initSurface();

    // Set instance of a class that implements "IVkIbApp" interface.
    // This instance will be used for calling "onUpdate" in each frame.
    // Note: app's "onDestroy" is called in the destructor of this class.
    // Note: if app is nullptr, then functions from IVkIbApp will not be used.
    void setVkIbApp(IVkIbApp *app);

    ignimbrite::ID<ignimbrite::IRenderDevice::Surface> getSurfaceId();
    ignimbrite::IRenderDevice *getRenderDevice();

private:
    // Create render device
    void initDevice();

    // Destroy device, surface and release app's resources (if exists)
    void destroyAll();
    void destroyDevice();
    void destroySurfaceResources();

    // Try to call "onUpdate" in the app (if exists).
    // This function is called on "QEvent::UpdateRequest"
    void updateApp();

    void exposeEvent(QExposeEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    bool event(QEvent *e) override;

private:
    bool isSurfaceExist;
    ignimbrite::IRenderDevice *device;
    ignimbrite::ID<ignimbrite::IRenderDevice::Surface> surfaceId;

    QVulkanInstance *qvkInstance;

    IVkIbApp *vkIbApp;
};
}

#endif // VKIBWINDOW_H

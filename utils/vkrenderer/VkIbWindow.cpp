#include "VkIbWindow.h"
#include <VulkanExtensions.h>
#include <QProcessEnvironment>

using namespace ignimbrite;
using namespace vulkanwindow;

VkIbWindow::VkIbWindow() : vkIbApp(nullptr), isSurfaceExist(false)  {
    setSurfaceType(QSurface::SurfaceType::VulkanSurface);
    initDevice();
}

VkIbWindow::~VkIbWindow() {
    destroyAll();
}

ID<IRenderDevice::Surface> VkIbWindow::getSurfaceId() {
    return surfaceId;
}

std::shared_ptr<ignimbrite::IRenderDevice> VkIbWindow::getRenderDevice() {
    return device;
}

void VkIbWindow::initDevice() {
    const char *ext[2];
    ext[0] = "VK_KHR_surface";

#if defined(_WIN32) || defined(Q_WS_WIN)
    ext[1] = "VK_KHR_win32_surface";
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        ext[1] = "VK_MVK_ios_surface";
    #elif TARGET_OS_MAC
        ext[1] = "VK_MVK_macos_surface";
    #else
        ASSERT("Unknown Apple platform.");
    #endif
#else
    #if Q_OS_ANDROID
        ext[1] = "VK_KHR_android_surface";
    #else
        auto env = QProcessEnvironment::systemEnvironment();

        QString value = env.value(QLatin1String("WAYLAND_DISPLAY"));
        if (!value.isEmpty()) {
            ext[1] = "VK_KHR_wayland_surface";
        } else {
            value = env.value(QLatin1String("DISPLAY"));
            if (!value.isEmpty()) {
                ext[1] = "VK_KHR_xcb_surface";
            } else {
                Q_ASSERT_X(false, "VulkanRenderDevice initialization", "Unknown windowing system");
            }
        }
    #endif
#endif

    device = std::make_shared<VulkanRenderDevice>(2, ext);

    // allocate as QWindow stores only a pointer
    qvkInstance = new QVulkanInstance();
    qvkInstance->setLayers(QByteArrayList()
                   << "VK_LAYER_GOOGLE_threading"
                   << "VK_LAYER_LUNARG_parameter_validation"
                   << "VK_LAYER_LUNARG_object_tracker"
                   << "VK_LAYER_LUNARG_core_validation"
                   << "VK_LAYER_LUNARG_image"
                   << "VK_LAYER_LUNARG_swapchain"
                   << "VK_LAYER_GOOGLE_unique_objects");

    // set to QVulkanInstance VkInstance from created render device
    VulkanExtensions::setVulkanInstance(*((VulkanRenderDevice*)device.get()), qvkInstance);

    if (!qvkInstance->create()) {
        qFatal("Failed to create Vulkan instance: %d", qvkInstance->errorCode());
    }

    // set QVulkanInstance for this window
    setVulkanInstance(qvkInstance);
}

void VkIbWindow::destroyAll() {
    // firstly, delete surface resources
    destroySurfaceResources();

    // release app resources to delete device
    if (vkIbApp != nullptr) {
        vkIbApp->onDestroy();
    }

    // all associated resources are deleted,
    // device can be deleted
    destroyDevice();
}

void VkIbWindow::initSurface() {
    if (isSurfaceExist) {
        return;
    }

    if (device) {
        // to get surface window must be shown
        surfaceId = VulkanExtensions::createSurfaceQtWidget(*((VulkanRenderDevice*)device.get()), this);
        isSurfaceExist = true;

    } else {
        qFatal("Can't create surface: initDevice() must be called before initSurface()");
    }
}

void VkIbWindow::setVkIbApp(IVkIbApp *app) {
    vkIbApp = app;
}

void VkIbWindow::destroyDevice() {
    device.reset();

    // QVulkanInstance doesn't own VkInstance
    delete qvkInstance;

    qvkInstance = nullptr;
}

void VkIbWindow::destroySurfaceResources() {
    if (!isSurfaceExist) {
        return;
    }

    // destroy ignimbrite surface, but not VkSurfaceKHR
    ignimbrite::VulkanExtensions::destroySurface(*((VulkanRenderDevice*)device.get()), surfaceId, false);

    isSurfaceExist = false;
}

void VkIbWindow::updateApp() {
    if (vkIbApp != nullptr) {
        vkIbApp->onUpdate();
    }
}

void VkIbWindow::exposeEvent(QExposeEvent *) {
    // ensure that window is initilized
    if (isExposed()) {
        if (!device) {
            initDevice();
        }

        if (!isSurfaceExist) {
            qDebug("Exposed but not initialized. Initializing VkIBWindow");
            initSurface();
        }

        requestUpdate();

    } else {
        if (isSurfaceExist) {
            qDebug("Unexposed VkIBWindow");
            //destroySurfaceResources();
        }
    }
}

void VkIbWindow::resizeEvent(QResizeEvent *e) {
    // do nothing, as swapBuffers will resize automatically
}

bool VkIbWindow::event(QEvent *e) {

    switch (e->type()) {

    case QEvent::UpdateRequest:
        updateApp();
        requestUpdate();
        break;

    case QEvent::PlatformSurface:
        if (static_cast<QPlatformSurfaceEvent *>(e)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
            destroySurfaceResources();
        break;

    default:
        break;
    }

    return QWindow::event(e);
}

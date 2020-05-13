#ifndef IBENGINE_H
#define IBENGINE_H

// corecvs
#include "core/cameracalibration/cameraModel.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "core/geometry/mesh/meshCache.h"

// vkrenderer
#include <IRenderEngine.h>
#include "VkIbWindow.h"

namespace vulkanwindow {

class IbEngine : public IVkIbApp {
public:
    // Special interface for per-frame updates.
    // It can be used for implementation of per-frame logic.
    class IUpdateable {
    public:
        // This function will be called in each frame
        virtual void onUpdate() = 0;
    };

public:
    explicit IbEngine(VkIbWindow *vkIbWindow);

    // Set main camera's properties
    void setOrthoCamera(
            float orthoWidth,
            float posx, float posy, float posz,
            float axisx, float axisy, float axisz, float angleRad);
    void setPerspectiveCamera(
            float fovYRad,
            float posx, float posy, float posz,
            float axisx, float axisy, float axisz, float angleRad);

    void setLightRotation(
            float axisx, float axisy, float axisz, float angleRad);

    // Set Mesh3DDecorated for rendering
    void setMesh3dDecorated(
            const Mesh3DDecorated *meshDecorated,
            const SceneShadedOpenGLCache *cache = nullptr);
            //const ShadedSceneControlParameters *params = nullptr);

    // Set IUpdateable
    void setUpdateable(IUpdateable *updateable);

    void drawScreenPoint2d(
            float x0, float y0,
            float colorR, float colorG, float colorB, float size = 1);
    void drawScreenPoint3d(
            float x0, float y0, float z0,
            float colorR, float colorG, float colorB, float size = 1);
    void drawScreenLine2d(
            float x0, float y0, float x1, float y1,
            float colorR, float colorG, float colorB);
    void drawLine3d(
            float x0, float y0, float z0,
            float x1, float y1, float z1,
            float colorR, float colorG, float colorB);

public:
    // the next functions uses corecvs' classes

    void setCamera(const CameraModel *cameraModel = nullptr);

    void setOrthoCamera(
            float orthoWidth,
            const corecvs::Vector3df &position,
            const corecvs::Vector3df &axis, float angleRad);
    void setOrthoCamera(
            float orthoWidth,
            const corecvs::Vector3df &position,
            const corecvs::Quaternion &rotation);
    void setPerspectiveCamera(
            float fovYRad,
            const corecvs::Vector3df &position,
            const corecvs::Vector3df &axis, float angleRad);
    void setPerspectiveCamera(
            float fovYRad,
            const corecvs::Vector3df &position,
            const corecvs::Quaternion &rotation);
    void setLightRotation(
            const corecvs::Vector3df &axis, float angleRad);
    void setLightRotation(
            const corecvs::Quaternion &rotation);

    void drawScreenPoint2d(
            const corecvs::Vector2df &p,
            const corecvs::RGBColor &rgb8, float size = 1);
    void drawScreenPoint3d(
            const corecvs::Vector3df &p,
            const corecvs::RGBColor &rgb8, float size = 1);
    void drawScreenLine2d(
            const corecvs::Vector2df &a, const corecvs::Vector2df &b,
            const corecvs::RGBColor &rgb8);
    void drawLine3d(
            const corecvs::Vector3df &a, const corecvs::Vector3df &b,
            const corecvs::RGBColor &rgb8);

public:
    void onUpdate() override;
    void onDestroy() override;

private:
    void loadShaders();

private:
    // structs to store points and edges from Mesh3d
    struct M3DDPoint {
        ignimbrite::Vec3f position;
        ignimbrite::Vec4f color;
        float size;
    };
    struct M3DDEdge {
        ignimbrite::Vec3f start;
        ignimbrite::Vec3f end;
        ignimbrite::Vec4f color;
        float width;
    };

private:
    std::shared_ptr<ignimbrite::IRenderDevice> device;
    std::shared_ptr<ignimbrite::IRenderEngine> engine;

    // camera to render
    bool cameraIsActive;
    std::shared_ptr<ignimbrite::Camera> mainCamera;

    // light source
    const int ShadowMapSize = 2048;
    std::shared_ptr<ignimbrite::Light> mainLight;

    IUpdateable *updateable = nullptr;

    // meshes
    // NOTE: only one mesh currently available
    std::vector<std::shared_ptr<ignimbrite::IRenderable>> renderables;

    // points and lines
    std::vector<M3DDPoint> points;
    std::vector<M3DDEdge> lines;

    // default materials
    std::shared_ptr<ignimbrite::Material> whiteMaterial;
    std::shared_ptr<ignimbrite::Material> shadowMaterial;
    // default sampler for textures
    std::shared_ptr<ignimbrite::Sampler> defaultSampler;

    // shader paths
    const char *ShaderPath_MeshTextured_Vert = "shaders/spirv/shadowmapping/MeshTexturedShadowed.vert.spv";
    const char *ShaderPath_MeshTextured_Frag = "shaders/spirv/shadowmapping/MeshTexturedShadowed.frag.spv";
    const char *ShaderPath_Shadows_Vert = "shaders/spirv/shadowmapping/Shadows.vert.spv";
    const char *ShaderPath_Shadows_Frag = "shaders/spirv/shadowmapping/Shadows.frag.spv";
};

}

#endif // IBENGINE_H

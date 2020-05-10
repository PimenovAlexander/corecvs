#include "IbEngine.h"

#include <RenderEngine.h>
#include <Shader.h>
#include <MaterialFullscreen.h>
#include <FileUtils.h>

#include "MeshDecoratedRenderable.h"

// position, normal, uv
#define VERTEX_STRIDE_SIZE (sizeof(float) * (3 + 3 + 2))

using namespace ignimbrite;

vulkanwindow::IbEngine::IbEngine(vulkanwindow::VkIbWindow *vkIbWindow) {
    engine = std::make_shared<RenderEngine>();

    device = std::shared_ptr<IRenderDevice>(vkIbWindow->getRenderDevice());
    auto surfaceId = vkIbWindow->getSurfaceId();

    engine->setRenderDevice(device);
    engine->setTargetSurface(surfaceId);
    engine->setRenderArea(0, 0, vkIbWindow->width(), vkIbWindow->height());

    // Vulkan has inverted y axis and half of z axis
    glm::mat4x4 vkClipMatrix = {1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, -1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.5f, 0.0f,
                        0.0f, 0.0f, 0.5f, 1.0f};

    // init light
    mainLight = std::make_shared<Light>();
    mainLight->setType(Light::Type::Directional);
    mainLight->setCastShadow(true);
    mainLight->rotate({1,0,0}, M_PI / 4.0f);
    mainLight->setClipMatrix(vkClipMatrix);

    engine->addLightSource(mainLight);

    // init camera
    mainCamera = std::make_shared<Camera>();
    // set aspect of the window
    mainCamera->setAspect((float)vkIbWindow->width() / vkIbWindow->height());
    mainCamera->setClipMatrix(vkClipMatrix);
    mainCamera->setNearView(0.1f);
    mainCamera->setFarView(400.0f);
    engine->setCamera(mainCamera);
    cameraIsActive = false;

    auto presentationPass = MaterialFullscreen::fullscreenQuad("shaders/", surfaceId, device);
    engine->setPresentationPass(presentationPass);

    // init shadows
    auto shadowTarget = std::make_shared<RenderTarget>(device);
    shadowTarget->createTargetFromFormat(ShadowMapSize, ShadowMapSize, RenderTarget::DefaultFormat::DepthStencil);

    auto sampler = std::make_shared<Sampler>(device);
    sampler->setHighQualityFiltering(SamplerRepeatMode::ClampToBorder);

    shadowTarget->getDepthStencilAttachment()->setSampler(sampler);
    engine->setShadowTarget(mainLight, shadowTarget);

    loadShaders();
}

void vulkanwindow::IbEngine::loadShaders() {
    // Main shader
    std::vector<uint8> vertSpv, fragSpv;
    FileUtils::loadBinary(ShaderPath_MeshTextured_Vert, vertSpv);
    FileUtils::loadBinary(ShaderPath_MeshTextured_Frag, fragSpv);

    RefCounted<Shader> shader = std::make_shared<Shader>(device);
    shader->fromSources(ShaderLanguage::SPIRV, vertSpv, fragSpv);
    shader->reflectData();
    shader->generateUniformLayout();

    // Shadow shader
    std::vector<uint8> shVertSpv, shFragSpv;
    FileUtils::loadBinary(ShaderPath_Shadows_Vert, shVertSpv);
    FileUtils::loadBinary(ShaderPath_Shadows_Frag, shFragSpv);

    RefCounted<Shader> shadowShader = std::make_shared<Shader>(device);
    shadowShader->fromSources(ShaderLanguage::SPIRV, shVertSpv, shFragSpv);
    shadowShader->reflectData();
    shadowShader->generateUniformLayout();

    // Pipeline
    IRenderDevice::VertexBufferLayoutDesc vertexBufferLayoutDesc = {};
    vertexBufferLayoutDesc.usage = VertexUsage::PerVertex;
    // Position, normal, texture coords
    vertexBufferLayoutDesc.stride = VERTEX_STRIDE_SIZE;
    vertexBufferLayoutDesc.attributes.resize(3);
    auto& attr = vertexBufferLayoutDesc.attributes;
    attr[0].format = DataFormat::R32G32B32_SFLOAT;
    attr[0].offset = 0;
    attr[0].location = 0;
    attr[1].format = DataFormat::R32G32B32_SFLOAT;
    attr[1].offset = sizeof(float) * 3;
    attr[1].location = 1;
    attr[2].format = DataFormat::R32G32_SFLOAT;
    attr[2].offset = sizeof(float) * 3 + sizeof(float) * 3;
    attr[2].location = 2;

    RefCounted<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>(device);
    pipeline->setTargetFormat(engine->getOffscreenTargetFormat());
    pipeline->setShader(shader);
    pipeline->setVertexBuffersCount(1);
    pipeline->setVertexBufferDesc(0, vertexBufferLayoutDesc);
    pipeline->setDepthTestEnable(true);
    pipeline->setDepthWriteEnable(true);
    //pipeline->setPolygonMode(PolygonMode::Line);
    pipeline->createPipeline();

    // Sampler
    auto sampler = std::make_shared<Sampler>(device);
    sampler->setHighQualityFiltering();

    const uint8 blackPixel[] = { 0, 0, 0, 0 };
    const uint8 whitePixel[] = { 255, 255, 255, 255 };

    // Texture to use if there is no shadow map
    auto defaultShadowTexture = std::make_shared<Texture>(device);
    defaultShadowTexture->setDataAsRGBA8(1, 1, blackPixel, true);
    defaultShadowTexture->setSampler(sampler);

    auto defaultAlbedoTexture = std::make_shared<Texture>(device);
    defaultAlbedoTexture->setDataAsRGBA8(1, 1, whitePixel, true);
    defaultAlbedoTexture->setSampler(sampler);

    whiteMaterial = std::make_shared<Material>(device);
    whiteMaterial->setGraphicsPipeline(pipeline);
    whiteMaterial->createMaterial();
    whiteMaterial->setTexture2D("texShadowMap", defaultShadowTexture);
    whiteMaterial->setTexture2D("texAlbedo", defaultAlbedoTexture);
    whiteMaterial->updateUniformData();

    IRenderDevice::VertexBufferLayoutDesc vertShadowLayoutDesc = {};
    vertShadowLayoutDesc.stride = vertexBufferLayoutDesc.stride;
    vertShadowLayoutDesc.usage = VertexUsage::PerVertex;
    // use only 3 first floats as position
    vertShadowLayoutDesc.attributes.push_back({0, 0, DataFormat::R32G32B32_SFLOAT});

    RefCounted<GraphicsPipeline> shadowsPipeline = std::make_shared<GraphicsPipeline>(device);
    shadowsPipeline->setTargetFormat(engine->getShadowTargetFormat());
    shadowsPipeline->setShader(shadowShader);
    shadowsPipeline->setPolygonCullMode(PolygonCullMode::Back);
    shadowsPipeline->setDepthTestEnable(true);
    shadowsPipeline->setDepthWriteEnable(true);
    shadowsPipeline->setDepthCompareOp(CompareOperation::LessOrEqual);
    shadowsPipeline->setVertexBuffersCount(1);
    shadowsPipeline->setVertexBufferDesc(0, vertShadowLayoutDesc);
    shadowsPipeline->createPipeline();

    shadowMaterial = std::make_shared<Material>(device);
    shadowMaterial->setGraphicsPipeline(shadowsPipeline);
    shadowMaterial->createMaterial();
}

void vulkanwindow::IbEngine::setCamera(const CameraModel *cameraModel) {
    if (cameraModel == nullptr)
    {
        cameraIsActive = false;
        return;
    }

    auto pos = cameraModel->extrinsics.position;
    auto axis = cameraModel->extrinsics.orientation.getAxis();
    float32 angle = cameraModel->extrinsics.orientation.getAngle();

    switch (cameraModel->intrinsics->projection) {
    case ProjectionType::PINHOLE: {
        float fov = static_cast<PinholeCameraIntrinsics*>(cameraModel->intrinsics.get())->getVFov();
        setPerspectiveCamera(fov, pos.x(), pos.y(), pos.z(), axis.x(), axis.y(), axis.z(), angle);
        break;
    }
    case ProjectionType::ORTHOGRAPHIC: {
        float orthoWidth = cameraModel->intrinsics->w();
        setOrthoCamera(orthoWidth, pos.x(), pos.y(), pos.z(), axis.x(), axis.y(), axis.z(), angle);
        break;
    }
    default: {
        cameraIsActive = false;
        qDebug("Can't set camera to render engine as CameraModel's projection "
               "is not perspective nor orthographic.\n");
        return;
    }
    }
}

void vulkanwindow::IbEngine::setOrthoCamera(float orthoWidth, float posx, float posy, float posz, float axisx, float axisy, float axisz, float angleRad)
{
    mainCamera->setType(Camera::Type::Orthographic);
    mainCamera->setOrthoWidht(orthoWidth);
    mainCamera->setPosition({posx,posy,posz});
    mainCamera->setRotation({axisx,axisy,axisz}, angleRad);
    mainCamera->recalculate();
    cameraIsActive = true;
}

void vulkanwindow::IbEngine::setPerspectiveCamera(float fovYRad, float posx, float posy, float posz, float axisx, float axisy, float axisz, float angleRad)
{
    mainCamera->setType(Camera::Type::Perspective);
    mainCamera->setFov(fovYRad);
    mainCamera->setPosition({posx,posy,posz});
    mainCamera->setRotation({axisx,axisy,axisz}, angleRad);
    mainCamera->recalculate();
    cameraIsActive = true;
}

void vulkanwindow::IbEngine::setMesh3dDecorated(
        const Mesh3DDecorated *meshDecorated,
        const SceneShadedOpenGLCache *cache) {
        //const ShadedSceneControlParameters *params) {

    points.clear();
    lines.clear();

    bool renderPoints = false;
    bool renderEdges = false;

    // TODO: cache, params

    // extract points lines to render them separately

    if (renderPoints) {
        for (int i = 0; i < meshDecorated->vertexes.size(); i++) {
            const auto &p = meshDecorated->vertexes[i];

            Vec4f color = { 1.0f, 1.0f, 1.0f, 1.0f };

            if (meshDecorated->hasColor && meshDecorated->vertexesColor.size() > i)  {
                const auto &c = meshDecorated->vertexesColor[i];
                color = { c.r(), c.g(), c.b(), c.a() };
            }

            points.push_back({
                                 { p.x(),p.y(),p.z() },
                                 color,
                                 1.0f
                             });
        }
    }

    if (renderEdges) {
        for (int i = 0; i < meshDecorated->edges.size(); i++) {
            const auto &edge = meshDecorated->edges[i];

            const auto &a = meshDecorated->vertexes[edge.x()];
            const auto &b = meshDecorated->vertexes[edge.y()];

            Vec4f color = { 1.0f, 1.0f, 1.0f, 1.0f };

            if (meshDecorated->hasColor && meshDecorated->edgesColor.size() > i) {
                const auto &c = meshDecorated->edgesColor[i];
                color = { c.r(), c.g(), c.b(), c.a() };
            }

            lines.push_back({
                                { a.x(), a.y(), a.z() },
                                { b.x(), b.y(), b.z() },
                                color,
                                1.0f
                            });
        }

        // TODO: face edges
    }

    // TODO: multiple meshes
    renderables.clear();

    // create new renderable to draw faces of the mesh
    std::shared_ptr<MeshDecoratedRenderable> newMesh = std::make_shared<MeshDecoratedRenderable>(device, VERTEX_STRIDE_SIZE);
    newMesh->setRenderMaterial(whiteMaterial);
    newMesh->setShadowRenderMaterial(shadowMaterial);
    newMesh->setCanApplyCulling(false);
    newMesh->setMaxViewDistance(1000.0f);
    newMesh->setMesh(meshDecorated, 0);

    renderables.push_back(newMesh);

    engine->addRenderable(newMesh);
}

void vulkanwindow::IbEngine::drawScreenPoint2d(float x0, float y0, float colorR, float colorG, float colorB, float size) {
    if (mainCamera && cameraIsActive) {
        engine->addScreenPoint2d({x0,y0},{colorR,colorG,colorB,1.0f},size);
    } else {
        qDebug("Point2d will not be drawn as no camera is set.\n");
    }
}

void vulkanwindow::IbEngine::drawScreenPoint3d(float x0, float y0, float z0, float colorR, float colorG, float colorB, float size) {
    if (mainCamera && cameraIsActive) {
        engine->addPoint3d({x0,y0,z0},{colorR,colorG,colorB,1.0f},size);
    } else {
        qDebug("Point3d will not be drawn as no camera is set.\n");
    }
}

void vulkanwindow::IbEngine::drawScreenLine2d(float x0, float y0, float x1, float y1, float colorR, float colorG, float colorB) {
    if (mainCamera && cameraIsActive) {
        engine->addScreenLine2d({x0,y0},{x1,y1},{colorR,colorG,colorB,1.0f},1);
    } else {
        qDebug("Line2d will not be drawn as no camera is set.\n");
    }
}

void vulkanwindow::IbEngine::drawLine3d(float x0, float y0, float z0, float x1, float y1, float z1, float colorR, float colorG, float colorB) {
    if (mainCamera && cameraIsActive) {
        engine->addLine3d({x0,y0,z0},{x1,y1,z1},{colorR,colorG,colorB,1.0f},1);
    } else {
        qDebug("Line3d will not be drawn as no camera is set.\n");
    }
}

void vulkanwindow::IbEngine::onUpdate() {
    if (mainCamera && cameraIsActive) {

        for (const auto &p : points) {
            engine->addPoint3d(p.position, p.color, p.size);
        }

        for (const auto &e : lines) {
            engine->addLine3d(e.start, e.end, e.color, e.width);
        }

        engine->addLine3d({0,0,0}, mainLight->getDirection(), {1,1,0,1}, 1);
        engine->addLine3d({0,0,0}, mainLight->getRight(), {0,1,0,1}, 1);
        engine->addLine3d({0,0,0}, mainLight->getUp(), {1,0,0,1}, 1);

        mainLight->rotate({0,1,0}, 1.0f/120.0f);

        engine->draw();
    }
}

void vulkanwindow::IbEngine::onDestroy() {

}

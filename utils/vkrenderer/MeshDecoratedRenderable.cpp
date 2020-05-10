#include "MeshDecoratedRenderable.h"

namespace vulkanwindow {

using namespace ignimbrite;

MeshDecoratedRenderable::MeshDecoratedRenderable(std::shared_ptr<IRenderDevice> device, uint32_t vertexStride)
{
    this->device = device;
    this->vertexStride = vertexStride;

    setCastShadows(true);
    setVisible(true);
    setLayerID((uint32)IRenderable::DefaultLayers::Solid);
}

MeshDecoratedRenderable::~MeshDecoratedRenderable()
{
    releaseBuffers();
}

void MeshDecoratedRenderable::setRenderMaterial(std::shared_ptr<Material> material)
{
    this->renderMaterial = material;
}

void MeshDecoratedRenderable::setShadowRenderMaterial(std::shared_ptr<Material> material)
{
    this->shadowMaterial = material;
}

void MeshDecoratedRenderable::setRotation(const Vec3f &axis, float angle)
{
    this->rotation = glm::rotate(angle, axis);
}

void MeshDecoratedRenderable::rotate(const Vec3f &axis, float angle)
{
    this->rotation = glm::rotate(angle, axis) * this->rotation;
}

void MeshDecoratedRenderable::setPosition(const Vec3f &position)
{
    this->position = position;
}

void MeshDecoratedRenderable::translate(const Vec3f &offset)
{
    this->position += offset;
}

void MeshDecoratedRenderable::setScale(const Vec3f &scale)
{
    this->scale = scale;
}

void MeshDecoratedRenderable::setMesh(const corecvs::Mesh3DDecorated *meshDecorated, uint32_t materialId)
{
    if (!device)
    {
        throw std::runtime_error("No device was set in MeshDecoratedRenderable");
    }

    this->meshDecorated = meshDecorated;
    this->meshMaterialId = materialId;
}


void MeshDecoratedRenderable::releaseBuffers()
{
    if (vertexBuffer.isNotNull())
    {
        device->destroyVertexBuffer(vertexBuffer);
        vertexBuffer = ID<IRenderDevice::VertexBuffer>();
    }
    if (indexBuffer.isNotNull())
    {
        device->destroyIndexBuffer(indexBuffer);
        indexBuffer = ID<IRenderDevice::IndexBuffer>();
    }
}

void MeshDecoratedRenderable::createBuffers(int vertCount, int indexCount)
{
    vertData.resize(vertCount);
    indexData.resize(indexCount);

    uint32 vbSize = vertexStride * vertCount;
    vertexBuffer = device->createVertexBuffer(BufferUsage::Static, vbSize, nullptr);

    uint32 ibSize = sizeof(uint32) * indexCount;
    indexBuffer = device->createIndexBuffer(BufferUsage::Static, ibSize, nullptr);
}

void MeshDecoratedRenderable::updateBuffers()
{
    if (meshDecorated == nullptr)
    {
        currentIndexCount = 0;
        return;
    }

    // TODO: material -- textures

    int faceCount = 0;

    for (size_t i = 0; i < meshDecorated->faces.size(); i++)
    {
        int matId = meshDecorated->texId[i][corecvs::Mesh3DDecorated::MATERIAL_NUM];
        if (matId == meshMaterialId)
        {
            faceCount++;
        }
    }

    // if buffers weren't created or their size is too small
    if ((vertexBuffer.isNull() && indexBuffer.isNull()) || faceCount * 3 > indexData.size() || faceCount * 3 > vertData.size())
    {
        createBuffers(faceCount * 3, faceCount * 3);
    }

    int i = 0;

    for (size_t faceId = 0; faceId < meshDecorated->faces.size(); faceId++)
    {
        int matId = meshDecorated->texId[faceId][corecvs::Mesh3DDecorated::MATERIAL_NUM];
        if (matId != meshMaterialId)
        {
            continue;
        }

        const auto &face = meshDecorated->faces[faceId];
        const auto &vs = meshDecorated->vertexes;

        vertData[i * 3 + 0].Position = {
            vs[face.x()].x(),
            vs[face.x()].y(),
            vs[face.x()].z(),
        };
        vertData[i * 3 + 1].Position = {
            vs[face.y()].x(),
            vs[face.y()].y(),
            vs[face.y()].z(),
        };
        vertData[i * 3 + 2].Position = {
            vs[face.z()].x(),
            vs[face.z()].y(),
            vs[face.z()].z(),
        };

        if (meshDecorated->hasNormals)
        {
            const auto &normalId = meshDecorated->normalId[faceId];
            const auto &ns = meshDecorated->normalCoords;

            vertData[i * 3 + 0].Normal = {
                ns[normalId.x()].x(),
                ns[normalId.x()].y(),
                ns[normalId.x()].z(),
            };
            vertData[i * 3 + 1].Normal = {
                ns[normalId.y()].x(),
                ns[normalId.y()].y(),
                ns[normalId.y()].z(),
            };
            vertData[i * 3 + 2].Normal = {
                ns[normalId.z()].x(),
                ns[normalId.z()].y(),
                ns[normalId.z()].z(),
            };
        }
        else
        {
            vertData[i * 3 + 0].Normal = {0,1,0};
            vertData[i * 3 + 1].Normal = {0,1,0};
            vertData[i * 3 + 2].Normal = {0,1,0};
        }

        if (meshDecorated->hasTexCoords)
        {
            const auto &texId = meshDecorated->texId[faceId];
            const auto &ts = meshDecorated->textureCoords;

            vertData[i * 3 + 0].TexCoords = {
                ts[texId.x()].x(),
                ts[texId.x()].y(),
            };
            vertData[i * 3 + 1].TexCoords = {
                ts[texId.y()].x(),
                ts[texId.y()].y(),
            };
            vertData[i * 3 + 2].TexCoords = {
                ts[texId.z()].x(),
                ts[texId.z()].y(),
            };
        }
        else
        {
            vertData[i * 3 + 0].TexCoords = {0,0};
            vertData[i * 3 + 1].TexCoords = {0,0};
            vertData[i * 3 + 2].TexCoords = {0,0};
        }

        indexData[i * 3 + 0] = i * 3 + 0;
        indexData[i * 3 + 1] = i * 3 + 1;
        indexData[i * 3 + 2] = i * 3 + 2;

        // TODO: colors

        i++;
    }

    currentIndexCount = indexData.size();
}

void MeshDecoratedRenderable::onAddToScene(const IRenderContext &context)
{
    // Do nothing
}

void MeshDecoratedRenderable::onRenderQueueEntered(float32 distFromViewPoint)
{
    // Possibly select LOD, but this mesh has only LOD 0
    // Do nothing
}

void MeshDecoratedRenderable::onRender(const IRenderContext &context)
{
    // if doesn't cast shadows, then it wasn't updated in onShadowRender function
    if (!castShadows())
    {
        updateBuffers();
    }

    if (currentIndexCount == 0)
    {
        return;
    }

    auto device = context.getRenderDevice();
    auto camera = context.getCamera();
    auto light = context.getGlobalLight();

    auto model = glm::translate(position) * rotation * glm::scale(scale);

    auto camViewProj = camera->getViewProjClipMatrix();

    if (light != nullptr) {
        auto lightViewProj = light->getViewProjClipMatrix();

        renderMaterial->setMat4("CommonParams.lightSpace", lightViewProj);
        renderMaterial->setVec3("CommonParams.lightDir", light->getDirection());
        renderMaterial->setTexture2D("texShadowMap", context.getShadowMap());
    }

    renderMaterial->setMat4("CommonParams.viewProj", camViewProj);
    renderMaterial->setMat4("CommonParams.model", model);
    renderMaterial->setVec3("CommonParams.cameraPos", camera->getPosition());

    renderMaterial->updateUniformData();
    renderMaterial->bindGraphicsPipeline();
    renderMaterial->bindUniformData();

    device->drawListBindVertexBuffer(vertexBuffer, 0, 0);
    device->drawListBindIndexBuffer(indexBuffer, IndicesType::Uint32, 0);
    device->drawListDrawIndexed(currentIndexCount, 1);
}

void MeshDecoratedRenderable::onShadowRenderQueueEntered(float32 distFromViewPoint)
{
    // Possibly select LOD, but this mesh has only LOD 0
    // Do nothing
}

void MeshDecoratedRenderable::onShadowRender(const IRenderContext &context)
{
    updateBuffers();

    if (currentIndexCount == 0)
    {
        return;
    }

    auto device = context.getRenderDevice();
    auto light = context.getGlobalLight();

    auto model = glm::translate(position) * rotation * glm::scale(scale);
    auto lightMVP = light->getViewProjClipMatrix() * model;

    // todo: another bindings
    shadowMaterial->setMat4("ShadowParams.depthMVP", lightMVP);

    shadowMaterial->updateUniformData();
    shadowMaterial->bindGraphicsPipeline();
    shadowMaterial->bindUniformData();

    device->drawListBindVertexBuffer(vertexBuffer, 0, 0);
    device->drawListBindIndexBuffer(indexBuffer, IndicesType::Uint32, 0);
    device->drawListDrawIndexed(currentIndexCount, 1);
}

Vec3f MeshDecoratedRenderable::getWorldPosition() const
{
    return position;
}

AABB MeshDecoratedRenderable::getWorldBoundingBox() const
{
    return AABB({0,0,0}, 1000000.0f);
}

void MeshDecoratedRenderable::updateAABB()
{
    // TODO
}

Material *MeshDecoratedRenderable::getRenderMaterial()
{
    return renderMaterial.get();
}

Material *MeshDecoratedRenderable::getShadowRenderMaterial()
{
    return shadowMaterial.get();
}

}

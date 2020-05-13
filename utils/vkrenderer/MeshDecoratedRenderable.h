#ifndef MESHDECORATEDRENDERABLE_H
#define MESHDECORATEDRENDERABLE_H

#include <IRenderable.h>
#include "core/geometry/mesh/mesh3DDecorated.h"

namespace vulkanwindow {

class MeshDecoratedRenderable : public ignimbrite::IRenderable
{
    public:
        MeshDecoratedRenderable(std::shared_ptr<ignimbrite::IRenderDevice> device, uint32_t vertexStride);
        ~MeshDecoratedRenderable() override;

        void setDefaultSampler(std::shared_ptr<ignimbrite::Sampler> sampler);
        void setRenderMaterial(std::shared_ptr<ignimbrite::Material> material);
        void setShadowRenderMaterial(std::shared_ptr<ignimbrite::Material> material);

        void setRotation(const ignimbrite::Vec3f &axis, float angle);
        void rotate(const ignimbrite::Vec3f &axis, float angle);
        void setPosition(const ignimbrite::Vec3f &position);
        void translate(const ignimbrite::Vec3f &offset);
        void setScale(const ignimbrite::Vec3f &scale);

        void setMesh(const corecvs::Mesh3DDecorated *meshDecorated, uint32_t materialId = -1);

        // IRenderable

        void onAddToScene(const ignimbrite::IRenderContext &context) override;
        void onRenderQueueEntered(ignimbrite::float32 distFromViewPoint) override;
        void onRender(const ignimbrite::IRenderContext &context) override;
        void onShadowRenderQueueEntered(ignimbrite::float32 distFromViewPoint) override;
        void onShadowRender(const ignimbrite::IRenderContext &context) override;

        ignimbrite::Vec3f getWorldPosition() const override;
        ignimbrite::AABB getWorldBoundingBox() const override;
        ignimbrite::Material *getRenderMaterial() override;
        ignimbrite::Material *getShadowRenderMaterial() override;

private:
        // Mesh3dDecorated is updated frequently, so
        // refill all buffers every frame
        void updateBuffers();
        void updateAABB();
        void releaseBuffers();
        void createBuffers(int vertCount, int indexCount);

        // Load textures from OBJ material from Mesh3dDecorated
        void uploadTextures();

private:
        struct M3DDVertex
        {
            ignimbrite::Vec3f Position;
            ignimbrite::Vec3f Normal;
            ignimbrite::Vec2f TexCoords;
        };

private:
        std::shared_ptr<ignimbrite::IRenderDevice> device;

        std::shared_ptr<ignimbrite::Sampler> defaultSampler;
        std::shared_ptr<ignimbrite::Material> renderMaterial;
        std::shared_ptr<ignimbrite::Material> shadowMaterial;

        std::shared_ptr<ignimbrite::Texture> diffuseTexture;

        ignimbrite::AABB  aabb;
        ignimbrite::Mat4f rotation  = ignimbrite::Mat4f(1.0f);
        ignimbrite::Vec3f position  = ignimbrite::Vec3f(0.0f, 0.0f, 0.0f);
        ignimbrite::Vec3f scale     = ignimbrite::Vec3f(1.0f, 1.0f, 1.0f);

        uint32_t vertexStride = 0;
        uint32_t currentIndexCount = 0;

        std::vector<M3DDVertex> vertData;
        std::vector<uint32_t> indexData;

        ignimbrite::ID<ignimbrite::IRenderDevice::VertexBuffer> vertexBuffer;
        ignimbrite::ID<ignimbrite::IRenderDevice::IndexBuffer>  indexBuffer;

        // mesh to render
        const corecvs::Mesh3DDecorated *meshDecorated;
        uint32_t meshMaterialId;
};

}

#endif // MESHDECORATEDRENDERABLE_H

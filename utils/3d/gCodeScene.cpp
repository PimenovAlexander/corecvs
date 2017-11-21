#include "gCodeScene.h"
#include "core/fileformats/gcodeLoader.h"


GCodeScene::GCodeScene() :
    owned(NULL)
{
    mMesh = new Mesh3DDecorated;
}

void GCodeScene::drawMyself(CloudViewDialog *dialog)
{
    SceneShaded::drawMyself(dialog);
}

bool GCodeScene::dump(const QString &/*targetFile*/)
{
    return false;
}

void GCodeScene::setParameters(void *params)
{
    SYNC_PRINT(("GCodeScene::setParameters()\n"));
    mParameters = *static_cast<DrawGCodeParameters *>(params);

    SceneShaded::mParameters.point.type = ShaderPreset::PRESET;
    SceneShaded::mParameters.edge .type = ShaderPreset::PRESET;
    SceneShaded::mParameters.face .type = ShaderPreset::PRESET;

    SceneShaded::mParameters.setFaceColorOverride(false);
    SceneShaded::mParameters.setEdgeColorOverride(false);
    SceneShaded::mParameters.setPointColorOverride(false);

    ShadedSceneControlParameters *paramsShaded = &(SceneShaded::mParameters);
    SceneShaded::setParameters((void *)(paramsShaded));
    updateMesh();
}

void GCodeScene::updateMesh()
{
    SYNC_PRINT(("GCodeScene::updateMesh()\n"));

    mMesh->clear();
    GCodeToMesh transformer;
    transformer.mParameters = mParameters;
    transformer.renderToMesh(*owned, *mMesh);
}

GCodeScene::~GCodeScene()
{

}

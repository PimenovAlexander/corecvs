#ifndef BILLBOARDCAPTION3DSCENE_H
#define BILLBOARDCAPTION3DSCENE_H

#include <vector>
#include <string>

#include "core/math/vector/vector3d.h"

#include "scene3D.h"
#include "cloudViewDialog.h"

class BillboardCaption3DScene : public Scene3D
{
public:
    Draw3dParameters mParameters;

    /* Data */
    std::vector<std::pair<Vector3dd, std::string>> mLabels;

    BillboardCaption3DScene();

    virtual void prepareMesh(CloudViewDialog * /*dialog*/) override {}
    virtual void drawMyself(CloudViewDialog * /*dialog*/) override;

    virtual bool dump(const QString &/*targetFile*/) override { return false; }

    virtual void setParameters(void * params)
    {
        mParameters = *static_cast<Draw3dParameters *>(params);
    }

    virtual ParametersControlWidgetBase *getContolWidget()
    {
        Draw3dParametersControlWidget *result = new Draw3dParametersControlWidget();
        result->setParameters(mParameters);
        return result;
    }

#if 0
    virtual void replaceMesh(Mesh3D *newMesh) {
        delete_safe(owned);
        owned = newMesh;
    }

    virtual void setMesh(Mesh3D *newMesh) {
        owned = newMesh;
    }
#endif

    virtual ~BillboardCaption3DScene();

};

#endif // BILLBOARDCAPTION3DSCENE_H

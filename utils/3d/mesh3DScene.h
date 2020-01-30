#pragma once
/**
 * \file mesh3D.h
 *
 * \date Nov 13, 2012
 **/

#include <ostream>

#include "core/utils/global.h"

#include "scene3D.h"
#include "cloudViewDialog.h"
#include "core/fileformats/plyLoader.h"
#include "core/xml/generated/draw3dParameters.h"
#include "draw3dParametersControlWidget.h"
#include "draw3dCameraParametersControlWidget.h"



class Mesh3DScene : /*public Mesh3D,*/ public Scene3D {

public:
    Draw3dParameters mParameters;

    /* This mesh is not used so far.*/
    Mesh3D *owned = NULL;

    Mesh3DScene() :
        owned(NULL)
    {}

    virtual void prepareMesh(CloudViewDialog * /*dialog*/) {}
    virtual void drawMyself(CloudViewDialog * /*dialog*/);

    virtual bool dump(const QString &targetFile) override;

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

    virtual void replaceMesh(Mesh3D *newMesh) {
        delete_safe(owned);
        owned = newMesh;
    }

    virtual void setMesh(Mesh3D *newMesh) {
        owned = newMesh;
    }

    virtual ~Mesh3DScene() {
        delete_safe(owned);
    }

};


/* Some basic primitive objects */


class CameraScene : public Mesh3DScene {

public:
    Draw3dCameraParameters mParameters;

    virtual ParametersControlWidgetBase *getContolWidget()
    {
//        qDebug("CameraScene::getContolWidget() : Called");

        Draw3dCameraParametersControlWidget *result = new Draw3dCameraParametersControlWidget();
        result->setParameters(mParameters);
        return result;
    }

    virtual void setParameters(void * params)
    {
        //qDebug() << "Camera new parameters arrived";
        mParameters = *static_cast<Draw3dCameraParameters *>(params);
    }

    virtual void prepareMesh(CloudViewDialog * /*dialog*/);
    virtual void drawMyself(CloudViewDialog *dialog);

    virtual ~CameraScene() {}
};

class StereoCameraScene : public Mesh3DScene {
    RectificationResult mStereoPair;
    CameraScene mCamera1;
    CameraScene mCamera2;
public:
    StereoCameraScene(const RectificationResult &stereoPair);

    virtual void prepareMesh(CloudViewDialog * /*dialog*/);
    virtual void drawMyself(CloudViewDialog *dialog);

    virtual ~StereoCameraScene() {}

};



/* EOF */

#pragma once
/**
 * \file scene3D.h
 *
 * \date Nov 13, 2012
 **/

#include <ostream>
#include <QtCore/QString>

#include "core/utils/global.h"

#include "core/math/matrix/matrix44.h"
#include "generated/draw3dParameters.h"
#include "parametersControlWidgetBase.h"

class CloudViewDialog;

using std::ostream;


/**
 *   Part of the hierarchically displayable scene
 **/
class Scene3D
{
private:
    void initDecalState(CloudViewDialog *dialog, const Draw3dParameters *parameters, Matrix44 &projector);

public:
    enum SceneType {
        SCENE_3D,
        COORDINATE_FRAME
    };

    QString name;
    Scene3D *parent;
    SceneType type;

    Matrix44 transform;
    bool visible;

    /* This function sets common materials */
    virtual void commonSetup   (CloudViewDialog *dialog, const Draw3dParameters *parameters);
    virtual void commonTierdown(CloudViewDialog *dialog, const Draw3dParameters *parameters);

    /* Despite the fact all callers use this function to draw, */
    virtual void draw(CloudViewDialog *dialog);

    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);

    virtual bool dump(const QString &targetFile);

    virtual void setParameters(void * /*params*/) {}

    /* Not the best practice.*/
    virtual ParametersControlWidgetBase *getContolWidget()
    {
//        qDebug("Scene3D::getContolWidget() : Called");
        return NULL;
    }

    virtual bool isComplexObject()
    {
        return (type == COORDINATE_FRAME);
    }

    Scene3D() :
        parent(NULL),
        type(SCENE_3D),
        transform(1.0),
        visible(true)
    {}

    virtual ~Scene3D();
};



/*EOF*/






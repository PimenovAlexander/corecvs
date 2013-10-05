#pragma once

/**
 * \file coordinateFrame.h
 *
 * \date Dec 1, 2012
 **/

#include "scene3D.h"
#include "draw3dParametersControlWidget.h"

class CloudViewDialog;

class CoordinateFrame : public Scene3D
{
public:
    vector<QSharedPointer<Scene3D> > mChildren;
    Draw3dParameters mParameters;

    CoordinateFrame();
    virtual ~CoordinateFrame();


    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);
    virtual void dumpPCD(ostream &out);
    virtual void dumpPLY(ostream &out);


    virtual void setParameters(void * params)
    {
        mParameters = *static_cast<Draw3dParameters *>(params);
    };

    virtual ParametersControlWidgetBase *getContolWidget()
    {
        Draw3dParametersControlWidget *result = new Draw3dParametersControlWidget();
        result->setParameters(mParameters);
        return result;
    };

};

/* EOF */

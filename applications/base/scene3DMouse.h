/**
 * \file scene3DMouse.h
 * \brief Add Comment Here
 *
 * \date Dec 8, 2011
 * \author alexander
 */

#ifndef SCENE3D_H_
#define SCENE3D_H_

#include "triangulator.h"
#include "cloudViewDialog.h"
#include "../utils/3d/draw3dViMouseParametersControlWidget.h"




class Scene3DMouse : public Scene3D
{
public:
    Draw3dViMouseParameters mParameters;



    virtual void drawMyself(CloudViewDialog *dialog);
    virtual void dumpPCD(ostream &out);
    virtual void dumpPLY(ostream &out);

    virtual ParametersControlWidgetBase *getContolWidget()
    {
        Draw3dViMouseParametersControlWidget *result = new Draw3dViMouseParametersControlWidget();
        result->setParameters(mParameters);
        return result;
    };

    virtual void setParameters(void *params)
    {
        mParameters = *static_cast<Draw3dViMouseParameters *>(params);
    };

    Cloud *cloud;
    Scene3DMouse();
    virtual ~Scene3DMouse();
};

#endif /* SCENE3D_H_ */

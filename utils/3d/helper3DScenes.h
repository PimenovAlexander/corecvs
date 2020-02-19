#ifndef HELPER_3D_SCENES_H
#define HELPER_3D_SCENES_H

#include "scene3D.h"
#include "uis/cloudview/cloudViewDialog.h"

class Grid3DScene : public Scene3D {
private:
    GLuint mGridId;
public:
    const static int GRID_SIZE;
    const static int GRID_STEP;

    Grid3DScene() :
        mGridId(0)
    {}

    void render();

    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);
};


class Plane3DScene : public Scene3D {
    GLuint mPlaneListId;

public:
    Plane3DScene() :
        mPlaneListId(0)
    {}

    void render(void);
    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);
};


class Plane3DGeodesicScene : public Scene3D {
    GLuint mPlaneListId;

public:
    Plane3DGeodesicScene() :
        mPlaneListId(0)
    {}

    void render(void);
    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);
};



#endif // HELPER3DSCENES_H

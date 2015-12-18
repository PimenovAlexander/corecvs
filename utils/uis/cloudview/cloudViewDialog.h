#pragma once

#include <vector>
#include <QtCore/QObject>
#include <QDialog>
#include <QFileDialog>
#include <QImage>

#include "global.h"

#include "ui_cloudViewDialog.h"
#include "g12Buffer.h"
#include "bufferFactory.h"
#include "viAreaWidget.h"
#include "triangulator.h"
#include "transform3DSelector.h"
#include "3d/scene3D.h"
#include "3d/draw3dParametersControlWidget.h"
#include "3d/coordinateFrame.h"
#include "frames.h"
#include "coordinateFrame.h"
#include "treeSceneController.h"


using namespace corecvs;
using std::vector;

class CloudViewDialog : public ViAreaWidget
{
    Q_OBJECT

public:
    const static double ROTATE_STEP;
    const static double ZOOM_DIVISION;
    const static double SHIFT_STEP;


    const static double START_X;
    const static double START_Y;
    const static double START_Z;


    CloudViewDialog(QWidget *parent = 0);
    ~CloudViewDialog();

public slots:
    void setCollapseTree(bool collapse);

    void downRotate();
    void upRotate();
    void leftRotate();
    void rightRotate();
    void resetCameraPos();
    void resetCamera();
    void resetCameraSlot();

    void zoomIn();
    void zoomOut();


    void childMoveEvent       (QMouseEvent *event);
    void childPressEvent      (QMouseEvent *event);
    void childReleaseEvent    (QMouseEvent *event);
    void childWheelEvent      (QWheelEvent *event);
    void childKeyPressEvent   (QKeyEvent *event);
    void childKeyReleaseEvent (QKeyEvent *event);


    void keyPressEvent   (QKeyEvent *event);
    void keyReleaseEvent (QKeyEvent *event);

    void initializeGLSlot();
    void repaintGLSlot();
    void resizeGLSlot(int width, int height);

    void updateCameraMatrix();
    void updateHelperObjects();

    void savePointsPCD();
    void savePointsPLY();

    /* Tree manipulation functions */
    void toggledVisibility();
//    void treeItemClicked(QTreeWidgetItem *, int);

    /* Load New Mesh*/
    void loadMesh();
    void addCoordinateFrame();

    void saveParameters();
    void loadParameters();


public:
    Ui_CloudViewDialogClass mUi;

    enum SceneId {
        MAIN_SCENE,
        ADDITIONAL_SCENE,
        CAMERA_PAIR,
        FIRST_USER_SCENE,
        GAPER_ZONE = FIRST_USER_SCENE,
        CONTROL_ZONE,
        DETECTED_PERSON,
        DISP_CONTROL_ZONE,
        DISP_PREDICTED_CONTROL_ZONE,
        CLUSTER_ZONE,
        CLUSTER_SWARM,
        HEAD_SEARCH_ZONE,

        HAND_1,
        HAND_2,
        HAND_FIRST = HAND_1,
        HAND_LAST  = HAND_2,
        /* Should be the last one*/
        SCENE_NUMBER
    };

    QSharedPointer<Scene3D> mScenes[SCENE_NUMBER];
    TreeSceneController *mSceneControllers[SCENE_NUMBER];
    QSharedPointer<RectificationResult> mRectificationResult;
    QSharedPointer<QImage> mCameraImage[Frames::MAX_INPUTS_NUMBER];

    GLuint texture(int cameraId);

    void setNewScenePointer (QSharedPointer<Scene3D> scene, int sceneId = MAIN_SCENE);
    void setNewRectificationResult (QSharedPointer<RectificationResult> rectificationResult);
    void setNewCameraImage (QSharedPointer<QImage> texture, int cameraId = Frames::RIGHT_FRAME);

    Matrix44 mCamera;
    double mCameraZoom;

    enum SubScene {
        SUBSCENE_PLANE,
        SUBSCENE_GRID,
        SUBSCENE_CAMERA,
        SUBSCENE_USER
    };

    enum CameraType {
        ORTHO_TOP,
        ORTHO_LEFT,
        ORTHO_FRONT,
        PINHOLE_AT_0,
        RIGHT_CAMERA,
        LEFT_CAMERA,
        FACE_CAMERA
    };

protected:
    int mIsTracking;
    QPoint mTrack;
    //vector<TreeSceneController *> mControllers;
    TreeSceneModel mTreeModel;

    /* OpenGL textures */
public:
    GLuint mFancyTexture;
    TreeSceneController* addSubObject (QString name, QSharedPointer<Scene3D> scene, bool visible = true);
    void addMesh(QString name, Mesh3D *mesh);
private:
    GLuint mCameraTexture[Frames::MAX_INPUTS_NUMBER];

protected:


    void setZoom(double value);
    void zoom(int delta);
};

// EOF: CLOUDVIEWDIALOG_H

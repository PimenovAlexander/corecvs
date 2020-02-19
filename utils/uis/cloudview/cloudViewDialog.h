#pragma once

#include <vector>
#include <QtCore/QObject>
#include <QDialog>
#include <QFileDialog>
#include <QImage>

#include "core/utils/global.h"

#include "ui_cloudViewDialog.h"
#include "buffers/g12Buffer.h"
#include "buffers/bufferFactory.h"
#include "viAreaWidget.h"
#include "rectification/triangulator.h"
#include "widgets/transform3DSelector.h"
#include "3d/scene3D.h"
#include "3d/draw3dParametersControlWidget.h"
#include "3d/coordinateFrame.h"
#include "framesources/frames.h"
#include "3d/coordinateFrame.h"
#include "treeSceneController.h"
#include "cameracalibration/cameraModel.h"

#include "uis/textLabelWidget.h"
#include "stats/calculationStats.h"


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


    CloudViewDialog(QWidget *parent = NULL, QString name = QString());
    ~CloudViewDialog();

public slots:
    void setCollapseTree(bool collapse);

    /*These rotate model*/
    void downRotate();
    void upRotate();
    void leftRotate();
    void rightRotate();
    /* These rotate camera */
    void clockRotate();
    void anticlockRotate();

    void backgroundColorChanged();

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

    void saveMesh();
    void savePointsPLY();

    /* Tree manipulation functions */
    void toggledVisibility();
//    void treeItemClicked(QTreeWidgetItem *, int);

    /* Load New Mesh*/
    void loadMesh();
    void addCoordinateFrame();

    void saveParameters();
    void loadParameters();


    void statsOpen();

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

        FIXTUE_SCENE,
        /* Should be the last one*/
        SCENE_NUMBER
    };

    QSharedPointer<Scene3D>             mScenes[SCENE_NUMBER];
    TreeSceneController                *mSceneControllers[SCENE_NUMBER];
    QSharedPointer<RectificationResult> mRectificationResult;
    QSharedPointer<QImage>              mCameraImage[Frames::MAX_INPUTS_NUMBER];

    /**
     * mCamera stores "sort of" modelview matrix
     ***/
    Matrix44                            mCamera;
    double                              mCameraZoom;

    GLuint                              texture(int cameraId);
    GLuint                              mFancyTexture;

    void setNewScenePointer (QSharedPointer<Scene3D> scene, int sceneId = MAIN_SCENE);
    void setNewRectificationResult (QSharedPointer<RectificationResult> rectificationResult);
    void setNewCameraImage (QSharedPointer<QImage> texture, int cameraId = Frames::RIGHT_FRAME);


    void setCamera(const CameraModel &model);
    void lookAt   (const Vector3dd   &point);


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

        ORTHO_TOP_LEFT,
        ORTHO_LEFT_LEFT,
        ORTHO_FRONT_LEFT,

        PINHOLE_AT_0,
        RIGHT_CAMERA,
        LEFT_CAMERA,
        FACE_CAMERA,
        USER_CAMERA,
        USER_ORTHO_CAMERA
    };


    bool isOrtho(CameraType type)
    {
        if (type == ORTHO_TOP      || type == ORTHO_LEFT      || type == ORTHO_FRONT      ||
            type == ORTHO_TOP_LEFT || type == ORTHO_LEFT_LEFT || type == ORTHO_FRONT_LEFT ||
            type == USER_ORTHO_CAMERA)
        {
            return true;
        }
        return false;
    }

    bool isLeft(CameraType type)
    {
        if (type == ORTHO_TOP_LEFT || type == ORTHO_LEFT_LEFT || type == ORTHO_FRONT_LEFT )
        {
            return true;
        }
        return false;
    }


    const QOpenGLContext *getAreaContext();

    /* OpenGL textures */
public:
    TreeSceneController* addSubObject (QString name, QSharedPointer<Scene3D> scene, bool visible = true);
    void addMesh(QString name, Mesh3D *mesh);
private:
    GLuint                      mCameraTexture[Frames::MAX_INPUTS_NUMBER];

    RGBColor mBackgroundColor;

protected:
  //vector<TreeSceneController*> mControllers;
    bool                        mIsLeft = false;
    int                         mIsTracking;
    QPoint                      mTrack;
    TreeSceneModel              mTreeModel;

    BaseTimeStatisticsCollector mStatsCollector;
    TextLabelWidget             mStatisticsDialog;

    void setZoom(double value);
    void zoom(int delta);
};

// EOF: CLOUDVIEWDIALOG_H

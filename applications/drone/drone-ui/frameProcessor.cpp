#include "frameProcessor.h"

#include <QApplication>
#include <imageCaptureInterfaceQt.h>

#include "physicsMainWindow.h"

#include <math/matrix/homographyReconstructor.h>

#include <geometry/renderer/simpleRenderer.h>

#include <geometry/convexHull.h>

FrameProcessor::FrameProcessor(QObject *parent) : QThread(parent)
{
    SYNC_PRINT(("FrameProcessor::processFrame():called\n"));
    detector = PatternDetectorFabric::getInstance()->fabricate("Apriltag");
}

void FrameProcessor::processFrame(ImageCaptureInterface::FrameMetadata frameData)
{
    static std::deque<Affine3DQ> path;

    Statistics stats;
    static int count=0;
    count++;

    static bool skipping = false;
    if (skipping)
    {
        return;
    }
    skipping = true;
    QApplication::processEvents();
    skipping = false;

    while (path.size() > 100) {
        path.pop_back();
    }


//    SYNC_PRINT(("New frame arrived\n"));
    ImageCaptureInterface::FramePair pair = input->getFrameRGB24();
    RGB24Buffer * result = pair.rgbBufferLeft();
    pair.setRgbBufferLeft(NULL);
    pair.freeBuffers();

    if (mPatternToPose.poseParameters.undistort())
    {
        stats.startInterval();
        RadialCorrection radialCorrection(mCameraModel.distortion);
        RGB24Buffer *undistorted = result->doReverseDeformation<RGB24Buffer, RadialCorrection  >(radialCorrection);
        stats.endInterval("Undistort");
        delete_safe(result);
        result = undistorted;
    }

    if (detector != NULL)
    {
        vector<PatternDetectorResult> patterns;

        stats.startInterval();
        stats.enterContext("Detector ->");

        detector->setStatistics(&stats);
        detector->setInputImage(result);
        detector->operator ()();
        detector->getOutput(patterns);

        stats.leaveContext();

        stats.startInterval();
        /* Debug draw should be improved */
        for (size_t i = 0; i < patterns.size(); i++)
        {
            PatternDetectorResult &pattern = patterns[i];
            if (pattern.empty())
                continue;

            vector<Vector2dd> points;
            Vector2dd center = Vector2dd::Zero();
            for (auto po: pattern) {
                points.push_back(po.projection);
                center += po.projection;
            }

            Polygon perefery = ConvexHull::ConvexHullCompute(points);
            RGBColor color = RGBColor::parula((double)i / (patterns.size()));

            for (size_t k = 0; k < perefery.size(); k++) {
                 result->drawLine(perefery.getPoint(k), perefery.getNextPoint(k), color);
            }

            center /= pattern.size();
            AbstractPainter<RGB24Buffer> p(result);            
            p.drawFormat(center.x(), center.y(), color, 2, "%d", pattern.mId);
        }

        stats.resetInterval("Basic Draw");

        /* Try to locate myself */
       if (!patterns.empty())
       {
           double scale = mPatternToPose.poseParameters.patternScale();
           PatternDetectorResult &pattern = patterns[0];
           HomographyReconstructor reconstructor;

           for (PointObservation &po: pattern)
           {
               reconstructor.addPoint2PointConstraint(scale * po.point.xy(), po.projection);
           }

           Matrix33  homography = reconstructor.getBestHomography();

           if (mPatternToPose.poseParameters.drawHomography())
           {
               for (int i = 0; i < 9; i++)
                   for (int j = 0; j < 9; j++)
                   {
                       Vector2dd in = Vector2dd (i / 8.0, j / 8.0) * scale;
                       Vector2dd out = homography * in;
                       result->drawCrosshare3(out, RGBColor::Red());
                   }
           }

           PinholeCameraIntrinsics *pinhole = mCameraModel.getPinhole();
           Matrix33 K = pinhole->getKMatrix33();
           Affine3DQ affine = HomographyReconstructor::getAffineFromHomography(K, homography);
           // cout << affine << endl;

           path.push_front(affine);

           if (mPatternToPose.poseParameters.drawCube())
           {
               CameraModel simulated(pinhole->clone());
               simulated.setLocation(affine);
               SimpleRenderer renderer;
               renderer.modelviewMatrix = simulated.getCameraMatrix();
               Mesh3D mesh;
               mesh.switchColor();
               mesh.addAOB(Vector3dd(0,0,0), Vector3dd(1,1,1));
               for (size_t i = 0; i < mesh.facesColor.size(); i++) {
                   mesh.facesColor[i] = RGBColor::rainbow((double)i/ mesh.facesColor.size());
               }
               renderer.render(&mesh, result);
           }
       }
       stats.resetInterval("3d Draw");

    } else {
        SYNC_PRINT(("FrameProcessor::processFrame(): detector is NULL\n"));
    }

    Mesh3DDecorated *mesh = new Mesh3DDecorated();
    mesh->switchColor();
    {
        for (Affine3DQ &a : path)
        {
            mesh->currentTransform = (Matrix44)a;
            mesh->addOrts();
            mesh->currentTransform = Matrix44::Identity();
        }
    }

    target->uiMutex.lock();
    target->uiQueue.emplace_back(new DrawRequestData);
    target->uiQueue.back()->mImage = result;
    target->uiQueue.back()->stats = stats;    
    target->uiQueue.back()->mMesh = mesh;
    target->uiMutex.unlock();

    target->updateUi();

}

void FrameProcessor::setPatternDetectorParameters(GeneralPatternDetectorParameters params)
{
    SYNC_PRINT(("FrameProcessor::setPatternDetectorParameters(): called\n"));
    cout << params << endl;


    delete_safe(detector);
    detector = PatternDetectorFabric::getInstance()->fabricate(params.provider);
    if (detector == NULL) {
        SYNC_PRINT(("FrameProcessor::setPatternDetectorParameters(): were not able to create detector <%s>\n", params.provider.c_str()));
        return;
    }
    mPatternToPose = params;

    //  std::map<std::string, corecvs::DynamicObject> providerParameters;
    for (auto &it: params.providerParameters)
    {
        detector->setParameters(it.first, it.second);
    }
    SYNC_PRINT(("FrameProcessor::setPatternDetectorParameters(): New detector <%s> created\n", params.provider.c_str()));
}

void FrameProcessor::setCameraModel(CameraModel params)
{
    SYNC_PRINT(("FrameProcessor::setCameraModel(): called\n"));
    mCameraModel = params;
}

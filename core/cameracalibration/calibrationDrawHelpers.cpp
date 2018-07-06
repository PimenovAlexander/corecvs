#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/cameraModel.h"
#include "core/camerafixture/fixtureScene.h"
#include "core/alignment/selectableGeometryFeatures.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/camerafixture/cameraFixture.h"

using namespace corecvs;


void CalibrationDrawHelpers::setParameters(const CalibrationDrawHelpersParameters &params)
{
    *static_cast<CalibrationDrawHelpersParameters *>(this) = params;
}

void CalibrationDrawHelpers::drawCamera(Mesh3D &mesh, const CameraModel &cam, double scale)
{
    //SYNC_PRINT(("CalibrationDrawHelpers::drawCamera(): called \n"));
    //cout << "Camera color: " << mesh.currentColor << endl;

    double w = cam.intrinsics->w();
    double h = cam.intrinsics->h();

    Vector3dd
            center      = Vector3dd( 0,  0,  0),
            center2     = Vector3dd( 0,  0,  1) * scale,
            topLeft     = Vector3dd( 0,  0,  1) * scale,
            topRight    = Vector3dd( w,  0,  1) * scale,
            bottomRight = Vector3dd( w,  h,  1) * scale,
            bottomLeft  = Vector3dd( 0,  h,  1) * scale,
            imageCenter = Vector3dd( w / 2.0,  h / 2.0, 1) * scale;

    Vector3dd edges[] =
    {
        center,      center2,
        center,      topLeft,
        center,      topRight,
        center,      bottomRight,
        center,      bottomLeft,
        topLeft,     topRight,
        topRight,    bottomRight,
        bottomRight, bottomLeft,
        bottomLeft,  topLeft,

        topLeft,     imageCenter
    };

    const int edgenumber = CORE_COUNT_OF(edges) / 2;

    if (cam.intrinsics->isPinhole())
    {
        PinholeCameraIntrinsics *pinhole = static_cast<PinholeCameraIntrinsics *>(cam.intrinsics.get());
        Matrix33 invK   = pinhole->getInvKMatrix33();

        for (int i = 0; i < edgenumber; ++i)
        {
            Vector3dd v1 = cam.extrinsics.camToWorld(invK * edges[i * 2    ]);
            Vector3dd v2 = cam.extrinsics.camToWorld(invK * edges[i * 2 + 1]);

            mesh.addLine(v1, v2);
         //   cout << v1 << " " << v2 << endl;
        }

        RGBColor color = mesh.currentColor;

        if (solidCameras())
        {
            Vector3dd faces[] =
            {
                topLeft , topRight   , bottomLeft,
                topRight, bottomRight, bottomLeft,

                center  , topLeft    , bottomLeft,
                center  , bottomLeft , bottomRight,
                center  , bottomRight, topRight,
                center  , topRight   , topLeft
            };

            const int facenumber = CORE_COUNT_OF(faces) / 3;

            for (int i = 0; i < facenumber; ++i)
            {
                if (i == 0) {
                    mesh.setColor(color);
                } else {
                    mesh.setColor(RGBColor::Yellow());

                }

                Vector3dd v1 = cam.extrinsics.camToWorld(invK * faces[i * 3    ]);
                Vector3dd v2 = cam.extrinsics.camToWorld(invK * faces[i * 3 + 1]);
                Vector3dd v3 = cam.extrinsics.camToWorld(invK * faces[i * 3 + 2]);

                mesh.addTriangle(v1, v2, v3);
             //   cout << v1 << " " << v2 << endl;
            }
        }

        mesh.setColor(color);
    } else {
        int step = gridStepForCameras();
        Vector3dd center = cam.extrinsics.camToWorld(Vector3dd::Zero());

        AbstractBuffer<Vector3dd> poses (step , step );
        for (int i = 0; i < step ; i++)
        {
            for (int j = 0; j < step ; j++)
            {
                Vector2dd pos = cam.intrinsics->size() / (step  - 1) * Vector2dd(j,i);
                Vector3dd dir = cam.extrinsics.camToWorld(cam.intrinsics->reverse(pos) * scale);
                poses.element(i,j) = dir;
            }
        }

        mesh.addLine(center, poses.element(     0,      0));
        mesh.addLine(center, poses.element(     0, step -1));
        mesh.addLine(center, poses.element(step -1,      0));
        mesh.addLine(center, poses.element(step -1, step -1));

        if (!solidCameras())
        {
            for (int i = 0; i < step  - 1 ; i++)
            {
                for (int j = 0; j <  step  - 1; j++)
                {                    
                    bool cornerMark = (i < step / 2  && j < step / 2);

                    if (cornerMark) {
                        mesh.pushColor(RGBColor::Yellow());
                    }

                    mesh.addLine(poses.element(i,j), poses.element(i    ,j + 1));
                    mesh.addLine(poses.element(i,j), poses.element(i + 1,j    ));

                    if (cornerMark) {
                        mesh.popColor();
                    }

                }
            }

            for (int i = 0; i < step  - 1 ; i++)
            {
                 mesh.addLine(poses.element(i,step  - 1), poses.element(i + 1, step  - 1));
            }

            for (int j = 0; j <  step  - 1; j++)
            {
                mesh.addLine(poses.element(step  - 1, j), poses.element(step  - 1, j + 1));
            }
        } else {
            for (int i = 0; i < step  - 1 ; i++)
            {
                for (int j = 0; j <  step  - 1; j++)
                {
                    RGBColor color = mesh.currentColor;
                    bool cornerMark = (i < step / 2  && j < step / 2);

                    if (cornerMark) {
                        mesh.setColor(RGBColor::Yellow());
                    }

                    mesh.addTriangle(poses.element(i,j), poses.element(i    ,j + 1), poses.element(i + 1,j + 1));

                    if (cornerMark) {
                        mesh.setColor(color);
                    }

                }
            }
        }

        mesh.pushColor(RGBColor::Blue());
        Vector2dd pos = cam.intrinsics->principal();
        Vector3dd dir = cam.extrinsics.camToWorld(cam.intrinsics->reverse(pos) * scale * 2);
        mesh.addLine(center, dir);
        mesh.popColor();
    }


    if (printNames())
    {
        AbstractPainter<Mesh3D> p(&mesh);
            mesh.mulTransform(Matrix44::Shift(cam.extrinsics.position));
            mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(0.0, 0.0, 0, scale / 50.0, "Cam: %s", cam.nameId.c_str());
        mesh.popTransform();
    }



    //Vector3dd ppv = qc * (invK.mulBy2dRight(cam.intrinsics.principal) * scale) + cc;

    //mesh.addLine(ppv, qc * (invK * center) + cc);
}

void CalibrationDrawHelpers::drawCameraEx (Mesh3DDecorated &mesh, const CameraModel &cam, double scale, int id)
{
    int step = gridStepForCameras();
    Vector3dd center = cam.extrinsics.camToWorld(Vector3dd::Zero());

    AbstractBuffer<Vector3dd> poses (step , step );
    AbstractBuffer<Vector2dd> tex   (step , step );

    for (int i = 0; i < step ; i++)
    {
        for (int j = 0; j < step ; j++)
        {
            Vector2dd texuv = Vector2dd(j,i) / (step  - 1);
            Vector2dd pos = cam.intrinsics->size() * texuv;
            Vector3dd dir = cam.extrinsics.camToWorld(cam.intrinsics->reverse(pos) * scale);
            poses.element(i,j) = dir;
            tex  .element(i,j) = texuv;
        }
    }

    mesh.currentTexture = id;
    mesh.addLine(center, poses.element(     0,      0));
    mesh.addLine(center, poses.element(     0, step -1));
    mesh.addLine(center, poses.element(step -1,      0));
    mesh.addLine(center, poses.element(step -1, step -1));

    for (int i = 0; i < step  - 1 ; i++)
    {
        for (int j = 0; j <  step  - 1; j++)
        {
            RGBColor color = mesh.currentColor;
            bool cornerMark = (i < step / 2  && j < step / 2);

            if (cornerMark) {
                mesh.setColor(RGBColor::Yellow());
            }


            mesh.addTriangleT(poses.element(i    , j    ), Vector2dd(tex.element(i    , j   )),
                              poses.element(i    , j + 1), Vector2dd(tex.element(i    , j + 1)),
                              poses.element(i + 1, j + 1), Vector2dd(tex.element(i + 1, j + 1)));

            mesh.addTriangleT(poses.element(i + 1, j    ), Vector2dd(tex.element(i + 1, j   )),
                              poses.element(i    , j    ), Vector2dd(tex.element(i    , j   )),
                              poses.element(i + 1, j + 1), Vector2dd(tex.element(i + 1, j + 1)));

            if (cornerMark) {
                mesh.setColor(color);
            }

        }
    }


    mesh.pushColor(RGBColor::Blue());
    Vector2dd pos = cam.intrinsics->principal();
    Vector3dd dir = cam.extrinsics.camToWorld(cam.intrinsics->reverse(pos) * scale * 2);
    mesh.addLine(center, dir);
    mesh.popColor();



    if (printNames())
    {
        AbstractPainter<Mesh3D> p(&mesh);
            mesh.mulTransform(Matrix44::Shift(cam.extrinsics.position));
            mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(0.0, 0.0, 0, scale / 50.0, "Cam: %s", cam.nameId.c_str());
        mesh.popTransform();
    }
}

void CalibrationDrawHelpers::drawFixture(Mesh3D &mesh, const CameraFixture &ps, double scale)
{
//    SYNC_PRINT(("CalibrationHelpers::drawPly():called\n"));
    if (drawFixtureCams())
    {
        for (size_t cam = 0; cam < ps.cameras.size(); cam++)
        {
            mesh.setColor(RGBColor::getPalleteColor((int)cam));
            //SYNC_PRINT(("CalibrationDrawHelpers::drawPly color %d(%d)\n", colorId, CORE_COUNT_OF(palette)));           
            drawCamera(mesh, ps.getRawCamera((int)cam), scale);
        }
    }


    corecvs::Vector3dd xps(scale,   0.0,   0.0),
                       yps(  0.0, scale,   0.0),
                       zps(  0.0,   0.0, scale),
                       ori(  0.0,   0.0,   0.0);
    mesh.currentColor = corecvs::RGBColor::Red();
    mesh.addLine(ps.location * ori, ps.location * xps);
    mesh.currentColor = corecvs::RGBColor::Green();
    mesh.addLine(ps.location * ori, ps.location * yps);
    mesh.currentColor = corecvs::RGBColor::Blue();
    mesh.addLine(ps.location * ori, ps.location * zps);

    if (printNames())
    {
        AbstractPainter<Mesh3D> p(&mesh);
        mesh.mulTransform(Matrix44::Shift(ps.location.shift) * Matrix44::Scale(scale / 22.0));
        mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(1.0, 1.0, 0, 1.0, "%s", ps.name.c_str());
        mesh.popTransform();
    }
}

void CalibrationDrawHelpers::drawFixtureEx(Mesh3DDecorated &mesh, const CameraFixture &ps, double scale)
{
    CORE_UNUSED(mesh);
    CORE_UNUSED(ps);
    CORE_UNUSED(scale);
}

void CalibrationDrawHelpers::drawSceneGeometry(Mesh3D &mesh, const FixtureSceneGeometry &fg, double /*scale*/)
{
    mesh.setColor(RGBColor::Red());
    mesh.addFlatPolygon(fg);
}


void CalibrationDrawHelpers::drawObservationList(Mesh3D &mesh, const ObservationList &list)
{
    mesh.currentColor = RGBColor(~0u);
    for (const PointObservation& pt: list)
    {
        mesh.addPoint(pt.point);
    }
}

void CalibrationDrawHelpers::drawScenePoint(Mesh3D &mesh, const SceneFeaturePoint &fp, double scale)
{
    mesh.setColor(fp.color);
    Vector3dd pos = fp.getDrawPosition(preferReprojected(), forceKnown());

    if (!largePoints()) {
        mesh.addPoint(pos);
    } else {
        mesh.addIcoSphere(pos, scale / 100.0, 2);
    }

    if (printNames()) {
        AbstractPainter<Mesh3D> p(&mesh);
        mesh.mulTransform(Matrix44::Shift(pos) * Matrix44::Scale(scale / 22.0));
        mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(1.0, 1.0, 0, 1.0, "%s", fp.name.c_str());
        mesh.popTransform();
    }

    if (drawObservations()) {
        for (auto it = fp.observations.begin(); it != fp.observations.end(); ++it)
        {
            FixtureCamera *cam = it->first;
            if (cam == NULL) continue;

            const SceneObservation &observ = it->second;
            CameraFixture *fixture = cam->cameraFixture;
            if (fixture == NULL) continue;

            FixtureCamera rawCam = fixture->getWorldCamera(cam);

            if (fp.hasKnownPosition && drawTrueLines()) {
                mesh.pushColor(RGBColor::Cyan());
                mesh.addLine(rawCam.extrinsics.position, fp.position);
                mesh.popColor();
            } else {
            }

            {
                mesh.pushColor(RGBColor::Yellow());
                Ray3d ray = rawCam.rayFromPixel(observ.getUndist());
                Vector3dd p2 = ray.getPoint(projectionRayLength() * scale);
                mesh.addLine(ray.p, p2);
                mesh.popColor();
            }

            {
                mesh.pushColor(RGBColor::Green());
                Ray3d ray = observ.getFullRay();
                Vector3dd p2 = ray.getPoint(projectionRayLength() * scale);
                mesh.addLine(ray.p, p2);
                mesh.popColor();
            }

            /*Ray3d rayDir(rawCam.extrinsics.position, observ.observDir);
            Vector3dd p3 = rayDir.getPoint(scale);
            mesh.addLine(rayDir.p, p3);*/

            /*if (!largePoints()) {
                mesh.addPoint(p2);
            } else {
                mesh.addIcoSphere(p2, scale / 100.0, 2);
            }*/
        }
    }
}

void CalibrationDrawHelpers::drawScene(Mesh3D &mesh, const FixtureScene &scene, double scale)
{
    int count = 0;
    for (FixtureCamera *cam: scene.orphanCameras())
    {
        mesh.setColor(RGBColor::getPalleteColor(count++));
        drawCamera(mesh, *cam, scale);
    }

    for (CameraFixture *ps: scene.fixtures())
    {
        drawFixture(mesh, *ps, scale);
    }

    for (SceneFeaturePoint *fp: scene.featurePoints())
    {
        drawScenePoint(mesh, *fp, scale);
    }

    for (FixtureSceneGeometry *fg: scene.geometries())
    {
        drawSceneGeometry(mesh, *fg, scale);
    }

}

void CalibrationDrawHelpers::drawScene(Mesh3DDecorated &mesh, const FixtureScene &scene, double scale)
{
    int count = 0;
    for (FixtureCamera *cam: scene.orphanCameras())
    {
        mesh.setColor(RGBColor::getPalleteColor(count++));
        drawCameraEx(mesh, *cam, scale, 0);
    }

    for (CameraFixture *ps: scene.fixtures())
    {
        drawFixtureEx(mesh, *ps, scale);
    }

    for (SceneFeaturePoint *fp: scene.featurePoints())
    {
        drawScenePoint(mesh, *fp, scale);
    }

    for (FixtureSceneGeometry *fg: scene.geometries())
    {
        drawSceneGeometry(mesh, *fg, scale);
    }
}



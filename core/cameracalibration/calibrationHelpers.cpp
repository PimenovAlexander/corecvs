#include "core/cameracalibration/calibrationHelpers.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/calibrationCamera.h"
//#include "calibrationPhotostation.h"
#include "core/camerafixture/fixtureScene.h"
#include "core/alignment/selectableGeometryFeatures.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/camerafixture/cameraFixture.h"

using namespace corecvs;

RGBColor CalibrationHelpers::palette[] =
{
    RGBColor(0x762a83u),
    RGBColor(0xaf8dc3u),
    RGBColor(0xe7d4e8u),
    RGBColor(0xd9f0d3u),
    RGBColor(0x7fbf7bu),
    RGBColor(0x1b7837u)
};

void CalibrationHelpers::drawCamera(Mesh3D &mesh, const CameraModel &cam, double scale)
{
    double w = cam.intrinsics.w();
    double h = cam.intrinsics.h();

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
    Matrix33 invK   = cam.intrinsics.getInvKMatrix33();

    for (int i = 0; i < edgenumber; ++i)
    {
        Vector3dd v1 = cam.extrinsics.camToWorld(invK * edges[i * 2    ]);
        Vector3dd v2 = cam.extrinsics.camToWorld(invK * edges[i * 2 + 1]);

        mesh.addLine(v1, v2);
     //   cout << v1 << " " << v2 << endl;
    }

    if (printNames)
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

#if 0
void CalibrationHelpers::drawPly(Mesh3D &mesh, const Photostation &ps, double scale)
{
    // Colorblind-safe palette
    CameraLocationData loc = ps.getLocation();
    Quaternion qs = loc.orientation.conjugated();
    std::cout << qs << std::endl;

    int colorId = 0;
    for (size_t cam = 0; cam < ps.cameras.size(); cam++)
    {
        mesh.setColor(palette[colorId]);
        colorId = (colorId + 1) % CORE_COUNT_OF(palette);
        drawCamera(mesh, ps.getRawCamera((int)cam), scale);
    }


    corecvs::Vector3dd xps(scale,   0.0,   0.0),
                       yps(  0.0, scale,   0.0),
                       zps(  0.0,   0.0, scale),
                       ori(  0.0,   0.0,   0.0);
    mesh.currentColor = corecvs::RGBColor(255,   0,   0);
    mesh.addLine(ps.location * ori, ps.location * xps);
    mesh.currentColor = corecvs::RGBColor(  0, 255,   0);
    mesh.addLine(ps.location * ori, ps.location * yps);
    mesh.currentColor = corecvs::RGBColor(  0,   0, 255);
    mesh.addLine(ps.location * ori, ps.location * zps);

    if (printNames)
    {
        AbstractPainter<Mesh3D> p(&mesh);
        mesh.mulTransform(Matrix44::Shift(ps.location.shift) * Matrix44::Scale(scale / 22.0));
        mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(scale / 5.0, scale / 5.0, 0, scale / 3.0, "%s", ps.name.c_str());
        mesh.popTransform();
    }
}
#endif

void CalibrationHelpers::drawPly(Mesh3D &mesh, const CameraFixture &ps, double scale)
{
//    SYNC_PRINT(("CalibrationHelpers::drawPly():called\n"));


    // Colorblind-safe palette
    // CameraLocationData loc = ps.getLocation();
    // Quaternion qs = loc.orientation.conjugated();
    // std::cout << qs << std::endl;

    int colorId = 0;

    if (drawFixtureCams)
    {
        for (size_t cam = 0; cam < ps.cameras.size(); cam++)
        {
            mesh.setColor(palette[colorId]);
            colorId = (colorId + 1) % CORE_COUNT_OF(palette);
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

    if (printNames)
    {
        AbstractPainter<Mesh3D> p(&mesh);
        mesh.mulTransform(Matrix44::Shift(ps.location.shift) * Matrix44::Scale(scale / 22.0));
        mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(1.0, 1.0, 0, 1.0, "%s", ps.name.c_str());
        mesh.popTransform();
    }
}


void CalibrationHelpers::drawPly(Mesh3D &mesh, const ObservationList &list)
{
    mesh.currentColor = RGBColor(~0u);
    for (const PointObservation& pt: list)
    {
        mesh.addPoint(pt.point);
    }
}

#if 0
void CalibrationHelpers::drawPly(Mesh3D &mesh, const Photostation &ps, const ObservationList &list, double scale)
{
    drawPly(mesh, list);
    drawPly(mesh, ps, scale);
}
#endif

void CalibrationHelpers::drawPly(Mesh3D &mesh, SceneFeaturePoint fp, double scale)
{

    mesh.setColor(fp.color);
    if (!fp.hasKnownPosition)
        fp.position = fp.reprojectedPosition;
    if (!largePoints) {
        mesh.addPoint(fp.position);
    } else {
        mesh.addIcoSphere(fp.position, scale / 100.0, 2);
    }

    if (printNames) {
        AbstractPainter<Mesh3D> p(&mesh);
        mesh.mulTransform(Matrix44::Shift(fp.position) * Matrix44::Scale(scale / 22.0));
        mesh.setColor(RGBColor::Blue());
        p.drawFormatVector(1.0, 1.0, 0, 1.0, "%s", fp.name.c_str());
        mesh.popTransform();
    }

    if (drawObservations) {
        for (auto it = fp.observations.begin(); it != fp.observations.end(); ++it)
        {
            FixtureCamera *cam = it->first;
            const SceneObservation &observ = it->second;
            CameraFixture *fixture = cam->cameraFixture;
            FixtureCamera rawCam = fixture->getWorldCamera(cam);

            if (fp.hasKnownPosition) {
                mesh.addLine(rawCam.extrinsics.position, fp.position);
            } else {

            }

            Ray3d ray = rawCam.rayFromPixel(observ.observation);
            Vector3dd p2 = ray.getPoint(scale);
            mesh.addLine(ray.p, p2);

            /*Ray3d rayDir(rawCam.extrinsics.position, observ.observDir);
            Vector3dd p3 = rayDir.getPoint(scale);
            mesh.addLine(rayDir.p, p3);*/


            if (!largePoints) {
                mesh.addPoint(p2);
            } else {
                mesh.addIcoSphere(p2, scale / 100.0, 2);
            }

        }

    }
}

void CalibrationHelpers::drawScene(Mesh3D &mesh, const FixtureScene &scene, double scale)
{
    for (FixtureCamera *cam: scene.orphanCameras)
    {
        drawCamera(mesh, *cam, scale);
    }

    for (CameraFixture *ps: scene.fixtures)
    {
        drawPly(mesh, *ps, scale);
    }

    for (SceneFeaturePoint *fp: scene.points)
    {
        drawPly(mesh, *fp, scale);
    }
}



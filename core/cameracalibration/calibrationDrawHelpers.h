#pragma once

#include "core/buffers/rgb24/rgbColor.h"
#include "core/xml/generated/calibrationDrawHelpersParameters.h"

#include "core/geometry/mesh/mesh3d.h"
#include "core/geometry/mesh/mesh3DDecorated.h"

namespace corecvs {

class Mesh3D;
class CameraModel;
class ObservationList;

class SceneFeaturePoint;
class FixtureScene;
class CameraFixture;
class FixtureSceneGeometry;

class CalibrationDrawHelpers : public CalibrationDrawHelpersParameters
{
public:

    enum RenderStyle {
        COLOR_PER_CAM,
        COLOR_PER_STATION
    };

    CalibrationDrawHelpers()
    {}

    void setParameters(const CalibrationDrawHelpersParameters &params);

    void drawCamera   (Mesh3D          &mesh, const CameraModel &cam, double scale);
    void drawCameraEx (Mesh3DDecorated &mesh, const CameraModel &cam, double scale, int id);

    void drawObservationList (Mesh3D &mesh, const ObservationList &list);

    void drawFixture   (Mesh3D          &mesh, const CameraFixture &ps   , double scale);
    void drawFixtureEx (Mesh3DDecorated &mesh, const CameraFixture &ps   , double scale);

    void drawScenePoint    (Mesh3D &mesh, const SceneFeaturePoint &fp, double scale);
    void drawSceneGeometry (Mesh3D &mesh, const FixtureSceneGeometry &fg, double scale);


    void drawScene  (Mesh3D          &mesh, const FixtureScene &scene, double scale = 1.0);
    void drawScene  (Mesh3DDecorated &mesh, const FixtureScene &scene, double scale = 1.0);


};

} // namespace corecvs

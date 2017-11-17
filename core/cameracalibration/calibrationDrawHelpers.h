#pragma once

#include "core/buffers/rgb24/rgbColor.h"
#include "core/xml/generated/calibrationDrawHelpersParameters.h"

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

    static RGBColor palette[6];


#if 0
    bool    printNames;
    bool    privateColor;
    bool    largePoints;

    /* */
    bool    drawFixtureCams;
    bool    drawObservations;
    bool    drawRays;
#endif

    CalibrationDrawHelpers()
#if 0
      : printNames(false)
      , privateColor(false)
      , largePoints(false)
      , drawFixtureCams(true)
      , drawObservations(false)
      , drawRays(false)
#endif
    {}

    void setParameters(const CalibrationDrawHelpersParameters &params);

    void drawCamera (Mesh3D &mesh, const CameraModel &cam, double scale);

    void drawPly    (Mesh3D &mesh, const ObservationList &list);
    void drawPly    (Mesh3D &mesh, const CameraFixture &ps   , double scale);
    void drawPly    (Mesh3D &mesh, const SceneFeaturePoint &fp, double scale);
    void drawPly    (Mesh3D &mesh, const FixtureSceneGeometry &fg, double scale);


    void drawScene  (Mesh3D &mesh, const FixtureScene &scene, double scale = 1.0);
};

} // namespace corecvs

#pragma once

#include "core/buffers/rgb24/rgbColor.h"

namespace corecvs {

class Mesh3D;
class CameraModel;
class Photostation;
class ObservationList;

class SceneFeaturePoint;
class FixtureScene;
class CameraFixture;

class CalibrationHelpers
{
public:

    enum RenderStyle {
        COLOR_PER_CAM,
        COLOR_PER_STATION
    };

    static RGBColor palette[];

    bool    printNames;
    bool    privateColor;
    bool    largePoints;

    /* */
    bool    drawFixtureCams;
    bool    drawObservations;
    bool    drawRays;

    CalibrationHelpers()
      : printNames(false)
      , privateColor(false)
      , largePoints(false)
      , drawFixtureCams(true)
      , drawObservations(false)
      , drawRays(false)
    {}

    void drawCamera (Mesh3D &mesh, const CameraModel &cam, double scale);

    void drawPly    (Mesh3D &mesh, const Photostation &ps, double scale = 50.0);
    void drawPly    (Mesh3D &mesh, const ObservationList &list);
    void drawPly    (Mesh3D &mesh, const Photostation &ps, const ObservationList &list, double scale);
    void drawPly    (Mesh3D &mesh, const CameraFixture &ps, double scale);

    void drawPly    (Mesh3D &mesh, corecvs::SceneFeaturePoint fp, double scale);

    void drawScene  (Mesh3D &mesh, const FixtureScene &scene, double scale = 1.0);
};

} // namespace corecvs

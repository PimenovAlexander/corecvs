#include "gcodeToSceneInterpreter.h"
#include <core/buffers/rgb24/abstractPainter.h>
#include <core/geometry/rectangle.h>

using namespace std;
using namespace corecvs;

GcodeToSceneInterpreter::GcodeToSceneInterpreter(QGraphicsScene *scene) : scene(scene) {}

bool GcodeToSceneInterpreter::straightHook(int type, const GCodeInterpreter::MachineState &before,
                                           const GCodeInterpreter::MachineState &after) {
    if (type == 1)
        scene->addLine(before.position.x(), - before.position.y(), after.position.x(), - after.position.y());
    return true;
}

bool GcodeToSceneInterpreter::arkHook(const GCodeInterpreter::MachineState &before,
                                      const GCodeInterpreter::MachineState &after, const PlaneFrame &frame,
                                      double maxArg) {
    Vector3dd currentPosition = before.position;

    double arg = 0;
    int arcSteps = 100;
    for (int i = 0; i <= arcSteps; i++) {
        arg += maxArg / arcSteps;
        Vector3dd subTarget = frame.p1 + frame.e1 * cos(arg) + frame.e2 * sin(arg);
        scene->addLine(currentPosition.x(), - currentPosition.y(), subTarget.x(), - subTarget.y());
        currentPosition = subTarget;
    }

    scene->addLine(currentPosition.x(), - currentPosition.y(), after.position.x(), - after.position.y());
    return true;
}

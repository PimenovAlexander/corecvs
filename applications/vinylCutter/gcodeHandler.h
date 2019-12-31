#ifndef CORECVS_GCODEHANDLER_H
#define CORECVS_GCODEHANDLER_H

#include <fstream>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDebug>

#include "core/geometry/mesh3d.h"
#include "core/fileformats/gcodeLoader.h"

using namespace std;
using namespace corecvs;

class GcodeHandler {
    class GcodeTransformer : public GCodeInterpreter {
    public:
        bool gcodeHook(const GCodeProgram::Code &c);
        GCodeProgram result;

        GcodeTransformer(double xShift, double yShift);
    private:
        double xShift = 0;
        double yShift = 0;
    };

public:
    void drawMesh(QGraphicsScene *scene);
    static GcodeHandler *getInstance();
    void loadGcode(std::string filePath);
    void saveGcode(std::string filePath);
    void applyShift(double xShift, double yShift);
    void compensateDragKnife(double offset, double touchZ);

private:
    GcodeHandler();
    static GcodeHandler *instance;
    GCodeProgram program;
    GcodeLoader loader;
};



#endif //CORECVS_GCODEHANDLER_H

#ifndef CORECVS_GCODEHANDLER_H
#define CORECVS_GCODEHANDLER_H

#include <cfloat>
#include <vector>
#include <QGraphicsScene>
#include <core/fileformats/gcodeLoader.h>


using namespace std;
using namespace corecvs;

class GcodeHandler {
    class GcodeTransformer : public GCodeInterpreter {
    public:
        GcodeTransformer(double xShift, double yShift);
        double initXShift = DBL_MAX;
        double initYShift = DBL_MAX;
        GCodeProgram result;

    private:
        bool gcodeHook(const GCodeProgram::Code &c);
        double xShift = 0;
        double yShift = 0;
        bool gcodeEndFlag = false;
    };

public:
    void loadGcode(string filePath);
    void drawMesh(QGraphicsScene *scene);
    void saveGcode(string filePath);
    void compensateDragKnife(double offset, double touchZ);
    Vector2dd applyShift(double xShift, double yShift);
    static GcodeHandler *getInstance();

private:
    GcodeHandler();
    GCodeProgram program;
    GcodeLoader loader;
    static GcodeHandler *instance;
};


#endif //CORECVS_GCODEHANDLER_H

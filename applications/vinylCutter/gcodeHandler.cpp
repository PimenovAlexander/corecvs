#include <gcodeHandler.h>
#include <gcodeToSceneInterpreter.h>
#include <QDebug>


GcodeHandler::GcodeHandler() {}

void GcodeHandler::drawMesh(QGraphicsScene *scene) {
//    corecvs::Mesh3D mesh;
//    corecvs::GCodeToMesh renderer;
//    renderer.renderToMesh(program, mesh);
//    for (auto edge : mesh.edges) {
//        auto v1 = mesh.vertexes[edge.x()];
//        auto v2 = mesh.vertexes[edge.y()];
//        scene->addLine(v1.x(), v1.y(), v2.x(), v2.y());
//    }
    GcodeToSceneInterpreter sceneInterpreter(scene);
    sceneInterpreter.executeProgram(program);
}

GcodeHandler *GcodeHandler::instance = 0;

GcodeHandler *GcodeHandler::getInstance() {
    if (instance == 0) {
        instance = new GcodeHandler();
    }

    return instance;
}

void GcodeHandler::loadGcode(std::string filePath) {
    ifstream file;
    file.open(filePath, ios::in);
    if (file.fail()) {
        qDebug() << "ERROR: Can't open input gcode file \n", filePath;
    }

    loader.loadGcode(file, program);
}

Vector2dd GcodeHandler::applyShift(double xShift, double yShift) {
    GcodeTransformer transformer(xShift, yShift);
    transformer.executeProgram(program);
    program = transformer.result;

    Vector2dd shift;
    shift.x() = transformer.initXShift;
    shift.y() = transformer.initYShift;
    return shift;
}

void GcodeHandler::compensateDragKnife(double offset, double touchZ) {
    GCodeCompensator compensator;
    compensator.compensateDragKnife(program, offset, touchZ);
    program = compensator.result;
}

void GcodeHandler::saveGcode(std::string filePath) {
    ofstream file;
    file.open(filePath, ios::out);
    if (file.fail()) {
        qDebug() << "ERROR: Can't open output gcode file \n", filePath;
    }

    loader.saveGcode(file, program);
}

bool GcodeHandler::GcodeTransformer::gcodeHook(const corecvs::GCodeProgram::Code &c) {
    corecvs::GCodeProgram::Code code = c;

    if ((code.area == 'm') && (code.number == 5)) gcodeEndFlag = true;

    if (not gcodeEndFlag) {
        if (code.hasParameter('x')) {
            double xVal = code.getParameter('x');
            code.setParameter('x', xVal + xShift);
            xVal = code.getParameter('x');
            if (initXShift > xVal) initXShift = xVal;
        }

        if (code.hasParameter('y')) {
            double yVal = code.getParameter('y');
            code.setParameter('y', yVal + yShift);
            yVal = code.getParameter('y');
            if (yVal < initYShift) initYShift = yVal;
        }
    }

    result.program.push_back(code);
    return true;
}

GcodeHandler::GcodeTransformer::GcodeTransformer(double xShift, double yShift) : xShift(xShift),
                                                                                 yShift(yShift),
                                                                                 gcodeEndFlag(false) {};
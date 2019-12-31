#include <gcodeHandler.h>
#include <gcodeToSceneInterpreter.h>


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

void GcodeHandler::applyShift(double xShift, double yShift) {
    GcodeTransformer transformer(xShift, yShift);
    transformer.executeProgram(program);
    program = transformer.result;
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

    if (code.hasParameter('x') && code.hasParameter('y')) {
        double xVal = code.getParameter('x');
        double yVal = code.getParameter('y');

        code.setParameter('x', xVal + xShift);
        code.setParameter('y', yVal + yShift);
    }

    result.program.push_back(code);
    return true;
}

GcodeHandler::GcodeTransformer::GcodeTransformer(double xShift, double yShift) : xShift(xShift),
                                                                                 yShift(yShift) {};
#ifndef CORECVS_GCODETOSVGINTERPRETER_H
#define CORECVS_GCODETOSVGINTERPRETER_H

#include <QGraphicsScene>
#include <core/fileformats/gcodeLoader.h>


using namespace corecvs;

class GcodeToSceneInterpreter : public GCodeInterpreter {
public:
    virtual bool straightHook(int type, const MachineState &before, const MachineState &after) override;

    virtual bool arkHook(const MachineState &before, const MachineState &after, const corecvs::PlaneFrame &frame,
                         double maxArg) override;

    GcodeToSceneInterpreter(QGraphicsScene *scene);

private:
    QGraphicsScene *scene;
};

#endif //CORECVS_GCODETOSVGINTERPRETER_H

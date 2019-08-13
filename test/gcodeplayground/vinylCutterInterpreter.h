#ifndef VINYLCUTTERINTERPRETER_H
#define VINYLCUTTERINTERPRETER_H

#include "core/fileformats/gcodeLoader.h"


class VinylCutterInterpreter : public corecvs::GCodeInterpreter {

public:
    corecvs::GCodeProgram result;

    double knifeOffset;
    double touchZ = 0.5;

    corecvs::Vector2dd tangent;



    // GCodeInterpreter interface
public:
    virtual bool straightHook(int type, const MachineState &before, const MachineState &after) override;
    virtual bool arkHook(const MachineState &before, const MachineState &after, const corecvs::PlaneFrame &frame, double maxArg) override;

};
#endif // VINYLCUTTERINTERPRETER_H

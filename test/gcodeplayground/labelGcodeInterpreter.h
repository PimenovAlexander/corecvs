#ifndef LABELGCODEINTERPRETER_H
#define LABELGCODEINTERPRETER_H

#include "core/fileformats/gcodeLoader.h"

#include <core/buffers/rgb24/rgb24Buffer.h>
#include <core/fileformats/svgLoader.h>
#include <core/geometry/rectangle.h>

class LabelGcodeInterpreter : public corecvs::GCodeInterpreter
{
public:
    double scale = 0.25;

    std::string part_label;
    std::string label;
    corecvs::RGB24Buffer * canvas = NULL;
    corecvs::SvgGroup *svg = NULL;
    corecvs::Rectangled rectangle;

    virtual void executeProgram(const corecvs::GCodeProgram &program)  override;


    virtual bool commentHook  (const std::string &text) override;

    virtual bool straightHook(int type, const MachineState &before, const MachineState &after) override;
    virtual bool arkHook(const MachineState &before, const MachineState &after, const corecvs::PlaneFrame &frame, double maxArg) override;


    LabelGcodeInterpreter();

    void labelObject();
};

#endif // LABELGCODEINTERPRETER_H

#include "vinylCutterInterpreter.h"

/* This in heavily unfinished */

using namespace corecvs;

bool VinylCutterInterpreter::straightHook(int type, const GCodeInterpreter::MachineState &before, const GCodeInterpreter::MachineState &after)
{
    CORE_UNUSED(type);
    CORE_UNUSED(before);
    CORE_UNUSED(after);

#if  0
    if (after.position.z() < touchZ)
    {

        double len = (after.position - before.position).l2Metric();
        Vector2dd moveTangent = (after.position - before.position).normalised().xy();
    }
#endif


    return true;
}

bool VinylCutterInterpreter::arkHook(const GCodeInterpreter::MachineState &before, const GCodeInterpreter::MachineState &after, const PlaneFrame &frame, double maxArg)
{
    CORE_UNUSED(before);
    CORE_UNUSED(after);
    CORE_UNUSED(frame);
    CORE_UNUSED(maxArg);

    SYNC_PRINT(("VinylCutterInterpreter::arkHook(): not supported\n"));

    return true;
}

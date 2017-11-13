/**
 * \file rgbColor.cpp
 * \brief Add Comment Here
 *
 * \date Dec 4, 2011
 * \author alexander
 */
#include "core/utils/global.h"
#include "core/buffers/rgb24/rgbColor.h"

namespace corecvs {

/**
 *  Looks extremely unsafe because it depends on the order of static initialization.
 *  Should check standard if this is ok
 **/
Reflection RGBColor::reflect;
int RGBColor::dummy = RGBColor::staticInit();

int RGBColor::staticInit()
{
    reflect.name = ReflectionNaming(
        "RGBColor",
        "RGBColor",
        ""
    );

    reflect.objectSize = sizeof(RGBColor);

    reflect.fields.push_back(new IntField(FIELD_R, 0, "r"));
    reflect.fields.push_back(new IntField(FIELD_G, 0, "g"));
    reflect.fields.push_back(new IntField(FIELD_B, 0, "b"));
    reflect.fields.push_back(new IntField(FIELD_A, 0, "a"));
    return 0;
}

RGBColor RGBColor::rainbow(double x)
{
    x *= 6;
    x = clamp(x, 0., 6.); 

    switch ((int)x)
    {
        case 0:
            return lerpColor(   Red(), Orange(), x);
        case 1:
            return lerpColor(Orange(), Yellow(), x - 1);
        case 2:
            return lerpColor(Yellow(),  Green(), x - 2);
        case 3:
            return lerpColor( Green(),   Blue(), x - 3);
        case 4:
            return lerpColor(  Blue(), Indigo(), x - 4);
        case 5:
        default:
            return lerpColor(Indigo(), Violet(), x - 5);
    }
}

} //namespace corecvs

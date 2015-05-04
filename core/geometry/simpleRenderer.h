#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

#include "matrix44.h"


namespace corecvs {

class SimpleRenderer
{
public:
    SimpleRenderer();


    Matrix44 modelviewMatrix;

};

} // namespace corecvs

#endif // SIMPLERENDERER_H

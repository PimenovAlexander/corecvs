#ifndef LIBPCLWRAPPER_H
#define LIBPCLWRAPPER_H

#include <vector>
#include <core/math/vector/vector3d.h>
#include <core/math/matrix/matrix44.h>

class LibPCLWrapper
{
public:

    bool trace = false;

    LibPCLWrapper(bool trace = false) :
        trace(trace)
    {}

    corecvs::Matrix44 legacyCall(
        std::vector<corecvs::Vector3dd> &scene,  /* Would search in here */
        std::vector<corecvs::Vector3dd> &sample, /* Would match this sample */
        double *cost = NULL);

#if 0
    corecvs::Matrix44 legacyCallPlane(
        std::vector<corecvs::Vector3dd> &scene,  /* Would search in here */
        std::vector<corecvs::Vector3dd> &sample, /* Would match this sample */
        double *cost = NULL);
#endif


};

#endif // LIBPCLWRAPPER_H

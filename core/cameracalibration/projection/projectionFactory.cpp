#include "projectionFactory.h"

namespace corecvs {

/* FACTORY */

CameraProjection *ProjectionFactory::projectionById(const ProjectionType::ProjectionType &projection)
{
    switch (projection) {
    case  ProjectionType::PINHOLE:
    default:
        return new PinholeCameraIntrinsics();
    case  ProjectionType::EQUIDISTANT:
        return new EquidistantProjection();
    case  ProjectionType::OMNIDIRECTIONAL:
        return new OmnidirectionalProjection();
    case  ProjectionType::STEREOGRAPHIC:
        return new StereographicProjection();
    case  ProjectionType::EQUISOLID:
        return new EquisolidAngleProjection();
    }
    return NULL;
}

Reflection *ProjectionFactory::reflectionById(const ProjectionType::ProjectionType &projection)
{
    switch (projection) {
    case  ProjectionType::PINHOLE:
    default:
        return &PinholeCameraIntrinsics::reflection;
    case  ProjectionType::EQUIDISTANT:
        return &EquidistantProjection::reflection;
    case  ProjectionType::OMNIDIRECTIONAL:
        return &OmnidirectionalProjection::reflection;
    case  ProjectionType::STEREOGRAPHIC:
        return &StereographicProjection::reflection;
    case  ProjectionType::EQUISOLID:
        return &EquisolidAngleProjection::reflection;
    }
    return NULL;
}


} //  namespace corecvs




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

const Reflection *ProjectionFactory::reflectionById(const ProjectionType::ProjectionType &projection)
{
    switch (projection) {
    case  ProjectionType::PINHOLE:
    default:
        return PinholeCameraIntrinsics::getReflection();
    case  ProjectionType::EQUIDISTANT:
        return EquidistantProjection::getReflection();
    case  ProjectionType::OMNIDIRECTIONAL:
        return OmnidirectionalProjection::getReflection();
    case  ProjectionType::STEREOGRAPHIC:
        return StereographicProjection::getReflection();
    case  ProjectionType::EQUISOLID:
        return EquisolidAngleProjection::getReflection();
    }
    return NULL;
}


} //  namespace corecvs




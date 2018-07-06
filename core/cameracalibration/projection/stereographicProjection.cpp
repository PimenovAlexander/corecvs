#include "stereographicProjection.h"

namespace corecvs {

StereographicProjection::StereographicProjection() :
    CameraProjection(ProjectionType::STEREOGRAPHIC)
{}

StereographicProjection::StereographicProjection(const Vector2dd &principal, double focal, const Vector2dd &size) :
    ProjectionBaseParameters(principal.x(), principal.y(), focal, size.x(), size.y(), size.x(), size.y()),
    CameraProjection(ProjectionType::STEREOGRAPHIC)
{}

StereographicProjection::~StereographicProjection()
{

}

} // namespace corecvs ;


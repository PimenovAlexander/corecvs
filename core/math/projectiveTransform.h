/**
 * \file projectiveTransform.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Feb 25, 2010
 * \author alexander
 */

#ifndef PROJECTIVETRANSFORM_H_
#define PROJECTIVETRANSFORM_H_

#include "deformMap.h"
#include "abstractBuffer.h"
#include "matrix33.h"
#include "vector2d.h"
namespace corecvs {

/**
 * Projective transform is a wrapper around Matrix33 class
 * that allows to implement the projective deformation of the
 * Abstract Continuous Buffer
 *
 */
class ProjectiveTransform : public Matrix33, public DeformMap<int32_t, double>
{
public:
    ProjectiveTransform() : Matrix33() {}
    ProjectiveTransform(double _A) : Matrix33(_A) {}
    ProjectiveTransform(const double* data) : Matrix33(data) {}
    ProjectiveTransform(const ProjectiveTransform &_M) : Matrix33(_M) {}
    ProjectiveTransform(const Matrix33 &_M) : Matrix33(_M) {}

    ProjectiveTransform(const Vector3dd &_V1,const Vector3dd &_V2,const Vector3dd &_V3) :
        Matrix33 (_V1,_V2,_V3) {}

    inline Vector2dd map(int32_t y, int32_t x) const
    {
        return (Vector2dd)((*this) * Vector2dd(x, y));
    }

    inline Vector2dd map(double y, double x) const
    {
        return (Vector2dd)((*this) * Vector2dd(x, y));
    }

    inline Vector2dd map(const Vector2dd &v) const
    {
        return (Vector2dd)((*this) * v);
    }

    inline Vector2dd operator()(const Vector2dd &v) const
    {
        return map(v);
    }

    ~ProjectiveTransform(){}
};


} //namespace corecvs
#endif /* PROJECTIVETRANSFORM_H_ */


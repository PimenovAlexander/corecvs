#ifndef COVMATRIX22_H
#define COVMATRIX22_H

#include "core/math/vector/vector3d.h"
#include "core/math/matrix/matrix22.h"

namespace corecvs {

template<typename ElementType>
class GenericCovMatrix22 : public Vector3d<ElementType> {
public:

    GenericCovMatrix22() {}

    GenericCovMatrix22(const ElementType &value = 0.0) : Vector3d<ElementType>(value, 0.0, value)
    {}

    GenericCovMatrix22(const ElementType &xx, const ElementType &xy, const ElementType &yy) : Vector3d<ElementType>(xx, xy, yy) {}

    ElementType &xx() {
        return this->x();
    }

    ElementType &xy() {
        return this->y();
    }

    ElementType &yy() {
        return this->z();
    }

    const ElementType &xx() const {
        return this->x();
    }

    const ElementType &xy() const {
        return this->y();
    }

    const ElementType &yy() const {
        return this->z();
    }

    ElementType det() const {
        return (xx() * yy() - xy() * xy());
    }

    ElementType trace() const
    {
        return (xx() + yy());
    }


    void addVector(const Vector2d<ElementType> &vector)
    {
        xx() += vector.x() * vector.x();
        xy() += vector.x() * vector.y();
        yy() += vector.y() * vector.y();
    }

    void addVector(const ElementType &vx, const ElementType &vy)
    {
        xx() += vx * vx;
        xy() += vx * vy;
        yy() += vy * vy;
    }

    void eigen(ElementType &l1, Vector2d<ElementType> &v1, ElementType &l2, Vector2d<ElementType> &v2, ElementType EIGTOLERANCE = 1e-9) const
    {
        if (std::fabs(xy()) > EIGTOLERANCE)
        {
            /** Charactristic polynomial
                    l^2 - (xx() + yy()) * l + (xx() * yy() - xy() * xy()) = 0
                                 B                         C              = 0
             **/
            ElementType B  = trace();
            ElementType B2 = B / 2;
            ElementType C  = det();

            ElementType D = B2 * B2 - C;
            if (D < 0) D = 0;  /* Covarience matrix is always normal and positively defined */
            ElementType sD = sqrt(D);

            ElementType l_small = B2 - sD;
            ElementType l_big   = B2 + sD;

            /** Now we have two eigenvalues **/
            v1.x() = xy();
            v1.y() = l_big - xx();

            v2.x() = xy();
            v2.y() = l_small - xx();

            l1 = l_big;
            l2 = l_small;
        } else {
            if (std::fabs(xx()) > std::fabs(yy()))
            {
                l1 = xx();
                v1 = Vector2d<ElementType>::OrtX();
                l2 = yy();
                v2 = Vector2d<ElementType>::OrtY();
            } else {
                l1 = yy();
                v1 = Vector2d<ElementType>::OrtY();
                l2 = xx();
                v2 = Vector2d<ElementType>::OrtX();
            }

        }
    }

    /**
     * Comuting eigen values and vectors
     *
     * \param A              - input matrix
     * \param lambda1        - first eigenvalue
     * \param e1             - first eigenvector
     * \param lambda2        - second eigenvalue
     * \param e2             - second eigenvector
     * \param EIGTOLERANCE
     **/
    static void eigen(const GenericCovMatrix22 &A,
                      ElementType &lambda_max, Vector2d<ElementType> &e1,
                      ElementType &lambda_min, Vector2d<ElementType> &e2, ElementType EIGTOLERANCE = 1e-9)
    {
        A.eigen(lambda_max, e1, lambda_min, e2, EIGTOLERANCE);
    }


    GenericMatrix22<ElementType> toMatrix22() const {
        return GenericMatrix22<ElementType>(xx(), xy(), xy(), yy());
    }

    friend std::ostream & operator <<(std::ostream &out, const GenericCovMatrix22 &matrix)
    {
        out << matrix.toMatrix22() << std::endl;
        return out;
    }


};


typedef GenericCovMatrix22<float > CovMatrix22f;
typedef GenericCovMatrix22<double> CovMatrix22d;


} // namespace corecvs

#endif // COVMATRIX22_H

#ifndef PROJECTIONMODELS_H
#define PROJECTIONMODELS_H

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"


namespace corecvs {

class CameraProjection {
public:
    /**
     * Returns 2d image point of the 3d target object
     *
     * If point is not visible result is undefined
     **/
    virtual Vector2dd   project(const Vector3dd &p) const = 0;

    /**
     * Returns 3d vector that 2d image point of the 3d target object
     *
     * If point is not visible result is undefined
     **/
    virtual Vector3dd   reverse(const Vector2dd &p) const = 0;


    virtual bool      isVisible(const Vector3dd &p) const = 0;

    /**
     *  Optional inteface part that automates projection usage as function
     **/
    template<class ElementType>
    static ElementType rayToAngle(const Vector3d<ElementType> &ray)
    {
        return atan2(ray.xy().l2Metric(), ray.z());
    }

    virtual ~CameraProjection();
};



class StereographicProjection : public CameraProjection {
public:
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    StereographicProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}

public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {

        double tau = rayToAngle<double>(p);
        Vector2dd dir = p.xy().normalised();
        return dir * 2 * focal * tan(tau / 2);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = 2 * atan2(r / 2, focal);
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }

    virtual ~StereographicProjection();
};

/**
 *   Equidistant projection
 *
 **/
template<class ElementType>
class GenericEquidistantProjection : public CameraProjection {
public:
    enum {
        PRINCIPAL_X,
        PRINCIPAL_Y,
        FOCAL,
        PARAMTER_LAST
    };

    ElementType parameter[PARAMTER_LAST];

    /** Focal length */
    ElementType focal() const
    {
        return parameter[FOCAL];
    }

    /** Principal point of optical axis on image plane (in pixel). Usually center of imager */

    Vector2d<ElementType> principal() const
    {
        return Vector2d<ElementType>(parameter[PRINCIPAL_X], parameter[PRINCIPAL_Y]);
    }

    void project(const Vector3d<ElementType> &in, Vector2d<ElementType> out) const
    {
        ElementType tau = rayToAngle(in);
        Vector2d<ElementType> dir = in.xy().normalised();
        out = dir * focal() * tau + principal();
    }

    void reverse(const Vector2d<ElementType> &in, Vector3d<ElementType> out) const
    {
        Vector2d<ElementType> shift = in - principal();
        ElementType r = shift.l2Metric();
        shift /= r;
        ElementType tau = r / focal();
        out = Vector3d<ElementType>(shift.normalised() * sin(tau), cos(tau));
    }
};



class EquidistantProjection : public GenericEquidistantProjection<double> {
public:
    EquidistantProjection(const Vector2dd &principal, double focal)
    {
        parameter[PRINCIPAL_X] = principal.x();
        parameter[PRINCIPAL_Y] = principal.y();
        parameter[FOCAL] = focal;
    }

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        Vector2dd res;
        GenericEquidistantProjection<double>::project(p, res);
        return res;
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector3dd res;
        GenericEquidistantProjection<double>::reverse(p, res);
        return res;
    }

    /**
     *  Optional inteface part that automates projection usage as function
     **/

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }
};

/**
 * Equisolid projection
 **/

class EquisolidAngleProjection : public CameraProjection {
public:
    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    EquisolidAngleProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        double tau = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * 2 * focal * sin(tau / 2);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = 2 * asin(r / 2.0 / focal);
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }
};

class OrthographicProjection : public CameraProjection {
public:

    Vector2dd principal;        /**< Principal point of optical axis on image plane (in pixel). Usually center of imager */
    double    focal;            /**< Focal length */

    OrthographicProjection(Vector2dd principal, double focal) :
        principal(principal),
        focal(focal)
    {}

    // CameraProjection interface
public:
    virtual Vector2dd project(const Vector3dd &p) const override
    {
        double tau = rayToAngle(p);
        Vector2dd dir = p.xy().normalised();
        return dir * focal * sin(tau);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Vector2dd shift = p - principal;
        double r = shift.l2Metric();
        shift /= r;
        double tau = asin(r / focal);
        return Vector3dd(shift * tau, 1.0).normalised();
    }

    /* TODO: Function not actually implemented */
    virtual bool isVisible(const Vector3dd &/*p*/) const override
    {
        return false;
    }
};


#if 0
class CatadioptricProjection
{
public:
    Vector2dd principal;
    double    focal;


    virtual Vector2dd project(const Vector3dd &p) const override
    {
        Vector3d n =
            point.squaredNorm() == 0.0 ? Vector3d(0, 0, 1) : point.normalized();
        double rho_tilde = n.template block<2, 1>(0, 0).norm();
        int polyN_real = polyN;
        for (polyN_real = polyN - 1; polyN_real >= 0; --polyN_real)
          if (poly[polyN_real != 0])
            break;
        polyN_real += 1;
        VectorXd poly_g(polyN_real + 2);
        /// poly w.r.t. gamma
        poly_g[0] = 1;
        poly_g[1] = -n[2];
        for (int i = 2; i < 2 + polyN_real; ++i) {
          poly_g[i] = poly[i - 2] * std::pow(rho_tilde, i);
        }
        poly_g /= poly_g[polyN_real + 1];

        Eigen::MatrixXd companion(polyN_real + 1, polyN_real + 1);
        companion.setZero();
        for (int i = 0; i < polyN_real + 1; ++i) {
          companion(i, polyN_real) = -poly_g[i];
          if (i == 0)
            continue;
          companion(i, i - 1) = 1.0;
        }
        double bestRoot = std::numeric_limits<double>::max();
        VectorXcd roots = companion.eigenvalues();
        for (int i = 0; i < roots.rows(); ++i) {
          if (roots[i].real() < 0.0)
            continue;
          if (std::abs(roots[i].imag()) > 0.0) // 1e-2 * roots[i].real())
            continue;
          bestRoot = std::min(bestRoot, roots[i].real());
        }

        if (!std::isfinite(bestRoot)) {
          bestRoot = 1.0;
        }
        return n.template block<2, 1>(0, 0) * bestRoot * intrinsics[0] +
               Vector2d(intrinsics[1], intrinsics[2]);
    }

    virtual Vector3dd reverse(const Vector2dd &p) const override
    {
        Map<const SE3<T>> camToWorld(pose);
        Map<const Eigen::Matrix<T, 3, 1>> intrinsics(intrinsic);
        Map<const Eigen::Matrix<T, -1, 1>> poly(catadioptric, polyN);
        Map<Eigen::Matrix<T, 3, 1>> res(residual);

        typedef Eigen::Matrix<T, 3, 1> Vector3;
        typedef Eigen::Matrix<T, 2, 1> Vector2;
        T cx(intrinsic[1]), cy(intrinsic[2]), fx(intrinsic[0]);

        Vector3 world = worldPoint.cast<T>();
        Vector3 camera = (camToWorld.inverse() * world).normalized();

        Vector2 proj = projection.cast<T>();
        proj[0] -= cx;
        proj[1] -= cy;
        proj /= fx;
        T rho = proj.norm();
        T rho2 = proj.squaredNorm();

        Eigen::Matrix<T, -1, 1> polyDeg = poly;
        polyDeg[0] = rho2;
        for (int i = 1; i < polyN; i += 2) {
          polyDeg[i] = polyDeg[i - 1] * rho;
          if (i + 1 < polyN)
            polyDeg[i + 1] = rho2 * polyDeg[i - 1];
        }
        T f(1.0);
        for (int i = 0; i < polyN; ++i)
          f += polyDeg[i] * poly[i];

        Vector3 cameraRef = Vector3(proj[0], proj[1], f).normalized();

        res = cameraRef - camera;
        return true;
    }

};
#endif

} // namespace corecvs


#endif // PROJECTIONMODELS_H


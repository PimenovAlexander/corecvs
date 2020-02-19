#ifndef OMNIDIRECTIONALPROJECTION_H
#define OMNIDIRECTIONALPROJECTION_H

#include "polynomial/polynomial.h"
#include "polynomial/polynomialSolver.h"

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "function/function.h"

#include "cameracalibration/projection/projectionModels.h"
#include "xml/generated/projectionBaseParameters.h"
#include "xml/generated/omnidirectionalBaseParameters.h"


namespace corecvs{

template<class ElementType>
class GenericOmnidirectionalProjection : public CameraProjection {



};

/**
 *  Omnidirectional Projection
 *
 *  Point in the image if formed by the ray which parameters are computed as follows
 *
 *  First direction from the principle point is estimated. It is then scaled by "focal distance".
 *  Distance to the principal point would affect the z component of the ray directon
 *
 *  \f[ \overrightarrow {q} = (\overrightarrow{p} - \overrightarrow{p}_{principal})/f \f]
 *  \f[ r = \vert \overrightarrow {q} \vert = q_x^2 + q_y^2 \f]
 *
 *  After this a polinom is computed. First coeficients is 1.0 second one is 0.0 - to keep smoothness at zero
 *
 *  \f[ z = 1 + 0 \cdot r + n_0 r^2 + n_1 r^3 + \cdots + n_i r^{i+2} \f]
 *
 *  Then the output ray is created
 *
 *  \f[ \overrightarrow d = \overrightarrow {(q_x, q_y, z)} \f]
 *  \f[ {\overrightarrow d_n} = {{\overrightarrow {d}} \over {\vert \overrightarrow{d} \vert }} \f]
 *
 *
 *  Omnidirectional Projection is quite universal.
 *  For example ortographic projection is particular case of Omnidirectional.
 *
 *
 *  \f[ d_n = (q_x, q_y, \sqrt {1 - q_x^2 - q_y ^ 2} ) \f]
 *
 *  \f[ \sqrt {1 - q_x^2 - q_y ^ 2} = 1 - {x^2 \over 2} - {x^4 \over 8} - {x^6 \over 16} - {5x^6 \over 128} + \cdots \f]
 *
 *
 *  Thus we had described reverse projection, now we have to consider forward transformation
 *  generaly we need to intersect the ray with polynome in 2d
 *
 *   \f[ \overrightarrow d_n = \overrightarrow {(x_r,y_r,z_r)} \f]
 *
 *  Ray \f$ z = ({{z_r} \over {\sqrt {(x_r^2, y_r^2)}}}) r \f$
 *  Polynom \f$ z = 1 + 0 \cdot r + n_0 r^2 + n_1 r^3 + \cdots + n_i r^{i+2} \f$
 *
 *  Subtracting and solving for \f$r\f$
 *
 *    \f[ 0 = 1 - (z_r / \sqrt{x_r^2 + y_r^2}) *r + n_0 r^2 + n_1 r^3 + \cdots + n_i r^{i+2} \f]
 *
 *  We expect an smallest real positive root to be the \f$r\f$ value we need. Finally
 *    \f[ p = \left({{x_r} \over {\sqrt {(x_r^2 + y_r^2)}}} r, {{y_r} \over {\sqrt {(x_r^2 + y_r^2)}}} r \right) \cdot f + \overrightarrow{p}_{principal} \f]
 *
 *
 **/

/*
 *  \f[ r = ((p_x - p_x^0)/f, (p_y - p_y^0)/f, 1.0 ) = (q_x, q_y, 1.0)\f]
*/

class OmnidirectionalProjection : public OmnidirectionalBaseParameters, public CameraProjection
{
public:

    OmnidirectionalProjection() :
        CameraProjection(ProjectionType::OMNIDIRECTIONAL)
    {}

    OmnidirectionalProjection(const Vector2dd &principal, double focal, const Vector2dd &size) :
        OmnidirectionalBaseParameters(principal.x(), principal.y(), focal, {} ,size.x(), size.y(), size.x(), size.y()),
        CameraProjection(ProjectionType::OMNIDIRECTIONAL)
    {

    }


    virtual Vector2dd project(const Vector3dd &p) const override
    {
        // SYNC_PRINT(("OmnidirectionalProjection::project(%lf %lf %lf):called\n", p.x(), p.y(), p.z()));

        Vector3dd norm = p.normalised();
        vector<double> coefs;
        vector<double> roots;

        coefs.reserve(mN.size() + 2);
        coefs.push_back(1.0);
        coefs.push_back(- norm.z() / norm.xy().l2Metric());
        coefs.insert(coefs.end(), mN.begin(), mN.end());

        while (coefs.back() == 0.0) {
            coefs.pop_back();
        }

        Polynomial poly(coefs);
        PolynomialSolver solver;
        solver.solve(poly, roots);

        // cout << "Polinom:" << poly << std::endl;
        // cout << "Roots:";
        double bestRoot = std::numeric_limits<double>::max();
        for (size_t i = 0; i < roots.size(); i++)
        {
           // cout << roots[i] << " ";
           if (roots[i] < 0)
               continue;
           bestRoot = std::min(bestRoot, roots[i]);
        }
        //cout << endl;

        return norm.xy().normalised() * bestRoot * focal() + principal();
    }


    virtual Vector3dd reverse(const Vector2dd &p) const override
    {   
        Vector2dd preDeform = (p - principal()) / focal();

        double d2 = preDeform.sumAllElementsSq();
        double d  = std::sqrt(d2);

        double f = 1.0;
        double x;

        /* We actually should add starting from higher powers (smaller value) */
        x = d2;
        for (size_t i = 0; i < mN.size(); i +=2)
        {
            f += x * mN[i];
            x *= d2;
        }

        x = d * d2;
        for (size_t i = 1; i < mN.size(); i +=2)
        {
            f += x * mN[i];
            x *= d2;
        }

        return Vector3dd(preDeform.x(), preDeform.y(), f).normalised();
    }

    virtual bool isVisible(const Vector3dd &p) const override
    {
        Vector2dd proj = project(p);

        if (!proj.isInRect(Vector2dd(0.0, 0.0), size()))
        {
            return false;
        }
        return true;
    }

    /**
     * Returns target image size
     **/
    virtual Vector2dd size() const override
    {
        return  Vector2dd(sizeX(), sizeY());
    }

    virtual Vector2dd distortedSize() const override
    {
        return  Vector2dd(distortedSizeX(), distortedSizeY());
    }

    virtual Vector2dd principal() const override
    {
        return  Vector2dd(principalX(), principalY());
    }

    /* Misc */
    virtual OmnidirectionalProjection *clone() const override
    {
        OmnidirectionalProjection *p = new OmnidirectionalProjection();
        *p = *this;
        return p;
    }

    virtual DynamicObjectWrapper getDynamicWrapper() override
    {
        return DynamicObjectWrapper(getReflection(), static_cast<OmnidirectionalBaseParameters *>(this));
    }




    virtual ~OmnidirectionalProjection() {}

};

}

#endif // OMNIDIRECTIONALPROJECTION_H

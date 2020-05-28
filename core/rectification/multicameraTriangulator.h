#ifndef MULTICAMERATRIANGULATOR_H
#define MULTICAMERATRIANGULATOR_H

#include <vector>

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "math/matrix/matrix.h"
#include "math/matrix/matrix33.h"
#include "math/matrix/matrix44.h"
#include "function/function.h"

namespace corecvs {

using corecvs::Vector2dd;
using corecvs::Vector3dd;
using corecvs::Matrix;
using corecvs::Matrix33;
using corecvs::Matrix44;

class MulticameraTriangulator
{
public:
    std::vector<Matrix44>  P;
    std::vector<Vector2dd> xy;

    bool trace    = false;
    bool hasError = true;

    MulticameraTriangulator()
    {}

    void addCamera(const Matrix44 &_P, const Vector2dd &_xy)
    {
        P.push_back(_P);
        xy.push_back(_xy);
    }

    /**
     * This is a mutiview triangulation.
     *
     * We have a set of following equations, \f$P_i\f$ represents the i-th camera matrix (3 x 4), \f$(u, v, t)\f$ is a homogenious vector in image plane, and \f$X = (X_x, X_y, X_z, X_w)\f$ is a 3D point
     * We know P matrices and \f$(x_i = u_i/t_i, y_i = v_i/t_i)\f$
     *
     * \f{eqnarray*}
     *   (u_1, v_1, t_1)^T = P_1 X \\
     *   (u_2, v_2, t_2)^T = P_2 X \\
     *    \cdots  = \cdots       \\
     *   (u_n, v_n, t_n)^T = P_n X
     * \f}
     *
     * We will form the LSE problem
     *
     * \f[ AY=0 \f]
     *
     * A matrix will have a form
     *
     *
     *   \f[ A =
     *    \left( \begin{array}{cccc|cccc}
     *        \multicolumn{4}{c|}{\multirow{3}{*}{$-P_1$}} &   x_1  &     0   & \cdots &    0   \\
     *                 &   &   &                           &   y_1  &     0   & \cdots &    0   \\
     *                 &   &   &                           &    1   &     0   & \cdots &    0   \\ \hline
     *        \multicolumn{4}{c|}{\multirow{3}{*}{$-P_2$}} &    0   &    x_2  & \cdots &    0   \\
     *                 &   &   &                           &    0   &    y_2  & \cdots &    0   \\
     *                 &   &   &                           &    0   &     1   & \cdots &    0   \\ \hline
     *          \cdots &   &   &                           & \cdots &  \cdots & \ddots & \cdots \\ \hline
     *        \multicolumn{4}{c|}{\multirow{3}{*}{$-P_n$}} &    0   &     0   & \cdots &   x_n  \\
     *                 &   &   &                           &    0   &     0   & \cdots &   y_n  \\
     *                 &   &   &                           &    0   &     0   & \cdots &    1
     *    \end{array} \right)
     *  \f]
     *
     * And a vector would be
     *
     * \f[ Y = \pmatrix {
     *        X_x  \cr
     *        X_y  \cr
     *        X_z  \cr
     *        X_w  \cr
     *        t_1  \cr
     *        t_2  \cr
     *        \cdots \cr
     *        t_n }
     * \f]
     *
     *  So each 3 line block will produce
     *
     * \f[
     *      -P_1  \pmatrix { X_x \cr X_y \cr X_z \cr X_w } + \pmatrix { x_1 t_1 \cr y_1 t_1 \cr t1 } = 0
     * \f]
     *
     *  An this will lead to
     *
     * \f[
     *      P_1  \pmatrix { X_x \cr X_y \cr X_z \cr X_w } = \pmatrix { u_1 \cr v_1 \cr t1 }
     * \f]
     *
     *
     **/
    Vector3dd triangulate(bool *ok = NULL);

    double reprojErrorAlgbra(const Vector3dd &input);

private:
    Matrix constructMatrix();
    void printUnitySums(const Matrix &A);

public:
    /**
     * Updates the guess with LM
     *
     *
     **/
    Vector3dd   triangulateLM(Vector3dd initialGuess, bool *ok = NULL);

    Matrix33    getCovarianceInvEstimation(const Vector3dd &at) const;

    std::vector<Vector2dd> reprojectionError(const Vector3dd &input);
    double                 reprojError(const Vector3dd &input);

    class CostFunction : public FunctionArgs
    {
    public:
        MulticameraTriangulator *mTriangulator;

        CostFunction(MulticameraTriangulator *triang)
            : FunctionArgs(Vector3dd::LENGTH, (int)triang->P.size() * 2)
            , mTriangulator(triang)
        {}

        virtual void operator()(const double in[], double out[]);

        virtual Matrix getJacobian(const double in[], double delta) override;
        virtual Matrix getLSQHessian(const double *in, double delta = 1e-5) override;
    };


    /* Ability to serialize and restore state */
    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
           visitor.visit(P , "matrices");
           visitor.visit(xy, "xy"      );
        }

    /**
     * Get number of measurements
     **/
    int getSize()
    {
        return (int)P.size();
    }

    /**
     * Returns a triangulator with the state holding a subset of the current one
     **/
    MulticameraTriangulator subset(const std::vector<bool> &mask);

};

} // namespace corecvs

#endif // MULTICAMERATRIANGULATOR_H

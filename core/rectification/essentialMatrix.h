#pragma once
/**
 * \file essentialMatrix.h
 * \brief Add Comment Here
 *
 * \date Oct 1, 2011
 * \author alexander
 */
#include "matrix33.h"
#include "vector3d.h"
#include "matrix.h"
#include "quaternion.h"
#include "line.h"
#include "correspondanceList.h"

namespace corecvs {


/**
 *  This class stores the relative position of first and second camera
 *
 *  It basically consist of the 2 operations - rotation and shift.
 *  \f[
 *   (x_{2}, y_{2}, z_{2}) = T R (x_{2}, y_{2}, z_{2})^T
 *  \f]
 *
 **/
class EssentialDecomposition
{
public:
	/* Rotation that is applied to the coordinate system prior to shift*/
    Matrix33  rotation;

	/* Shift that is applied after rotation */
    Vector3dd direction;

    EssentialDecomposition() :
        rotation(1.0),
        direction(1.0, 0.0, 0.0)
    {
    }

    EssentialDecomposition(const Matrix33 &_rotation, const Vector3dd &_direction) :
        rotation(_rotation),
        direction(_direction.normalised())
    {}

    /**
     *  Triangulation taking as input left and right 2D projective vectors of the pixel
     *  in left (\f$n_1\f$) and right (\f$n_2\f$) cameras and returning lengths of 3D-vectors
     *  of the pixel for left and right cameras. If the camera translation vector is \f$t\f$
     *  and 3D-vectors are \f$x_1\f$ and \f$x_2\f$ then
     *
     *  \f[ x_1 - x_2 = t \f]
     *
     *  or
     *
     *  \f[ s_1 n_1 - s_2 n_2 = t \f]
     *
     *  If the equation is not perfect then we have general decomposition of the
     *  vector \f$t\f$ in non-orthogonal basis
     *
     *  \f[ s_1 n_1 - s_2 n_2 + z [n_1 \times n_2] = t \f]
     *
     *  Making matrix out of columns \f$n_1\f$, \f$n_2\f$, \f$[n_1 \times n_2]\f$ we
     *  obtain non-homogeneous linear system in \f$d_1\f$, \f$d_2\f$, \f$z\f$.
     *
     *  \param left  [in]  Vector to the point in left  CAMERA frame
     *  \param right [in]  Vector to the point in right CAMERA frame
     *  \param s1    [out]  Scaler that will transform left vector to intersection point in LEFT  CAMERA frame
     *  \param s2    [out]  Scaler that will transform left vector to intersection point in RIGHT CAMERA frame
     *  \param err   [out]  Scaler that shows a residual error
     *
     *
     **/
    void getScaler(
            const Vector3dd &right,
            const Vector3dd &left,
            double &s1, double &s2, double &err) const
    {
        Vector3dd n1 = right;
        Vector3dd n2 = rotation * left;
        Matrix33 m = Matrix33::FromColumns(n1, -n2, n1 ^ n2);

        Vector3dd scaler = m.inv() * direction;
        s1  = scaler.x();
        s2  = scaler.y();
        err = scaler.z();
    }

    /**
     *  Triangulation taking as input left and right 2D projective vectors of the pixel
     *  in left (\f$n_1\f$) and right (\f$n_2\f$) cameras and returning lengths of 3D-vectors
     *  of the pixel for left and right cameras. If the camera translation vector is \f$t\f$
     *  and 3D-vectors are \f$x_1\f$ and \f$x_2\f$ then
     *
     *  \f[ x_1 - x_2 = t \f]
     *
     *  Considering triangle made by vectors \f$x_1\f$, \f$x_2\f$ and \f$t\f$ and making use of
     *  the sine law we obtain
     *
     *  \f[ ||x_1|| = \frac{||n_2 \times t||}{||n_1 \times n_2||}, \f]
     *  \f[ ||x_2|| = \frac{||n_1 \times t||}{||n_1 \times n_2||}, \f]
     *  \f[ z = (t, [\times n_1 \times n_2]). \f]
     *
     *  \param left  [in]  Vector to the point in left  CAMERA frame
     *  \param right [in]  Vector to the point in right CAMERA frame
     *  \param d1    [out]  Distance to intersection point in LEFT  CAMERA frame
     *  \param d2    [out]  Distance to intersection point in RIGHT CAMERA frame
     *  \param err   [out]  Scaler that shows a residual error
     *
     **/
    void getDistance(
            const Vector3dd &right,
            const Vector3dd &left,
            double &d1, double &d2, double &err) const
    {
        Vector3dd n1 = right.normalised();
        Vector3dd n2 = rotation * left.normalised();
        double t = (n1 ^ n2).l2Metric();

        d1 = (n2 ^ direction).l2Metric() / t;
        d2 = (n1 ^ direction).l2Metric() / t;
        err = direction & (n1 ^ n2).normalised();
    }

    /**
     *   Shortcut for Vector2dd of getScaler()
     **/
    void getScaler(
            const Vector2dd &right2d,
            const Vector2dd &left2d,
            double &s1, double &s2, double &err) const
    {
        Vector3dd right = Vector3dd(right2d, 1.0);
    	Vector3dd left  = Vector3dd(left2d , 1.0);
        getScaler(right, left, s1, s2, err);
    }

    /**
	 *   Shortcut for Vector2dd of getDistance()
	 **/
    void getDistance(
            const Vector2dd &right2d,
            const Vector2dd &left2d,
            double &d1, double &d2, double &err) const
    {
        Vector3dd right = Vector3dd(right2d, 1.0);
    	Vector3dd left  = Vector3dd(left2d , 1.0);
        getDistance(right, left, d1, d2, err);
    }

    /**
	 *   Shortcut for Correspondance of getScaler()
	 **/
    void getScaler(const Correspondance &corr, double &s1, double &s2, double &err) const
    {
        Vector3dd right = Vector3dd(corr.start.x(), corr.start.y(), 1.0);
    	Vector3dd left  = Vector3dd(corr.end  .x(), corr.end  .y(), 1.0);
        getScaler(right, left, s1, s2, err);
    }

    /**
	 *   Shortcut for Correspondance of getDistance()
	 **/
    void getDistance(const Correspondance &corr, double &d1, double &d2, double &err) const
    {
        Vector3dd right = Vector3dd(corr.start.x(), corr.start.y(), 1.0);
        Vector3dd left  = Vector3dd(corr.  end.x(), corr.  end.y(), 1.0);
        getDistance(right, left, d1, d2, err);
    }

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(rotation,  Matrix33 (1.0),         "rotation");
        visitor.visit(direction, Vector3dd(1.0,0.0,0.0), "direction");
    }

	friend ostream & operator <<(ostream &out, const EssentialDecomposition &edecomp)
    {
		Quaternion q = Quaternion::FromMatrix(edecomp.rotation);
		out << "This matrix is rotating " << radToDeg(q.getAngle()) << "deg around axis: ";
		out << q.getAxis() << endl;
		out << "And then shifting by vector:" << endl;
		out << edecomp.direction << endl;
		return out;
    }


};

/**
 *  Both essential and fundamental matrices hold the following property:
 *
 * \code
 *             [ a_11   a_12  a_13 ]  [ x']    [ 0 ]
 * [x, y ,1]   [ a_21   a_22  a_23 ]  [ y'] =  [ 0 ]
 *             [ a_31   a_32  a_33 ]  [ 1 ]    [ 0 ]
 * \endcode
 *
 * In case of Fundamental matrix \f$\pmatrix{ x & y & 1 }\f$ is a point in the image captured
 * by uncalibrated camera. In case of Essential Matrix \f$\pmatrix{ x & y & 1 }\f$ is a point
 * captured by normalized camera
 *
 * \f[
 *      \pmatrix{ x & y & 1 }
 *      \pmatrix{
 *        F_{1,1} & F_{1,2} & F_{1,3}\cr
 *        F_{2,1} & F_{2,2} & F_{2,3}\cr
 *        F_{3,1} & F_{3,2} & F_{3,3}}
 *      \pmatrix{ x' \cr y' \cr 1} = \pmatrix{ 0 \cr 0 \cr 0}
 * \f]
 *
 **/
class EssentialMatrix : public Matrix33
{
public:
    EssentialMatrix() : Matrix33(Matrix33::CrossProductLeft(Vector3dd(1.0,0.0,0.0))) {};
    EssentialMatrix(const Matrix33 &other) : Matrix33(other) {};

    /**
     * The inliers calculation is as follows:
     *  \f[
     *
     *      \pmatrix{ x & y & 1 }
     *      \pmatrix{
     *        F_{1,1} & F_{1,2} & F_{1,3}\cr
     *        F_{2,1} & F_{2,2} & F_{2,3}\cr
     *        F_{3,1} & F_{3,2} & F_{3,3}}
     *      \pmatrix{ x' \cr y' \cr 1} = \pmatrix{ 0 \cr 0 \cr 0}
     * \f]
     *
     *  Where (x,y) are the coordinates in "right" frame and (x' y') are form the
     *  "left" frame
     *
     *  We first get the epipolar line for a "right" input point. This line will be in
     *  "left" frame
     *
     *  \f[
     *      \pmatrix{ x & y & 1 }
     *      \pmatrix{
     *        F_{1,1} & F_{1,2} & F_{1,3}\cr
     *        F_{2,1} & F_{2,2} & F_{2,3}\cr
     *        F_{3,1} & F_{3,2} & F_{3,3}
     *        } = \pmatrix{ ep_x & ep_y & ep_z }
     *  \f]
     *
     *  Thus we have an equation of the line
     *
     *  \f[
     *      \pmatrix{ ep_x & ep_y & ep_z }  \pmatrix{ x' \cr y' \cr 1} = \pmatrix{ 0 \cr 0 \cr 0}
     *  \f]
     *
     *  Here is the equation of the line - \f$ax' + by' + c = 0\f$. And now we want to know how far is the real right point from the predicted by the current model -
     *
     *   \f[ D = \frac{\left | a x' + b y' + c \right |}{\sqrt{a^2+b^2}}. \f]
     **/
     double epipolarDistance(const Vector2dd &right, const Vector2dd &left) const
     {
         Vector3dd line = this->mulBy2dLeft(right);
         Line2d epiline(line);
         return epiline.distanceTo(left);
     }

     double epipolarDistance(const Correspondance &data) const
     {
         return epipolarDistance(data.start, data.end);
     }

    /**
     *  \f[ 2 E E^T E - tr ( E E^T ) E = 0 . \f]
     */
    Matrix33 criteriaMatrix() const
    {
        Matrix33 &E = *(Matrix33 *)this;
        Matrix33 ET = E.transposed();
        Matrix33 EET = E * ET;
        return (2 * EET * E) - E * EET.trace();
    }

    /**
     *  \f[ \left| [ 2 E E^T E - tr ( E E^T ) E \right|_F  \f]
     */
    double quality() const
    {
        return (criteriaMatrix().frobeniusNorm());
    }

    /**
     *   Returns the essential matrix based on the rotation and the shift of the camera
     *   \f[
     *      [t]_{\times} R
     *   \f]
     *
     **/
    static EssentialMatrix compose(const Matrix33 &R, const Vector3dd &t)
    {
        return Matrix33::CrossProductLeft(t) * R;
    }

    /**
     * Calculating decomposition of essential matrix E in form
     * \f[ E = [t]_xR \f].
     * where \f$[t]_x\f$ is the left cross product of vector \f$t=(t_1,t_2,t_3)\f$ in matrix form:
     *
     * \f[ [t]_x = \pmatrix{
     *           0 & -t_3 &  t_2\cr
     *         t_3 &    0 & -t_1\cr
     *        -t_2 &  t_1 &    0 } \f]
     *
     * and \f$t\times b = [t]_x b\f$. R is rotation matrix. We use SVD decomposition to compute \f$[t]_x\f$ and R from E:
     * An SVD of E gives
     * \f[ E = U\Sigma V^T \f]
     * where U and V are orthogonal \f$ 3 \times 3 \f$ matrices and  \f$\Sigma\f$ is a \f$ 3 \times 3 \f$  diagonal matrix with
     * \f[
     *  \Sigma = \pmatrix{
     *         s & 0 & 0 \cr
     *         0 & s & 0 \cr
     *         0 & 0 & 0 }
     * \f]
     *
     * The diagonal entries of \f$\Sigma\f$ are the singular values of E which, according to the internal constraints of the
     * essential matrix, must consist of two identical and one zero value. Define
     * \f[
     *   W = \pmatrix{
     *         0 & -1 & 0 \cr
     *         1 &  0 & 0 \cr
     *         0 &  0 & 1 }
     * \f]
     *  with \f$ W^{-1} = W^{T} = \
     *   \pmatrix{
     *        0 & 1 & 0 \cr
     *       -1 & 0 & 0 \cr
     *        0 & 0 & 1 } \f$
     * and make the following:
     * \f[ [t]_x = U \, \Sigma \, W \, U^{T} \f]
     * \f[ R = U \, W^{-1} \, V^{T} \f]
     * For more information you can see http://en.wikipedia.org/wiki/Essential_matrix But there decomposition not the same, as we have.
     */
    void decompose(Matrix33 &R, Vector3dd &t)
    {
        Matrix33 W(
                0.0, -1.0, 0.0,
                1.0,  0.0, 0.0,
                0.0,  0.0, 1.0
        );
        Matrix33 WT(W.transposed());

        Matrix33  U = *this;
        Vector3dd vS;
        Matrix33  V;
        Matrix::svdDesc(&U, &vS, &V);

        Matrix33 S = Matrix33::Scale3(vS);
        Matrix33 VT = V.transposed();
        Matrix33 UT = U.transposed();

        R = U * WT * VT;
        Matrix33 T = U * S * W * UT;

    #ifdef TRACE
        cout << "WS:" << endl << vS << endl;
        cout << "====================================" << endl;
        cout << "V :" << V << endl;
        cout << "VT:" << VT << endl;
        cout << "WS:" << W * S << endl;
        cout << "====================================" << endl;
        cout << "T :" << T << endl;
        cout << "ER:" << R * T << endl;
    #endif

        t = Vector3dd(
                T.a(2,1) - T.a(1,2),
                T.a(0,2) - T.a(2,0),
                T.a(1,0) - T.a(0,1)) / 2.0;
    }

    void decompose(Matrix33 &R, Matrix33 &R1, Vector3dd &t)
    {
        Matrix33 W(
                0.0, -1.0, 0.0,
                1.0,  0.0, 0.0,
                0.0,  0.0, 1.0
        );
        Matrix33 WT(W.transposed());

        Matrix33  U = *this;
        Vector3dd vS;
        Matrix33  V;
        Matrix::svdDesc(&U, &vS, &V);

        Matrix33 S = Matrix33::Scale3(vS);
        Matrix33 VT = V.transposed();
        Matrix33 UT = U.transposed();

                 R  = U * WT * VT;
                 R1 = U * W  * VT;
        Matrix33 T  = U * S * W * UT;

    #ifdef TRACE
        cout << "U :" << U.det() << endl << U << endl;
        cout << "WS:" << endl << vS << endl;
        cout << "V :" << V.det() << endl << V << endl;
        cout << "====================================" << endl;
        cout << "VT:" << VT << endl;
        cout << "WS:" << W * S << endl;
        cout << "====================================" << endl;
        cout << "T :" << T << endl;
        cout << "ER:" << R * T << endl;
    #endif

        t = Vector3dd(
                T.a(2,1) - T.a(1,2),
                T.a(0,2) - T.a(2,0),
                T.a(1,0) - T.a(0,1)) / 2.0;
    }

    void decompose(Matrix33 rot[4], Vector3dd trans[4])
    {
        Matrix33 W(
                0.0, -1.0, 0.0,
                1.0,  0.0, 0.0,
                0.0,  0.0, 1.0
        );
        Matrix33 WT(W.transposed());

        Matrix33  U = *this;
        Vector3dd vS;
        Matrix33  V;
        Matrix::svdDesc(&U, &vS, &V);

        if (U.det() < 0) U = - U;
        if (V.det() < 0) V = - V;

        Matrix33 S = Matrix33::Scale3(vS);
        Matrix33 VT = V.transposed();
        Matrix33 UT = U.transposed();

        Matrix33  R  = U * WT * VT;
        Matrix33  R1 = U * W  * VT;
        Matrix33  T  = U * S * W * UT;

        Vector3dd t = Vector3dd(
                T.a(2,1) - T.a(1,2),
                T.a(0,2) - T.a(2,0),
                T.a(1,0) - T.a(0,1)) / 2.0;
        rot[0] = R;  trans[0] =  t;
        rot[1] = R;  trans[1] = -t;
        rot[2] = R1; trans[2] =  t;
        rot[3] = R1; trans[3] = -t;

    }

    void decompose(EssentialDecomposition decomposition[4])
    {
        Matrix33 rot[4];
        Vector3dd trans[4];
        decompose(rot, trans);
        for (int i = 0; i < 4; i++)
        {
            decomposition[i] = EssentialDecomposition(rot[i], trans[i]);
        }
    }

    /**
     * Calculating vectors for left and right nullspaces of the 3 by 3 matrix of rank 2
     *
     * \f[ e_1 F = 0 \f]
     * \f[ F e_2 = 0 \f]
     *
     * We use this function to find both epipoles for a given fundamental matrix.
     *
     *
     * \f[
     *      \pmatrix{ E1_x & E1_y & E1_z}
     *      \pmatrix{
     *        F_{1,1} & F_{1,2} & F_{1,3}\cr
     *        F_{2,1} & F_{2,2} & F_{2,3}\cr
     *        F_{3,1} & F_{3,2} & F_{3,3}} = \pmatrix{ 0 \cr 0 \cr 0}
     * \f]
     * \f[
     *      \pmatrix{
     *        F_{1,1} & F_{1,2} & F_{1,3}\cr
     *        F_{2,1} & F_{2,2} & F_{2,3}\cr
     *        F_{3,1} & F_{3,2} & F_{3,3}}
     *      \pmatrix{ E2_x \cr E2_y \cr E2_z} = \pmatrix{ 0 \cr 0 \cr 0}
     * \f]
     *
     * \param[out]  leftNullspace of the matrix (it correspond to first image)
     * \param[out] rightNullspace of the matrix (it correspond to second image)
     *
     */
    void nullspaces(Vector3dd *leftNullspace, Vector3dd *rightNullspace)
    {
        Matrix33  A(*this);
        Vector3dd W(0.0);
        Matrix33  V(0.0);
        Matrix::svd(&A, &W, &V);
        double mind = numeric_limits<double>::max();
        int mini = -1;
        for (int i = 0; i < Vector3dd::LENGTH; i++)
        {
            if (W[i] < mind)
            {
                mind = W[i];
                mini = i;
            }
        }
        *leftNullspace  = A.aW(mini);
        *rightNullspace = V.aW(mini);
    }


    /**
     * \brief This function approximates the input matrix with rank 2 matrix to remove the noise
     * form Fundamental Matrix
     *
     * The algorithm is - decompose the F matrix with SVD. Take the smallest singular value and replace it with 0
     */
    void assertRank2()
    {
        Vector3dd W;
        Matrix33  V;
        Matrix::svd(this, &W, &V);

    #ifdef TRACE
            printf("The singular values:\n");
            W.print();
            printf("\n");
    #endif

        // Leave out the last singular number. To assert that rank is 2
        int mini = -1;
        double minval = numeric_limits<double>::max();

        // is it needed here?
//        unsigned max = -1;
        double maxval = numeric_limits<double>::min();

        for (int i = 0; i < Vector3dd::LENGTH; i ++)
        {
            double w = W[i];
            if (w <= minval)
            {
                mini = i;
                minval = w;
            }

            if (w >= maxval)
            {
//                max = i;
                maxval = w;
            }
        }

        Matrix33 Wbig(0.0);
        for (int i = 0; i < Vector3dd::LENGTH; i++)
        {
            if (i != mini)
                Wbig.a(i, i) = (W[i] / maxval);
        }

        V.transpose();

        *this = *this * Wbig * V;
    }


    void projectToEssential()
    {
        Vector3dd W;
        Matrix33  V;
        Matrix::svd(this, &W, &V);

#ifdef TRACE
        printf("The singular values:\n");
        W.print();
        printf("\n");
#endif

        // Leave out the last singular number. To assert that rank is 2
        unsigned mini = 0;
        double minval = numeric_limits<double>::max();

        for (unsigned i = 0; i < 3; i ++)
        {
            if (W[i] <= minval)
            {
                mini = i;
                minval = W[i];
            }
        }

        Matrix33 Wbig(0.0);
        double sum = 0.0;
        for (unsigned i = 0; i < 3; i++)
        {
            if (i != mini)
                sum += W[i];
        }

        for (unsigned i = 0; i < 3; i++)
        {
            if (i != mini)
                Wbig.a(i, i) = 1.0;//sum / 2.0;
        }

        V.transpose();

        *this = *this * Wbig * V;
    }


};

} //namespace corecvs


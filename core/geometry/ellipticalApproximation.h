#pragma once
/**
 * \file ellipticalApproximation.h
 * \brief Add Comment Here
 *
 * \date Apr 14, 2011
 * \author alexander
 */
#include <math.h>
#include "vector2d.h"
#include "matrix.h"

namespace corecvs {

/**
 *   This class searches the ellipse that looks like a given blob
 *
 *   We use image moments (p,q)
 *   \f[
 *       M_{pq}=\int\limits_{-\infty}^{\infty} \int\limits_{-\infty}^{\infty} x^py^qf(x,y) \,dx\, dy
 *   \f]
 *
 *   Or in discrete case
 *   \f[
 *       M_{pq}=\sum\limits_{-\infty}^{\infty} \sum\limits_{-\infty}^{\infty} x^py^qf(x,y) \,dx\, dy
 *   \f]
 *
 *   <p>
 *   \f$M_{0,0}\f$ moment is a total weight of the area <br>
 *   \f$\bar{x}=\frac{M_{10}}{M_{00}}\f$ and \f$\bar{y}=\frac{M_{01}}{M_{00}}\f$ are mean x and mean y <br>
 *   <p>
 *
 *   We can now define central moments
 *   \f[
 *       \mu_{pq} = \int\limits_{-\infty}^{\infty} \int\limits_{-\infty}^{\infty} (x - \bar{x})^p(y - \bar{y})^q f(x,y) \, dx \, dy
 *   \f]
 *
 *   Covariance could be expressed as:
 *   \f[
 *   cov[I(x,y)] = \pmatrix{ \mu'_{20} & \mu'_{11} \cr \mu'_{11} & \mu'_{02} }.
 *   \f]
 *
 *   Covariance matrix is Hermitian and symmetric, so spectral theorem apply.
 *
 *   \f[
 *   cov[I(x,y)] = U \Lambda U^T
 *   \f]
 *
 **/
class EllipticalApproximation
{
public:
    int count;
    Vector2dd sum;
    Vector2dd sumSquares;
    double mixedMoment;

    /**
     *  Get number of points for which approximation is computed
     **/
    int getSize() const
    {
        return count;
    }

    /**
     *  Position of input points center
     **/
    Vector2dd getMean() const
    {
        return sum / (double)count;
    }

    Vector2dd getCenter() const
    {
        return getMean();
    }

    Vector2dd getMeanSquare() const
    {
        return sumSquares / (double)count;
    }

    Vector2dd getPerAxisDeviationSquare() const
    {
        Vector2dd mean   = getMean();
        Vector2dd meanSq = getMeanSquare();
        return meanSq - (mean * mean);
    }

    Vector2dd getPerAxisDeviation() const
    {
        Vector2dd perAxisDeviationSquare = getPerAxisDeviationSquare();
        return Vector2dd(sqrt(perAxisDeviationSquare.x()), sqrt(perAxisDeviationSquare.y()));
    }

    double getDeviationSquare() const
    {
        Vector2dd perAxisDeviationSquare = getPerAxisDeviationSquare();
        return perAxisDeviationSquare.x() + perAxisDeviationSquare.y();
    }

    double getRadius() const
    {
        return sqrt(getDeviationSquare());
    }

    /**
     *   this function computes \f$\mu'_{11}\f$
     **/
    double mu11() const
    {
        Vector2dd mean = getMean();
        return (mixedMoment / (double)count) - mean.x() * mean.y();
    }

    double getCenteredMixedMoment() const
    {
        return mu11();
    }

    double getEllipseOrientation() const
    {
        Vector2dd perAxisDeviation = getPerAxisDeviationSquare();
        double momentDiff = perAxisDeviation.x() - perAxisDeviation.y();
        return 0.5 * atan2(2 * getCenteredMixedMoment(), momentDiff);
    }

    /**
     *   Length of the axis are in fact square roots of the eigenvalues of
     *   the covariance matrix.
     *
     *   This function returns two eigenvalues of
     *   Covariance could be expressed as:
     *   \f[
     *      cov[I(x,y)] = \pmatrix{ \mu'_{20} & \mu'_{11} \cr \mu'_{11} & \mu'_{02} }.
     *   \f]
     *
     *   which for the 2 by 2 matrix can be directly computed as
     *
     *   \f[
     *      det (\pmatrix{ \mu'_{20} & \mu'_{11} \cr \mu'_{11} & \mu'_{02} }  - \lambda I ) = 0
     *   \f]
     *
     *   \f[
     *      det \pmatrix{ \mu'_{20} - \lambda & \mu'_{11} \cr \mu'_{11} & \mu'_{02} - \lambda } = 0
     *   \f]
     *
     *   \f{eqnarray}
     *      (\mu'_{20} - \lambda) (\mu'_{02} - \lambda) - \mu'_{11} \mu'_{11} = 0 \\
     *      \lambda^2 - (\mu'_{20} + \mu'_{02}) \lambda - {\mu'_{11}}^2 + \mu'_{02} * \mu'_{20} = 0 \\
     *
     *      \lambda_{1,2} =
     *       \frac { (\mu'_{20} + \mu'_{02}) \pm \sqrt{ (\mu'_{20} + \mu'_{02})^2 - 4 (-{\mu'_{11}}^2 + \mu'_{02} * \mu'_{20})}}
     *             {2} \\
     *
     *      \lambda_i = \frac{\mu'_{20} + \mu'_{02}}{2} \pm \frac{\sqrt{4{\mu'}_{11}^2 + ({\mu'}_{20}-{\mu'}_{02})^2 }}{2}
     *
     *   \f}
     *
     **/
    Vector2dd getEllipseAxisSquare() const
    {
        Vector2dd perAxisDeviationSquare = getPerAxisDeviationSquare();
        double momentDiff = perAxisDeviationSquare.x() - perAxisDeviationSquare.y();
        double centeredMixedMoment = mu11();

        double b =   (perAxisDeviationSquare.x() + perAxisDeviationSquare.y()) / 2.0;
        double det = sqrt(4.0 * centeredMixedMoment * centeredMixedMoment +  momentDiff * momentDiff);
        double longAxis  = b + 0.5 * det;
        double shortAxis = b - 0.5 * det;
        return Vector2dd(longAxis, shortAxis);
    }

    /**
     *   This function computes eigenvector corresponding to largest eigenvalue
     *   \f[
     *      \pmatrix{ \mu'_{20} & \mu'_{11} \cr \mu'_{11} & \mu'_{02} } x = \lambda x
     *   \f]
     *
     *   \f[
     *      \pmatrix{ \mu'_{20} - \lambda & \mu'_{11} \cr \mu'_{11} & \mu'_{02} - \lambda }
     *      \pmatrix{ a \cr b}
     *       = 0
     *   \f]
     *
     *   So are lines of the matrix are linearly dependent
     *   \f{eqnarray}
     *       (\mu'_{20} - \lambda) a + \mu'_{11} b &=& 0 \\
     *       \mu'_{11} a + (\mu'_{02} - \lambda) b &=& 0
     *   \f}
     *
     *   So
     *
     *   \f[
     *   x = (a,b)^T = \alpha (\mu'_{11}, -(\mu'_{20} - \lambda))
     *   \f]
     *
     **/
    Vector2dd getMainDirection() const
    {
        Vector2dd axisSquare = getEllipseAxisSquare();
        double value = axisSquare.x();

        Vector2dd perAxisDeviationSquare = getPerAxisDeviationSquare();
        double mixed = mu11();

        return Vector2dd(mixed, value - perAxisDeviationSquare.x()).normalised();

    }

    Vector2dd getEllipseAxis() const
    {
        Vector2dd axisSquare = getEllipseAxisSquare();
        return Vector2dd(sqrt(axisSquare.x()), sqrt(axisSquare.y()));
    }

    inline void addPoint (double x, double y)
    {
        addPoint(Vector2dd(x,y));
    }

    inline void addPoint (const Vector2dd &point)
    {
        count++;
        sum += point;
        sumSquares += point * point;
        mixedMoment += point.x() * point.y();
    }

    EllipticalApproximation() :
        count(0),
        sum(0.0),
        sumSquares(0.0),
        mixedMoment(0.0)
    {

    }
    ~EllipticalApproximation();
};

template<typename ElementType>
class EllipticalApproximationUnified
{
public:
    EllipticalApproximationUnified() :
         mSum(ElementType(0))
       , mCount(0)
    {
        ElementType forSize;
        mInfMatrix = new Matrix(forSize.size(), forSize.size());
    }

    EllipticalApproximationUnified(const EllipticalApproximationUnified &other) :
        mSum(ElementType(0))
      , mCount(0)
    {
        this->mInfMatrix = new Matrix(other.mInfMatrix);
    }

    ~EllipticalApproximationUnified()
    {
        delete_safe(mInfMatrix);
    }

    void addPoint (ElementType point)
    {
        int row, column;
        for (column = 0; column < mInfMatrix->w ; column++)
        {
            for (row = 0; row < mInfMatrix->h; row++)
            {
                mInfMatrix->a(row, column) = mInfMatrix->a(row, column) + point.at(row) * point.at(column);
            }
        }
        mSum += point;
        mCount++;
    }

    int size() const
    {
        return mCount;
    }

    void getEllipseParameters()
    {
        Matrix A(*mInfMatrix /= double(mCount));
        Matrix W(1, mInfMatrix->w);
        Matrix V(mInfMatrix->h, mInfMatrix->w);

        Matrix::svd(&A, &W, &V);

        mAxes.clear();
        mValues.clear();

        for (int i = 0; i < mInfMatrix->h; i++)
        {

            ElementType forPush;
            for (int k = 0; k < mInfMatrix->h; k ++) forPush.element[k] = V.a(k,i);

            mAxes.push_back(forPush);
            mValues.push_back(W.a(0,i));
        }
    }

//private:
    Matrix*                  mInfMatrix;
    ElementType              mSum;
    std::vector<ElementType> mAxes;
    std::vector<double>      mValues;
    int                      mCount;
};

} //namespace corecvs


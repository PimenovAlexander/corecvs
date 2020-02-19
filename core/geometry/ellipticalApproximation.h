#pragma once
/**
 * \file ellipticalApproximation.h
 * \brief Add Comment Here
 *
 * \date Apr 14, 2011
 * \author alexander
 */
#include <math.h>
#include <set>

#include "utils/global.h"

#include "math/vector/vector2d.h"
#include "math/matrix/matrix.h"

#include "math/affine.h"

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

    bool isEmpty() const
    {
        return (count == 0);
    }

    /**
     *  Position of input points center
     **/
    Vector2dd getMean() const
    {
        if (isEmpty()) {
            return Vector2dd(0.0);
        }
        return sum / (double)count;
    }

    Vector2dd getCenter() const
    {
        return getMean();
    }

    Vector2dd getMeanSquare() const
    {
        if (count == 0) {
            return Vector2dd(1.0);
        }
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
        return  perAxisDeviationSquare.cwiseSqrt();
    }

    double getDeviationSquare() const
    {
        Vector2dd perAxisDeviationSquare = getPerAxisDeviationSquare();
        return perAxisDeviationSquare.sumAllElements();
    }

    double getRadius() const
    {
        return sqrt(getDeviationSquare());
    }

    double getRadiusAround0() const
    {
        return sqrt(getMeanSquare().sumAllElements());
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

    inline void addApproximation (const EllipticalApproximation &appox)
    {
        count += appox.count;
        sum += appox.sum;
        sumSquares += appox.sumSquares;
        mixedMoment += appox.mixedMoment;
    }

    EllipticalApproximation() :
        count(0),
        sum(0.0),
        sumSquares(0.0),
        mixedMoment(0.0)
    {}

    void reset()
    {
        count = 0;
        sum = Vector2dd (0.0, 0.0);
        sumSquares = Vector2dd (0.0, 0.0);
        mixedMoment = 0.0;
    }

    ~EllipticalApproximation();
};

template<typename ElementType>
class EllipticalApproximationUnified
{
protected:
    int getDimention() const
    {
        ElementType forSize;
        return forSize.size();
    }

public:
    EllipticalApproximationUnified() :
         mCovMatrix(NULL)
       , mSum(ElementType(0))
       , mCount(0)
    {
        int d = getDimention();
        mCovMatrix = new Matrix(d, d);
    }

    EllipticalApproximationUnified(const EllipticalApproximationUnified &other) :
        mCovMatrix(NULL)
      , mSum(other.mSum)      
      , mAxes(other.mAxes)
      , mValues(other.mValues)
      , mCount(other.mCount)
    {
        this->mCovMatrix = new Matrix(other.mCovMatrix);
       // SYNC_PRINT(("EllipticalApproximationUnified(const EllipticalApproximationUnified &other) called\n"));
    }

    EllipticalApproximationUnified& operator =(const EllipticalApproximationUnified &other)
    {
        if (&other != this)
        {
            delete_safe(this->mCovMatrix);

            this->mCovMatrix = new Matrix(other.mCovMatrix);
            this->mSum    = other.mSum;
            this->mCount  = other.mCount;
            this->mAxes   = other.mAxes;
            this->mValues = other.mValues;
        }
        return *this;
        // SYNC_PRINT(("EllipticalApproximationUnified::operator =(const EllipticalApproximationUnified &other) called\n"));
    }

    ~EllipticalApproximationUnified()
    {
        delete_safe(mCovMatrix);
    }


    /**
     * Computing covariance matrix adding point by point
     *
     *
     **/
    void addPoint (const ElementType &point)
    {
        int row, column;
        for (column = 0; column < mCovMatrix->w ; column++)
        {
            for (row = 0; row < mCovMatrix->h; row++)
            {
                mCovMatrix->a(row, column) = mCovMatrix->a(row, column) + point.at(row) * point.at(column);
            }
        }

        mSum += point;
        mCount++;
    }

    template <class Container>
    void addPoints(const Container &pointContainer) {
        for (auto &p : pointContainer) {
            addPoint(p);
        }
    }

    int size() const
    {
        return mCount;
    }

    bool getEllipseParameters()
    {
        if (mCount == 0) {
            SYNC_PRINT(("Can't approximate 0 points\n"));
            return false;
        }

        if (mCovMatrix == NULL) {
            SYNC_PRINT(("Can't approximate non existant matrix\n"));
            return false;
        }

        if (mCovMatrix->h == 0 || mCovMatrix->w == 0) {
            SYNC_PRINT(("Can't approximate non existant matrix\n"));
            return false;
        }

        /**
         * From the final covariance matrix (this is unfortunately not computationally stable)
         * We will use the fact that
                sigma(X, Y) = E[(X - E[X])(Y - E[Y])] = E[XY] - E[Y]E[X]
         */
        Matrix A(*mCovMatrix);
        for (int row = 0; row < A.h; row++)
        {
            for (int column = 0; column < A.w; column++)
            {
               //                E[X,Y]
                A.a(row,column) = (A.a(row,column) / mCount) - (mSum.at(row) / mCount) * (mSum.at(column) / mCount);
            }
        }

        /* Using SVD here is kind of stupid. We already have covariance matrix */
#ifdef SVD_ATTEMPT
        Matrix W(1, mInfMatrix->w);
        Matrix V(mInfMatrix->h, mInfMatrix->w);

        Matrix::svd(&A, &W, &V);

        mAxes.clear();
        mValues.clear();

        for (int i = 0; i < mInfMatrix->w; i++)
        {
            ElementType forPush;
            for (int k = 0; k < mInfMatrix->h; k ++) {
                forPush.element[k] = V.a(k,i);
            }

            mAxes.push_back(forPush);
            mValues.push_back(W.a(0,i));
        }
#else
        DiagonalMatrix D(A.h);
        Matrix V(A.h, A.w);
        int res = Matrix::jacobi(&A, &D, &V, NULL, false);
        if (res != 0) {
            Matrix B(*mCovMatrix);
            for (int row = 0; row < B.h; row++)
            {
                for (int column = 0; column < B.w; column++)
                {
                    B.a(row,column) = (B.a(row,column) / mCount) - (mSum.at(row) / mCount) * (mSum.at(column) / mCount);
                }
            }

            cout << "Problem " << res << " with matrix: " << endl << B << endl;

            for (int row = 0; row < A.h; row++) {
                D.a(row) = A.a(row, row);

                for (int column = 0; column < A.w; column++)
                    V.a(row, column) = (row == column) ? 1 : 0;
            }

        }

        mAxes.clear();
        mValues.clear();

        for (int i = 0; i < mCovMatrix->w; i++)
        {
            ElementType forPush;
            for (int k = 0; k < mCovMatrix->h; k ++) {
                forPush.element[k] = V.a(k,i);
            }

            mAxes.push_back(forPush);
            mValues.push_back(D.a(i));
        }
#endif
        for (unsigned i = 0; i < mValues.size(); i++)
        {
            int maxid = i;
            double max = mValues[i];
            for (unsigned j = i + 1; j < mValues.size(); j++)
            {
                if (fabs(mValues[j]) > max) {
                    max = fabs(mValues[j]);
                    maxid = j;
                }
            }

            std::swap(mValues[maxid], mValues[i]);
            std::swap(mAxes  [maxid], mAxes  [i]);
        }

        return true;
    }

    int getSize() const
    {
        return mCount;
    }

    int count() const
    {
        return mCount;
    }

    bool isEmpty() const
    {
        return (mCount == 0);
    }

    ElementType getMean() const
    {
        if (isEmpty()) {
            return ElementType(0);
        }
        return mSum / (double)mCount;
    }

    ElementType getCenter() const
    {
        return getMean();
    }

    double getRadius() const
    {
        if (isEmpty()) {
            return 0.0;
        }

        ElementType mean = getMean();
        double radius = 0.0;
        for (int i = 0; i < getDimention(); i++)
        {
            radius += mCovMatrix->a(i,i) / mCount - (mean.at(i) * mean.at(i));
        }
        return sqrt(radius);
    }

    double getRadiusForDim(size_t dim) const
    {
        if (isEmpty()) {
            return 0.0;
        }
        ElementType mean = getMean();
        double radius = mCovMatrix->a((int)dim, (int)dim) / mCount - (mean.at(dim) * mean.at(dim));
        return sqrt(radius);
    }

    ElementType getRadiusPerDim() const {
        if (isEmpty()) {
            return ElementType(0.0);
        }

        ElementType result;
        ElementType mean = getMean();
        for (int i = 0; i < getDimention(); i++)
        {
            result[i] = sqrt(mCovMatrix->a(i,i) / mCount - (mean.at(i) * mean.at(i)));
        }
        return result;
    }

    double getRadiusAround0() const
    {
        if (isEmpty()) {
            return 0.0;
        }

        double radius = 0.0;
        for (int i = 0; i < mCovMatrix->h; i++)
        {
            radius += mCovMatrix->a(i,i) / mCount;
        }
        return sqrt(radius);
    }

    /**
     * Returns matrix that moves cluster center to zero and orientates its axis to orts
     **/
    Matrix44 getEigenTransform() const
    {
        return Matrix44(Matrix33::FromColumns(mAxes[0], mAxes[1], mAxes[2]), -getCenter());
    }

    Affine3DQ getEigenMove() const
    {
        Vector3dd a0 = mAxes[0];
        Vector3dd a1 = mAxes[1];
        Vector3dd a2 = mAxes[2];
        a0.normalise();
        a1.normalise();
        a2.normalise();

        if (((a0 ^ a1) & a2) < 0) a2 = -a2;

        Matrix33 m = Matrix33::FromRows(a0, a1, a2);
        return Affine3DQ(Quaternion::FromMatrix(m)) * Affine3DQ(-getCenter());
    }

//
//private:
    Matrix*                  mCovMatrix;
    ElementType              mSum;
    std::vector<ElementType> mAxes;
    std::vector<double>      mValues;
    int                      mCount;
};

typedef EllipticalApproximationUnified<Vector3dd> EllipticalApproximation3d;

template <>
inline int EllipticalApproximationUnified<double>::getDimention() const
{
    return 1;
}

template <>
inline void EllipticalApproximationUnified<double>::addPoint (const double &point)
{
    mCovMatrix->a(0, 0) += point * point;
    mSum += point;
    mCount++;
}


template <>
inline double EllipticalApproximationUnified<double>::getRadius () const
{
    if (isEmpty()) {
        return 0.0;
    }

    double mean = getMean();
    double radius = 0.0;
    for (int i = 0; i < getDimention(); i++)
    {
        radius += mCovMatrix->a(i,i) / mCount - mean * mean;
    }
    return sqrt(radius);
}

class EllipticalApproximation1d : public EllipticalApproximationUnified<double>
{
public:
    double mMin;
    double mMax;

    EllipticalApproximation1d() :
        mMin(numeric_limits<double>::max()),
        mMax(-numeric_limits<double>::max())
    {}

    void addPoint (double point)
    {
        EllipticalApproximationUnified<double>::addPoint(point);
        if (point < mMin) mMin = point;
        if (point > mMax) mMax = point;
    }

    double getMin() const
    {
        return mMin;
    }

    double getMax() const
    {
        return mMax;
    }


    friend std::ostream & operator <<(std::ostream &out, const EllipticalApproximation1d &stats)
    {
        out << "Min:" << stats.getMin()  << endl;
        out << "Max:" << stats.getMax()  << endl;
        out << "Avg:" << stats.getMean() << endl;
        return out;
    }

};

class SDevApproximation1d
{
public:
    double mSqSum;
    double mSum;
    int    mCount;

    SDevApproximation1d() :
        mSqSum(0.0),
        mSum(0.0),
        mCount(0)
    {}

    void addPoint (double point)
    {
       mSqSum += point * point;
       mSum += point;
       mCount ++;
    }

    double getAverage() const
    {
        if (mCount == 0)
            return 0.0;

        return mSum / mCount;
    }

    double getSDev() const
    {
        if (mCount == 0)
            return 0.0;

        double avg = getAverage();
        return (mSqSum / mCount) - avg * avg;
    }

    double getDev() const
    {
        return sqrt(getSDev());
    }
};

/* Inheritance needed here */
class SDevApproximation1dMedian
{
public:
    double mSqSum;
    double mSum;
    int    mCount;
    std::multiset<double> minHeap, maxHeap;

    SDevApproximation1dMedian() :
        mSqSum(0.0),
        mSum(0.0),
        mCount(0),
        minHeap(),
        maxHeap()
    {}

    void addPoint (double point)
    {
       mSqSum += point * point;
       mSum += point;
       mCount ++;
       if(maxHeap.size() == 0 || point < *maxHeap.rbegin())
           maxHeap.insert(point);
       else
           minHeap.insert(point);
       if(maxHeap.size() > minHeap.size() + 1) {
           double mid = *maxHeap.rbegin();
           maxHeap.erase(--maxHeap.end());
           minHeap.insert(mid);
       }
       if(minHeap.size() > maxHeap.size() + 1) {
           double mid = *minHeap.begin();
           minHeap.erase(minHeap.begin());
           maxHeap.insert(mid);
       }
    }

    double getAverage() const
    {
        if (mCount == 0)
            return 0.0;

        return mSum / mCount;
    }

    double getMedian() const
    {
        if (mCount == 0)
            return 0.0;

        if (maxHeap.size() > minHeap.size())
            return *maxHeap.rbegin();
        if (maxHeap.size() < minHeap.size())
            return *minHeap.begin();
        return (*minHeap.begin() + *maxHeap.rbegin()) / 2;
    }

    double getSDev() const
    {
        if (mCount == 0)
            return 0.0;

        double avg = getAverage();
        return (mSqSum / mCount) - avg * avg;
    }

    double getDev() const
    {
        return sqrt(getSDev());
    }
};



} //namespace corecvs

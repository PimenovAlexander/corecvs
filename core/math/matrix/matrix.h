#pragma once
/**
 * \file matrix.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */
#include <algorithm>
#include <vector>

#include "global.h"

#include "abstractBuffer.h"
#include "matrix33.h"
#include "matrix44.h"
#include "diagonalMatrix.h"
#include "vector.h"

namespace corecvs {

using std::vector;
using std::binder2nd;
using std::plus;
using std::minus;


typedef AbstractBuffer<double, int32_t> MatrixBase;

class Matrix : public MatrixBase
{
public:
    enum InverstionAlgorithm{
        INVERT_LU,
        INVERT_SVD
    };


    /**
     * This function creates a matrix with all zero elements
     *
     * \param h
     *         The height of the matrix
     * \param w
     *         The width of the matrix
     */
    Matrix(int32_t h, int32_t w) : MatrixBase (h, w) {};

    Matrix(const Matrix &that) : MatrixBase (that) {};
    explicit Matrix(const Matrix *that) : MatrixBase (that) {};
    explicit Matrix(const DiagonalMatrix &that);

    Matrix(Matrix *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        MatrixBase(src, x1, y1, x2, y2) {};

    /**
     * This function creates a matrix filling it with values form the given array
     * line by line.
     *
     * \param h
     *         The height of the matrix
     * \param w
     *         The width of the matrix
     * \param data
     *         The line by line raw values of the matrix
     */
    Matrix(int32_t h, int32_t w, double *data) : MatrixBase(h, w, data) {};

#if 0
    /**
     * This function creates a matrix filling it with values form the given vector
     * line by line.
     *
     * \param h
     *         The height of the matrix
     * \param w
     *         The width of the matrix
     * \param data
     *         The line by line raw values of the matrix
     */
    Matrix(int32_t h, int32_t w, std::vector<double> &data);
#endif

    /**
     * This constructs the diagonal matrix
     *
     * \f[
     *  M = \pmatrix {
     *      value &      0 & \cdots &     0  & 0      \cr
     *          0 &  value & \cdots &     0  & 0      \cr
     *     \vdots & \vdots & \ddots & \vdots & \vdots \cr
     *          0 &      0 & \cdots &  value & 0
     *  }
     * \f]
     *
     **/
    Matrix(int32_t h, int32_t w, double value);

    Matrix(int32_t h, int32_t w, bool shouldInit) : MatrixBase(h, w, shouldInit) {};

    explicit Matrix(Matrix33 &in);
    explicit Matrix(Matrix44 &in);


    /**
     * The element getter for a matrix that is used like vector (i.e has height or width of 1)
     *
     * \remark !!!! IT IS UNTIMATELY DICOURAGED TO USE THIS FUNCTION NOW !!!!
     *
     * \param x
     *         An index of the vector element
     * \return
     *         A matrix element
     **/
   /* inline double &a(int32_t x)
    {
        return data[x];
    };

    const inline double &a(int32_t x) const
    {
        return data[x];
    }; */

    /**
     * Pseudonim to resemble the mathematics notation
     *
     * \param x the column of the matrix
     * \param y the row of the matrix
     **/
    inline double &a(int32_t y, int32_t x)
    {
        return MatrixBase::element(y,x);
    };

    const inline double &a(int32_t y, int32_t x) const
    {
        return MatrixBase::element(y,x);
    };

    /**
     *  Matrix multiplication.
     **/
    Matrix *mul(const Matrix& V);

    friend Matrix operator *(const Matrix &A, const Matrix &B);
    friend Matrix operator +(const Matrix &A, const Matrix &B);
    friend Matrix operator -(const Matrix &A, const Matrix &B);


    friend Matrix operator *=(Matrix &M, const DiagonalMatrix &D);
    friend Matrix operator *(const Matrix &M, const DiagonalMatrix &D);
    friend Matrix operator *(const DiagonalMatrix &D, const Matrix &M);


    friend Vector operator *(const Matrix &M, const Vector &V);
    friend Vector operator *(const Vector &V, const Matrix &M);


    /**
     *  \brief Matrix transposition.
     *
     *  \return
     *        \f$M^\top\f$
     *
     **/
    Matrix *transposed() const;
    Matrix t() const;
    void transpose();

    //double det(void) const; /* NYI*/
    double trace(void) const;

    double frobeniusNorm() const;

    static void svd (Matrix  *A, Matrix *W, Matrix *V);
    static void svd (Matrix33 *A, Vector3dd *W, Matrix33 *V);
    static void svdDesc (Matrix33 *A, Vector3dd *W, Matrix33 *V);

    static bool matrixSolveGaussian(Matrix *A, Matrix *B);

    inline Matrix& operator +=(const Matrix& V)
    {
        this->binaryOperationInPlace<plus<double> >(V);
        return *this;
    }

    inline Matrix& operator -=(const Matrix& V)
    {
        this->binaryOperationInPlace<minus<double> >(V);
        return *this;
    }

    /**
     * Multiply matrix by scalar
     *  \f$M := M \alpha\f$
     *
     * */

    inline Matrix& operator *=(const double v)
    {
        using std::multiplies;
        binder2nd<multiplies<double> > multiplier(multiplies<double>(), v);
        this->mapOperationElementwize<binder2nd<multiplies<double> > >(multiplier);
        return *this;
    }

    /**
     * Divide matrix by scalar
     *    \f$M := M {\frac 1 \alpha}\f$
     * */
    inline Matrix& operator /=(const double v)
    {
        using std::divides;
        binder2nd<divides<double> > divider(divides<double>(), v);
        this->mapOperationElementwize<binder2nd<divides<double> > >(divider);
        return *this;
    }

    /**
     * Check matrix for equality
     **/
    inline bool operator ==(const Matrix &M) const
    {
        return this->isEqual(M);
    }

    operator Matrix33() const;



    /**
     *  All elements are negated
     *    \f$M := -M\f$
     * */
    inline void neg()
    {
        using std::negate;
        this->mapOperationElementwize<negate<double> >(negate<double>());
    }

    Matrix negative() const;

    Matrix inv() const;
    Matrix invSVD() const;
    double detSVD() const;

    void print()
    {
        cout << *this;
    }

    friend ostream & operator <<(ostream &out, const Matrix &matrix);

    inline bool notTooFar(const Matrix *V, double epsilon = 0.0) const
    {
        int h = getH();
        int w = getW();
        if (h != V->getH() || w != V->getW())
            return false;

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            if (  this->element(i,j) > V->element(i,j) + epsilon ||
                  this->element(i,j) < V->element(i,j) - epsilon )
                return false;
        return true;
    }

/*pivate:
    static void svd (int h, int w, double *A, double *W, double *V);*/
};

} //namespace corecvs


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
#include <cmath>
#include <functional>

#include "core/utils/global.h"

#include "core/buffers/abstractBuffer.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/matrix/matrix44.h"
#include "core/math/matrix/diagonalMatrix.h"
#include "core/math/vector/vector.h"

namespace corecvs {

using std::vector;
using std::plus;
using std::minus;


typedef AbstractBuffer<double, int32_t> MatrixBase;

class Matrix : public MatrixBase
{
public:
    void promoteToGpu() { }
    enum InverstionAlgorithm{
        INVERT_LU,
        INVERT_SVD
    };


    /**
     * This function creates a matrix with all zero or uninitailized elements
     *
     * \attention
     *          Generally using this method could create undefined behavior.
     *          Use it only when you would immediatly fill whole matrix with elements.
     *          Otherwise, please explicitly state that some elements are uninitialzed.
     * \param h
     *         The height of the matrix
     * \param w
     *         The width of the matrix
     * \param fillWithZero
     *         if true matrix would be filled with zero
     *
     */

    Matrix(int32_t h, int32_t w, bool fillWithZero = true) : MatrixBase (h, w, fillWithZero) {}


    Matrix(const Matrix &that) : MatrixBase (that) {}
    explicit Matrix(const Matrix *that) : MatrixBase (that) {}
    explicit Matrix(const DiagonalMatrix &that);

    Matrix(const Matrix &src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        MatrixBase(src, x1, y1, x2, y2) {}

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
    Matrix(int32_t h, int32_t w, double *data) : MatrixBase(h, w, data) {}

    Matrix() : MatrixBase() {}
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

    explicit Matrix(const Matrix33 &in);
    explicit Matrix(const Matrix44 &in);


    /**
     * Pseudonim to resemble the mathematics notation
     *
     * \param x the column of the matrix
     * \param y the row of the matrix
     **/
    inline double &a(int32_t y, int32_t x)
    {
        return MatrixBase::element(y,x);
    }

    const inline double &a(int32_t y, int32_t x) const
    {
        return MatrixBase::element(y,x);
    }

    /**
     *  Matrix multiplication.
     **/
    Matrix *mul(const Matrix& V);

    friend Matrix operator -(const Matrix &A);

    friend Matrix operator *(const double &a, const Matrix &B);
    friend Matrix operator *(const Matrix &B, const double &a);

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
    Matrix ata() const;

#ifdef WITH_BLAS
    double det(void) const;
#endif
    double trace(void) const;

    double frobeniusNorm() const;

    static void svd (Matrix  *A, Matrix *W, Matrix *V);
    static void svd (Matrix  *A, DiagonalMatrix *W, Matrix *V);
    /*
     * Computes "square root" of symmetrical posdef matrix and applies additional
     * transform pretransform (on the left)
     *
     * This routine returns A^{-1}
     */

    Matrix invPosdefSqrt(const Matrix* preTransform = nullptr) const;

    static void svd (Matrix33 *A, Vector3dd *W, Matrix33 *V);
    static void svdDesc (Matrix33 *A, Vector3dd *W, Matrix33 *V);

    static int jacobi(Matrix *a, DiagonalMatrix *d, Matrix *v, int *nrotpt, bool trace = false);

    static bool matrixSolveGaussian(Matrix *A, Matrix *B);

    /*
     * Linear system solving with different paths for factoring posdef/symmetric matrices
     */
    bool        linSolve(const corecvs::Vector &B, corecvs::Vector &res, bool symmetric = false, bool posDef = false) const;
    static bool LinSolve(const corecvs::Matrix &A, const corecvs::Vector &B, corecvs::Vector &res, bool symmetric = false, bool posDef = false);
    /*
     * Linear system solving with use of schur-complement structure (only with block-diagonal lower-right part)
     * Note that you shoul use it only when you are sure that lower (block-diagonal) part is well-conditioned
     */
    static bool LinSolveSchurComplement(const corecvs::Matrix &A, const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false, bool explicitInvers = false);
    bool        linSolveSchurComplement(const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false);

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
        this->mapOperationElementwize([v] (Matrix::InternalElementType e) { return e * v; });
        return *this;
    }

    /**
     * Divide matrix by scalar
     *    \f$M := M {\frac 1 \alpha}\f$
     * */
    inline Matrix& operator /=(const double v)
    {
        this->mapOperationElementwize([v] (Matrix::InternalElementType e) { return e / v; });
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
    std::pair<bool, Matrix> incompleteCholseky();
    Matrix invSVD() const;
    double detSVD() const;

    Vector2d32 getMinCoord() const;
    Vector2d32 getMaxCoord() const;
    Vector trsv(const Vector &v, const char* trans, bool up, int N) const
    {
        Vector res(v);
        for (int i = 0; i < N; ++i)
            res = dtrsv(res, up, trans[i] == 'N');
        return res;
    }
    Vector dtrsv(const Vector &v, bool upper = true, bool notrans = true) const;
    Vector dtrsv_un(const Vector &v) const { return dtrsv(v, true, true); }
    Vector dtrsv_ut(const Vector &v) const { return dtrsv(v, true, false); }
    Vector dtrsv_ln(const Vector &v) const { return dtrsv(v, false, true); }
    Vector dtrsv_lt(const Vector &v) const { return dtrsv(v, false, false); }

    Matrix column(int column);
    Matrix row   (int row);

    void print()
    {
        cout << *this;
    }

    friend std::ostream & operator <<(std::ostream &out, const Matrix &matrix);
    void print(std::ostream &out);

    inline bool notTooFar(const Matrix *V, double epsilon = 0.0, bool trace = false ) const
    {
        int h = getH();
        int w = getW();
        if (h != V->getH() || w != V->getW()) {
            if (trace) {
                SYNC_PRINT(("[%d x %d] != [%d x %d]", h, w, V->getH(), V->getW()));
            }
            return false;
        }

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            if (  this->element(i,j) > V->element(i,j) + epsilon ||
                  this->element(i,j) < V->element(i,j) - epsilon )
            {
                if (trace) {
                    SYNC_PRINT(("[%d x %d] %lf != %lf\n", i, j, this->element(i,j), V->element(i,j)));
                }
                return false;
            }
        return true;
    }

    inline bool isFinite()
    {
        bool result = true;
        auto toucher = [&result] (int, int, Matrix::InternalElementType &e) -> void { result &= std::isfinite(e); };
        this->touchOperationElementwize(toucher);
        return result;
    }

/* Some more specific way to call multiplication */
    static Matrix multiplyHomebrew  (const Matrix &A, const Matrix &B, bool parallel = true, bool vectorize = true);
    static Matrix multiplyHomebrewMD(const Matrix &M, const DiagonalMatrix &D);
    static Vector multiplyHomebrewMV(const Matrix &M, const Vector &V);

#ifdef WITH_BLAS
    static Matrix multiplyBlas(const Matrix &A, const Matrix &B);
#endif
    /**
     * @brief AVX powered block matrix multiplication
     * @param[in] A matrix m x k size
     * @param[in] B matrix k x n size
     * @warning This method uses static context and must be called only from single thread
     * @return A * B = C matrix m x n size
     */
    static Matrix multiplyBlasReplacement(const Matrix &A, const Matrix &B);
    // static Matrix multiplyBlasReplacement(const Matrix &A, const Matrix &B, BlockMM8Context &context);
};

} //namespace corecvs


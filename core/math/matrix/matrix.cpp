/**
 * \file matrix.cpp
 * \brief Generic dense matrix of the arbitatry size
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */
#include "utils/global.h"
#include "math/sparseMatrix.h"
#include "math/matrix/matrix.h"
#include "math/matrix/matrix33.h"
#include "math/matrix/blasReplacement.h"
#include "math/sse/sseWrapper.h"
#include "tbbwrapper/tbbWrapper.h"

#include "wrappers/cblasLapack/cblasLapackeWrapper.h"

namespace corecvs {

using std::ostream;
using std::istream;


/* The constructor is not in single cycle due to possible stride of the matrix */
Matrix::Matrix(const Matrix33 &in) : MatrixBase(3, 3, false)
{
    a(0,0) = in.a(0,0);
    a(0,1) = in.a(0,1);
    a(0,2) = in.a(0,2);

    a(1,0) = in.a(1,0);
    a(1,1) = in.a(1,1);
    a(1,2) = in.a(1,2);

    a(2,0) = in.a(2,0);
    a(2,1) = in.a(2,1);
    a(2,2) = in.a(2,2);
}

/* TODO: Make this inline */
Matrix::Matrix(const Matrix44 &in) : MatrixBase(4, 4, false)
{
    for (int i = 0; i < in.H; i++)
        for (int j = 0; j < in.W; j++)
        {
            a(i,j) = in.a(i,j);
        }
}

Matrix::Matrix(int32_t h, int32_t w, double value) : MatrixBase(h, w, (double)(0.0))
{
    int32_t minDim = CORE_MIN(h,w);
    for (int32_t i = 0; i < minDim; i++)
        MatrixBase::element(i,i) = value;
}

Matrix::Matrix(const DiagonalMatrix &that) : MatrixBase(that.size(), that.size())
{
    for (int i = 0; i < that.size(); i++)
        this->a(i,i) = that.at(i);
}

/*Matrix::Matrix(int32_t h, int32_t w, std::vector<double> &data) : CMatrixBase(h, w, (double)(0.0))
{
    int size = data.size()
    for (unsigned i = 0; i < data.size(); i++)
        this->data[i] = data[i];
}*/

Matrix::operator Matrix33() const
{
    return Matrix33 (
     a(0,0), a(0,1), a(0,2),
     a(1,0), a(1,1), a(1,2),
     a(2,0), a(2,1), a(2,2)
    );
}

Matrix operator -(const Matrix &A)
{
    Matrix result(A);
    for (int i = 0; i < A.h; ++i)
    {
        for (int j = 0; j < A.w; ++j)
        {
            result.a(i, j) *= -1.0;
        }
    }
    return result;
}

Matrix operator *(const double &a, const Matrix &B)
{
    Matrix result(B.h, B.w, false);
    int row, column;
    for (row = 0; row < result.h; row++)
    {
        for (column = 0; column < result.w; column++)
        {
             result.a(row, column) = B.a(row, column) * a;
        }
    }
    return result;
}

Matrix operator *(const Matrix &B, const double &a)
{
    return a * B;
}


/* TODO: Merge functions below */
Matrix *Matrix::mul(const Matrix& V)
{
    CORE_ASSERT_TRUE(this->w == V.h, "Matrices have wrong sizes");
    Matrix *result = new Matrix(this->h, V.w, false);

    int row, column, runner;
    for (row = 0; row < result->h; row++)
        for (column = 0; column < result->w; column++)
        {
            double sum = 0;
            for (runner = 0; runner < this->w; runner++)
            {
                sum += this->a(row, runner) * V.a(runner, column);
            }
            result->a(row, column) = sum;
        }

    return result;
}

#define WITH_DIRTY_GEMM_HACKS

// I've prepared dirty implementation of GEMM/GEMV-like functions with fma-intrinsics and tbb stuff.
// GEMM is also can be optimized with any BLAS-like library providing
// cblas interface (build with WITH_BLAS and appropriate linker flags)
//
// USE IT ON YOUR OUR RISK
//
// Note, that it can be further improved with aligned reading,
// block-matrix multiplication, etc; but adding BLAS-library seems to be better alternative.
//
// TODO: You'll need to modify cvs.pro, config.pri to make it buildable.
// Easiest way is to comment out all -msse -mavx ... compiler keys, add -march=native (and -Ofast).
//
#ifndef WITH_DIRTY_GEMM_HACKS

Matrix operator *(const Matrix &A, const Matrix &B)
{
    CORE_ASSERT_TRUE(A.w == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, B.w, false);

    int row, column, runner;
    for (row = 0; row < result.h; row++)
        for (column = 0; column < result.w; column++)
        {
            double sum = 0;
            for (runner = 0; runner < A.w; runner++)
            {
                sum += A.a(row, runner) * B.a(runner, column);
            }
            result.a(row, column) = sum;
        }

    return result;
}

Vector operator *(const Matrix &M, const Vector &V)
{
    CORE_ASSERT_TRUE(M.w == V.size(), "Matrix and vector have wrong sizes");
    Vector result(M.h);
    int row, column;
    for (row = 0; row < M.h; row++)
    {
        double sum = 0.0;
        for (column = 0; column < M.w; column++)
        {
            sum += V.at(column) * M.a(row, column);
        }
        result.at(row) = sum;
    }
    return result;
}

Vector operator *(const Vector &V, const Matrix &M)
{
    CORE_ASSERT_TRUE(M.h == V.size(), "Matrix and vector have wrong sizes");
    Vector result(M.w);
    int row, column;
    for (column = 0; column < M.w; column++)
    {
        double sum = 0.0;
        for (row = 0; row < M.h; row++)
        {
            sum += V.at(row) * M.a(row, column);
        }
        result.at(column) = sum;
    }
    return result;
}

Matrix operator *=(Matrix &M, const DiagonalMatrix &D)
{
    CORE_ASSERT_TRUE(M.w == D.size(), "Matrices have wrong sizes");
    for (int i = 0; i < M.h; i++)
    {
         for (int j = 0; j < M.w; j++)
         {
            M.a(i,j) *= D.at(i);
         }
    }
    return M;
}

Matrix operator *(const Matrix &M, const DiagonalMatrix &D)
{
    CORE_ASSERT_TRUE(M.w == D.size(), "Matrices have wrong sizes");
    Matrix result(M.h, M.w);
    for (int i = 0; i < M.h; ++i)
    {
        for (int j = 0; j < M.w; ++j)
        {
            result.a(i, j) = M.a(i, j) * D.at(j);
        }
    }
    return result;
}

Matrix operator *(DiagonalMatrix &D, const Matrix &M)
{
    CORE_ASSERT_TRUE(M.h == D.size(), "Matrices have wrong sizes");
    Matrix result(M.h, M.w);
    for (int i = 0; i < M.h; ++i)
    {
        for (int j = 0; j < M.w; ++j)
        {
            result.a(i, j) = M.a(i, j) * D.at(i);
        }
    }
    return result;
}

#else // !WITH_DIRTY_GEMM_HACKS

#ifdef WITH_BLAS
Matrix Matrix::multiplyBlas(const Matrix &A, const Matrix &B)
{
    CORE_ASSERT_TRUE(A.w == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, B.w, false);
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A.h, B.w, A.w, 1.0, A.data, A.stride, B.data, B.stride, 0.0, result.data, result.stride);
    return result;
}
#endif

Matrix Matrix::multiplyBlasReplacement(const Matrix &A, const Matrix &B)
{
    CORE_ASSERT_TRUE(A.w == B.h, "Matrices have wrong sizes");
    static BlockMM8Context context;
    Matrix result(A.h, B.w, true);
    BlockMM8 block(&A, &B, &result, context);
    block();
    return result;
}


Matrix operator *(const Matrix &A, const Matrix &B)
{
#ifdef WITH_BLAS
    return Matrix::multiplyBlas(A, B);
#else
    CORE_ASSERT_TRUE(A.w == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, B.w, false);

    corecvs::parallelable_for(0, result.h, 8, ParallelMM<>(&A, &B, &result), !(A.h < 64));

    //Matrix::multiplyHomebrew(A, B, true, !(A.h < 64)); // TODO: it has a bug, see testMatrixOperations!!!

    return result;
#endif
}

Vector operator *(const Matrix &M, const Vector &V)
{
    CORE_ASSERT_TRUE(M.w == V.size(), "Matrix and vector have wrong sizes");
    if (M.h >= 64)
    {
#ifdef WITH_BLAS
        Vector result(M.h);
        cblas_dgemv (CblasRowMajor, CblasNoTrans, M.h, M.w, 1.0, &M.element(0, 0), M.stride, &V[0], 1, 0.0, &result[0], 1);
        return result;
#else
        return Matrix::multiplyHomebrewMV(M, V);
#endif
    }

    Vector result(M.h);
    for (int row = 0; row < M.h; row++)
    {
        double sum = 0.0;
        for (int column = 0; column < M.w; column++)
        {
            sum += V.at(column) * M.a(row, column);
        }
        result.at(row) = sum;
    }
    return result;
}

Vector operator *(const Vector &V, const Matrix &M)
{
    CORE_ASSERT_TRUE_P(M.h == V.size(), ("Matrix and vector have wrong sizes [%dx%d] %d", M.h, M.w, V.size()));
    Vector result(M.w);

#ifdef WITH_BLAS
    if (M.h < 32)
    {
#endif
       for (int column = 0; column < M.w; ++column)
       {
           double sum = 0.0;
           for (int row = 0; row < M.h; ++row)
           {
               sum += V.at(row) * M.a(row, column);
           }
           result.at(column) = sum;
       }
#ifdef WITH_BLAS
    }
    else
    {
        cblas_dgemv (CblasRowMajor, CblasTrans, M.h, M.w, 1.0, &M.element(0, 0), M.stride, &V[0], 1, 0.0, &result[0], 1);
    }
#endif
    return result;
}


Matrix operator *=(Matrix &M, const DiagonalMatrix &D)
{
    CORE_ASSERT_TRUE(false, "TODO: Matrix operator *=(Matrix &M, const DiagonalMatrix &D) is implemented badly");       // TODO: check the implementation: result is squared matrix!
    int32_t minDim = CORE_MIN(M.h, M.w);
    minDim = CORE_MIN(minDim, D.size());
    for (int i = 0; i < minDim; i++)
    {
         for (int j = 0; j < minDim; j++)
         {
            M.a(i,j) *= D.at(i);
         }
    }
    return M;
}


Matrix operator *(const Matrix &M, const DiagonalMatrix &D)
{
    CORE_ASSERT_TRUE(M.w == D.size(), "Matrix and DiagonalMatrix have wrong sizes");
    if (M.h < 16)
    {
       Matrix result(M.h, M.w);
       for (int i = 0; i < M.h; ++i)
       {
           for (int j = 0; j < M.w; ++j)
           {
               result.a(i, j) = M.a(i, j) * D.at(j);
           }
        }
        return result;
    }

    return Matrix::multiplyHomebrewMD(M, D);
}

Matrix operator *(const DiagonalMatrix &D, const Matrix &M)
{
    CORE_ASSERT_TRUE(M.h == D.size(), "DiagonalMatrix and Matrix have wrong sizes");
    Matrix result(M.h, M.w);
    for (int i = 0; i < M.h; ++i)
    {
        for (int j = 0; j < M.w; ++j)
        {
            result.a(i, j) = M.a(i, j) * D.at(i);
        }
    }
    return result;
}

#endif // WITH_DIRTY_GEMM_HACKS

/* TODO: Use abstract buffer binaryOperation elementWize */
Matrix operator +(const Matrix &A, const Matrix &B)
{
    CORE_ASSERT_TRUE(A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, A.w, false);

    for (int row = 0; row < result.h; row++)
    {
        for (int column = 0; column < result.w; column++)
        {
            result.a(row, column) = A.a(row, column) + B.a(row, column);
        }
    }

    return result;
}


Matrix operator -(const Matrix &A, const Matrix &B)
{
    CORE_ASSERT_TRUE(A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, A.w, false);

    for (int row = 0; row < result.h; row++)
    {
        for (int column = 0; column < result.w; column++)
        {
            result.a(row, column) = A.a(row, column) - B.a(row, column);
        }
    }
    return result;
}


ostream & operator <<(ostream &out, const Matrix &matrix)
{
    streamsize wasPrecision = out.precision(std::numeric_limits<double>::digits10 + 2);
    std::ios_base::fmtflags wasFlags(out.flags());
    int wasWidth = out.width();
    out << std::scientific;

    out << "[";

    for (int i = 0; i < matrix.h; i++)
    {
        for (int j = 0; j < matrix.w; j++)
        {
            out.width(20);
            out << matrix.a(i, j) << " ";
        }
        if (i + 1 < matrix.h)
            out << ";\n";
    }
    out << "]" << std::endl;
    out.precision(wasPrecision);
    out.flags(wasFlags);
    out.width(wasWidth);
    return out;
}

void Matrix::print(ostream &out)
{
    out << "{";
    for (int i = 0; i < h; i++)
    {
        out << "{";
        for (int j = 0; j < w; j++)
        {
            out << a(i, j) << ((j != w - 1) ? "," : "");
        }
        out << "}" << ((i != h - 1) ? ",\n" : "\n") ;
    }
    out << "}";
}


/* Merge three functions below */
Matrix *Matrix::transposed() const
{
    Matrix* result = new Matrix(this->w, this->h, false);
    for (int row = 0; row < result->h; row++)
    {
        for (int column = 0; column < result->w; column++)
        {
            result->a(column, row) = this->a(row, column);
        }
    }

    return result;
}

void Matrix::transpose()
{
    CORE_ASSERT_TRUE(this->h == this->w, "Matrix should be square to transpose.");

    for (int row = 0; row < this->h; row++)
    {
        for (int column = row + 1; column < this->w; column++)
        {
            double tmp;
            tmp = this->a(column, row);
            this->a(column, row) = this->a(row, column);
            this->a(row, column) = tmp;
        }
    }
}

Matrix Matrix::t() const
{
    Matrix result(this->w, this->h, false);
    for (int row = 0; row < result.h; row++)
    {
        for (int column = 0; column < result.w; column++)
        {
            result.a(row, column) = this->a(column, row);
        }
    }

    return result;
}

Matrix Matrix::negative() const
{
   Matrix result(this->w, this->h, false);
   for (int row = 0; row < result.h; row++)
   {
       for (int column = 0; column < result.w; column++)
       {
           result.a(row, column) = -this->a(row, column);
       }
   }
   return result;
}

double Matrix::trace() const
{
    int minDim = h < w ? h : w;
    double sum = 0.0;
    for (int i = 0; i < minDim; i++)
        sum += a(i,i);
    return sum;
}

double Matrix::frobeniusNorm() const
{
    double norm = 0.0;
    for (int row = 0; row < h; row++)
    {
        for (int column = 0; column < w; column++)
        {
            double val = a(row, column);
            norm += val * val;
        }
    }
    return norm;
}



#define Sign(u,v)               ( (v)>=0.0 ? Abs(u) : -Abs(u) )

#define Max(u,v)                ( (u)>(v)?  (u) : (v) )

#define Abs(x)                  ( (x)>0.0?  (x) : (-(x)) )

void Matrix::svdDesc (Matrix33 *A, Vector3dd *W, Matrix33 *V)
{
    svd (A, W, V);
    if (W->at(0) < W->at(1)) { A->swapColumns(0,1); V->swapColumns(0,1); std::swap(W->at(0), W->at(1)); };
    if (W->at(0) < W->at(2)) { A->swapColumns(0,2); V->swapColumns(0,2); std::swap(W->at(0), W->at(2)); };
    if (W->at(1) < W->at(2)) { A->swapColumns(1,2); V->swapColumns(1,2); std::swap(W->at(1), W->at(2)); };
}

void Matrix::svd (Matrix33 *A, Vector3dd *W, Matrix33 *V)
{
    Matrix mA(*A);
    Matrix mW(1, 3);
    Matrix mV(3, 3);
    svd (&mA, &mW, &mV);

    *A = (Matrix33)mA;
    *W = Vector3dd(mW.a(0,0), mW.a(0,1), mW.a(0,2));
    *V = (Matrix33)mV;
}

Matrix Matrix::invPosdefSqrt(const Matrix* preTransform) const
{
    CORE_ASSERT_TRUE_S(h == w);
    CORE_ASSERT_TRUE_S(!preTransform || (preTransform->h == preTransform->w && preTransform->h == h));
#ifndef WITH_BLAS
    Matrix U(w, w), W(1, h), V(*this);
    DiagonalMatrix d(h);
    Matrix::jacobi(&V, &d, &U, 0);
    for (int i = 0; i < h; ++i)
        W.a(0, i) = d.a(i, i);
#else
    Matrix A(*this);
    //int lwork, liwork;
    int m;
    std::unique_ptr<int[]> isuppz(new int[2*h]);
    Matrix W(1, h), U(h, h);
    double vl = 0.0, vu = 0.0, abstol = -1.0;
    int il = 1, iu = h;
    LAPACKE_dsyevr(LAPACK_ROW_MAJOR, 'V', 'A', 'U', h, &A.a(0, 0), A.stride, vl, vu, il, iu, abstol, &m, &W.a(0,0), &U.a(0, 0), U.stride, isuppz.get());
    CORE_ASSERT_TRUE_S(m == h);
#endif
    for (int i = 0; i < h; ++i)
    {
        auto sqrt = std::sqrt(W.a(0, i));
        for (int j = 0; j < w; ++j)
        {
            U.a(j, i) /= sqrt;
        }
    }
    return preTransform ? U ** preTransform : U;
}

/**
 * \brief Solves the linear equation.
 *
 * This functions solves the equation
 * \f[
 *    Ax = B
 * \f]
 * by the Gaussian method of eliminating variables.
 *
 *
 */
bool Matrix::matrixSolveGaussian(Matrix *A, Matrix *B)
{
    CORE_ASSERT_FALSE(A->h != A->w || A->w != B->h || B->w != 1, "Matrix has wrong sizes");

    int row, column, rowRunner, runner;
    int n = A->w;
    /* Walk thought the matrix from left to right */
    for (column = 0; column < n; column++)
    {
        /* Find the row absolute maximum and row number with maximum */
        int rowWithMax = -1;
        double maxInColumn = 0.0;
        for (row = column; row < n; row ++)
        {
            double absA = fabs (A->a(row,column));
            if (absA > maxInColumn)
            {
                maxInColumn = absA;
                rowWithMax = row;
            }
        }

        if (rowWithMax == -1) {
            return false;
            if (B->a(column,0) != 0)
                return false;
            for (rowRunner = 0; rowRunner < n; rowRunner++)
                if (A->a(column,rowRunner) != 0)
                    return false;
        }
        // Now we will work with the row of the same number as the column
        row = column;
        // Now put the row to the i-th position by interchange
        if (rowWithMax != row)
        {
            double tmp;
            for (rowRunner = 0; rowRunner < n; rowRunner ++)
            {
                tmp = A->a(row, rowRunner);
                A->a(row, rowRunner) = A->a(rowWithMax, rowRunner);
                A->a(rowWithMax, rowRunner) = tmp;
            }
            tmp = B->a(row,0);
            B->a(row,0) = B->a(rowWithMax,0);
            B->a(rowWithMax,0) = tmp;
        }
        // And now do the correction of all the rows
        // First divide all the row by the value of (column, row)
        double a = A->a(column, column);
        for (runner = column + 1; runner < n; runner++)
        {
            double d = A->a(runner, column);
            for (rowRunner = column; rowRunner < n; rowRunner++)
            {
                if (d != 0) {
                    A->a(runner, rowRunner) = A->a(runner, rowRunner) * a - A->a(column, rowRunner) * d;
                }
            }
            if (d != 0) {
                B->a(runner,0) = B->a(runner, 0) * a - B->a(column,0) * d;
            }
        }
    }
    for (int i = n - 1; i >= 0; i --)
    {
        for (int j = i + 1; j < n; j ++)
        {
            B->a(i, 0) -= B->a(j, 0) * A->a(i, j);
        }
        B->a(i, 0) /= A->a(i, i);
    }
    return true;
}

/**
 * This function returns an inverse of the matrix.
 *
 * The calculation is done through LU decomposition do far
 *
 * http://en.wikipedia.org/wiki/Gaussian_elimination
 * TODO: http://en.wikipedia.org/wiki/Gauss%E2%80%93Jordan_elimination
 *
 * \attention No check is done, if the matrix is invertible or not
 *
 * \return The \f$I^{-1}\f$
 *
 * */
Matrix Matrix::inv() const
{
    /*
     * Old implementation has bug:
     *  1  1  1                                                 1 0 0
     * (0  0  1).inv() = (sky falls and the earth collapses) = (0 1 0)
     *  0  1  1                                                 0 0 1
     *
     *  It is not funny to discover this bug after some hours spent
     *  debugging my code, so I commented it out and replaced with LAPACK calls (I've also fixed old code, if you care)
     *
     *  Note, however, that you should never invert matrix.
     *  Even if it comes at zero cost (and even if someone
     *  pay you each time you invert matrix)
     */
#ifdef WITH_BLAS
    corecvs::Matrix copy(*this);
    std::unique_ptr<int[]> pivot_(new int[h]);
    int* pivot = pivot_.get();
    CORE_ASSERT_TRUE_S(h == w);
    LAPACKE_dgetrf(LAPACK_ROW_MAJOR, copy.h, copy.w, &copy.a(0, 0), copy.stride, pivot);
    LAPACKE_dgetri(LAPACK_ROW_MAJOR, copy.h, &copy.a(0, 0), copy.stride, pivot);
    return copy;

#else // WITH_BLAS

    unsigned i, j, k;
    double multiplier;

    CORE_ASSERT_TRUE(this->h == this->w, "Matrix should be square to invert.");

    unsigned rank = this->h;
    // Start with identity matrix
    Matrix result(rank, rank, 1.0);
    Matrix copy(this);

    for (i = 0; i < rank; ++i)
    {
        uint pivotRow = i;
        double pivotValue = std::abs(copy.a(i, i));
        for (uint j = i + 1; j < rank; ++j)
        {
            double abs = std::abs(copy.a(j, i));
            if (abs > pivotValue)
            {
                pivotValue = abs;
                pivotRow = j;
            }
        }
        if (pivotRow != i)
        {
            for (uint j = i; j < rank; ++j)
            {
                std::swap(copy.a(pivotRow, j), copy.a(i, j));
            }
            for (uint j = 0; j < rank; ++j)
            {
                std::swap(result.a(pivotRow, j), result.a(i, j));
            }
        }
        double divider = copy.a(i,i);
        if (fabs(divider) == 0)
        {
            return result;
        }
        for (j = 0; j < rank; ++j)
        {
              copy.a(i, j) /= divider;
            result.a(i, j) /= divider;
        }

        for (j = i + 1; j < rank; ++j)
        {
            multiplier = copy.a(j,i);
            if (fabs (copy.a(j,i)) > 0.0)
            {
                for (k = 0; k < rank; ++k)
                {
                      copy.a(j, k) -=   copy.a(i, k) * multiplier;
                    result.a(j, k) -= result.a(i, k) * multiplier;
                }
            }
        }
    }

    for (i = rank; i > 0; --i)
    {
        for (j = 0; j + 1 < i; ++j)
        {
            multiplier = copy.a(j,i - 1);
            for (k = 0; k < rank; k++)
            {
                 copy.a(j, k) -=   copy.a(i - 1, k) * multiplier;
               result.a(j, k) -= result.a(i - 1, k) * multiplier;
            }
        }
    }

    return result;
#endif // !WITH_BLAS
}

bool corecvs::Matrix::linSolve(const corecvs::Vector &B, corecvs::Vector &res, bool symmetric, bool posDef) const
{
    return LinSolve(*this, B, res, symmetric, posDef);
}

bool corecvs::Matrix::LinSolve(const corecvs::Matrix &A, const corecvs::Vector &B
    , corecvs::Vector &res
    , bool symmetric
    , bool posDef)
{
    CORE_ASSERT_TRUE_S(A.h == B.size());
    CORE_ASSERT_TRUE_S(A.h == A.w);
#ifdef WITH_BLAS
    corecvs::Matrix copy(A);
    res = B;
    decltype(LAPACKE_dgetrf(0, 0, 0, 0, 0, 0)) info;
    if (!posDef)
    {
        std::unique_ptr<int[]> pivot_(new int[std::min(A.h, A.w)]);
        int *pivot = pivot_.get();
        if (!symmetric)
        {
            info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, copy.h, copy.w, &copy.a(0, 0), copy.stride, pivot);
            if (!info) LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', res.size(), 1, &copy.a(0, 0), copy.stride, pivot, &res[0], 1);
        }
        else
        {
            info = LAPACKE_dsytrf(LAPACK_ROW_MAJOR, 'U', copy.h, &copy.a(0, 0), copy.stride, pivot);
            if (!info) LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', res.size(), 1, &copy.a(0, 0), copy.stride, pivot, &res[0], 1);
        }
    }
    else
    {
        info = LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'U', copy.w, &copy.a(0, 0), copy.stride);
        if (!info) LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', copy.w, 1, &copy.a(0, 0), copy.stride, &res[0], 1);
    }
    return info == 0;
#else // WITH_BLAS
    CORE_UNUSED(symmetric);
    CORE_UNUSED(posDef);
    res = A.inv() * B;
    return true;

#endif // !WITH_BLAS
}

bool corecvs::Matrix::LinSolveSchurComplement(const corecvs::Matrix &M, const corecvs::Vector &Bv
    , const std::vector<int> &diagBlocks
    , corecvs::Vector &res
    , bool symmetric
    , bool posDef
    , bool)
{
    /*
     * So we partition M and B into
     * +---+---+   /   \   /   \
     * | A | B |   | x |   | a |
     * +---+---+ * +---+ = +---+
     * | C | D |   | y |   | b |
     * +---+---+   \   /   \   /
     * Where D is block-diagonal well-conditioned matrix
     *
     * Then we invert D explicitly and solve
     * x = (A-BD^{-1}C)^{-1}(a-BD^{-1}b)
     * y = D^{-1}(b-Cx)
     *
     * Note that M is symmetric => D is symmetric, (A-BD^{-1}C) is symmetric
     *           M is posdef    => D is posdef,    (A-BD^{-1}C) is symmetric (TODO: isposdef)
     */

    auto Ah = diagBlocks[0],
         Aw = diagBlocks[0];
    auto Bw = M.w - Aw,
         Bh = Ah;
    auto Cw = Aw,
         Ch = M.h - Ah;
    auto Dw = Bw;

#ifndef WITH_BLAS
    CORE_UNUSED(posDef);

    std::vector<corecvs::Matrix> matrices;

    // "Factorizing"
    for (size_t i = 0; i + 1 < diagBlocks.size(); ++i)
    {
        auto from = diagBlocks[i], to = diagBlocks[i + 1];
        matrices.emplace_back(M, from, from, to, to);
    }
    corecvs::parallelable_for(0, (int)matrices.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
            matrices[i] = matrices[i].inv();
    });

    auto A = const_cast<corecvs::Matrix&>(M).createView<corecvs::Matrix>(0, 0, Ah, Aw),
         B = const_cast<corecvs::Matrix&>(M).createView<corecvs::Matrix>(0, Aw, Bh, Bw),
         C = const_cast<corecvs::Matrix&>(M).createView<corecvs::Matrix>(Ah, 0, Ch, Cw);

    // Computing BD^{-1}
    corecvs::Matrix BDinv(Bh, Dw);
    corecvs::parallelable_for(0, (int)matrices.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;

            auto bv  = B.createView<corecvs::Matrix>(0, begin - Aw, Bh, len);

            auto foo = bv * matrices[i];

            for (int k = 0; k < foo.h; ++k)
                for (int j = begin; j < end; ++j)
                    BDinv.a(k, j - Aw) = foo.a(k, j - begin);

        }
    });

    // Computing lhs/rhs
    corecvs::Vector a(Ah, &Bv[0]), b(Ch, &Bv[Ah]);
    auto rhs = a - BDinv * b;
    auto lhs = A - BDinv * C;

    // Solving for x
    corecvs::Vector x(Aw), y(Bw);
    bool foo = lhs.linSolve(rhs, x, symmetric, false);

    if (!foo) return false;

    // Solving for y
    rhs = b - C * x;
    corecvs::parallelable_for(0, (int)matrices.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;
            corecvs::Vector bcx(len);
            for (int j = 0; j < len; ++j)
                bcx[j] = rhs[j + begin - Aw];
            auto res = matrices[i] * bcx;
            for (int j = begin; j < end; ++j)
                y[j - Aw] = res[j - begin];
        }
    });

#else // !WITH_BLAS

    /*
     * The same as above, but with fancy LAPACK
     */
    auto N = diagBlocks.size() - 1;
    std::vector<int> pivots(Dw), pivotIdx(N);

    std::vector<corecvs::Matrix> qrd(N);
    for (size_t i = 0; i < N; ++i)
    {
        qrd[i] = corecvs::Matrix(M, diagBlocks[i], diagBlocks[i], diagBlocks[i + 1], diagBlocks[i + 1]);
        pivotIdx[i] = diagBlocks[i] - diagBlocks[0];
    }

    // Factorizing (without "\"")
    corecvs::parallelable_for(0, (int)N, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i < r.end(); ++i)
                {
                    auto& MM = qrd[i];
                    if (!symmetric)
                    {
                        LAPACKE_dgetrf(LAPACK_ROW_MAJOR, MM.h, MM.w, MM.data, MM.stride, &pivots[pivotIdx[i]]);
                    }
                    else
                    {
                        if (posDef)
                            LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'U', MM.h, MM.data, MM.stride);
                        else
                            LAPACKE_dsytrf(LAPACK_ROW_MAJOR, 'U', MM.h, MM.data, MM.stride, &pivots[pivotIdx[i]]);
                    }
                }
            });

    // Computing BD^{-1}
    auto A = const_cast<corecvs::Matrix&>(M).createView<corecvs::Matrix>(0, 0, Ah, Aw),
         B = const_cast<corecvs::Matrix&>(M).createView<corecvs::Matrix>(0, Aw, Bh, Bw),
         C = const_cast<corecvs::Matrix&>(M).createView<corecvs::Matrix>(Ah, 0, Ch, Cw);

    // Computing BD^{-1}
    //recvs::Matrix BDinv(Bh, Dw);
    corecvs::Matrix DinvtBt = B.t();
    CORE_ASSERT_TRUE_S(DinvtBt.h == Dw && DinvtBt.w == Bh);
    corecvs::parallelable_for(0, (int)qrd.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;
            auto& MM = qrd[i];
            auto ptr = &DinvtBt.a(diagBlocks[i] - diagBlocks[0], 0);
            if (!symmetric)
            {
                LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'T', len, Bh, MM.data, MM.stride, &pivots[pivotIdx[i]], ptr, DinvtBt.stride);
            }
            else
            {
                if (posDef)
                    LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', len, Bh, MM.data, MM.stride, ptr, DinvtBt.stride);
                else
                    LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', len, Bh, MM.data, MM.stride, &pivots[pivotIdx[i]], ptr, DinvtBt.stride);
            }
        }
    });
    // Computing lhs/rhs
    corecvs::Vector a(Ah, &Bv[0]), b(Ch, &Bv[Ah]);
    auto rhs = a - b * DinvtBt;

    auto lhs = A;
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, lhs.h, lhs.w, C.h, -1.0, DinvtBt.data, DinvtBt.stride, C.data, C.stride, 1.0, lhs.data, lhs.stride);

    // Solving for x
    corecvs::Vector x(Aw), y(Bw);
    bool foo = lhs.linSolve(rhs, x, symmetric, false);

    if (!foo) return false;

    // Solving for y
    rhs = b - C * x;
    corecvs::parallelable_for(0, (int)qrd.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;
            auto& MM = qrd[i];
            corecvs::Vector bcx(len);
            for (int j = 0; j < len; ++j)
                bcx[j] = rhs[j + begin - Aw];
            if (!symmetric)
            {
                LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', len, 1, MM.data, MM.stride, &pivots[pivotIdx[i]], &bcx[0], 1);
            }
            else
            {
                if (posDef)
                    LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', len, 1, MM.data, MM.stride, &bcx[0], 1);
                else
                    LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', len, 1, MM.data, MM.stride, &pivots[pivotIdx[i]], &bcx[0], 1);
            }
            auto res = bcx;
            for (int j = begin; j < end; ++j)
                y[j - Aw] = res[j - begin];
        }
    });

#endif // WITH_BLAS

    for (int i = 0; i < Aw; ++i)
        res[i] = x[i];
    for (int j = 0; j < Bw; ++j)
        res[j + Aw] = y[j];

    return true;
}

bool corecvs::Matrix::linSolveSchurComplement(const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric, bool posDef)
{
    return corecvs::Matrix::LinSolveSchurComplement(*this, B, diagBlocks, res, symmetric, posDef);
}

Matrix Matrix::invSVD() const
{
    CORE_ASSERT_TRUE(this->h == this->w, "Matrix should be square to invert.");
    int rank = this->h;
    Matrix X(this);
    Matrix result(rank, rank, 1.0);
    Matrix W(1,    rank);
    Matrix V(rank, rank);

    Matrix::svd (&X, &W, &V);

    X.transpose();
  //  cout << "Singular:" << W << endl;

    for (int i = 0; i < rank; i++)
    {
        for (int j = 0; j < rank; j++)
        {
            if (W.a(0,i) > 1e-5)
                X.a(i,j) /= W.a(0,i);
        }
    }
    return V * X;
}

/*
 * FIXME: This is obviously incorrect, since determinant computed with this
 * routine will always be >= 0
 */
double Matrix::detSVD() const
{
    CORE_ASSERT_TRUE(this->h == this->w, "Matrix should be square to invert.");
    int rank = this->h;
    Matrix X(this);
    Matrix result(rank, rank, 1.0);
    Matrix W(1,    rank);
    Matrix V(rank, rank);

    Matrix::svd (&X, &W, &V);

    double det = 1.0;
    for (int i = 0; i < rank; i++)
    {
       det *= W.a(0,i);
    }
    return det;
}

#ifdef WITH_BLAS
double corecvs::Matrix::det() const
{
    CORE_ASSERT_TRUE(w == h, "Determinant of square matrices can be computed");
    corecvs::Vector tau(w);
    corecvs::Matrix R = *this;
    LAPACKE_dgeqrf(LAPACK_ROW_MAJOR, h, w, &R.a(0, 0), stride, &tau[0]);
    double det = 1.0;
    for (int i = 0; i < w; ++i)
        det *= R.a(i, i);
    double tauM = 1.0;
    for (int i = 0; i < w; ++i)
        if (tau[i] != 0.0)
            tauM *= -1.0;
    return det * tauM;
}
#endif // WITH_BLAS

Vector2d32 Matrix::getMinCoord() const
{
    Vector2d32 result(0, 0);
    double value = numeric_limits<double>::max();

    for (int row = 0; row < h; row++)
    {
        for (int column = 0; column < w; column++)
        {
            if (this->a(row, column) < value)
            {
                value = this->a(row, column);
                result = Vector2d32(row, column);
            }
        }
    }
    return result;
}

Vector2d32 Matrix::getMaxCoord() const
{
    Vector2d32 result(0, 0);
    double value = -numeric_limits<double>::max();

    for (int row = 0; row < h; row++)
    {
        for (int column = 0; column < w; column++)
        {
            if (this->a(row, column) > value)
            {
                value = this->a(row, column);
                result = Vector2d32(row, column);
            }
        }
    }
    return result;
}

Matrix Matrix::column(int column)
{
    Matrix toReturn(h, 1);
    for (int row = 0; row < h; row++)
    {
        toReturn.a(row, 0) = this->a(row, column);
    }
    return toReturn;
}

Matrix Matrix::row(int row)
{
    Matrix toReturn(1, w);
    for (int column = 0; column < w; column++)
    {
        toReturn.a(0, column) = this->a(row, column);
    }
    return toReturn;
}


/**
 *
 * \brief This method contains SVD implementation.
 *
 * \remark This SVD is not http://en.wikipedia.org/wiki/Snayperskaya_Vintovka_Dragunova
 *
 * \remark I have used an existing solution with unclear license. So far let it be so.
 *  In fact this solution is a reworked solution from "Numerical Recipies in C", so it it will
 *  be easy to re-implement it to make it absolutely clean.
 *
 *  Seems like this code is an semi-automatic translation from Fortran.
 *
 *
 *  \author Alexander Pimenov based on code form "Numerical Recipies in C" and looking over Tina code
 */

/**
 * Given a matrix A, Matrix::svd() computes its singular value decomposition, A = U W V^T.
 * The matrix U replaces A on output. The diagonal matrix of singular values W is output
 * as a matrix W. The matrix V (not the transpose V T ) is output as V.
 */
void Matrix::svd (Matrix *A, Matrix *W, Matrix *V)
{
    CORE_ASSERT_TRUE((A != NULL && W != NULL && V != NULL), "NULL input Matrix\n");

    CORE_ASSERT_TRUE((W->h ==    1 && W->w == A->w), "Matrix W has wrong size\n");
    CORE_ASSERT_TRUE((A->w == V->h && A->w == V->w), "Matrix V has wrong size\n");

    int n = A->w;
    int m = A->h;

    CORE_ASSERT_FALSE((m < n), "SVDCMP: You must augment A with extra zero rows");
    CORE_ASSERT_TRUE(n > 0, "A width should not be zero");

#ifdef WITH_BLAS
    // Stupidity remains here, just non-super-stupid method
    Vector vv(n);
    LAPACKE_dgesvd(LAPACK_ROW_MAJOR, 'O', 'A', m, n, &A->a(0, 0), A->stride, &W->a(0, 0), 0, 1, &V->a(0, 0), V->stride, &vv[0]);
    V->transpose();
#else
    int     i, its, j, jj, k, l=0, nm = 0, nm1 = n - 1, mm1 = m - 1;
    double  c, f, h, s, x, y, z;
    double  anorm = 0.0;
    double  g = 0.0;
    double scale = 0.0;
    double *rv1;

    rv1 = new double[n];

    /* Householder reduction to bidigonal form */
    for (i = 0; i < n; i++)
    {
        l = i + 1;
        rv1[i] = scale * g;
        g = s = scale = 0.0;
        if (i < m)
        {
            for (k = i; k < m; k++)
                scale += fabs(A->a(k,i));
            if (scale)
            {
                for (k = i; k < m; k++)
                {
                    A->a(k,i) /= scale;
                    s += A->a(k,i) * A->a(k,i);
                }
                f = A->a(i,i);
                g = -Sign(sqrt(s), f);
                h = f * g - s;
                A->a(i,i) = f - g;
                if (i != nm1)
                {
                    for (j = l; j < n; j++)
                    {
                        for (s = 0.0, k = i; k < m; k++)
                            s += A->a(k,i) * A->a(k,j);
                        f = s / h;
                        for (k = i; k < m; k++)
                            A->a(k,j) += f * A->a(k,i);
                    }
                }
                for (k = i; k < m; k++)
                    A->a(k,i) *= scale;
            }
        }
        W->a(0,i) = scale * g;
        g = s = scale = 0.0;
        if (i < m && i != nm1)
        {
            for (k = l; k < n; k++)
                scale += fabs(A->a(i,k));
            if (scale)
            {
                for (k = l; k < n; k++)
                {
                    A->a(i,k) /= scale;
                    s += A->a(i,k) * A->a(i,k);
                }
                f = A->a(i,l);
                g = -Sign(sqrt(s), f);
                h = f * g - s;
                A->a(i,l) = f - g;
                for (k = l; k < n; k++)
                    rv1[k] = A->a(i,k) / h;
                if (i != mm1)
                {
                    for (j = l; j < m; j++)
                    {
                        for (s = 0.0, k = l; k < n; k++)
                            s += A->a(j,k) * A->a(i,k);
                        for (k = l; k < n; k++)
                            A->a(j,k) += s * rv1[k];
                    }
                }
                for (k = l; k < n; k++)
                    A->a(i,k) *= scale;
            }
        }
        anorm = Max(anorm, (fabs(W->a( 0, i)) + fabs(rv1[i])));
    }

    /* Accumulation of right-hand transformations */
    for (i = n - 1; i >= 0; i--)
    {
        if (i < nm1)
        {
            if (g)
            {
                /* double division to avoid possible underflow */
                for (j = l; j < n; j++)
                    V->a( j, i) = (A->a( i, j) / A->a( i, l)) / g;
                for (j = l; j < n; j++)
                {
                    for (s = 0.0, k = l; k < n; k++)
                        s += A->a(i, k) * V->a(k, j);
                    for (k = l; k < n; k++)
                        V->a(k, j) += s * V->a(k, i);
                }
            }
            for (j = l; j < n; j++)
                V->a(i, j) = V->a(j, i) = 0.0;
        }
        V->a(i, i) = 1.0;
        g = rv1[i];
        l = i;
    }

    /* Accumulation of left-hand transformations */
    for (i = n - 1; i >= 0; i--)
    {
        l = i + 1;
        g = W->a(0, i);
        if (i < nm1)
            for (j = l; j < n; j++)
                A->a(i, j) = 0.0;
        if (g)
        {
            g = 1.0 / g;
            if (i != nm1)
            {
                for (j = l; j < n; j++)
                {
                    for (s = 0.0, k = l; k < m; k++)
                        s += A->a(k, i) * A->a(k, j);
                    f = (s / A->a(i, i)) * g;
                    for (k = i; k < m; k++)
                        A->a(k, j) += f * A->a(k, i);
                }
            }
            for (j = i; j < m; j++)
                A->a(j, i) *= g;
        }
        else {
            for (j = i; j < m; j++)
                A->a(j, i) = 0.0;
        }
        ++A->a(i, i);
    }

    /* diagonalization of the bidigonal form */
    for (k = n - 1; k >= 0; k--)
    {                           /* loop over singlar values */
        for (its = 0; its < 30; its++)
        {                       /* loop over allowed iterations */
            int flag = 1;
            for (l = k; l >= 0; l--)
            {                   /* test for splitting */
                nm = l - 1;     /* note that rv1[l] is always zero */
                if (Abs(rv1[l]) + anorm == anorm)
                {
                    flag = 0;
                    break;
                }
                if (nm >= 0 && fabs(W->a(0, nm)) + anorm == anorm)
                    break;
            }
            if (l < 0) {
                printf("possible bugfix: l=-1 => 0\n");
                l = 0;      // bugfix: negative l is possible here!
            }
            if (flag)
            {
                c = 0.0;        /* cancellation of rv1[l], if l>1 */
                s = 1.0;
                for (i = l; i <= k; i++)
                {
                    f = s * rv1[i];
                    if (Abs(f) + anorm != anorm)
                    {
                        g = W->a(0, i);
                        h = Vector2dd(f, g).getLengthStable();
                        W->a(0, i) = h;
                        h = 1.0 / h;
                        c = g * h;
                        s = (-f * h);
                        for (j = 0; j < m; j++)
                        {
                            y = A->a(j, nm);
                            z = A->a(j, i);
                            A->a(j, nm) = y * c + z * s;
                            A->a(j,  i) = z * c - y * s;
                        }
                    }
                }
            }

            z = W->a(0, k);
            if (l == k)
            {                   /* convergence */
                if (z < 0.0)
                {
                    W->a(0, k) = -z;
                    for (j = 0; j < n; j++)
                        V->a(j, k) = (-V->a(j, k));
                }
                break;
            }
            if (its == 30)
                printf("No convergence in 30 SVDCMP iterations");

            x = W->a(0, l);           /* shift from bottom 2-by-2 minor */
            nm = k - 1;
            y = W->a(0, nm);
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = Vector2dd(f, 1.0).getLengthStable();
            /* next QR transformation */
            f = ((x - z) * (x + z) + h * ((y / (f + Sign(g, f))) - h)) / x;
            c = s = 1.0;
            for (j = l; j <= nm; j++)
            {
                i = j + 1;
                g = rv1[i];
                y = W->a(0, i);
                h = s * g;
                g = c * g;
                z = Vector2dd(f,h).getLengthStable();
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y = y * c;
                for (jj = 0; jj < n; jj++)
                {
                    x = V->a(jj, j);
                    z = V->a(jj, i);
                    V->a(jj, j) = x * c + z * s;
                    V->a(jj, i) = z * c - x * s;
                }
                z = Vector2dd(f,h).getLengthStable();
                W->a(0, j) = z;       /* rotation can be arbitrary id z=0 */
                if (z)
                {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = (c * g) + (s * y);
                x = (c * y) - (s * g);
                for (jj = 0; jj < m; jj++)
                {
                    y = A->a(jj, j);
                    z = A->a(jj, i);
                    A->a(jj, j) = y * c + z * s;
                    A->a(jj, i) = z * c - y * s;
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            W->a(0, k) = x;
        }
    }
    delete[] rv1;
#endif
}

std::pair<bool, Matrix> Matrix::incompleteCholseky()
{
    auto res = SparseMatrix(*this).incompleteCholseky();
    return std::make_pair(res.first, Matrix(res.second));
}


Vector Matrix::dtrsv(const Vector &v, bool upper, bool notrans) const
{
    Vector res(v);
    CORE_ASSERT_TRUE_S(v.size() == h && v.size() == w);
#ifdef WITH_BLAS
    cblas_dtrsv(CblasRowMajor, upper ? CblasUpper : CblasLower, notrans ? CblasNoTrans : CblasTrans, CblasNonUnit, w, &a(0, 0), stride, &res[0], 1);
#else
    int cs = (upper ? 0 : 2) + (notrans ? 0 : 1);
    switch (cs)
    {
    case 0:
        for (int i = h - 1; i >= 0; --i)
        {
            double pivot = a(i, i), sum = res[i];
            CORE_ASSERT_TRUE_S(pivot != 0.0);
            for (int j = i + 1; j < w; ++j)
                sum -= res[j] * a(i, j);
            res[i] = sum / pivot;
        }
        break;
    case 1:
        for (int j = 0; j < w; ++j)
        {
            double pivot = a(j, j), sum = res[j];
            CORE_ASSERT_TRUE_S(pivot != 0.0);
            for (int i = j - 1; i >= 0; --i)
                sum -= res[i] * a(i, j);
            res[j] = sum / pivot;
        }
        break;
    case 2:
        for (int i = 0; i < h; ++i)
        {
            double pivot = a(i, i), sum = res[i];
            CORE_ASSERT_TRUE_S(pivot != 0.0);
            for (int j = 0; j < i; ++j)
                sum -= res[j] * a(i, j);
            res[i] = sum / pivot;
        }
        break;
    case 3:
        for (int j = w - 1; j >= 0; --j)
        {
            double pivot = a(j, j), sum = res[j];
            CORE_ASSERT_TRUE_S(pivot != 0.0);
            for (int i = j + 1; i < h; ++i)
                sum -= res[i] * a(i, j);
            res[j] = sum / pivot;
        }
    default:
        break;
    }
#endif
    return res;
}

void Matrix::svd(Matrix *A, DiagonalMatrix *W, Matrix *V)
{
    CORE_ASSERT_TRUE((W->size() == A->w), "Matrix W has wrong size\n");
    Matrix WVec(1, W->size());
    svd(A, &WVec, V);
    for (int i = 0; i < W->size(); i++)
    {
        W->a(i) = WVec.a(0, i);
    }
}

 /**
  *  \author Alexander Pimenov based on code form "Numerical Recipies in C"
  *
  *   Computes all eigenvalues and eigenvectors of a real symmetric matrix a[1..n][1..n].
  *   On output, elements of a above the diagonal are destroyed. d[1..n] returns the eigenvalues of a v[1..n][1..n]
  *   is a matrix whose columns contain, on output, the normalized eigenvectors of a nrot returns the number of Jacobi rotations that were required.
  **/

#define ROTATE(mat,i,j,k,l) g = mat->a(i,j); \
                            h = mat->a(k,l); \
                            mat->a(i,j) = g - s * ( h + g * tau); \
                            mat->a(k,l) = h + s * ( g - h * tau);

int Matrix::jacobi(Matrix *a, DiagonalMatrix *d, Matrix *v, int *nrotpt, bool trace)
{
    CORE_ASSERT_TRUE((a != NULL && d != NULL && v != NULL), "NULL input Matrix\n");
    CORE_ASSERT_TRUE((a->h == a->w), "Matrix A is not square\n");
    CORE_ASSERT_TRUE((v->h == a->h && v->w == a->w), "Matrix V has wrong size\n");
    CORE_ASSERT_TRUE((a->w == d->size()), "Matrix D has wrong size\n");

    int j,iq,ip,i;
    double tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

    int n = a->h;

    b = new double[n];
    z = new double[n];

    for (ip = 0;ip < n; ip++) {   // Initialize to the identity matrix.
        for ( iq = 0; iq < n; iq++) {
            v->a(ip,iq) = 0.0;
        }
        v->a(ip,ip) = 1.0;
    }

    for (ip = 0;ip < n; ip++)  // Initialize b and d to the diagonal of a .
    {
        b[ip] = a->a(ip,ip);
        d->a(ip) = a->a(ip,ip);
        z[ip] = 0.0;             // This vector will accumulate terms of the form ta_pq
    }

    int nrot=0;

    for (i = 0; i < 50; i++)
    {
        sm = 0.0;
        for (ip = 0; ip < n - 1; ip++)   //  Sum off-diagonal elements.
        {
            for (iq = ip + 1; iq < n; iq++) {
                sm += fabs(a->a(ip,iq));
            }
        }

        if (trace) SYNC_PRINT(("Matrix::jacobi(): It:%d val:%.17g\n", i, sm));

        if (sm == 0.0) { // The normal return, which relies on quadratic convergence to machine underflow.
            deletearr_safe (z);
            deletearr_safe (b);
            if (nrotpt != NULL) {
                *nrotpt = nrot;
            }
            return 0;
        }

        if (i < 4) {
            tresh = 0.2 * sm / (n * n); // ...on the first three sweeps.
        } else {
            tresh = 0.0;          //...thereafter.
        }

        for (ip = 0;ip < n-1; ip++)
        {
            for (iq = ip + 1; iq < n; iq++)
            {
                g = 100.0 * fabs(a->a(ip,iq)); // After four sweeps, skip the rotation if the off-diagonal element is small.

                if (i > 4 && (double)(fabs(d->a(ip)) + g) == (double)fabs(d->a(ip))
                          && (double)(fabs(d->a(iq)) + g) == (double)fabs(d->a(iq)))
                {
                    a->a(ip,iq) = 0.0;
                }
                else if (fabs(a->a(ip,iq)) > tresh)
                {
                    h = d->a(iq) - d->a(ip);
                    if ((fabs(h) + g) == fabs(h)) {
                        t = (a->a(ip,iq)) / h;  // t =1 /(2 \tau)
                    } else {
                        theta = 0.5 * h / (a->a(ip,iq));
                        t = 1.0 / (fabs(theta) + sqrt(1.0 + theta*theta));
                        if (theta < 0.0) t = -t;
                    }

                    c = 1.0 / sqrt(1.0 + t * t);
                    s = t * c;
                    tau = s / (1.0 + c);
                    h = t * a->a(ip,iq);
                    z[ip] -= h;
                    z[iq] += h;
                    d->a(ip) -= h;
                    d->a(iq) += h;
                    a->a(ip,iq) = 0.0;

                    for (j = 0; j <= ip - 1; j++) { // Case of rotations 1 <= j < p .
                        ROTATE(a, j, ip, j, iq)
                    }
                    for (j = ip + 1; j <= iq - 1;j++) { // Case of rotations p < j < q .
                        ROTATE(a, ip, j, j, iq)
                    }
                    for (j = iq + 1; j < n; j++) { // Case of rotations  q < j<= n .
                        ROTATE(a, ip, j, iq, j)
                    }
                    for (j = 0; j < n; j++) {
                        ROTATE(v, j, ip, j, iq)
                    }
                    ++nrot;
                }
            }
        }
        for (ip = 0;ip < n; ip++)
        {
            b[ip] += z[ip];
            d->a(ip) = b[ip];                   // Update d with the sum of ta_pq and reinitialize z
            z[ip] = 0.0;
        }
    }
    deletearr_safe (b);
    deletearr_safe (z);

    SYNC_PRINT(("Too many iterations in routine jacobi\n"));

    return 1;
}
#undef ROTATE

Matrix Matrix::ata() const
{
#ifndef WITH_BLAS
    return t() * *this;
#else
    Matrix res(w, w);
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, w, w, h, 1.0, data, stride, data, stride, 0.0, res.data, res.stride);
    return res;
#endif
}

} //namespace corecvs

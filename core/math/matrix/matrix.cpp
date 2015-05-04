/**
 * \file matrix.cpp
 * \brief Generic dense matrix of the arbitatry size
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */
#include <algorithm>

#include "global.h"
#include "matrix.h"
#include "matrix33.h"

namespace corecvs {

/* The constructor is not in single cycle due to possible stride of the matrix */
Matrix::Matrix(Matrix33 &in) : MatrixBase(3, 3, false)
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
Matrix::Matrix(Matrix44 &in) : MatrixBase(4, 4, false)
{
    for (int i = 0; i < in.H; i++)
        for (int j = 0; j < in.W; j++)
        {
            a(i,j) = in.a(i,j);
        }
}

Matrix::Matrix(int32_t h, int32_t w, double value) : MatrixBase(h, w, (double)(0.0))
{
    int32_t minDim = min(h,w);
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

Matrix operator *(const double &a, const Matrix &B)
{
    Matrix result(B.h, B.w, false);
    int row, column;
    for (row = 0; row < result.h ; row++)
    {
        for (column = 0; column < result.w ; column++)
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
    ASSERT_TRUE (this->w == V.h, "Matrices have wrong sizes");
    Matrix *result = new Matrix(this->h, V.w, false);

    int row, column, runner;
    for (row = 0; row < result->h ; row++)
        for (column = 0; column < result->w ; column++)
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


Matrix operator *(const Matrix &A, const Matrix &B)
{
    ASSERT_TRUE (A.w == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, B.w, false);

    int row, column, runner;
    for (row = 0; row < result.h ; row++)
        for (column = 0; column < result.w ; column++)
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
    ASSERT_TRUE (M.w == V.size(), "Matrix and vector have wrong sizes");
    Vector result(M.h);
    int row, column;
    for (row = 0; row < M.h; row++)
    {
        double sum = 0.0;
        for (column = 0; column < M.w ; column++)
        {
            sum += V.at(column) * M.a(row, column);
        }
        result.at(row) = sum;
    }
    return result;
}

Vector operator *(const Vector &V, const Matrix &M)
{
    ASSERT_TRUE (M.h == V.size(), "Matrix and vector have wrong sizes");
    Vector result(M.w);
    int row, column;
    for (column = 0; column < M.w ; column++)
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
    int32_t minDim = min(M.h,M.w);
    minDim = min(minDim, D.size());
    for (int i = 0; i < minDim ; i++)
    {
         for (int j = 0; j < minDim ; j++)
         {
            M.a(i,j) *= D.at(i);
         }
    }
    return M;
}

Matrix operator *(const Matrix &M, const DiagonalMatrix &D)
{
    int32_t minDim = min(M.h,M.w);
    minDim = min(minDim, D.size());
    Matrix result(minDim, minDim, false);
    for (int i = 0; i < minDim ; i++)
    {
         for (int j = 0; j < minDim ; j++)
         {
             result.a(i,j) = M.a(i,j) * D.at(i);
         }
    }
    return result;
}

Matrix operator *(DiagonalMatrix &D, const Matrix &M)
{
    int32_t minDim = min(M.h,M.w);
    minDim = min(minDim, D.size());
    Matrix result(minDim, minDim, false);
    for (int i = 0; i < minDim ; i++)
    {
        for (int j = 0; j < minDim ; j++)
        {
            result.a(i,j) = M.a(i,j) * D.at(j);
        }
    }
    return result;
}


/* TODO: Use abstract buffer binaryOperation elementWize */
Matrix operator +(const Matrix &A, const Matrix &B)
{
    ASSERT_TRUE (A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, A.w, false);

    for (int row = 0; row < result.h ; row++)
    {
        for (int column = 0; column < result.w ; column++)
        {
            result.a(row, column) = A.a(row, column) + B.a(row, column);
        }
    }

    return result;
}


Matrix operator -(const Matrix &A, const Matrix &B)
{
    ASSERT_TRUE (A.w == B.w && A.h == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, A.w, false);

    for (int row = 0; row < result.h ; row++)
    {
        for (int column = 0; column < result.w ; column++)
        {
            result.a(row, column) = A.a(row, column) - B.a(row, column);
        }
    }
    return result;
}


ostream & operator <<(ostream &out, const Matrix &matrix)
{
    streamsize wasPrecision = out.precision(6);
    for (int i = 0; i < matrix.h; i++)
    {
        out << "[";
        for (int j = 0; j < matrix.w; j++)
        {
            out.width(9);
            out << matrix.a(i, j) << " ";
        }
        out << "]\n";
    }
    out.precision(wasPrecision);
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


/* Merge three functions below*/
Matrix *Matrix::transposed() const
{
    Matrix* result = new Matrix(this->w, this->h, false);
    for (int row = 0; row < result->h ; row++)
    {
        for (int column = 0; column < result->w ; column++)
        {
            result->a(column, row) = this->a(row, column);
        }
    }

    return result;
}

void Matrix::transpose()
{
    ASSERT_TRUE(this->h == this->w, "Matrix should be square to transpose.");

    for (int row = 0; row < this->h ; row++)
    {
        for (int column = row + 1; column < this->w ; column++)
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
    for (int row = 0; row < result.h ; row++)
    {
        for (int column = 0; column < result.w ; column++)
        {
            result.a(row, column) = this->a(column, row);
        }
    }

    return result;
}

Matrix Matrix::negative() const
{
   Matrix result(this->w, this->h, false);
   for (int row = 0; row < result.h ; row++)
   {
       for (int column = 0; column < result.w ; column++)
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
    for (int row = 0; row < h ; row++)
    {
        for (int column = 0; column < w ; column++)
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
    Matrix mW(1,3);
    Matrix mV(3,3);
    svd (&mA, &mW, &mV);

    *A = (Matrix33)mA;
    *W = Vector3dd(mW.a(0,0),mW.a(0,1),mW.a(0,2));
    *V = (Matrix33)mV;
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
    ASSERT_FALSE(A->h != A->w || A->w != B->h || B->w != 1, "Matrix has wrong sizes");

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
    unsigned i, j, k;

    double multiplier;

    ASSERT_TRUE(this->h == this->w, "Matrix should be square to invert.");

    unsigned rank = this->h;
    // Start with identity matrix
    Matrix result(rank, rank, 1.0);
    Matrix copy(this);

    for (i = 0; i < rank; ++i)
    {
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
}



Matrix Matrix::invSVD() const
{
    ASSERT_TRUE(this->h == this->w, "Matrix should be square to invert.");
    int rank = this->h;
    Matrix X(this);
    Matrix result(rank, rank, 1.0);
    Matrix W(1,    rank);
    Matrix V(rank, rank);

    Matrix::svd (&X, &W, &V);

    X.transpose();
  //  cout << "Singular:" << W << endl;

    for (int i = 0; i < rank ; i++)
    {
        for (int j = 0; j < rank ; j++)
        {
            if (W.a(0,i) > 1e-5)
                X.a(i,j) /= W.a(0,i);
        }
    }
    return V * X;
}

double Matrix::detSVD() const
{
    ASSERT_TRUE(this->h == this->w, "Matrix should be square to invert.");
    int rank = this->h;
    Matrix X(this);
    Matrix result(rank, rank, 1.0);
    Matrix W(1,    rank);
    Matrix V(rank, rank);

    Matrix::svd (&X, &W, &V);

    double det = 1.0;
    for (int i = 0; i < rank ; i++)
    {
       det *= W.a(0,i);
    }
    return det;
}

Vector2d32 Matrix::getMinCoord() const
{
    Vector2d32 result(0, 0);
    double value = numeric_limits<double>::max();

    for (int row = 0; row < h ; row++)
    {
        for (int column = 0; column < w ; column++)
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

    for (int row = 0; row < h ; row++)
    {
        for (int column = 0; column < w ; column++)
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
    for (int row = 0; row < h ; row++)
    {
        toReturn.a(row, 0) = this->a(row, column);
    }
    return toReturn;
}

Matrix Matrix::row(int row)
{
    Matrix toReturn(1, w);
    for (int column = 0; column < w ; column++)
    {
        toReturn.a(0, column) = toReturn.a(row, column);
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
    ASSERT_TRUE((A != NULL && W != NULL && V != NULL), "NULL input Matrix\n");

    ASSERT_TRUE((W->h ==    1 && W->w == A->w), "Matrix W has wrong size\n" );
    ASSERT_TRUE((A->w == V->h && A->w == V->w),"Matrix V has wrong size\n");

    int n = A->w;
    int m = A->h;

    /* BUG `nm' may be used uninitialized in this function */
    int     flag, i, its, j, jj, k, l=0, nm, nm1 = n - 1, mm1 = m - 1;
    double  c, f, h, s, x, y, z;
    double  anorm = 0.0;
    double  g = 0.0;
    double scale = 0.0;
    double *rv1;

    ASSERT_FALSE((m < n), "SVDCMP: You must augment A with extra zero rows");
    ASSERT_TRUE(n > 0 , "A width should not be zero");

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
            } else
                for (j = i; j < m; j++)
                    A->a(j, i) = 0.0;
            ++A->a(i, i);
        }
        /* diagonalization of the bidigonal form */
        for (k = n - 1; k >= 0; k--)
        {                           /* loop over singlar values */
            for (its = 0; its < 30; its++)
            {                       /* loop over allowed iterations */
                flag = 1;
                for (l = k; l >= 0; l--)
                {                   /* test for splitting */
                    nm = l - 1;     /* note that rv1[l] is always zero */
                    if (Abs(rv1[l]) + anorm == anorm)
                    {
                        flag = 0;
                        break;
                    }
                    if (fabs(W->a(0, nm)) + anorm == anorm)
                        break;
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
                                A->a( j, nm) = y * c + z * s;
                                A->a( j, i) = z * c - y * s;
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
}

void Matrix::svd(Matrix *A, DiagonalMatrix *W, Matrix *V)
{
    ASSERT_TRUE((W->size() == A->w), "Matrix W has wrong size\n" );
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
  *
  *
  *
  **/

#define ROTATE(mat,i,j,k,l) g = mat->a(i,j); h = mat->a(k,l); mat->a(i,j) = g - s * ( h + g * tau); mat->a(k,l) = h + s * ( g - h * tau);

int Matrix::jacobi(Matrix *a, DiagonalMatrix *d, Matrix *v, int *nrotpt)
{

    ASSERT_TRUE((a != NULL && d != NULL && v != NULL), "NULL input Matrix\n");
    ASSERT_TRUE((a->h == a->w), "Matrix A is not square\n" );
    ASSERT_TRUE((v->h == a->h && v->w == a->w), "Matrix V has wrong size\n" );
    ASSERT_TRUE((a->w == d->size()),"Matrix D has wrong size\n");

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

    for (i = 0; i < 50; i++) {
        sm = 0.0;
        for (ip = 0;ip < n - 1; ip++)   //  Sum off-diagonal elements.
        {
            for (iq = ip + 1;iq < n; iq++) {
                sm += fabs(a->a(ip,iq));
            }
        }

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
                    if ((float)(fabs(h) + g) == (float)fabs(h)) {
                        t = (a->a(ip,iq)) / h;  // t =1 /(2 \tau)
                    } else {
                        theta = 0.5 * h / (a->a(ip,iq));
                        t = 1.0 / (fabs(theta) + sqrt(1.0 + theta*theta));
                        if (theta < 0.0) t = -t;
                    }

                    c = 1.0 / sqrt(1 + t*t);
                    s = t * c;
                    tau = s / (1.0+c);
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
    SYNC_PRINT(("Too many iterations in routine jacobi"));
    return 1;
}
#undef ROTATE


} //namespace corecvs


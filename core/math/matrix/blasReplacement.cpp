#include "core/math/matrix/blasReplacement.h"

namespace corecvs {

// TODO: it has a bug, see testMatrixOperations result when change the operator *(Matrix&, Matrix&) to use this!!!
Matrix Matrix::multiplyHomebrew(const Matrix &A, const Matrix &B, bool parallel, bool vectorize)
{
    CORE_ASSERT_TRUE(A.w == B.h, "Matrices have wrong sizes");
    Matrix result(A.h, B.w, false);
    if (vectorize) {
        parallelable_for (0, result.h, 8, ParallelMM8<true> (&A, &B, &result), parallel);
    } else {
        parallelable_for (0, result.h, 8, ParallelMM8<false>(&A, &B, &result), parallel);
    }
    return result;
}

Matrix Matrix::multiplyHomebrewMD(const Matrix &M, const DiagonalMatrix &D)
{
    CORE_ASSERT_TRUE(M.w == D.size(), "Matrix and DiagonalMatrix have wrong sizes");
    Matrix result(M.h, M.w);

    parallelable_for (0, result.h, 8, ParallelMD(&M, &D, &result), true);
    return result;
}


Vector Matrix::multiplyHomebrewMV(const Matrix &M, const Vector &V)
{
    CORE_ASSERT_TRUE(M.w == V.size(), "Matrix and Vector have wrong sizes");
    Vector result(M.h);
    parallelable_for (0, M.h, 8, ParallelMV(&M, &V, &result));
    return result;
}


} //namespace corecvs

/**
 * \file  linlse.c
 * \brief This file describes the linear least squares estimation for image rectification
 *
 *
 * \ingroup corefiles
 * \date Jun 9, 2009
 * \author Alexander Pimenov
 */

#include "linlse.h"

/**
 * The least squares estimation matrix.
 *
 */
/*void lse(Matrix* X, Matrix *y, Matrix *result)
{
        Matrix *TX = matrixCreate(X->n, X->m);
        Matrix *A = matrixCreate(X->n, X->n);
        Matrix *D = matrixCreate(1, X->n);

        matrixTranspose(X, TX);
        matrixMultiply(TX, X, A);
        matrixMultiply(TX, y, D);

        matrixSolveGaussian(A, D);
        matrixClone(D, result);
}*/

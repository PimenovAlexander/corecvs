/**
 * \file matrix.c
 * \brief This file describes the big matrix manipulations.
 *  The large matrix with arbitrary sizes can be manipulated with the
 *  functions form this file
 *
 *
 *
 * \ingroup corefiles
 * \date May 22, 2009
 * \author: Alexander Pimenov
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "global.h"
#include "math.h"
#include "matrix.h"



/**
 * This function creates a matrix with all zero elements
 *
 * \param m the height of the matrix
 * \param n the width of the matrix
 */
Matrix *matrixCreate (int m, int n)
{
        Matrix *toReturn = (Matrix  *) dv_malloc (sizeof (Matrix));
        toReturn->m = m;
        toReturn->n = n;
        toReturn->data = (double *) dv_calloc(n * m , sizeof(double));

        return toReturn;
}

/**
 * This function creates a matrix filling it with values form the given array
 * line by line.
 *
 * \param m the height of the matrix
 * \param n the width of the matrix
 * \param raw the line by line raw values of the matrix
 */
Matrix *matrixCreateFromRaw (int m, int n, double *raw)
{
        Matrix *toReturn = (Matrix  *) dv_malloc (sizeof (Matrix));
        toReturn->m = m;
        toReturn->n = n;
        toReturn->data = (double *) dv_malloc(n * m * sizeof(double));
        memcpy(toReturn->data, raw, n * m * sizeof(double));
        return toReturn;
}


/**
 * This function deletes a matrix. And free the memory allocated for it.
 *
 * NB! Matrix should not be used in any way after being deleted
 *
 * \param toDelete a matrix to be disposed.
 */
void matrixDelete(Matrix *toDelete)
{
        if (toDelete->data != NULL)
                dv_free(toDelete->data);
        dv_free(toDelete);

}

/**
 * This function fills a square with identity matrix
 */
void matrixFillIdentity(Matrix *A)
{
#ifdef ASSERTS
        // Assert that the matrix is square
        if (A->m != A->n)
                ASSERT_FAIL("Matrix is not square");
#endif
        int row, column;
        for (row = 0; row < A->m; row ++)
        {
                for (column = 0; column < A->n; column++)
                {
                        A->data[row * A->n + column] = (row == column) ? 1.0 : 0.0;
                }
        }
}

void matrixPrint(Matrix *A)
{
        int row, column;
        for (row = 0; row < A->m; row ++)
        {
                printf("[");
                for (column = 0; column < A->n; column++)
                {
                        printf(" %lf", A->data[row * A->n + column]);
                }
                printf("]\n");
        }
}

void matrixAdd(Matrix *A, Matrix *B, Matrix *result)
{
#ifdef ASSERTS
        // Assert that the matrix has same sizes
        if (A->n != B->n || A->n != result->n || A->m != B->m || B->m != result->m)
                ASSERT_FAIL("Matrices have wrong sizes");
#endif

        int row, column;
        for (row = 0; row < A->m; row ++)
        {
                for (column = 0; column < A->n; column++)
                {
                        ELEM(result, row, column) = ELEM(A, row, column) + ELEM(B, row, column);
                }
        }
}

void matrixSubstract(Matrix *A, Matrix *B, Matrix *result)
{
#ifdef ASSERTS
        // Assert that the matrix has same sizes
        if (A->n != B->n || A->n != result->n || A->m != B->m || B->m != result->m)
                ASSERT_FAIL("Matrices have wrong sizes");
#endif

        int row, column;
        for (row = 0; row < A->m; row ++)
        {
                for (column = 0; column < A->n; column++)
                {
                        ELEM(result, row, column) = ELEM(A, row, column) - ELEM(B, row, column);
                }
        }
}


/**
 * Multiplies the two matrices
 */
void matrixMultiply(Matrix *A, Matrix *B, Matrix *result)
{
#ifdef ASSERTS
        // Assert that the matrix is square
        if (A->n != B->m || A->m != result->m || B->n != result->n)
                ASSERT_FAIL("Matrices have wrong sizes");
#endif
        int row, column, runner;
        for (row = 0; row < result->m ; row++)
                for (column = 0; column < result->n ; column++)
                {
                double sum = 0;
                for (runner = 0; runner < A->n; runner++)
                {
                        sum += ELEM(A, row, runner) * ELEM (B, runner, column);
                }
                result->data[row * result->n  + column] = sum;
        }
}

/**
 * \brief Multiplies the two matrices and return it's product
 *
 *        NB!! Do not forget to delete the product with matrixDelete()
 *
 * \param A  a matrix to multiply
 * \param B  a matrix to multiply by
 *
 * \return a new matrix with the multiplication result
 */
Matrix * matrixMultiplyNew(Matrix *A, Matrix *B)
{
        Matrix *toReturn = matrixCreate(A->m, B->n);
        matrixMultiply(A, B, toReturn);
        return toReturn;
}

/**
 * Copies the matrix
 * The destanation have to be prepared.
 *
 * \param A a matrix to clone
 * \param B a matrix to put clone to.
 */
void matrixCopy(Matrix *A, Matrix *B)
{
#ifdef ASSERTS
        // Assert that the matrices are same size
        if (A->n != B->n || A->m != B->m)
                ASSERT_FAIL("Matrices have wrong sizes");
#endif
        memcpy(B->data, A->data, sizeof (double) * A->m * A->n);
}

/**
 * Clones the matrix
 * The destanation have to be prepared.
 *
 * \param A a matrix to clone
 * \returns a matrix to put clone to.
 */
Matrix *matrixCopyNew(Matrix *A)
{
        Matrix *toReturn = matrixCreate(A->m, A->n);
        matrixCopy(A, toReturn);
        return toReturn;
}


/**
 * Transpose the matrix. The transposition will be put to already existing matrix
 */
void matrixTranspose(Matrix *A, Matrix *B)
{
#ifdef ASSERTS
        // Assert that the matrices have good sizes
        if (A->n != B->m || A->m != B->n)
                ASSERT_FAIL("Matrices have wrong sizes to transpose");
#endif
        int row, column;

        for (row = 0; row < B->m ; row++)
                for (column = 0; column < B->n ; column++)
                {
                double value = ELEM(A, column, row);
                ELEM(B, row, column) = value;
        }
}

/**
 * Transpose the matrix
 */
Matrix *matrixTransposeNew(Matrix *A)
{
        Matrix *toReturn = matrixCreate(A->n, A->m);
        matrixTranspose(A, toReturn);
        return toReturn;
}


/**
 * \brief Solves the linear equation.
 *
 */
int matrixSolveGaussian(Matrix *A, Matrix *B)
{
#ifdef ASSERTS
        // Assert that the matrix is square and the second matrix is a vector
        if (A->m != A->n || A->n != B->m || B->n != 1)
                ASSERT_FAIL("Matrix has wrong sizes");
#endif

        int row, column, rowRunner, runner;
        int n = A->m;
        // Walk thought the matrix from left to right
        for (column = 0; column < n; column++)
        {
                int rowWithMax = -1;
                double maxInColumn = 0;
                for (row = 0; row < n; row ++)
                {
                        double absA = fabs (A->data[row * n + column]);
                        if (absA > maxInColumn)
                        {
                                maxInColumn = absA;
                                rowWithMax = row;
                        }
                }
                if (rowWithMax == -1) {
                        if (B->data[column] != 0)
                                return 1;
                        for (rowRunner = 0; rowRunner < n; rowRunner++)
                                if (A->data[column * n + rowRunner] != 0)
                                        return 1;
                }
                // Now we will work with the row of the same number as the column
                row = column;
                // Now put the row to the i-th position by interchange
                if (rowWithMax != row)
                {
                        double tmp;
                        for (rowRunner = column; rowRunner < n; rowRunner ++)
                        {
                                tmp = A->data[row * n + rowRunner];
                                A->data[row * n + rowRunner] = A->data[rowWithMax * n + rowRunner];
                                A->data[rowWithMax * n + rowRunner] = tmp;
                        }
                        tmp = B->data[row];
                        B->data[row] = B->data[rowWithMax];
                        B->data[rowWithMax] = tmp;
                }
                // And now do the correction of all the rows
                // First divide all the row by the value of (column, row)
                double a = A->data[row * n + column];
                double ia = 1 / a;
                for (rowRunner = column; rowRunner < n; rowRunner ++)
                        A->data[row *n + rowRunner] *= ia;
                B->data[row] *= ia;

                for (runner = 0; runner < n; runner++)
                        if (runner != row) {
                        double d = A->data[runner * n + column];
                        for (rowRunner = column; rowRunner < n; rowRunner++)
                                A->data[runner * n + rowRunner] -= A->data[row * n + rowRunner] * d;
                        B->data[runner] -= B->data[row] * d;
                }

        }
        return 0;
}

// Solves system of linear equations
Matrix * matrixSolve(Matrix * A, Matrix * B)
{
        if (A->m != A->n || A->n != B->m || B->n != 1)
                ASSERT_FAIL("Matrix has wrong sizes");
        int n = A->m;
        int i, j, k;
        for (i = 0; i < n - 1; i++)
        {
                for (j = i + 1; j < n; j++)
                {
                        double value = A->data[n * j + i];
                        for (k = i; k < n; k++)
                        {
                                A->data[j * n + k] = A->data[j * n + k] - A->data[i * n + k] * value
                                                                         / A->data[i * n + i];
                        }
                        B->data[j] -= B->data[i] * value / A->data[i * n + i];
                }
        }
        Matrix * solution = matrixCreate(n, 1);
        solution->data[n - 1] = B->data[n - 1] / A->data[n * n - 1];
        for (i = n - 2; i >= 0; i--)
        {
                solution->data[i] = B->data[i];
                for (j = i + 1; j < n; j++)
                {
                        solution->data[i] -= A->data[i * n + j] * solution->data[j];
                }
                solution->data[i] = solution->data[i] / A->data[i * n + i];
        }
        return solution;
}

int matrixTriangulate(Matrix *A)
{
        int row, column, rowRunner, runner;
        int n = A->m;
        // Walk thought the matrix from left to right

        for (column = 0; column < n; column++)
        {
                int rowWithMax = -1;
                double maxInColumn = 0;
                for (row = 0; row < n; row ++)
                {
                        double absA = fabs (A->data[row * n + column]);
                        if (absA > maxInColumn)
                        {
                                maxInColumn = absA;
                                rowWithMax = row;
                        }
                }
                if (rowWithMax == -1) {
                        // \todo TODO: The zeroing out should be processed here... for homogenious equations
                }
                // Now we will work with the row of the same number as the column
                row = column;
                // Now put the row to the i-th position by interchange
                if (rowWithMax != row)
                {
                        double tmp;
                        for (rowRunner = column; rowRunner < n; rowRunner ++)
                        {
                                tmp = A->data[row * n + rowRunner];
                                A->data[row * n + rowRunner] = A->data[rowWithMax * n + rowRunner];
                                A->data[rowWithMax * n + rowRunner] = tmp;
                        }
                }
                // And now do the correction of all the rows
                // First divide all the row by the value of (column, row)
                double a = A->data[row * n + column];
                double ia = 1 / a;
                for (rowRunner = column; rowRunner < n; rowRunner ++)
                        A->data[row *n + rowRunner] *= ia;

                for (runner = 0; runner < n; runner++)
                        if (runner != row) {
                        double d = A->data[runner * n + column];
                        for (rowRunner = column; rowRunner < n; rowRunner++)
                                A->data[runner * n + rowRunner] -= A->data[row * n + rowRunner] * d;
                }

        }
        return 0;

}

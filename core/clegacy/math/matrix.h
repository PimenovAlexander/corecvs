/**
 * \file matrix.h
 * \brief A header for matrix.c
 *
 *
 * \ingroup corefiles
 *  \date May 22, 2009
 *  \author: Alexander Pimenov
 */

#ifndef MATRIX_H_
#define MATRIX_H_
#ifdef __cplusplus
    extern "C" {

#endif

/**
 * \brief This structure holds the arbitrary size dense matrix
 */
typedef struct Matrix_TAG {
 int m;  /**< The height of the matrix*/
 int n;  /**< The width of the matrix */
 double *data; /**< Matrix data line after the line*/
} Matrix;


/** This macro is used for fast matrix access */
#define ELEM(M, i, j) ((M)->data[(i) * (M)->n + (j)])

/** This macro is used for fast vector access */
#define VECELEM(V, i) ((V)->data[i])


Matrix *matrixCreate (int n, int m);
Matrix *matrixCreateFromRaw (int m, int n, double *raw);

void matrixDelete(Matrix *toDelete);

void matrixAdd(Matrix *A, Matrix *B, Matrix *result);
void matrixSubstract(Matrix *A, Matrix *B, Matrix *result);

void matrixPrint(Matrix *A);

void matrixMultiply(Matrix *A, Matrix *B, Matrix *result);
Matrix * matrixMultiplyNew(Matrix *A, Matrix *B);

void matrixCopy(Matrix *A, Matrix *B);
Matrix *matrixCopyNew(Matrix *A);

void matrixTranspose(Matrix *A, Matrix *B);
Matrix *matrixTransposeNew(Matrix *A);


void matrixFillIdentity(Matrix *A);
int matrixSolveGaussian(Matrix *A, Matrix *B);
Matrix * matrixSolve(Matrix * A, Matrix * B);

#ifdef __cplusplus
    } //     extern "C"
#endif

#endif /* MATRIX_H_ */


/**
 * \file cholesky.cpp
 * \brief This file holds Cholesky function inplementations
 *
 * \date Jan 10, 2011
 * \author alexander
 */

#include "kalman/cholesky.h"
namespace corecvs {

Cholesky::Cholesky()
{
}

Cholesky::~Cholesky()
{
}

/**
 *
 *  \brief Cholesky UDUT decomposition.
 *
 *  This decomposition is based on the recurrent relations
 *  that become obvious from the example below
 *
 *  \f{eqnarray*}
 *
 *     U &=& \pmatrix{
 *        1 & u_{01} & u_{02} \cr
 *        0 &      1 & u_{12} \cr
 *        0 &      0 &   1} \\
 *
 *     U^\top &=& \pmatrix{
 *        1      & 0      & 0 \cr
 *        u_{01} & 1      & 0 \cr
 *        u_{02} & u_{12} & 1} \\
 *
 *     D &=& \pmatrix{
 *        d_0 & 0   & 0   \cr
 *        0   & d_1 & 0   \cr
 *        0   & 0   & d_2 } \\
 *
 *     A = U D U^\top &=& \pmatrix{
 *      d_2 {u_{02}}^2    + d_1 {u_{01}}^2 + d_0 & d_2 u_{02} u_{12} + d_1 u_{01} & d_2 u_{02} \cr
 *      d_2 u_{02} u_{12} + d_1  u_{01}          & d_2 {u_{12}}^2+d_1             & d_2 u_{12} \cr
 *      d_2 u_{02}                               & d_2 u_{12}                     & d_2 }
 * \f}
 *
 *  so the recurrent relations are:
 *
 *  Starting form the last column
 * \f{eqnarray*}
 *     D_n &=& A_{nn} \\
 *     U_{in} &=& 1, for\quad i = n \\
 *            &=& A_{in} / D_n, otherwize
 * \f}
 *
 * And for other columns
 * \f{eqnarray*}
 *     D_{jj} &=& A_{jj} - \sum_{k=j+1}^n {D_k U_{jk}^2 } \\
 *     U_{ij} &=& 0, for\quad i > j \\
 *            &=& 1, for\quad i = j \\
 *            &=& (A_{ij} - \sum_{k=j+1}^n {D_k U_{jk} U_{ik}} )/ D_n, otherwize
 * \f}
 *
 *
 *
 * */

void Cholesky::udutDecompose(Matrix *A, Matrix **Uresult, DiagonalMatrix **Dresult)
{
    int n = A->h;
    Matrix *u         = new Matrix(n, n);
    DiagonalMatrix *d = new DiagonalMatrix(n);

    /* Starting with last row */
    int j = n - 1;
    d->a(j) = A->a(j,j);
    u->a(j,j) = 1.0;
    for (int i = 0; i < j; i ++)
    {
        u->a(i, j) = A->a(i, j) / d->a(j);
    }
    /* Recurrent continuation for columns */

    for (j = n - 2; j >= 0; j-- )
    {
        double sum = 0.0;
        for (int k = j + 1; k < n; k++)
        {
            double ujk = u->a(j,k);
            sum += d->a(k) * ujk * ujk;
        }
        d->a(j) = A->a(j,j) - sum;

        for (int i = 0; i < j; i++)
        {
            double sum = 0;
            for (int k = j + 1; k < n; k++)
            {
                double ujk = u->a(j,k);
                double uik = u->a(i,k);
                sum += d->a(k) * ujk * uik;
            }
            u->a(i,j) = (A->a(i,j) - sum ) / d->a(j);
        }

        u->a(j,j) = 1.0;

        for (int i = j + 1; i < n; i++ )
            u->a(i,j) = 0.0;

    }

    if (Uresult != NULL) {
        *Uresult = u;
    } else {
        delete u;
    }

    if (Dresult != NULL) {
        *Dresult = d;
    } else {
        delete d;
    }
}

bool Cholesky::uutDecompose(Matrix *A, Matrix **Uresult)
{
    bool isPositive = true;
    Matrix *U;
    DiagonalMatrix *D;
    Cholesky::udutDecompose(A, &U, &D);
    if (Uresult != NULL) {
        cout << "D" << *D << endl;
        for (int j = 0; j < U->w; j++)
        {
            if (D->a(j) < 0) {
                SYNC_PRINT(("One diagonal value is negative\n"));
                isPositive = false;
            }
        }
        for (int j = 0; j < U->w; j++)
        {

            double v = 0;
            if (D->a(j) > 0) /* We try to make small corrections...*/
                v = sqrt(D->a(j));
            for (int i = 0; i < U->h; i++)
            {
                U->element(i,j) = U->element(i,j) * v;
            }
        }
        *Uresult = U;
    } else {
        delete U;
    }
    return isPositive;
}


void Cholesky::udutDecompose(Matrix *A, UpperUnitaryMatrix **Uresult, DiagonalMatrix **Dresult)
{
    int n = A->h;
    UpperUnitaryMatrix *u  = new UpperUnitaryMatrix(n, false);
    DiagonalMatrix *d = new DiagonalMatrix(n);

    /* Starting with last row */
    int j = n - 1;
    d->a(j) = A->a(j,j);
    for (int i = 0; i < j; i ++)
    {
        u->u(i, j) = A->a(i, j) / d->a(j);
    }
    /* Recurrent continuation for columns */

    for (j = n - 2; j >= 0; j-- )
    {
        double sum = 0.0;
        for (int k = j+1; k < n; k++)
        {
            double ujk = u->a(j,k);
            sum += d->a(k) * ujk * ujk;
        }
        d->a(j) = A->a(j,j) - sum;

        for (int i = 0; i < j; i++)
        {
            double sum = 0;
            for (int k = j + 1; k < n; k++)
            {
                double ujk = u->a(j,k);
                double uik = u->a(i,k);
                sum += d->a(k) * ujk * uik;
            }
            u->u(i,j) = (A->a(i,j) - sum ) / d->a(j);
        }
    }

    if (Uresult != NULL)
        *Uresult = u;
    if (Dresult != NULL)
        *Dresult = d;
}

} //namespace corecvs


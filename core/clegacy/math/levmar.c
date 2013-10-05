/**
 * \file levmar.c
 * \brief This file contains the Levenberg-Marquardt algorithm for non-linear least squares solution
 *
 * References:
 *         http://en.wikipedia.org/wiki/Marquardt-Levenberg_algorithm
 *  http://www.fizyka.umk.pl/nrbook/c15-5.pdf
 *
 *
 * \ingroup corefiles
 *  \date May 22, 2009
 *  \author Alexander Pimenov
 */

#ifndef LEVMAR_C_
#define LEVMAR_C_

#include <stdio.h>
#include <string.h>

#include "global.h"

#include "matrix.h"
#include "levmar.h"
#include "stdlib.h"

#define LAMBDA_FACTOR 2

#undef DEEP_TRACE

/**
 * \brief this function runs the Levenberg-Marquardt non-linear least squares solutions
 *
 *
 */
double doLMFit(int num, int dimention, LMfunction ff, LMY yf, LMJ Jf, double startGuess[], double *lastGuess, int iterations)
{
        Matrix *D = matrixCreate(num, 1);
        Matrix *J = matrixCreate(num, dimention);
        Matrix *JT = matrixCreate(dimention, num);

        Matrix *A = matrixCreate(dimention, dimention);
        Matrix *Asolve = matrixCreate(dimention, dimention);
        Matrix *B = matrixCreate(dimention, 1);
        Matrix *Bsolve = matrixCreate(dimention, 1);

        double *newGuess = (double *) dv_malloc(dimention * sizeof(double));

        double lambda = 0.01;

#ifdef TRACE
        printf ("Matrix created...");
        printf ("Starting LM optimization...\n\n=================================\n");
#endif
        int count = 1;
        int i;
        double norm = 0;

        for (i = 0; i < num; i++)
        {
                double value = yf(i) - ff(i, startGuess);
                ELEM(D, i, 0) = value;
                norm += value * value;
        }

        while (count < iterations)
        {
                // Fill the matrices
                int j;
                for (i = 0; i < num; i++)
                {
                        for (j = 0; j < dimention; j++)
                        {
                                //printf("dimention %d dirivative\n", j);
                                fflush(stdout);
                                double result = Jf(i,j, startGuess);
                                ELEM(J, i,j) = result;
                        }
                }
                // Now form the equation
                // (trans(J) * J) * delta = trans(J) * (y - f(x))
                matrixTranspose(J, JT);
                matrixMultiply(JT, J, A);
                //printf("D size is %lf\n", norm);
                matrixMultiply(JT, D, B);

                int k;

                while (lambda < 1e12)
                {
                        matrixCopy(A, Asolve);
                        matrixCopy(B, Bsolve);


                        for (k = 0; k < dimention; k++)
                        {
                                Asolve->data[k * dimention + k] *= (1.0 + lambda );
                        }

                        if (matrixSolveGaussian(Asolve, Bsolve) != 0 )
                        {
                                printf("OOPS\n");
                        }

                        // Prepare new  guess vector;
                        for (j = 0; j < dimention; j++)
                                newGuess[j] = Bsolve->data[j]  + startGuess[j];

                        // Evaluate new deviation.
                        double norm1 = 0;
                        for (i = 0; i < num; i++)
                        {
                                double value = yf(i) - ff(i, newGuess);
                                ELEM(D, i, 0) = value;
                                norm1 += value * value;
                        }
#ifdef TRACE
                        printf("CV %s: %lf %s %lf :", (norm > norm1) ? "REJ" : "ACC", norm, (norm > norm1) ? ">" : "<" ,norm1 );
                        //printf("CV %lf  %lf :", norm, norm1 );
#endif

                        // Print new  guess vector;
                        for (j = 0; j < dimention; j++)
                        {
                                printf ("%lf ", Bsolve->data[j]);
                        }
                        printf("\n");
                        fflush(stdout);


                        if (norm1 < norm) // If the currnet solution is better
                        {
                                //printf("Current solution is better");
                                lambda /= LAMBDA_FACTOR;
                                for (j = 0; j < dimention; j++)
                                {
                                        startGuess[j] = newGuess[j];
                                }
                                norm = norm1;
                                break;
                        }
                        else
                        {
#ifdef DEEP_TRACE
                                printf("Current solution is worse. Try new lambda\n");
#endif
                                lambda *= LAMBDA_FACTOR;
                        }
                }

                //printf("\n");
                count ++;
        }
        if (lastGuess != NULL)
        {
                memcpy (lastGuess, newGuess, dimention * sizeof(double));
                dv_free(newGuess);
        }

        return norm;
}



#endif /* LEVMAR_C_ */

/**
 * \file cholesky.h
 * \brief This file holds Cholesky decomposition data types and function prototypes
 *
 * \date Jan 10, 2011
 * \author alexander
 */

#ifndef CHOLESKY_H_
#define CHOLESKY_H_
#include "math/matrix/matrix.h"
#include "math/matrix/diagonalMatrix.h"
#include "kalman/upperUnitaryMatrix.h"
namespace corecvs {

/**
 *  Class that implements Cholesky decomposition.
 *
 *  For details see Cholesky::udutDecompose()
 *
 **/
class Cholesky
{
public:

    static void udutDecompose(Matrix *A, Matrix **Uresult, DiagonalMatrix **Dresult);
    static bool uutDecompose (Matrix *A, Matrix **Uresult );

    static void udutDecompose(Matrix *A, UpperUnitaryMatrix **Uresult, DiagonalMatrix **Dresult);


    Cholesky();
    virtual ~Cholesky();
};


} //namespace corecvs
#endif /* CHOLESKY_H_ */


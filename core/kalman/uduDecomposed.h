#ifndef UDUDECOMPOSED_H_
#define UDUDECOMPOSED_H_

/**
 * \file uduDecomposed.h
 * \brief a header for uduDecomposed.c
 *
 * \date Feb 14, 2011
 * \author: alexander
 */

#include "core/utils/global.h"

#include "core/kalman/upperUnitaryMatrix.h"
#include "core/math/matrix/diagonalMatrix.h"
#include "core/kalman/cholesky.h"

namespace corecvs {

class UDUTDecomposed
{
public:
    UpperUnitaryMatrix *upper;
    DiagonalMatrix *diagonal;

    int getSize()
    {
        return diagonal->size();
    }

    UDUTDecomposed(Matrix *A)
    {
        Cholesky::udutDecompose(A, &upper, &diagonal);
    }


    ~UDUTDecomposed()
    {
        delete upper;
        delete diagonal;
    }

};

} //namespace corecvs
#endif  //UDUDECOMPOSED_H_




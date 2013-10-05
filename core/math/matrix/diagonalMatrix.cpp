/**
 * \file diagonalMatrix.cpp
 * \brief Add Comment Here
 *
 * \date Jan 10, 2011
 * \author alexander
 */

#include "diagonalMatrix.h"
namespace corecvs {

double DiagonalMatrix::det() const
{
    return mulAllElements();
}

double DiagonalMatrix::trace() const
{
    return sumAllElements();
}

void DiagonalMatrix::invert()
{
    for (int i = 0; i < size(); i++)
        at(i) = 1.0 / at(i);
}



} //namespace corecvs


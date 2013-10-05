/**
 * \file diagonalMatrix.h
 * \brief Add Comment Here
 *
 * \date Jan 10, 2011
 * \author alexander
 */

#ifndef DIAGONALMATRIX_H_
#define DIAGONALMATRIX_H_

#include <stdlib.h>
#include "fixedArray.h"
namespace corecvs {

class Matrix;

/**
 * This class stores square diagonal matrix
 *
 *
 * \f[
 *  M = \pmatrix {
 *        v_1 &      0 & \cdots &     0  \cr
 *          0 &    v_2 & \cdots &     0  \cr
 *     \vdots & \vdots & \ddots & \vdots \cr
 *          0 &      0 & \cdots &   v_n
 *  }
 * \f]
 *
 * Obviously only diagonal elements are stored.
 *
 * TODO: Should be a child of array
 *
 **/
class DiagonalMatrix : public FixedArrayBase<DiagonalMatrix, double>
{
public:
    typedef FixedArrayBase<DiagonalMatrix, double> BaseClass;

    explicit DiagonalMatrix(int _size) : BaseClass(_size)
    {}

    const double& a(const int rowAndColumn) const
    {
        return at(rowAndColumn);
    }

    double& a(const int rowAndColumn)
    {
        return at(rowAndColumn);
    }

    double a(const int row, const int column)
    {
        if (row == column)
            return at(row);
        else
            return 0.0;
    }

    double det() const;
    double trace() const;
    void invert();



    virtual ~DiagonalMatrix()
    {}



    /*friend DiagonalMatrix operator +(const DiagonalMatrix &D, const DiagonalMatrix &D1);
    friend DiagonalMatrix operator -(const DiagonalMatrix &D, const DiagonalMatrix &D1);*/

    /*friend Matrix operator +(const DiagonalMatrix &D, const Matrix &D1);
    friend Matrix operator -(const DiagonalMatrix &D, const Matrix &D1);*/

};


} //namespace corecvs
#endif /* DIAGONALMATRIX_H_ */


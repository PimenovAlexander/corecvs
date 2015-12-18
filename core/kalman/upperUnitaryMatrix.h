#ifndef UPPERUNITARYMATRIX_H_
#define UPPERUNITARYMATRIX_H_
/**
 * \file upperUnitaryMatrix.h
 * \brief Add Comment Here
 *
 * \date Jan 11, 2011
 * \author alexander
 */

#include <stdlib.h>
#include <stdio.h>


#include "matrix.h"
namespace corecvs {

/**
 *   This class holds the upper part of the triangular unitary matrix.
 *
 *
 *   <pre>
 *
 *    |  0   1   2   3   4   5   6 | 7
 *    |----------------------------|---
 *  0 |  x   0   1   2   3   4   5 |
 *  1 |      x   6   7   8   9  10 |
 *  2 |          x  11  12  13  14 |
 *  3 |       -      x  15  16  17 |
 *  4 |      | |         x  18  19 |
 *  5 |      | |             x  20 |
 *  6 |       -                  x |
 *    |-----------------------------
 *  7
 *   </pre>
 *
 *   For matrix of size NxN we will have to store (N-1)(N-2)/2 elements.
 *   (ie. for 7x7 matrix 7*6/2 = 21 element 0..20 )
 *
 *   To get the offset of the element at i,j (j>i)
 *   ((N-1) + (N - 1 - (i - 1))) / 2 * i + j - i - 1 =
 *    (2N - 1 - i) * i / 2 + j - i - 1
 *
 *   (i.e for element (3,4) -> (6 + 6 - 2) / 2 * 3 + 4 - 3 - 1 = 15  )
 *
 *
 *  TODO: Make indexing columnwise
 *   <pre>
 *
 *    |  0   1   2   3   4   5   6 | 7
 *    |----------------------------|---
 *  0 |  x   0   1   3   6  10  15 |
 *  1 |      x   2   4   7  11  16 |
 *  2 |          x   5   8  12  17 |
 *  3 |       -      x   9  13  18 |
 *  4 |      | |         x  14  19 |
 *  5 |      | |             x  20 |
 *  6 |       -                  x |
 *    |-----------------------------
 *  7
 *
 *  (i,j) -> (j * (j - 1)) / 2 + i;
 *  (6,3) -> 6*5/2 + 3 = 18
 *
 *  </pre>
 *
 *
 *
 * */
class UpperUnitaryMatrix
{
public:
    int size;
    int memSize;
    double *elements;

    static int getUpperSizeFor(int size)
    {
      return ((size - 1) * size) / 2;
    }

    int getUpperSize()
    {
      return getUpperSizeFor(size);
    }

    UpperUnitaryMatrix(int _size, bool shouldInit = true) :
        size(_size)
    {
        memSize = getUpperSizeFor(_size);
        elements = new double[memSize];
        if (shouldInit)
        {
            memset(elements, 0, sizeof(double) * memSize);
        }
    };

    double &u(int row, int column)
    {
        if (row > column)
        {
            int tmp = row; row = column; column = tmp;
        }
        int offset = internalOffset(row, column);
        DOTRACE(("R:%d C:%d off:%d\n", row, column, offset));
        CORE_ASSERT_TRUE_P(offset < memSize, ("Overflow in Upper Unitary %d >= %d\n", offset, memSize));
        return elements[offset];
     }

    double a(int row,int column)
    {
        if (row == column)
            return 1.0;
        if (row > column)
            return 0.0;
        int offset = internalOffset(row, column);
        DOTRACE(("R:%d C:%d off:%d\n", row, column, offset));
        CORE_ASSERT_TRUE_P(offset < memSize, ("Overflow in Upper Unitary %d >= %d\n", offset, memSize));
        return elements[offset];
    }

    Matrix *toMatrix() const
    {
        Matrix *toReturn = new Matrix(size, size, false);
        int count = 0;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (i > j)
                {
                    toReturn->a(i, j) = 0.0;
                } else if (i == j)
                {
                    toReturn->a(i, j) = 1.0;
                } else
                {
                    toReturn->a(i, j) = elements[count++];
                }
            }
        }
        return toReturn;
    }

    double det()
    {
        return 1.0;
    }

    friend ostream & operator <<(ostream &out, const UpperUnitaryMatrix &matrix)
    {
        streamsize wasPrecision = out.precision(6);
        int count = 0;
        for (int i = 0; i < matrix.size; i++)
        {
            int j = 0;
            out << "[";
            for (; j < i; j++){
                out.width(9);
                out << 0.0 << " ";
            }
            out.width(9);
            out << 1.0 << " ";
            for (j ++; j < matrix.size; j++)
            {
                out.width(9);
                out << matrix.elements[count++] << " ";
            }
            out << "]\n";
        }
        out.precision(wasPrecision);
        return out;
    }

    virtual ~UpperUnitaryMatrix()
    {
        delete[] elements;
        size = 0;
        elements = NULL;
    }

private:
    int internalOffset (int row, int column)
    {
        //return ((2 * size - 1 - row) / 2) * row + column - row - 1;
        return (column * (column - 1)) / 2 + row;
    }

};


} //namespace corecvs
#endif /* UPPERUNITARYMATRIX_H_ */


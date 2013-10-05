/**
 * \file cooSparseMatrix.h
 * \brief Header for the COO sparse matrix class
 *
 * \ingroup cppcorefiles
 * \date Jan 09, 2011
 * \author alexander
 */


#ifndef _COO_SPARSE_MATRIX_H_
#define _COO_SPARSE_MATRIX_H_


#include <vector>
#include "matrix.h"
namespace corecvs {
/**
 *
 * COOSparseMatrix is efficient matrix made in Matlab style - http://en.wikipedia.org/wiki/Sparse_matrix#Coordinate_list_.28COO.29
 *
 *
 * Matrix
 * <pre>
 *  0 1 2 0
 *  3 0 0 4
 *  0 5 0 0
 * </pre>
 *
 * will be stored like
 * elements  = [1,2,3,4,5] <br>
 * irow = [0,0,1,1,2] <br>
 * icol = [1,2,0,3,1] <br>
 * nphi = 5
 *
 * */

class COOSparseMatrix {
public:
    class Entry {
    public:
        double value; /**< value of the element */
        int row;     /**< row of the element   */
        int column;  /**< column of the element */

        Entry(double _value, int _row, int _column) :
            value(_value),
            row(_row),
            column(_column) {};
    };


    int     h;  /**< actual row number in the matrix */
    int     w;  /**< actual col number in the matrix */

    std::vector<Entry> elements;

    int nonZeroElemets() const
    {
        return (int)elements.size();
    }

    COOSparseMatrix(int _h, int _w, bool preAllocate = false) :
        h(_h), w(_w)
    {
        if (preAllocate)
            elements.reserve(h * w);
    }


    COOSparseMatrix(const Matrix *matrix, bool optimizeAllocation = false)
    {
        if (optimizeAllocation)
        {
            int countNonZero = 0;
            for (int i = 0; i < matrix->h; i++)
                for (int j = 0; j < matrix->w; j++)
                    if (matrix->a(i,j) != 0.0)
                        countNonZero++;

            elements.reserve(countNonZero);
        }

        h = matrix->h;
        w = matrix->w;

        for (int i = 0; i < matrix->h; i++)
        {
            for (int j = 0; j < matrix->w; j++)
            {
                if (matrix->a(i,j) != 0.0)
                {
                    elements.push_back(Entry(matrix->a(i,j),i,j));
                }
            }
        }
    }

    Matrix *toMatrix() const
    {
        Matrix *toReturn = new Matrix(h,w);
        std::vector<Entry>::const_iterator it;

        for (it = elements.begin(); it != elements.end(); it++)
        {
            toReturn->a((*it).row,(*it).column) = (*it).value;
        }
        return toReturn;
    }
};


} //namespace corecvs
#endif /* _COO_SPARSE_MATRIX_H_ */


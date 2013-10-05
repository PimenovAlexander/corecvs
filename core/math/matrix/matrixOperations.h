#ifndef MATRIXOPERATIONS_H_
#define MATRIXOPERATIONS_H_
/**
 * \file matrixOperations.h
 * \brief This header holds matrix operations that are common to
 * matrixes of different nature.
 *
 * \date Jul 4, 2011
 * \author alexander
 */

#include "global.h"
namespace corecvs {

template<typename RealType, typename ElementType>
class MatrixOperationsBase
{
public:
    /**
     *  Static cast functions
     **/
    inline RealType *realThis() {
        return static_cast<RealType *>(this);
    }

    inline const RealType *realThis() const {
        return static_cast<const RealType *>(this);
    }

private:
    /**
     *  Interface related functions
     **/
    inline ElementType &_at(int i, int j) {
        return realThis()->at(i, j);
    }

    inline const ElementType &_at(int i, int j) const {
        return realThis()->at(i, j);
    }

    inline int _height() const {
        return realThis()->height();
    }

    inline int _width() const {
        return realThis()->width();
    }

    inline RealType _createMatrix(int h, int w) const {
        return realThis()->createMatrix(h, w);
    }

public:


};


} //namespace corecvs
#endif  //MATRIXOPERATIONS_H_




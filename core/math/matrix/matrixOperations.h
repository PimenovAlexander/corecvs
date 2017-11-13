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

/**
 * \file matrixOperations.h
 * \brief This header holds matrix operations that are common to
 * matrixes of different nature.
 *
 * This class uses static polymorphism to access the elements of the container.
 * Two (or three) methods should be implemented to use MatrixOperationsBase
 *
 * ElementType &atm(int i, int j);
 * const ElementType &atm(int i, int j) const;
 * int height();
 * int width();
 * RealType createMatrix(int h, int w);
 *
 * Also this implementation expect that RealType has two defined subtype
 * InnerElementType
 *
 * This particular implementation doesn't expect that elements are continuous in memory
 *
 * This template class has 3 parameters
 *    ReturnType  - The type that should be returned in all constructive functions
 *    RealType    - The type that holds all the data needed for processing
 *    ElementType - The element type
 *
 * TODO: Consider using size_t instead of int
 * TODO: Consider using iterator and function object
 *
 * \date Aug 28, 2015
 **/

#include "core/utils/global.h"
namespace corecvs {

template<typename RealType, typename ElementType, typename ReturnType = RealType>
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
    inline ElementType &_atm(int i, int j) {
        return realThis()->atm(i, j);
    }

    inline const ElementType &_atm(int i, int j) const {
        return realThis()->atm(i, j);
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




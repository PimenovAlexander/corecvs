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

    static RealType Rotation3D(int axis, ElementType angle)
    {
        ReturnType result = RealType::createMatrix();
        for (int i = 0; i < result._height(); i++)
        {
            for (int j = 0; j < result._width(); j++)
            {
                result._atm(i, j) = (i == j) ? ElementType(1) : ElementType(0);
            }
        }

        if        (axis == 0) {
            result._atm(0,0) =  cos(angle); result._atm(0,1) =  sin(angle);
            result._atm(1,0) = -sin(angle); result._atm(1,1) =  cos(angle);
        } else if (axis == 1) {
            result._atm(0,0) =  cos(angle); result._atm(0,2) = -sin(angle);
            result._atm(2,0) =  sin(angle); result._atm(2,2) =  cos(angle);
        } else if (axis == 2) {
            result._atm(1,1) =  cos(angle); result._atm(1,2) =  sin(angle);
            result._atm(2,1) = -sin(angle); result._atm(2,2) =  cos(angle);
        }

        return result;

    }

    friend std::ostream & operator <<(std::ostream &out, const RealType &matrix)
    {
        //streamsize wasPrecision = out.precision(6);
        for (int i = 0; i < matrix._height(); i++)
        {
            out << "[";
            for (int j = 0; j < matrix._width(); j++)
            {
                out << (j == 0 ? "" : " ");
                //out.width(6);
                out << matrix._atm(i,j);
            }
            out << "]\n";
        }
        //out.precision(wasPrecision);
        return out;
    }

    friend std::istream & operator >>(std::istream &in, RealType &matrix)
    {
        for (int i = 0; i < matrix._height(); i++)
        {
            for (int j = 0; j < matrix._width(); j++)
            {
                while (true)
                {
                    in >> matrix._atm(i,j);
                    // cout << "Read:" << i << "," << j << " " << matrix.a(i,j) << endl;
                    if (in.good())
                        break;
                    if (in.eof())
                        return in;
                    /* Not a number clean the error and advance*/
                    in.clear();
                    // cout << "Skipped:" << ((char)in.peek()) << endl;
                    in.ignore();
                }

            }
        }
        return in;
    }

    void transpose()
    {
        CORE_ASSERT_TRUE(_height() == _width(), "Matrix should be square to transpose.");

        for (int row = 0; row < _height(); row++)
        {
            for (int column = row + 1; column < _width(); column++)
            {
                ElementType tmp;
                tmp = _atm(column, row);
                _atm(column, row) = _atm(row, column);
                _atm(row, column) = tmp;
            }
        }
    }


    ReturnType operator -()
    {
        ReturnType result = _createMatrix(_height(), _width());
        for (int i = 0; i < result._height(); i++)
        {
            for (int j = 0; j < result._width(); j++)
            {
                result._atm(i, j) = -_atm(i,j);
            }
        }
        return result;
    }

    friend ReturnType operator *(const ElementType &a, const RealType &B)
    {
        ReturnType result = B._createMatrix(B._height(), B._width());
        int row, column;
        for (row = 0; row < result._height(); row++)
        {
            for (column = 0; column < result._width(); column++)
            {
                 result._atm(row, column) = a *  B._atm(row, column);
            }
        }
        return result;
    }

    friend ReturnType operator *(const RealType &B, const ElementType &a)
    {
        ReturnType result = B._createMatrix(B._height(), B._width());
        int row, column;
        for (row = 0; row < result._height(); row++)
        {
            for (column = 0; column < result._width(); column++)
            {
                 result._atm(row, column) = B._atm(row, column) * a;
            }
        }
        return result;
    }


    /* Work in progress. We should probably use vector operations for this */

    friend ReturnType operator +(const RealType &A, const RealType &B)
    {
        CORE_ASSERT_TRUE(A._width() == B._width() && A._height() == B._height(), "Matrices have wrong sizes");
        ReturnType result = A._createMatrix(A._height(), A._width());

        int row, column;
        for (row = 0; row < result._height(); row++)
        {
            for (column = 0; column < result._width(); column++)
            {
                result._atm(row, column) = A._atm(row, column) + B._atm(row, column);
            }
        }

        return result;
    }


    friend ReturnType operator -(const RealType &A, const RealType &B)
    {
        CORE_ASSERT_TRUE(A._width() == B._width() && A._height() == B._height(), "Matrices have wrong sizes");
        ReturnType result = A._createMatrix(A._height(), A._width());

        int row, column;
        for (row = 0; row < result._height(); row++)
        {
            for (column = 0; column < result._width(); column++)
            {
                result._atm(row, column) = A._atm(row, column) - B._atm(row, column);
            }
        }

        return result;
    }

template<typename OtherType>
    ReturnType mul(const OtherType& V)
    {
        CORE_ASSERT_TRUE(this->_width() == V._height(), "Matrices have wrong sizes");
        ReturnType result = _createMatrix(this->_height(), V._width());

        int row, column, runner;
        for (row = 0; row < result._height(); row++)
            for (column = 0; column < result._width(); column++)
            {
                ElementType sum = ElementType(0);
                for (runner = 0; runner < this->_width(); runner++)
                {
                    sum += this->_atm(row, runner) * V._atm(runner, column);
                }
                result.atm(row, column) = sum;
            }

        return result;
    }


};

#if 0
template<typename RealType, typename ElementType, typename ReturnType = RealType>
class MatrixVisitOperationsBase
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
    visit



};
#endif

/**
 * A matrix over abstract buffer having a static size
 **/
#include "core/buffers/abstractBuffer.h"

template<class Element, int H = 3, int W = 3>
class AbsMatrixFixed : public AbstractBuffer<Element, int>, public MatrixOperationsBase<AbsMatrixFixed<Element>, Element>
{
public:
    typedef Element ElementType;

    AbsMatrixFixed() : AbstractBuffer<Element, int>(H, W) {}

    /* Matrix Operations interface */
    Element &atm(int i, int j) {
        return this->element(i, j);
    }
    const Element &atm(int i, int j) const {
        return this->element(i, j);
    }

    inline int height() const {
        return this->h;
    }

    inline int width() const {
        return this->w;
    }

    static AbsMatrixFixed createMatrix(int /*h*/ = H, int /*w*/ = W) {return AbsMatrixFixed(); }

    /* Additional helper function */
};

/**
 *  A matrix over fixed vector having a static size
 **/

#include "core/math/vector/fixedVector.h"

template<class Element, int H = 3, int W = 3>
class FixMatrixFixed : public FixedVector<Element, H * W>, public MatrixOperationsBase<FixMatrixFixed<Element>, Element>
{
public:
    typedef Element ElementType;

    FixMatrixFixed() :
        FixedVector<ElementType, H * W>()
    {}

    /* Matrix Operations interface */

    inline int height() const {
        return H;
    }

    inline int width() const {
        return W;
    }

    ElementType &atm(int i, int j) {
        return this->element[i * W + j];
    }
    const Element &atm(int i, int j) const {
        return this->element[i * W + j];
    }

    static FixMatrixFixed createMatrix(int /*h*/, int /*w*/) {return FixMatrixFixed(); }


    /* Additional helper function */
    inline void fillWithArgsList(const ElementType value, va_list marker)
    {
        this->atm(0, 0) = value;
        for (int i = 0; i < this->height(); i++)
            for (int j = 0; j < this->width(); j++)
                if (i != 0 || j != 0)
                    this->atm(i,j) = va_arg(marker, ElementType);
    }

    void fillWithArgs(const ElementType value, ...)
    {
        va_list  marker;
        va_start(marker, value);
        fillWithArgsList(value, marker);
        va_end(marker);
    }

    /* Actual operators */
    /**
     *  TODO: Support nonquare matrixes and non-fixed vectors and raise to Matrix operations
     **/
    template<typename VectorType>
    friend inline FixedVector<ElementType, H> operator *(const FixMatrixFixed &M, const VectorType &V)
    {
        FixedVector<ElementType, H> result;
        typedef typename std::remove_const<decltype(V.at(0) * M.atm(0, 0))>::type SumType;

        for (int row = 0; row < M.height(); row++)
        {
            SumType sum(0.0);

            for (int column = 0; column < M.width(); column++)
            {
                sum += V.at(column) * M.atm(row, column);
            }
            result.at(row) = sum;
        }
        return result;
    }

};


} //namespace corecvs
#endif  //MATRIXOPERATIONS_H_




#ifndef FIXEDARRAY_H_
#define FIXEDARRAY_H_
/**
 * \file fixedArray.h
 * \brief a header for fixedArray.c
 *
 * \date Jun 13, 2011
 * \author alexander
 */

#include <vector>

#include "global.h"

#include "memoryBlock.h"
#include "vectorOperations.h"
namespace corecvs {

template<typename RealType, typename ElementType>
class FixedArrayBase : public VectorOperationsBase<RealType, ElementType>
{
public:
    typedef ElementType InternalElementType;

    /**
     * Memory block that holds the data
     **/
    MemoryBlockRef memoryBlock;

    /**
     * The array of the data elements
     **/
    ElementType *element;
    int length;

    explicit inline FixedArrayBase(int _length) {
        _init(_length);
    }

    explicit inline FixedArrayBase(int _length, const ElementType* x)
    {
        _init(_length);
        for (int i = 0; i < length; i++)
            this->element[i] = x[i];
    }

    explicit inline FixedArrayBase(const std::vector<ElementType> &x)
    {
        _init((int)x.size());
        for (int i = 0; i < length; i++)
            this->element[i] = x[i];
    }

    int size() const
    {
        return length;
    }

    ElementType& operator [](int n)
    {
        return element[n];
    }

    const ElementType& operator [](int n) const
    {
        return element[n];
    }

    ElementType& at(int n)
    {
        return element[n];
    }

    const ElementType& at(int n) const
    {
        return element[n];
    }

    inline RealType createVector(int length) const {
        return RealType(length);
    }

    ~FixedArrayBase()
    {
        if (this->element != NULL)
        {
            for (int i = 0; i < length; i++)
            {
                element[i].~ElementType();
            }
        }
        this->element = NULL;
    }

private:
    void _init(int _length)
    {
        this->length = _length;
        memoryBlock.allocate(_length * sizeof(ElementType), 0x0);
        this->element = new(memoryBlock.getAlignedStart(0x0)) ElementType[_length];
    }

};

template<typename ElementType>
class FixedArray : public FixedArrayBase<FixedArray<ElementType>, ElementType>
{
public:
    typedef FixedArrayBase<FixedArray<ElementType>, ElementType> BaseClass;

    explicit inline FixedArray(int _length) :
        BaseClass(_length) {}
    explicit inline FixedArray(int _length, const ElementType* _x) :
        BaseClass(_length, _x) {}
    explicit inline FixedArray(const std::vector<ElementType> &_x) :
        BaseClass(_x) {}
};


} //namespace corecvs
#endif  //FIXEDARRAY_H_




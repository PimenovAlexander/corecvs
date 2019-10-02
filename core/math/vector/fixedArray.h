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
#include <memory>
#include <initializer_list>

#include "core/utils/global.h"

#include "core/math/vector/vectorOperations.h"

namespace corecvs {

template<typename RealType, typename ElementType>
class FixedArrayBase : public VectorOperationsBase<RealType, ElementType>
{
public:
    typedef ElementType InternalElementType;

    /**
     * The array of the data elements
     **/

    FixedArrayBase(int length = 0) : FixedArrayBase(length, 0)
    {
        for (int i = 0; i < length; ++i)
            new (data.get() + i) ElementType();
    }

    FixedArrayBase(int _length, const ElementType* x) : FixedArrayBase(_length, 0)
    {
        copyInit(x);
    }

    FixedArrayBase(const std::vector<ElementType> &x) : FixedArrayBase((int)x.size(), x.size() ? &x[0] : 0)
    {
    }

    FixedArrayBase(const std::initializer_list<ElementType> &l) : FixedArrayBase((int)l.size(), l.begin())
    {
    }

    FixedArrayBase(const FixedArrayBase &that) : FixedArrayBase(that.length, &that[0])
    {
    }

    FixedArrayBase(FixedArrayBase &&that) : length(that.length), data(std::move(that.data))
    {
    }

    FixedArrayBase& operator= (const FixedArrayBase &that)
    {
        if (this == &that)
            return *this;
        if (that.length > length)
        {
            length = that.length;
#ifndef WIN32
            data = std::unique_ptr<ElementType[], decltype(&free)>((ElementType*)aligned_alloc(32, sizeof(ElementType) * length), free);
#elif defined(__APPLE__)
            ElementType* _aligned_mem;
            posix_memalign(&_aligned_mem, 32, sizeof(ElementType) * length);
            data = std::unique_ptr<ElementType[], decltype(&free)>(_aligned_mem, free);
#else // VS2013 does not support c++11 aligned_alloc
            data = std::unique_ptr<ElementType[], decltype(&_aligned_free)>((ElementType*)aligned_alloc(32, sizeof(ElementType) * length), _aligned_free);
#endif
            copyInit(&that[0]);
        }
        else
        {
            length = that.length;
            for (auto& v: that)
                data[&v - &that[0]] = v;
        }
        return *this;
    }

    FixedArrayBase& operator=(FixedArrayBase &&that)
    {
        std::swap(data, that.data);
        std::swap(length, that.length);
        return *this;
    }

    operator ElementType*()
    {
        return &data[0];
    }

    operator const ElementType*() const
    {
        return &data[0];
    }

    int size() const
    {
        return length;
    }

    ElementType& operator [](int n)
    {
        return data[n];
    }

    const ElementType& operator [](int n) const
    {
        return data[n];
    }

    ElementType& at(int n)
    {
        return data[n];
    }

    const ElementType& at(int n) const
    {
        return data[n];
    }

    inline RealType createVector(int length) const
    {
        return RealType(length);
    }

    ElementType* begin()
    {
        return &data[0];
    }

    ElementType* end()
    {
        return &data[0] + length;
    }

    const ElementType* begin() const
    {
        return &data[0];
    }

    const ElementType* end() const
    {
        return &data[0] + length;
    }

    friend std::ostream& operator<<(std::ostream& o, const FixedArrayBase &fab)
    {
        o << "[";
        for (auto& v: fab)
            o << fab << (&v + 1 == fab.end() ? "]" : ", ");
        return o;
    }
private:
    explicit inline FixedArrayBase(int length, int) : length(length),

#ifndef WIN32
        data((ElementType*)aligned_alloc(32, sizeof(ElementType) * length), free)
#else // VS2013 does not support c++11 aligned_alloc
        data((ElementType*)aligned_alloc(32, sizeof(ElementType) * length), _aligned_free)
#endif
    {

    }
    void copyInit(const ElementType *from)
    {
        for (int i = 0; i < length; ++i)
            new (data.get() + i) ElementType(from[i]);
    }
    int length;
#ifndef WIN32
    std::unique_ptr<ElementType[], decltype(&free)> data;
#else // VS2013 does not support c++11 aligned_alloc
    std::unique_ptr<ElementType[], decltype(&_aligned_free)> data;
#endif

};

template<typename ElementType>
class FixedArray : public FixedArrayBase<FixedArray<ElementType>, ElementType>
{
public:
    typedef FixedArrayBase<FixedArray<ElementType>, ElementType> BaseClass;

    FixedArray(int _length = 0) : BaseClass(_length)
    {
    }
    FixedArray(int _length, const ElementType* _x) : BaseClass(_length, _x)
    {
    }
    FixedArray(const std::vector<ElementType> &_x) : BaseClass(_x)
    {
    }
    FixedArray(const std::initializer_list<ElementType> &l) : BaseClass(l)
    {
    }
};


} //namespace corecvs

#endif  //FIXEDARRAY_H_

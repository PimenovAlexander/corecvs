#ifndef ABSTRACTBUFFER_H
#define ABSTRACTBUFFER_H
/**
 * \file abstractBuffer.h
 * \brief a header for AbstractBuffer.cpp
 *
 *
 * \ingroup cppcorefiles
 * \date Oct 28, 2009
 * \author alexander
 *
 * This file contains a base class for a rectangular buffer
 *
 *
 * \todo TODO: Check int type usage, no need to use int32_t where it is not crucial
 */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <random>
#include <string>
#include <functional>
#include <type_traits>

#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "memory/memoryBlock.h"
#include "memory/alignedMemoryBlock.h"
#include "core/tbbwrapper/tbbWrapper.h"                 // BlockedRange
#include "core/math/mathUtils.h"                  // randRanged

#include "core/buffers/abstractBufferParams.h"

namespace corecvs {


using std::string;
using std::cout;
using std::endl;

/**
 * This class holds the mapping function that is applied to the buffer element
 * to get another buffer element.
 *
 *
 **/
template<typename InputType, typename OutputType>
class GenericMap
{
public:
    /***
     * map function that transforms the element of the buffer
     */
    OutputType map(InputType &in) { return in; }
};

/* */
template<typename ElementType>
class BufferMap : public GenericMap<ElementType, ElementType>
{
};

/**
 * Preliminary definitions
 */
template<typename ElementType, typename IndexType>
class AbstractBuffer;

template<typename ElementType, typename IndexType>
class AbstractKernel;





/**
 * This is a common buffer template.
 *
 *
 */
template<typename ElementType, typename IndexType = int32_t>
class AbstractBuffer : public AbstractBufferParams<IndexType>
{
public:
    /**
     * Type of the internal data type
     **/
    typedef ElementType InternalElementType;
    typedef IndexType   InternalIndexType;
    static const bool TRIVIALLY_COPY_CONSTRUCTIBLE =
#if __GNUG__ && __GNUC__ < 5
        __has_trivial_copy(ElementType);
#else
        std::is_trivially_copy_constructible<ElementType>::value;
#endif
    static const bool TRIVIALLY_DESTRUCTABLE = std::is_trivially_destructible<ElementType>::value;
    static const bool TRIVIALLY_DEFAULT_CONSTRUCTIBLE =
#if __GNUG__ && __GNUC__ < 5
        __has_trivial_constructor(ElementType);
#else
        std::is_trivially_constructible<ElementType>::value;
#endif

    enum BufferType {
        NORMAL_BUFFER,
        VIEW_BUFFER,
        EXTERNAL_DATA_BUFFER,
        EMPTY_BUFFER
    };

    /**
     * This constant represents the default alignment of the line in the buffer.
     * It is 16 bytes now to work with SSE and possibly with OPENCL.
     *
     **/
#if defined(WITH_SSE) || defined(WITH_NEON) /*|| defined(WITH_OPENCL)*/           // WITH_OPENCL is never defined for core projects to simplify projects deps
    static const size_t DATA_ALIGN_GRANULARITY = 0xF;          // alignment by 16 bytes
#else
    static const size_t DATA_ALIGN_GRANULARITY = 0xF;
#endif

    /**
     * Static member that will be used to prevent and debug memory leaks
     **/
    static atomic_int bufferCount;

    /**
     * The height of the buffer.
     *
     * \todo TODO: This field is public do far, but that is dangerous.
     **/
    //IndexType h;

    /**
     * The width of the buffer
     **/
    //IndexType w;

    /**
     * The stride of the buffer
     *
     * \todo TODO: This field is public do far, but that is dangerous.
     * TODO: Could happen it would be nice to have this in bytes
     **/
    //IndexType stride;

    /**
     * The given stride value to have automatically chosen stride of the buffer
     **/
    static const IndexType STRIDE_AUTO = (IndexType)0;

    /**
     * Sometimes underling buffer should not be deleted
     *
     **/
    //bool shouldDelete;


    /**
     * The properly aligned pointer to an array with data elements, memory of which is kept by the protected field "memoryBlock"
     **/
    ElementType *data;


    /**
     * So far this type of constructor is used only for view creation
     * Should refactor to make it private
     **/
    AbstractBuffer() : AbstractBufferParams<IndexType>()
        , data(NULL)
        , memoryBlock()
        , flags(EMPTY_BUFFER)
    {
        atomic_inc_and_fetch(&bufferCount);
    }

    /**
     *  Constructor that generates an empty buffer filled with zero values
     *
     *  \param h
     *      Height of the buffer
     *  \param w
     *      Width of the buffer
     *  \param shouldInit
     *      true if you need to clear the buffer with zeros
     **/
    AbstractBuffer(IndexType h, IndexType w, bool shouldInit = true, IndexType stride = STRIDE_AUTO)
    {
        _init(h, w, stride, shouldInit);
    }

    /**
     *  Constructor that generates an empty buffer filled with zero values
     *
     *  \param size
     *      Vector with buffer sizes (x is width, y is height)
     *  \param shouldInit
     *      true if you need to clear the buffer with zeros
     *  \param stride
     *      size of stride, STRIDE_AUTO for automatic stride
     *
     **/
    AbstractBuffer(Vector2d<IndexType> size, bool shouldInit = true, IndexType stride = STRIDE_AUTO)
    {
        _init(size.y(), size.x(), stride, shouldInit);
    }

    /**
     * Copy constructor.
     *
     * This makes an exact copy saving the stride
     */
    AbstractBuffer(const AbstractBuffer &that)
    {
        _init(that.getH(), that.getW(), that.getStride(), false);
        _copy(this->data, that.data, this->h, this->w, this->stride, this->stride);
//        memcpy(this->data, that.data, that.sizeInBytes());
    }

    /**
     * Constructor form the pointer to the buffer
     *
     * Not true - This makes an exact copy saving the stride
     */
    AbstractBuffer(const AbstractBuffer *that)
    {
//        _init(that->h, that->w, that->stride, false);
//        memcpy(this->data, that->data, that->sizeInBytes());
        _init(that->getH(), that->getW(), false);
        fillWith(*that);
    }


    /**
     * Constructor from static data
     *
     */
    AbstractBuffer(IndexType h, IndexType w, ElementType *_data)
    {
       _init(h, w, false);
       if (_data)
           fillWithRaw(_data);
    }

    AbstractBuffer(Vector2d<IndexType> size, ElementType *_data)
    {
       _init(size.y(), size.x(), false);
       if (_data)
           fillWithRaw(_data);
    }

    AbstractBuffer(IndexType h, IndexType w, IndexType stride, ElementType* data)
    {
        _init(h, w, stride, false);
        if (data)
            fillWithRaw(data);
    }

    AbstractBuffer(Vector2d<IndexType> size, IndexType stride, ElementType* data)
        : AbstractBuffer(size.y(), size.x(), stride, data)
    {
    }

#if 0
    /**
     * Constructor from data provided by arguments.
     *
     * \note - you should be very careful passing the right type
     */
    AbstractBuffer(IndexType h, IndexType w,  ...)
    {
        _init(h, w, false);
        va_list marker;
        va_start( marker, w );
        const ElementType value = va_arg(marker, ElementType);
        fillWithArgs(value, marker);
        va_end( marker );
    }
#endif

    AbstractBuffer(IndexType h, IndexType w, const ElementType &data, IndexType stride = STRIDE_AUTO)
    {
        _init(h, w, stride, false);
        fillWith(data);
    }

    AbstractBuffer(Vector2d<IndexType> size, const ElementType &data, IndexType stride = STRIDE_AUTO)
    {
        _init(size.y(), size.x(), stride, false);
        fillWith(data);
    }


    /*
     * It is slow and should not be used frequently
     */
    AbstractBuffer(const std::vector<std::vector<ElementType>> &vec, IndexType h, IndexType w, IndexType stride = STRIDE_AUTO)
    {
        std::vector<ElementType> el;
        el.reserve((size_t)w * h);
        for (IndexType i = 0; i < h; ++i)
        {
            for (IndexType j = 0; j < w; ++j)
            {
                el.emplace_back(vec[i][j]);
            }
        }
        _init(h, w, stride, false);
        fillWithRaw(&el[0]);
    }

    explicit operator std::vector<std::vector<ElementType>> () const
    {
        std::vector<std::vector<ElementType>> el(this->h);
        for (IndexType i = 0; i < this->h; ++i)
        {
            for (IndexType j = 0; j < this->w; ++j)
            {
                el[i].emplace_back(element(i, j));
            }
        }
        return el;
    }

    /**
     * \attention YOU SHOULD NEVER USE IT FOR SERIALIZING HUGE DATA
     * If you have such a need start casting h,w,stride etc to uint64_t
     **/
    template<typename V>
    void accept(V& visitor)
    {
        auto vec = (std::vector<std::vector<ElementType>>)(*this);
        int w = this->w, h = this->h, stride = this->stride;
        visitor.visit(w, (IndexType)0, "width");
        visitor.visit(h, (IndexType)0, "height");
        visitor.visit(stride, w, "stride");
        visitor.visit(vec, "data");
        if (w == this->w && h == this->h && stride == this->stride && data)
        {
            std::vector<ElementType> vec2;
            for (IndexType i = 0; i < h; ++i)
                for (IndexType j = 0; j < w; ++j)
                    vec2.emplace_back(vec[i][j]);
            fillWithRaw(&vec2[0]);
            return;
        }
        *this = AbstractBuffer(vec, h, w, stride);
    }

    /**
     * Destructor. Main buffer will be deleted with MemoryBlockRef smart pointer destruction
     *
     * If the buffer is created on top of the other, the best way is
     * to first set data to NULL
     * \todo TODO: Should this be virtual? Usually we do not cast pointers to base type.
     **/
    ~AbstractBuffer()
    {
        this->data = NULL;
        atomic_dec_and_fetch(&bufferCount);                 // this must be always as it's incremented at each ctor
    }


template<typename ResultType>
    ResultType createView(const IndexType y, const IndexType x, const IndexType h, const IndexType w)
    {
        if (y < 0 || x < 0 || x >= w || y >= h) {
            SYNC_PRINT(("AbstractBuffer::createView(y=%d x=%d h=%d w=%d)", y, x, h, w));
        }

        // This is the only legitimate place to use default constructor
        ResultType toReturn;

        toReturn.h          = h;
        toReturn.w          = w;
        toReturn.stride     = this->stride;

        toReturn.data       = &(this->element(y, x));
        toReturn.flags      = VIEW_BUFFER;
        /**
         * Prevent original buffer from being deleted
         * MemoryBlock magically counts references
         **/
        toReturn.memoryBlock = memoryBlock;
        return toReturn;
    }


template<typename ResultType>
    ResultType createView()
    {
        return createView<ResultType>(0, 0, this->h, this->w);
    }


    /*
     * For some legacy stuff where assignment operator is not consistent with abstractBuffer's one;
     * thus limiting us too old pointer-based implementation for "sharing"
     */
template<typename ResultType>
    ResultType* createViewPtr(const IndexType y, const IndexType x, const IndexType h, const IndexType w)
    {
        // This is the only legitimate place to use default constructor
        ResultType* toReturn = new ResultType();

        toReturn->h          = h;
        toReturn->w          = w;
        toReturn->stride     = this->stride;

        toReturn->data       = &(this->element(y, x));
        toReturn->flags      = VIEW_BUFFER;
        /**
         * Prevent original buffer from being deleted
         * MemoryBlock magically counts references
         **/
        toReturn->memoryBlock = memoryBlock;
        return toReturn;
    }


template<typename ResultType>
    ResultType* createViewPtr()
    {
        return this->createViewPtr<ResultType>(0, 0, this->h, this->w);
    }


    /** This allows to build a buffer on top of the exising buffer. User is fully responsible for memory management **/
template<typename ResultType>
    static ResultType* CreateBuffer(IndexType _h, IndexType _w, IndexType _stride, ElementType *_data)
    {
        ResultType* toReturn = new ResultType();
        toReturn->_init(_h, _w, _stride, false, false);
        toReturn->data = _data;
        toReturn->flags = EXTERNAL_DATA_BUFFER;
        return toReturn;
    }


    /**
     * The element getter.
     *
     * A common practice in this project is for the y coordinate to come first
     **/
#if 1
    inline ElementType &element(const IndexType y, const IndexType x)
    {
        return data[(size_t)y * this->stride + x];
    }

    inline const ElementType &element(const IndexType y, const IndexType x) const
    {
        return data[(size_t)y * this->stride + x];
    }
#else
    inline ElementType &element(const size_t y, const size_t x)
    {
        return data[y * this->stride + x];
    }

    inline const ElementType &element(const size_t y, const size_t x) const
    {
        return data[y * this->stride + x];
    }
#endif

    /**
     * The element getter
     **/
    inline ElementType &element(const Vector2d<IndexType> &p)
    {
        return element(p.y(), p.x());
    }

    inline const ElementType &element(const Vector2d<IndexType> &p) const
    {
        return element(p.y(), p.x());
    }

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoord(const IndexType y,const  IndexType x) const
    {
        return (x >= 0) && (x < this->w) && (y >= 0) && (y < this->h);
    }

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoord(const Vector2d<IndexType> &p) const
    {
        return isValidCoord(p.y(), p.x());
    }

    /**
     *   Safe way to set element. Slow, however
     **/
    inline void setElement(const IndexType y, const IndexType x, const ElementType &e)
    {
        if (isValidCoord(y, x)) {
            element(y, x) = e;
        }
    }

    /**
     * Checks if this buffer has the same size as the other
     */
    inline bool hasSameSize (const  AbstractBuffer<ElementType, IndexType> *other) const
    {
        return (this->h == other->h) && (this->w == other->w);
    }

    /**
     * Checks if this buffer has the same size as the parameters given
     */
    inline bool hasSameSize (const IndexType &otherH, const IndexType &otherW) const
    {
        return (this->h == otherH) && (this->w == otherW);
    }

    /**
     * Checks if this buffer has zero size
     */
    inline bool hasZeroSize () const
    {
        return (this->h == 0) || (this->w == 0);
    }

    /**
     *  Height getter
     **/
    inline IndexType getH() const
    {
        return this->h;
    }

    /**
     *  Stride getter
     **/
    inline IndexType getStride() const
    {
        return this->stride;
    }

    /**
     *  Width getter
     **/
    inline IndexType getW() const
    {
        return this->w;
    }

    /**
     *  Buffer size getter
     **/
    inline Vector2d<IndexType> getSize() const
    {
        return Vector2d<IndexType>(this->w, this->h);
    }

    /**
     *  Checking for successfull allocation
     **/
    inline bool isAllocated() const
    {
        return data != NULL;
    }

    inline size_t numElements() const
    {
        return (size_t)this->h * this->stride;
    }

    /**
     *  Calculate image buffer size in bytes
     **/
    inline size_t sizeInBytes() const
    {
        return this->numElements() * sizeof(ElementType);
    }

    /**
     *  Calculate memory for all the structures in the object
     **/
    inline size_t memoryFootprint() const
    {
        return this->memoryBlock.getTotalObjectSize(this->sizeInBytes(), DATA_ALIGN_GRANULARITY) + sizeof(this);
    }


    /**
     *  This function fills the rectangle inside the buffer with the value.
     *
     *  You could use this to clean part of the buffer
     **/
    void fillRectangleWith(IndexType y, IndexType x, IndexType rectH, IndexType rectW, const ElementType &value)
    {
        CORE_ASSERT_TRUE_P(x >= 0 && y >= 0 && x < this->w && y < this->h, ("[%d:%d] origin is not inside the buffer [%dx%d]", x, y, this->w, this->h));
        CORE_ASSERT_TRUE_P(x + rectW >= 0       && y + rectH >= 0 &&
                           x + rectW <= this->w && y + rectH <= this->h, ("[%d:%d] right lower corner is not inside the buffer [%dx%d]", x + rectW, y + rectH, this->w, this->h));

        for (IndexType i = y; i < y + rectH; i++)
        {
            ElementType *line = &(element(i, x));
            for (IndexType j = 0; j < rectW; j++)
            {
                if (TRIVIALLY_COPY_CONSTRUCTIBLE && TRIVIALLY_DESTRUCTABLE)
                    *line++ = value;
                else
                {
                    line->~ElementType();
                    new (line++) ElementType(value);
                }
            }
        }
    }

    /**
     *  This function fills the whole buffer with the value.
     *
     *  You could use this to clean the buffer
     **/
    void fillWith(const ElementType &value)
    {
        fillRectangleWith(0, 0, this->h, this->w, value);
    }

    /**
     *  This function fills the whole buffer with the values from the given array
     *
     *  TODO: this function doesn't call the copy constructor. I believe it should.
     **/
    void fillWithRaw(ElementType *_data)
    {
       if (this->w == this->stride)
       {
           _copy(data, _data, this->w * this->h);
           return;
       }
       _copy(data, _data, this->h, this->w, this->stride, this->w);
    }

    /**
    *  This function fills the whole buffer with the values from the other buffer
    *
    *  TODO: this function doesn't call the copy constructor. I believe it should.
    **/
    inline void fillWith(const AbstractBuffer &other)
    {
        IndexType copyH = CORE_MIN(this->h, other.h);
        IndexType copyW = CORE_MIN(this->w, other.w);

        /* If buffers have same horizontal geometry use fast method*/
        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
            if (other.stride == this->stride && other.w == this->w)
            {
                memcpy((void *)this->data, (void *)other.data, sizeof(ElementType) * copyH * this->stride);
                return;
            }
            for (IndexType i = 0; i < copyH; i++)
            {
                memcpy((void *)&this->element(i, 0), (void *)&other.element(i, 0), sizeof(ElementType) * copyW);
            }
            return;
        }
        _copy(data, other.data, copyH, copyW, this->stride, other.stride);
    }

    inline void fillWith(const AbstractBuffer &other, IndexType targetY, IndexType targetX)
    {
        IndexType copyH = CORE_MIN(this->h - targetY, other.h);
        IndexType copyW = CORE_MIN(this->w - targetX, other.w);

        /* If buffers have same horizontal geometry use fast method*/
        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
            for (IndexType i = 0; i < copyH; i++)
            {
                memcpy(&this->element(i + targetY, targetX), &other.element(i, 0), sizeof(ElementType) * copyW);
            }
            return;
        }
        _copy(&this->element(targetY, targetX), other.data , copyH, copyW, this->stride, other.stride);
    }

    /**
     *  You could use this to clean the buffer
     *
     *  \note you should be very careful passing variables of the right type
     **/

    void fillWithArgs(const ElementType value, va_list marker)
    {
        this->element(0, 0) = value;
        for (IndexType i = 0; i < this->h; i++)
            for (IndexType j = 0; j < this->w; j++)
                if (i != 0 || j != 0)
                    this->element(i,j) = va_arg(marker, ElementType);
    }

    void fillWithArgs(const ElementType value, ...)
    {
        va_list  marker;
        va_start(marker, value);
        fillWithArgs(value, marker);
        va_end(marker);
    }

    void fillLineWithArgs(IndexType line, const ElementType value, ...)
    {
       va_list marker;
       va_start(marker, value);

       this->element(line, 0) = value;
       for (IndexType j = 1; j < this->w; j++)
           this->element(line,j) = va_arg(marker, ElementType);
       va_end(marker);
    }

    /** Fills the buffer by random values within the given range */
    void fillWithRands(ElementType valueMax /*= ElementType::max()*/, ElementType valueMin = ElementType(0))
    {
        std::mt19937 rng;
        std::uniform_int_distribution<ElementType> dist(valueMax, valueMin);

        for (IndexType i = 0; i < this->h; i++)
            for (IndexType j = 0; j < this->w; j++)
                this->element(i,j) = dist(rng);
    }

    void checkerBoard(IndexType square, ElementType valueMax /*= ElementType::max()*/, ElementType valueMin = ElementType(0))
    {
        if (square <= 0) square = 1;
        for (IndexType i = 0; i < this->h; i++)
        {
            for (IndexType j = 0; j < this->w; j++)
            {
                bool color = ((i / square) % 2) ^ ((j / square) % 2);
                this->element(i,j) = color ?  valueMin : valueMax;
            }
        }
    }

    /**
     * Constructor that copies an area
     * [min(y1,y2), max(y1,y2)) x [min(x1,x2), max(x1,x2))
     *
     * TODO: Check if changing block position on degenerate (x1,y1<0 || x2>w || y2 > h) is desired
     */
    AbstractBuffer(const AbstractBuffer &src, IndexType x1, IndexType y1, IndexType x2, IndexType y2)
    {
        // SYNC_PRINT(("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)\n", src->w, src->h, x1, y1, x2, y2));

        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);
        if (x1 < 0)  { x2 += (-x1); x1 = 0; }
        if (y1 < 0)  { y2 += (-y1); y1 = 0; }

        if (x2 > src.w) { x1 -= x2 - src.w; x2 = src.w; }
        if (y2 > src.h) { y1 -= y2 - src.h; y2 = src.h; }

        _init(y2 - y1, x2 - x1);

        CORE_ASSERT_TRUE_P((x1 >= 0) && (y1 >= 0) && (x2 > x1) && (y2 > y1),
            ("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)", src.w, src.h, x1, y1, x2, y2));
        CORE_ASSERT_TRUE_P((src.w >= x2) && (src.h >= y2),
            ("Internal error with input [%d x %d] (%d, %d) -> (%d, %d)", src.w, src.h, x1, y1, x2, y2));

        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
            for (IndexType i = 0; i < this->h; i++)
            {
                memcpy((void *)&element(i, 0), &(src.element(y1 + i, x1)), sizeof(ElementType) * this->w);
            }
        }
        else
        {
            for (IndexType i = 0; i < this->h; ++i)
            {
                _copy(data + i * this->stride, &src.element(y1 + i, x1), this->w);
            }
        }
    }


    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    void innerCycleDoConvolve(ReturnType *toReturn, AbstractKernel<ConvElementType, ConvIndexType> *kernel, int i)
    {
        for (IndexType j = 0; j < this->w; j++)
        {
            toReturn->element(i,j) =  kernel->template multiplyAtPoint<ElementType, IndexType>(this, i,j);
        }
    }


    template<typename ReturnType, typename SelfType, typename ConvElementType, typename ConvIndexType>
    class ParallelDoConvolve
    {
        SelfType   *buffer;
        ReturnType *toReturn;
        AbstractKernel<ConvElementType, ConvIndexType> *kernel;
        bool        onlyValid;

    public:
        ParallelDoConvolve(
                ReturnType *_toReturn,
                SelfType * _buffer,
                AbstractKernel<ConvElementType, ConvIndexType> *_kernel,
                bool onlyValid) :
         buffer(_buffer), toReturn(_toReturn), kernel(_kernel), onlyValid(onlyValid)
        {}

        void operator()(const BlockedRange<IndexType>& r) const
        {
            IndexType left =  onlyValid ? kernel->x : 0;
            IndexType right = onlyValid ? buffer->w + kernel->x - kernel->w + 1 : buffer->w;
            if (!onlyValid)
            {
                for (IndexType i = r.begin(); i != r.end(); i++)
                {
                    for (IndexType j = left; j < right; j++)
                    {
                        toReturn->element(i,j) = kernel->template multiplyAtPoint<ElementType, IndexType>(buffer, i, j);
                    }
                }
            }
            else
            {
                for (IndexType i = r.begin(); i != r.end(); i++)
                {
                    for (IndexType j = left; j < right; j++)
                    {
                        toReturn->element(i,j) = kernel->template multiplyAtPoint<ElementType, IndexType, true>(buffer, i, j);
                    }
                }
            }
        }
    };

    /**
     *  This function is used to convolve/cross-correlate a buffer with the kernel.
     *  In general convolution is
     *
     *  \f[
     *  (f * g )(t)
     *      = \int_{-\infty}^{\infty} f(\tau)\, g(t - \tau)\, d\tau
     *      = \int_{-\infty}^{\infty} f(t-\tau)\, g(\tau)\, d\tau
     *  \f]
     *
     *  But this function does more of cross-correlation.
     *
     *
     *
     **/
    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    void doConvolve(ReturnType *output, AbstractKernel<ConvElementType, ConvIndexType> *kernel, bool onlyValid = false, bool parallel = true)
    {
        /*TODO: Well we need to process this gracefully */
        if (output->h != this->h || output->w != this->w)
            return;

        IndexType top    = onlyValid ? kernel->y : 0;
        IndexType bottom = onlyValid ? this->h + kernel->y - kernel->h + 1 : this->h;
        parallelable_for(top, bottom, ParallelDoConvolve<ReturnType, AbstractBuffer<ElementType, IndexType>, ConvElementType, ConvIndexType>(output, this, kernel, onlyValid), parallel);
    }

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    ReturnType* doConvolve(AbstractKernel<ConvElementType, ConvIndexType> *kernel, bool onlyValid = false, bool parallel = true)
    {
        ReturnType *toReturn = new ReturnType(this->h, this->w);
        doConvolve(toReturn, kernel, onlyValid, parallel);
        return toReturn;
    }

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    class ParallelDoConvolve1
    {
        ReturnType *toReturn;
        AbstractKernel<ConvElementType, ConvIndexType> *kernel;
        AbstractBuffer<ElementType, IndexType> *buf;

    public:
        ParallelDoConvolve1(
            ReturnType *_toReturn,
            AbstractKernel<ConvElementType, ConvIndexType> *_kernel,
            AbstractBuffer<ElementType, IndexType> *_buf) :
        toReturn(_toReturn),
        kernel(_kernel),
        buf(_buf)
        {}

        void operator()(const BlockedRange<IndexType>& r) const
        {
            IndexType j;

            for (IndexType i = r.begin(); i != r.end(); i++)
            {
                ElementType *line = &buf->element(i, 0);
                for (j = kernel->centerX; j < toReturn->w - kernel->w + kernel->centerX; j++)
                {
                    toReturn->element(i,j) =
                            kernel->template multiplyAtPoint<ElementType, IndexType>(line, buf->stride, i, j);
                    line++;
                }
            }
        }
    };

    template<typename ReturnType, typename ConvElementType, typename ConvIndexType>
    ReturnType* doConvolve1(AbstractKernel<ConvElementType, ConvIndexType> *kernel)
    {
        IndexType i,j;
        ReturnType *toReturn = new ReturnType(this->h, this->w);
        parallelable_for(kernel->centerY, this->h - kernel->h + kernel->centerY,
                ParallelDoConvolve1<ReturnType, ConvElementType, ConvIndexType>(toReturn, kernel, this));
        return toReturn;
    }


    /* Scalar types related methods */
    /**
     *  This method tests for equality
     **/
    bool isEqual(const AbstractBuffer &that) const
    {
        if (that.h != this->h || that.w != this->w)
            return false;
        for (IndexType i = 0; i < this->h; i++)
        {
            const ElementType *thisElemRunner = &(this->element(i, 0));
            const ElementType *thatElemRunner = &(that.element(i, 0));
            for (IndexType j = 0; j < this->w; j++)
            {
                if (*thatElemRunner != *thisElemRunner)
                {
#ifdef ASSERTS
                    printf("Buffers have equal sizes [h=%d w=%d]\n", this->h, this->w);

                    printf("[ 0x");
                    for (size_t k = 0; k < sizeof(ElementType); k++)
                        printf("%02X ", ((uint8_t *)(void*)(thisElemRunner))[k]);
                    printf("] vs [ 0x");
                    for (size_t k = 0; k < sizeof(ElementType); k++)
                        printf("%02X ", ((uint8_t *)(void*)(thatElemRunner))[k]);
                    printf("]\nPos y=%d x=%d\n", i , j);
                    CORE_ASSERT_FAIL_P(("Buffers don't match"));
#endif
                    return false;
                }
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        return true;
    }

    bool isEqualTrace(const AbstractBuffer &that) const
    {
        if (that.h != this->h || that.w != this->w)
            return false;
        int diffs = 0;
        for (IndexType i = 0; i < this->h; i++)
        {
            const ElementType *thisElemRunner = &(this->element(i, 0));
            const ElementType *thatElemRunner = &(that.element(i, 0));
            for (IndexType j = 0; j < this->w; j++)
            {
                if (*thatElemRunner != *thisElemRunner)
                {
                    cout << "Differ Pos y="<< i << " x=" << j << " ";
                    cout << "Fst = " <<  *thisElemRunner << " sec= " << *thatElemRunner << endl;
                    diffs++;
                }
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        cout << "Totally " << diffs << " diffs" << endl;
        return diffs == 0;
    }

    /**
     *  This method performs the "oper" operation between this and that buffers and
     *  puts result to this buffer
     *
     *  \param that - the AbstractBuffer to be processed with this to this
     *  \param oper - operation to be performed
     **/

template<typename operation, typename OtherBuffer>
    void binaryOperationInPlace(const OtherBuffer &that, const operation &oper = operation())
    {
        IndexType h = CORE_MIN(this->h, that.h);
        IndexType w = CORE_MIN(this->w, that.w);
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            const typename OtherBuffer::InternalElementType *thatElemRunner = &(that.element(i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                /* Writing like this just to emphasize what is actually called */
                *thisElemRunner = oper.operator()(*thisElemRunner, *thatElemRunner);
                thisElemRunner++;
                thatElemRunner++;
            }
        }
    }

/*template<typename operation>
    void mapOperationElementwize(const ElementType &that)
    {
        for (IndexType i = 0; i < h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            for (IndexType j = 0; j < w; j++)
            {
                *thisElemRunner = operation().operator()(*thisElemRunner, that);
                thisElemRunner++;
            }
        }
    }*/

template<typename operation>
    void mapOperationElementwize (const operation &map)
    {
        for (IndexType i = 0; i < this->h; i++)
        {
            ElementType *thisElementRunner = &(this->element(i, 0));
            for (IndexType j = 0; j < this->w; j++)
            {
                *thisElementRunner = map.operator()(*thisElementRunner);
                thisElementRunner++;
            }
        }
    }


    // XXX: overload added in order to use lambdas supplied directly as arguments
template<typename operation>
    void touchOperationElementwize (const operation &map)
    {
        for (IndexType i = 0; i < this->h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i,0));
            for (IndexType j = 0; j < this->w; j++)
            {
                map.operator()(i, j, *thisElemRunner);
                thisElemRunner++;
            }
        }
    }

template<typename operation>
    void touchOperationElementwize (operation &map)
    {
        for (IndexType i = 0; i < this->h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            for (IndexType j = 0; j < this->w; j++)
            {
                map.operator()(i, j, *thisElemRunner);
                thisElemRunner++;
            }
        }
    }

friend std::ostream & operator <<(std::ostream &out, const AbstractBuffer &buffer)
    {
        // typedef AbstractBuffer::InternalIndexType IndexType;
        //
        for (IndexType i = 0; i < buffer.h; i++)
        {
           for (IndexType j = 0; j < buffer.w; j++)
           {
               out << buffer.element(i, j) << " ";
           }
           out << endl;
        }
        return out;
    }

    template<typename T=ElementType, typename I=IndexType>
    typename std::enable_if<AbstractBuffer<T, I>::TRIVIALLY_COPY_CONSTRUCTIBLE, bool>::type
    dump(std::ostream& s, bool binaryMode) const
    {
        AbstractBufferParams<I>::dump(s, sizeof(ElementType), binaryMode);
        if (binaryMode) {
            s.write((char *)data, sizeInBytes());
        }
        return !s.bad();
    }

    template<typename T=ElementType, typename I=IndexType>
    typename std::enable_if<AbstractBuffer<T, I>::TRIVIALLY_COPY_CONSTRUCTIBLE, bool>::type
    load(std::istream& s, bool binaryMode)
    {
        if (!AbstractBufferParams<I>::load(s, sizeof(ElementType), binaryMode))
            return false;

        _init(getH(), getW(), getStride(), false);

        if (!binaryMode)
            return true;                                    // loading in text mode is outside

        s.read((char *)data, sizeInBytes());
        return !s.bad();
    }

    /**
     *  Please check that you really don't want to invoke the copy constructor
     *
     **/
template<typename OtherType>
    OtherType *mirrorVerticalDump()
    {
        OtherType *toReturn = new OtherType(this->h, this->w);
        for (IndexType i = 0; i < this->h; i++)
        {
            _copy(&(toReturn->element(this->h - 1 - i, 0)), &(this->element(i, 0)), this->w);
        }
        return toReturn;
    }

  /** Mirrors current buffer vertically */
  void mirrorVertical()
  {
      for (IndexType i = 0; i < this->h / 2; i++)
      {
          _copy(&(this->element(this->h - 1 - i, 0)), &(this->element(i, 0)), this->w);
      }
  }

  /** Rotates current buffer 180 degrees */
  void rotate180()
  {
      for (IndexType i = 0; i < this->h / 2; i++)
      {
            ElementType *thisElemRunner = &(this->element(i, 0));
            ElementType *thatElemRunner = &(this->element(this->h - 1 - i, this->w - 1));
            for (IndexType j = 0; j < this->w; j++)
            {
                ElementType tmp = *thisElemRunner;
                *thisElemRunner = *thatElemRunner;
                *thatElemRunner = tmp;

                thatElemRunner--;
                thisElemRunner++;
            }
      }
  }

template<typename OtherType>
    OtherType *mirrorVertical()
    {
        OtherType *toReturn = new OtherType(this->h, this->w);
        for (IndexType i = 0; i < this->h; i++)
        {
            ElementType *thisElemRunner = &(this->element(i, 0));
            typename OtherType::InternalElementType *thatElemRunner = &(toReturn->element(this->h - 1 - i, 0));
            for (IndexType j = 0; j < this->w; j++)
            {
                *thatElemRunner = *thisElemRunner;
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        return toReturn;
    }


protected:

    /**
     * The memory block that holds and manages the data
     **/
    AMBReference   memoryBlock;
  //MemoryBlockRef memoryBlock;

    BufferType     flags;

private:

    /**
     *  This is a helper method for constructing.
     *  You should ensure it will never be called from anywhere except the constructing methods.
     *
     *  \param h
     *      Height of the buffer
     *  \param w
     *      Width of the buffer
     *  \param stride
     *      Buffer stride
     *  \param shouldInit
     *      true if the buffer should be filled with 0s
     *      false otherwize
     *  \param shouldAlloc
     *      true if memory should be allocated for the buffer.
     *      If you choose not to allocate - you could make the buffer base on the existing memory.
     *      Do not forget to somehow change constructor then
     *
     **/
    int _init(IndexType h, IndexType w, IndexType stride, bool shouldInit = true, bool shouldAlloc = true)
    {
        this->flags = NORMAL_BUFFER;
        this->setH(h);
        this->setW(w);
        this->setStride(stride != STRIDE_AUTO ? stride : _getStride(w));

        if (shouldAlloc)
        {
            size_t allocatedSize = this->sizeInBytes();
            auto wa = this->w, ha = this->h, sa = this->stride;
            memoryBlock =
                TRIVIALLY_DESTRUCTABLE ?
                    AMBReference(allocatedSize, DATA_ALIGN_GRANULARITY + 1) :
                    AMBReference(allocatedSize, DATA_ALIGN_GRANULARITY + 1, [=](AlignedMemoryBlock* b)
                    {
                        _del((ElementType *)b->getAlignedStart(), ha, wa, sa);
                        delete b;
                    });
            this->data = (ElementType *)memoryBlock.getAlignedStart();

            CORE_ASSERT_TRUE_P(this->data, ("out of memory or invalid buffer size (%" PRISIZE_T ")", allocatedSize));
            if (shouldInit || !TRIVIALLY_DEFAULT_CONSTRUCTIBLE) {
                CORE_CLEAR_MEMORY(this->data, allocatedSize);
                _initArray(this->data, h, w, sa);
            }
#if 0
#ifdef ASSERTS
            /**
             *  Mark the margin zone that is used for alignment with a distinct pattern.
             **/
            for (IndexType i = 0; i < this->getH(); i++)
            {
                uint8_t *start = reinterpret_cast<uint8_t *>(&this->element(i, this->w));
                uint8_t *end   = reinterpret_cast<uint8_t *>(&this->element(i + 1,   0));

                for (unsigned j = 0; start + j < end; j++)
                    start[j] = (uint8_t)(j * 2);
            }
#endif
#endif
        }
        else
        {
          //this->_allocatedSize = 0;
            this->data = NULL;
        }

        atomic_inc_and_fetch(&bufferCount);
        return 0;
    }

    int _getStride(int w)
    {
        /**
         *  Aligning w
         *  if w has ones in the part masked by DATA_STRIDE_GRANULARITY we should increment stride
         *
         *  TODO : should use pointer arithmetics instead
         * */
        IndexType strideGuess = w;
        size_t lineLen  = strideGuess * sizeof(ElementType);
        while (lineLen & DATA_ALIGN_GRANULARITY)
        {
           strideGuess++;
           lineLen += sizeof(ElementType);
        }
        return strideGuess;
    }

    int _init(IndexType h, IndexType w, bool shouldInit = true, bool shouldAlloc = true)
    {
        return _init(h, w, _getStride(w), shouldInit, shouldAlloc);
    }

    static void _copy(ElementType* dst, const ElementType* src, IndexType cnt)
    {
        /*
         * Using traits switch to memcpy if possible
         */
        if (TRIVIALLY_COPY_CONSTRUCTIBLE)
        {
            memcpy((void *)dst, (void *)src, cnt * sizeof(ElementType));
        }
        else
        {
            for (IndexType i = 0; i < cnt; ++i)
            {
                if (!TRIVIALLY_DESTRUCTABLE)
                    dst[i].~ElementType();
                new (dst + i) ElementType(src[i]);
            }
        }
    }

    static void _copy(ElementType* dst, const ElementType* src, IndexType h, IndexType w, IndexType strideDst, IndexType strideSrc)
    {
        for (IndexType i = 0; i < h; ++i)
            _copy(dst + (size_t)i * strideDst, src + (size_t)i * strideSrc, w);
    }
    static void _del(ElementType* ptr, IndexType h, IndexType w, IndexType stride)
    {
        CORE_UNUSED(ptr);
        CORE_UNUSED(h);
        CORE_UNUSED(w);
        CORE_UNUSED(stride);
        /*
         * Using traits switch between "do nothing" (memory is cleared by memoryBlock)
         * and "destruct all elements"
         */
        if (!TRIVIALLY_DESTRUCTABLE)
        {
            for (IndexType i = 0; i < h; ++i)
                for (IndexType j = 0; j < w; ++j)
                    ptr[(size_t)i * stride + j].~ElementType();
        }
    }

    static void _initArray(ElementType* ptr, IndexType h, IndexType w, IndexType stride)
    {
        /*
         * This initalizes array with default constructor
         */
        if (!TRIVIALLY_DEFAULT_CONSTRUCTIBLE)
        {
            for (IndexType i = 0; i < h; ++i)
                for (IndexType j = 0; j < w; ++j)
                    new (ptr + (size_t)i * stride + j) ElementType();
        }
        else
        {
            memset((void *)ptr, 0, sizeof(ElementType) * stride * h);
        }
    }

    static void _initArray(const ElementType& el, ElementType* ptr, IndexType h, IndexType w, IndexType stride)
    {
        for (IndexType i = 0; i < h; ++i)
        {
            for (IndexType j = 0; j < w; ++j)
            {
                new (ptr + (size_t)i * stride + j) ElementType(el);
            }
        }
    }

}; // AbstractBuffer

/**
 *  Counter for memory profiling
 **/
template<typename ElementType, typename IndexType>
atomic_int AbstractBuffer<ElementType, IndexType>::bufferCount(0);

} //namespace corecvs

#endif // ABSTRACTBUFFER_H

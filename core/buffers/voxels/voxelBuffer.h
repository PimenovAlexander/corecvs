/**
 * \file voxelBuffer.h
 * \brief Add Comment Here
 *
 * \date Sep 3, 2011
 * \author alexander
 */

#ifndef VOXELBUFFER_H_
#define VOXELBUFFER_H_

#include <vector>

#include "global.h"
#include "memoryBlock.h"
namespace corecvs {

/**
 *
 *
 * <pre>
 *
 *
 *              * * * * *      ^  Z (L)
 *              * * * * *     /
 *              * * * * *    /
 *              * * * * *   /
 *                         /
 *         * * * * *      /
 *         * * * * *     /
 *         * * * * *    /
 *         * * * * *   /
 *                    /
 *  | * * * * *      /
 *  | * * * * *     /
 *  | * * * * *    /
 *  | * * * * *
 *  |  --------> X (W)
 *  v
 * Y (H)
 *
 *
 *
 *
 * </pre>
 *
 **/
template<typename RealType, typename ElementType>
class VoxelBufferBase
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

    int l;
    int h;
    int w;

    int size;


    inline VoxelBufferBase(int _l, int _h, int _w) {
        _init(_l, _h, _w);
    }

    /*explicit inline FixedArrayBase(int _h, int _w, int _l, const ElementType* x)
    {
        _init(_h, _w, _l);
        for (int i = 0; i < length; i++)
            this->element[i] = x[i];
    }*/

    /*explicit inline FixedArrayBase(const std::vector<ElementType> &x)
    {
        _init(x.size());
        for (int i = 0; i < length; i++)
            this->element[i] = x[i];
    }*/

    /**
     * The element getter.
     *
     * A common practice in this project is for the y coordinate come first
     **/
    inline ElementType &element(const IndexType z, const IndexType y, const IndexType x)
    {
        return element[z * w * h + y * w + x];
    };

    inline const ElementType &element(const IndexType z, const IndexType y, const IndexType x) const
    {
        return element[z * w * h + y * w + x];
    };

    /**
     * The element getter
     **/
    inline ElementType &element(const Vector3d32 &p)
    {
        return element(p.z(), p.y(), p.x());
    };

    inline const ElementType &element(const Vector3d32 &p) const
    {
        return element(p.z(), p.y(), p.x());
    };

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoord(const IndexType z, const IndexType y,const  IndexType x) const
    {
        return (x >= 0) && (x < w) &&
               (y >= 0) && (y < h) &&
               (z >= 0) && (z < l);
    };

    /**
     *  Checks if coordinate lies within the buffer area
     **/
    inline bool isValidCoord(const Vector2d<IndexType> &p) const
    {
        return isValidCoord(p.z(), p.y(), p.x());
    };

    /**
     * Checks if this buffer has the same size as the other
     */
    inline bool hasSameSize (const VoxelBufferBase *other) const
    {
        return (this->h == other->h) &&
               (this->w == other->w) &&
               (this->l == other->l);
    };

    /**
     * Checks if this buffer has the same size as the parameters given
     */
    inline bool hasSameSize (const IndexType &otherL, const IndexType &otherH, const IndexType &otherW) const
    {
        return (this->l == otherL) &&
               (this->h == otherH) &&
               (this->w == otherW);
    };

    /**
     *  Length getter
     **/
    inline IndexType getL() const
    {
        return l;
    }

    /**
     *  Height getter
     **/
    inline IndexType getH() const
    {
        return h;
    }

    /**
     *  Width getter
     **/
    inline IndexType getW() const
    {
        return w;
    }

    Vector2d32 getSize() const
    {
        return Vector2d32(w, h, l);
    }

    ~FixedArrayBase()
    {
        if (this->element)
        {
            for (int i = 0; i < size; i++)
            {
                element[i].~ElementType();
            }

        }
        this->element = NULL;
    }

private:
    void _init(int _l, int _h, int _w)
    {
        this->l = _l;
        this->h = _h;
        this->w = _w;

        this->size = this->l * this->h * this->w;
        memoryBlock.allocate(this->size * sizeof(ElementType), 0x0);
        this->element = new(memoryBlock.getAlignedStart(0x0)) ElementType[this->size];
    }


};


} //namespace corecvs
#endif /* VOXELBUFFER_H_ */


#ifndef PUNCHEDBUFFEROPERATIONS_H_
#define PUNCHEDBUFFEROPERATIONS_H_

/**
 * \file punchedBufferOperations.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 27, 2010
 * \author alexander
 */



#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/tbbwrapper/tbbWrapper.h"
#include "core/buffers/float/dpImage.h"

namespace corecvs {

template<typename ThisTypeName, typename ElementType>
class PunchedBufferOperations
{
public:
    typedef ElementType  InternalElementType;

    /**
     *  Static cast functions
     **/
    inline ThisTypeName *realThis() {
        return static_cast<ThisTypeName *>(this);
    }

    bool isElementKnownVec(const Vector2d32 &p)
    {
        return realThis()->isElementKnown(p.y(), p.x());
    }

    bool isElementKnownVec(const Vector2d16 &p)
    {
        return realThis()->isElementKnown(p.y(), p.x());
    }

    void setElementUnknownVec(const Vector2d32 &p)
    {
        realThis()->setElementUnknown(p.y(), p.x());
    }

    void setElementUnknownVec(const Vector2d16 &p)
    {
        realThis()->setElementUnknown(p.y(), p.x());
    }


    class ParallelDensityCounter
    {
        ThisTypeName *realThis;
    public:
        int count;
        void operator()( const BlockedRange<int>& r )
        {
            int i, j;
            for (i = r.begin(); i < r.end(); i++)
                for (j = 0; j < realThis->w; j++)
                    if (realThis->isElementKnown(i,j))
                        count++;
        }
#ifdef WITH_TBB
        ParallelDensityCounter( ParallelDensityCounter& x, tbb::split ) :
            realThis(x.realThis)
          , count(0)
        {}

        void join( const ParallelDensityCounter& y ) {
            count += y.count;
        }
#endif
        ParallelDensityCounter(ThisTypeName *_realThis ) :
            realThis(_realThis)
          , count(0)
        {}
    };

    unsigned density()
    {
        ThisTypeName *realThis = (ThisTypeName *)this;
        ParallelDensityCounter sc(realThis);

        parallelable_reduce(0, realThis->h, sc);

        return sc.count;
    }

    template <class B>
    B *knownConvert(typename B::InternalElementType known = 1, typename B::InternalElementType unknown = 0)
    {
        B *toReturn = new B(realThis()->getSize(), false);
        for (int i = 0; i < toReturn->h; i++)
        {
            for (int j = 0; j < toReturn->w; j++)
            {
                toReturn->element(i,j) = realThis()->isElementKnown(i,j) ? known : unknown;
            }
        }

        return toReturn;
    }

    FpImage *knownAsFloat(float known = 1.0, float unknown = 0.0)
    {        
        FpImage *toReturn = new FpImage(realThis()->getSize(), false);      
        for (int i = 0; i < toReturn->h; i++)
        {
            for (int j = 0; j < toReturn->w; j++)
            {
                toReturn->element(i,j) = realThis()->isElementKnown(i,j) ? known : unknown;
            }
        }

//        SYNC_PRINT(("Created buffer %x of size [%d %d]\n", toReturn, toReturn->h, toReturn->w));
        return toReturn;
    }

     /* TODO: Should make this parallel*/
    void getStats(InternalElementType *sumOut, int *numOut)
    {
        InternalElementType sum = ThisTypeName::FlowElement(0);
        int num = 0;
        ThisTypeName *realThis = (ThisTypeName *)this;
        for (int i = 0; i < realThis->h; i++)
        {
            for (int j = 0; j < realThis->w; j++)
            {
               if (realThis->isElementKnown(i, j))
               {
                   sum += realThis->element(i, j);
                   num++;
               }
            }
        }

        if (sumOut != NULL) *sumOut = sum;
        if (numOut != NULL) *numOut = num;
    }

    bool isSubsetOf(const ThisTypeName &that)
    {
        if (that.h != realThis()->h || that.w != realThis()->w)
        {
            std::cout << "f1\n";
            return false;
        }
        int diff = 0, eq = 0, rev = 0;
        for (int i = 0; i < realThis()->h; i++)
        {
            const ElementType *thisElemRunner = &(realThis()->element(i, 0));
            const ElementType *thatElemRunner = &(that.element(i, 0));
            for (int j = 0; j < realThis()->w; j++)
            {
                if (realThis()->isElementKnown(i, j)) {
                if (*thatElemRunner != *thisElemRunner)
                {
//                    std::cout << "f2 " << i << ' ' << j << ' ' << *thisElemRunner << *thatElemRunner << '\n';
                    diff++;
                } else
                    eq++;
                if (!that.isElementKnown(i, j))
                    rev++;
                }
                thatElemRunner++;
                thisElemRunner++;
            }
        }
        std::cout << eq << ' ' << diff << " " << rev << " t1\n";
        return diff = 0;
    }


};

} //namespace corecvs
#endif /* PUNCHEDBUFFEROPERATIONS_H_ */


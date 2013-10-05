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



#include "global.h"

#include "vector2d.h"
#include "tbbWrapper.h"
namespace corecvs {

template<typename ThisTypeName, typename ElementType>
class PunchedBufferOperations
{
public:
    typedef ElementType  InternalElementType;

    bool isElementKnownVec(const Vector2d32 &p)
    {
        ThisTypeName *realThis = (ThisTypeName *)this;
        return realThis->isElementKnown(p.y(), p.x());
    }

    bool isElementKnownVec(const Vector2d16 &p)
    {
        ThisTypeName *realThis = (ThisTypeName *)this;
        return realThis->isElementKnown(p.y(), p.x());
    }

    void setElementUnknownVec(const Vector2d32 &p)
    {
        ThisTypeName *realThis = (ThisTypeName *)this;
        realThis->setElementUnknown(p.y(), p.x());
    }

    void setElementUnknownVec(const Vector2d16 &p)
    {
        ThisTypeName *realThis = (ThisTypeName *)this;
        realThis->setElementUnknown(p.y(), p.x());
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

};

} //namespace corecvs
#endif /* PUNCHEDBUFFEROPERATIONS_H_ */


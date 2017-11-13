#ifndef BUFFER3D_H
#define BUFFER3D_H

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
#include "core/geometry/mesh3d.h"
#include "core/rectification/triangulator.h"
#include "core/buffers/abstractContiniousBuffer.h"
#include "flow/punchedBufferOperations.h"
#include "core/clustering3d/cloud.h"

namespace corecvs {

typedef AbstractContiniousBuffer<SwarmPoint*, int32_t> Buffer3dBase;

class Buffer3d :
    public Buffer3dBase,
    public PunchedBufferOperations<Buffer3dBase, Buffer3dBase::InternalElementType>
{
private:
    /**
     *  Dummy element that will point to the unknown SwarmPoint.
     *  We so far set it to NULL;
     **/
    static SwarmPoint *p0;

    // Check: Who is the owner
    //G12Buffer *mBaseBuffer;
    //vector<SwarmPoint> *mSwarmPoints;
    Cloud *mCloud;

public:
    Buffer3d()
       /* mBaseBuffer(NULL),*/
       // mCloud(NULL)
    {}

    Buffer3d(int h, int w, Cloud *swarmPoints) :
        Buffer3dBase(h, w, p0)
    {
        /*mBaseBuffer = baseBuffer;*/
        addPoints(swarmPoints);
    }

    void addPoints(Cloud *sp)
    {
        mCloud = sp;

        if (mCloud->empty()) {
            return;
        }

        for (size_t i = 0; i < mCloud->size(); i++)
        {
            SwarmPoint *point = &((*mCloud)[i]);

            if (isValidCoord(point->texCoor.y(), point->texCoor.x()))
            {
                element(
                    fround(point->texCoor.y()),
                    fround(point->texCoor.x())) = point;
            }
        }
    }

    /*G12Buffer *getG12Buffer() const
    {
        return mBaseBuffer;
    }*/

    Cloud *getSwarmPoints() const
    {
        return mCloud;
    }

    void setSwarmPoints(Cloud *sp)
    {
        mCloud = sp;
    }



    inline bool isElementKnown(const int32_t y, const int32_t x) const
    {
        const SwarmPoint *a = this->element(y, x);
        return (a != NULL);
    }

    inline bool isElementValidAndKnown(const int32_t y, const int32_t x) const
    {
        return isValidCoord(y,x) && isElementKnown(y,x);
    }


    inline void setElementUnknown(const int32_t y, const int32_t x)
    {
        element(y, x) = NULL;
    }

    bool getNearestPoint(Vector2d32& p) const
    {
        Vector2d32 dir(0, 1);
        int squareSide = 1;

        while(true)
        {
            for (int j = 0; j < 2; j++)
            {
               for (int i = 0; i < squareSide; i++)
               {
                  if (!isValidCoord(p.y(), p.x()))
                     return false;
                  if (element(p) != NULL)
                     return true;
                  p += dir;
               }
               dir = dir.leftNormal();
            }
            squareSide++;
        }
    }

    double getLength(vector<SwarmPoint> &cloud, Vector2d32 p1, Vector2d32 p2);

    double getLength(Vector2d32 p1, Vector2d32 p2) const;

    int pointInBox(const AxisAlignedBox3d &box)
    {
        int count = 0;
        for (int i = 0; i < this->h; i++)
        {
            for (int j = 0; j < this->w; j++)
            {
               if (!this->isElementKnown(i, j))
                   continue;

               if (box.contains(this->element(i,j)->point))
               {
                   count++;
               }

            }
        }
        return count;
    }

    virtual ~Buffer3d()
    {
        delete_safe(mCloud);
    }

}; // Buffer3d

} //namespace corecvs

#endif // BUFFER3D_H

#ifndef TILE_GRID_H_
#define TILE_GRID_H_

/**
 * \file tileGrid.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Apr 11, 2010
 * \author alexander
 */

#include "global.h"

#include "abstractContiniousBuffer.h"
#include "segmentator.h"
#include "histogram.h"
#include "ellipticalApproximation.h"
#include "../math/mathUtils.h"


namespace corecvs {

class ClusteringTile
{
public:
    int depth;
    int selected;
    int nums;
    int roiNums;
};


class SegmentParameters
{
public:
    Vector2dd center;
    Vector2dd deviation;
    Vector2dd meanFlow;
    int pointNumStereo;
    int pointNumFlow;

    int levelLow;
    int levelHigh;


    SegmentParameters()
    {
        memset(this, 0, sizeof(SegmentParameters));
    }

};


/**
 *
 *
 * For information on moments see
 *    http://en.wikipedia.org/wiki/Image_moment
 * */
class SegmentPayload
{
public:
    Histogram *ZValues;
    Histogram *YValues;

    /**
     *   Ellipse that approximates the cloud of stereo points in 2D
     **/
    EllipticalApproximation ellipse;

    /**
     *  Mean flow in the segment area
     **/
    Vector2dd meanFlow;

    Vector2dd meanFlowSource;

    int pointNumStereo;
    int pointNumFlow;

    int levelLow;
    int levelHigh;

    int yLevelLow;
    int yLevelHigh;

    int isGood;

    double rotor;
    double divergence;

    /** The center of the topmost tile of the segment */
    Vector2d16 top;

    SegmentPayload()
    {
        memset(this, 0, sizeof(SegmentPayload));
        top = Vector2d16(-1, -1);
    }

    ~SegmentPayload()
    {
        delete_safe(ZValues);
        delete_safe(YValues);
    }

    void compensateDesync(uint64_t interframeDelay, uint64_t desyncTime)
    {
        double xShiftInMus = meanFlow.x() / interframeDelay;
        double xShiftInDesync = xShiftInMus * desyncTime;

        int intDesync = fround(xShiftInDesync);
        levelHigh += intDesync;
        levelLow  += intDesync;
    }
}; // SegmentPayload


class TileSegment : public BaseSegment<TileSegment>
{
public:
    /*TODO: These members should be moved to a sort of payload*/
    int size;
    int pointNum;
    SegmentPayload *payload;


    TileSegment() :
          size(0)
        , pointNum(0)
        , payload(NULL)
    {}

    SegmentPayload *getPayload()
    {
        if (payload == NULL)
        {
            payload = new SegmentPayload();
        }
        return payload;
    }

    void dadify()
    {
        BaseSegment<TileSegment>::dadify();
        if (father != NULL && father != this)
        {
            father->size += this->size;
            father->pointNum += this->pointNum;
        }
    }

    void addPoint(int /*i*/, int /*j*/, const ClusteringTile &tile)
    {
        this->size++;
        this->pointNum += tile.nums;
    }


    ~TileSegment()
    {
        if (payload != NULL)
            delete payload;
    }

    static bool sortPredicate(TileSegment *a1, TileSegment *a2)
    {
        return a1->pointNum >  a2->pointNum;
    }

}; // TileSegment


class TileSegmentator : public Segmentator<TileSegmentator, TileSegment>
{
public:
    typedef Segmentator<TileSegmentator, TileSegment>::SegmentationResult SegmentationResult;

    static int xZoneSize()
    {
        return 1;
    }

    static int yZoneSize()
    {
        return 1;
    }

    static bool canStartSegment(int /*i*/, int /*j*/, const ClusteringTile &tile)
    {
        return tile.selected;
    }

}; // TileSegmentator


typedef AbstractContiniousBuffer<ClusteringTile> TileGridBase;

class TileGrid : public TileGridBase
{
public:
    TileGrid(int32_t h, int32_t w) : TileGridBase (h,w) {};
    TileGrid(int32_t h, int32_t w, bool shouldInit) : TileGridBase (h,w, shouldInit) {};

    TileGrid(TileGrid &that) : TileGridBase (that) {};
    TileGrid(TileGrid *that) : TileGridBase (that) {};

    TileGrid(TileGrid *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        TileGridBase(src, x1, y1, x2, y2) {};

    TileGrid(int32_t h, int32_t w, ClusteringTile *data) : TileGridBase(h, w, data) {};


    void selectWithThreshold (int threshold, int h = 2,  int w = 2);

};



} //namespace corecvs
#endif /* TILE_GRID_H_ */


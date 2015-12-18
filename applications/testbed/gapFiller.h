#pragma once

/**
 * \file gapFiller.h
 *
 * \date Dec 2, 2013
 **/

class GapPayload
{

};

class GapsSegment : public BaseSegment<GapsSegment>
{
public:
    /*TODO: These members should be moved to a sort of payload*/
    int size;
//    int pointNum;
    GapPayload *payload;


    GapsSegment() :
          size(0)
//        , pointNum(0)
        , payload(NULL)
    {}

    GapPayload *getPayload()
    {
        if (payload == NULL)
        {
            payload = new GapPayload();
        }
        return payload;
    }

    void dadify()
    {
        BaseSegment<GapsSegment>::dadify();
        if (father != NULL && father != this)
        {
            father->size += this->size;
//            father->pointNum += this->pointNum;
        }
    }

    void addPoint(int /*i*/, int /*j*/, const uint8_t &tile)
    {
        this->size++;
//        this->pointNum += tile.nums;
    }


    ~GapsSegment()
    {
        if (payload != NULL)
            delete payload;
    }

    static bool sortPredicate(GapsSegment *a1, GapsSegment *a2)
    {
        return a1->size <  a2->size;
    }

}; // TileSegment


class GapsSegmentator : public Segmentator<GapsSegmentator, GapsSegment>
{
public:
    typedef Segmentator<GapsSegmentator, GapsSegment>::SegmentationResult SegmentationResult;

    static int xZoneSize()
    {
        return 1;
    }

    static int yZoneSize()
    {
        return 1;
    }

    static bool canStartSegment(int /*i*/, int /*j*/, const uint8_t &tile)
    {
        return (tile == 0);
    }
};



/* EOF */

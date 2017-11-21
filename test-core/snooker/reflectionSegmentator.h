/**
 * \file ReflectionSegmentator.h
 * \brief Add Comment Here
 *
 * \date Apr 20, 2011
 * \author alexander
 */

#ifndef REFLECTIONSEGMENTATOR_H_
#define REFLECTIONSEGMENTATOR_H_

#include "core/math/vector/vector2d.h"
#include "core/segmentation/segmentator.h"
#include "core/geometry/ellipticalApproximation.h"

class SnookerPayload;

class ReflectionPayload
{
public:
    Vector2dd hue;
    int saturation;
    EllipticalApproximation approx;
    SnookerPayload *owner;

    ReflectionPayload() :
        hue(0),
        saturation(0),
        owner(NULL)
    { }
};

class ReflectionSegment : public BaseSegment<ReflectionSegment>
{
public:
    ReflectionPayload *payload;

    void addPoint(int /*i*/, int /*j*/, const uint16_t & /*tile*/) {}
};

class ReflectionSegmentator : public Segmentator<ReflectionSegmentator, ReflectionSegment>
{
public:
    int threshold;

    ReflectionSegmentator(int _threshold) :
        threshold(_threshold)
    {}

    static int xZoneSize() { return 1; }

    static int yZoneSize() { return 1; }


    bool canStartSegment(int /*i*/, int /*j*/, const uint16_t &element)
    {
        return element > threshold;
    }

};

#endif /* REFLECTIONSEGMENTATOR_H_ */

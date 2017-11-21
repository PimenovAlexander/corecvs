/**
 * \file SnookerSegmentator.h
 * \brief Add Comment Here
 *
 * \date Apr 20, 2011
 * \author alexander
 */

#ifndef SNOOKERSEGMENTATOR_H_
#define SNOOKERSEGMENTATOR_H_

#include <limits>
#include <vector>

#include "core/buffers/rgb24/rgbColor.h"
#include "core/buffers/histogram/histogram.h"
#include "core/geometry/ellipticalApproximation.h"
#include "core/segmentation/segmentator.h"


class ReflectionPayload;

class SnookerPayload
{
  const static int EXPECTED_REFLECTIONS = 3;

public:
    RGBColor color;
    Histogram *hyst;
    int highPercentile;
    int xMin;
    int xMax;
    int yMin;
    EllipticalApproximation approx;
    std::vector<ReflectionPayload *> reflections;
    bool hasSingleCenter;
    Vector2dd ballCenter;

    SnookerPayload() :
        color(rand() % 255,rand() % 255,rand() % 255),
        hyst(new Histogram(0,255)),
        xMin(std::numeric_limits<int>::max()),
        xMax(0),
        yMin(std::numeric_limits<int>::max()),
        hasSingleCenter(false)
    { 
      reflections.reserve(EXPECTED_REFLECTIONS);
    }

    ~SnookerPayload() {
        delete hyst;
    }
};

class SnookerSegment : public BaseSegment<SnookerSegment>
{
public:
    SnookerPayload *payload;

    void addPoint(int /*i*/, int /*j*/, const uint16_t & /*tile*/) {}
};

class SnookerSegmentator : public Segmentator<SnookerSegmentator, SnookerSegment>
{
public:
    int threshold;

    SnookerSegmentator(int _threshold) :
        threshold(_threshold)
    {}

    static int xZoneSize() { return 1; }

    static int yZoneSize() { return 1; }


    bool canStartSegment(int /*i*/, int /*j*/, const uint16_t &element)
    {
        return element > threshold;
    }

};

#endif /* SNOOKERSEGMENTATOR_H_ */

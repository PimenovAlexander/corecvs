//
// Created by Ivan Kylchik on 24.11.2019.
//

#ifndef CORECVS_INSETOUTSET_H
#define CORECVS_INSETOUTSET_H

#include <iostream>
#include <map>
#include <vector>
#include "core/geometry/polygons.h"

using namespace corecvs;

const double TOLERANCE = 1.0e-11;

bool segmentsIntersect(const Segment2d& first, const Segment2d& second, Vector2dd* intersection) {
    bool intersect;
    *intersection = Segment2d::intersect(first, second, intersect);
    return intersect;
}

bool raysIntersect(const Ray2d& first, const Ray2d& second, Vector2dd* intersection) {
    bool intersect;
    *intersection = Ray2d::intersectionPoint(first, second, &intersect);
    return intersect;
}

Ray2d* inverseRay(Ray2d ray) {
    return new Ray2d (ray.a * (-1), ray.p);
}

/**
 * https://math.stackexchange.com/questions/322831/determing-the-distance-from-a-line-segment-to-a-point-in-3-space
 */
double distanceBetweenSegmentAndPoint(const Segment2d& seg, const Vector2dd& point) {
    Line2d line(seg);
    Vector2dd u = seg.b - seg.a;
    Vector2dd v = point - seg.a;
    double dotProduct = (u & v) / (u.sumAllElementsSq());
    Vector2dd projection = line.projectPointTo(point);
    if (dotProduct >= 0 && dotProduct <= 1) {
        return (projection - point).sumAllElementsSq();
    } else if (dotProduct < 0) {
        return (seg.a - point).sumAllElementsSq();
    } else {
        return (seg.b - point).sumAllElementsSq();
    }
}

double distanceBetweenSegments(const Segment2d& AB, const Segment2d& CD) {
    double AtoCD = distanceBetweenSegmentAndPoint(CD, AB.a);
    double BtoCD = distanceBetweenSegmentAndPoint(CD, AB.b);
    double CtoAB = distanceBetweenSegmentAndPoint(AB, CD.a);
    double DtoAB = distanceBetweenSegmentAndPoint(AB, CD.b);
    return std::min(std::min(AtoCD, BtoCD), std::min(CtoAB, DtoAB));
}

bool intersect(const PointPath& p, const Segment2d& seg, const double tolerance = TOLERANCE) {
    auto it = p.begin();
    for (int i = 0; i < p.size() - 1; ++i, ++it) {
        Vector2dd p1 = *it;
        Vector2dd p2 = *(std::next(it));
        Segment2d pSeg(p1, p2);
        if (distanceBetweenSegments(seg, pSeg) < (tolerance * tolerance) * 0.95) {
            return true;
        }
    }
    return false;
}

std::vector<Vector2dd> getIntersectionPoints(const PointPath& p, const Segment2d& seg) {
    std::vector<Vector2dd> result;
    auto it = p.begin();
    for (int i = 0; i < p.size() - 1; ++i, ++it) {
        Vector2dd p1 = *it;
        Vector2dd p2 = *(std::next(it));
        Segment2d pSeg(p1, p2);
        if (seg.a != pSeg.a && seg.b != pSeg.b) {
            Vector2dd intersection;
            if (segmentsIntersect(seg, pSeg, &intersection)) {
                result.push_back(intersection);
            }
        }
    }
    return result;
}

std::map<double, Vector2dd> getIntersectionPoints(const PointPath& p1, const PointPath& p2) {
    std::map<double, Vector2dd> intersectionPoints;
    double distance = 0.0;
    auto itP1 = p1.begin();
    for (int i = 0; i < p1.size() - 1; ++i, ++itP1) {
        Vector2dd point1 = *itP1;
        Vector2dd point2 = *(std::next(itP1));
        Segment2d seg(point1, point2);
        std::vector<Vector2dd> intersections = getIntersectionPoints(p2, seg);
        auto it = intersections.begin();
        while (it != intersections.end()) {
            Segment2d temp(seg.a, *it);
            intersectionPoints.insert(std::make_pair(distance + temp.getLength(), *it));
            ++it;
        }
        distance += seg.getLength();
    }
    return intersectionPoints;
}

PointPath doOffset(const PointPath& p, int offset) {
    PointPath shifted;
    auto it = p.begin();
    for (int i = 0; i < p.size() - 1; ++i, ++it) {
        Vector2dd p1 = *it;
        Vector2dd p2 = *(std::next(it));
        Vector2dd normal = (p2 - p1).rightNormal().normalised();

        shifted.push_back(p1 + normal * offset);
        shifted.push_back(p2 + normal * offset);
    }
    return shifted;
}

/**
 * https://hal.inria.fr/inria-00518005/document
 */
PointPath alg1(const PointPath& p, bool isClosed) {
    PointPath result;

    //todo if p.size <= 1

    auto it = p.begin();
    int size = isClosed ? p.size() : p.size() - 2;
    if (!isClosed) result.push_back(*it);
    for (int i = 0; i < size; i += 2, it += 2) {
        Vector2dd p1 = *it; //i
        Vector2dd p2 = *(it+1); //i+1
        Vector2dd p3 = (i + 2) % p.size() != 0 ? *(it+2) : *p.begin(); //i+2
        Vector2dd p4 = (i + 2) % p.size() != 0 ? *(it+3) : *(p.begin() + 1); //i+3
        Segment2d firstSeg = Segment2d(p1, p2);
        Segment2d secondSeg = Segment2d(p3, p4);
        Line2d firstLine(firstSeg);
        Line2d secondLine(secondSeg);

        if (firstLine.overlapWith(secondLine)) {
            // case 1: lines overlap
            result.push_back(p3);
        } else {
            //case 2: line have one intersection point
            Vector2dd intersectionPoint;
            if (segmentsIntersect(firstSeg, secondSeg, &intersectionPoint)) {
                // case 2a: intersection point is TIP for both segments
                result.push_back(intersectionPoint);
            } else {
                if (raysIntersect(firstLine.toRay(), secondLine.toRay(), &intersectionPoint) &&
                        !firstSeg.containsPoint(intersectionPoint) && !secondSeg.containsPoint(intersectionPoint)) {
                    // case 2b
                    // for both lines point is on ray, i.e. is FIP
                    // and intersection point lies on first ray, i.e. is PFIP
                    result.push_back(intersectionPoint);
                } else if (raysIntersect(*inverseRay(firstLine.toRay()), secondLine.toRay(), &intersectionPoint) &&
                        !firstSeg.containsPoint(intersectionPoint) && !secondSeg.containsPoint(intersectionPoint)) {
                    // case 2b
                    // intersection point doesn't lay on first ray, i.e. is NFIP
                    result.push_back(p2);
                    result.push_back(p3);
                } else {
                    intersectionPoint = firstLine.intersectWith(secondLine);
                    if ((firstSeg.containsPoint(intersectionPoint) && !secondSeg.containsPoint(intersectionPoint)) ||
                        (secondSeg.containsPoint(intersectionPoint) && !firstSeg.containsPoint(intersectionPoint))) {
                        // case 2c
                        // if intersection point is TIP for first segment and FIP for second
                        // or if intersection point is TIP for second segment and FIP for first
                        result.push_back(p2);
                        result.push_back(p3);
                    }
                }
            }
        }
    }
    // case 3 is not considered
    if (!isClosed) result.push_back(p.back());
    else result.push_back(result.front());
    return result;
}

std::vector<Segment2d> clipping(const PointPath& original, const PointPath& untrimmed, int offset, bool isClosed) {
    std::vector<Segment2d> segments;
    // step 1
    PointPath dual = alg1(doOffset(original, -offset), isClosed);
    std::map<double, Vector2dd> intersectionPoints;
    std::map<double, Vector2dd> intersectionPointsWithOriginal = getIntersectionPoints(untrimmed, original);
    std::map<double, Vector2dd> intersectionPointsWithDual = getIntersectionPoints(untrimmed, dual);
    std::map<double, Vector2dd> selfIntersectionPoints = getIntersectionPoints(untrimmed, untrimmed);

    intersectionPoints.insert(intersectionPointsWithOriginal.begin(), intersectionPointsWithOriginal.end());
    intersectionPoints.insert(intersectionPointsWithDual.begin(), intersectionPointsWithDual.end());
    intersectionPoints.insert(selfIntersectionPoints.begin(), selfIntersectionPoints.end());
    intersectionPoints.insert(std::make_pair(0, untrimmed.front()));
    if (!isClosed) intersectionPoints.insert(std::make_pair(std::numeric_limits<double>::max(), untrimmed.back()));

    // case 1 is impossible because i consider connecting points as intersecting
    // case 2
    auto it = intersectionPoints.begin();
    for (int i = 0; i < intersectionPoints.size() - 1; ++i, ++it) {
        Vector2dd first = it->second;
        Vector2dd second = std::next(it, 1)->second;
        segments.emplace_back(first, second);
    }

    auto itSeg = segments.begin();
    while (itSeg != segments.end()) {
        Segment2d seg = *itSeg;
        // intersect is checking both case 2 and step 2
        if (intersect(original, seg, abs(offset))) {
            segments.erase(itSeg);
        } else {
            ++itSeg;
        }
    }

    return segments;
}

/**
 * Produces a shift for a given offset
 *
 * @param p - point path to shift
 * @param offset - offset size: positive if outset, negative if inset
 * @param isClosed - true if p is polygon, false if p is point path
 * @return shifted vector of segments
 */
/* private */
std::vector<Segment2d> shift(const PointPath& p, int offset, bool isClosed) {
    PointPath shifted = doOffset(p, offset);
    PointPath untrimmed = alg1(shifted, isClosed);

    return clipping(p, untrimmed, offset, isClosed);
}

/**
 * Produces a point path shift for a given offset
 *
 * @param p - point path to shift
 * @param offset - offset size: positive if outset, negative if inset
 * @return shifted vector of segments
 */
std::vector<Segment2d> shiftPointPath(const PointPath& p, int offset) {
    return shift(p, offset, false);
}

/**
 * Produces a polygon shift for a given offset
 *
 * @param p - polygon to shift
 * @param offset - offset size: positive if outset, negative if inset
 * @return shifted vector of segments
 */
std::vector<Segment2d> shiftPolygon(const Polygon& p, int offset) {
    PointPath closed(p);
    closed.push_back(p.getPoint(0));
    return shift(closed, offset, true);
}

#endif //CORECVS_INSETOUTSET_H

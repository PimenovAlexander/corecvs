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

bool linesOverlap(const Line2d& first, const Line2d& second) {
    return first.normal().normalised() == second.normal().normalised();
}

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

double cross(const Vector2dd &a, const Vector2dd &b)
{
    return a.x() * b.y() - a.y() * b.x();
}

/**
 * https://www.lucidar.me/en/mathematics/check-if-a-point-belongs-on-a-line-segment/
 */
bool pointLiesOnSeg(const Segment2d& seg, const Vector2dd& point) {
    Vector2dd ab = seg.b - seg.a;
    Vector2dd ac = point - seg.a;
    if (abs(cross(ab, ac)) >= TOLERANCE) {
        return false;
    }

    double KAC = ab & ac;
    if (KAC < 0) return false;
    if (KAC <= TOLERANCE) return true;

    double KAB = ab.l2Metric() * ab.l2Metric();
    if (KAC > KAB) return false;
    if (abs(KAC - KAB) < TOLERANCE) return true;

    return true;
}

/**
 * https://math.stackexchange.com/questions/322831/determing-the-distance-from-a-line-segment-to-a-point-in-3-space
 */
double distanceBetweenSegmentAndPoint(const Segment2d& seg, const Vector2dd& point) {
    Line2d line(seg);
    Vector2dd u = seg.b - seg.a;
    Vector2dd v = point - seg.a;
    double dotProduct = (u & v) / (u.sumAllElementsSq());
    if (dotProduct >= 0 || dotProduct <= 1) {
        return line.distanceTo(point);
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

bool intersect(const Polygon& p, const Segment2d& seg) {
    for (int i = 0; i < p.size(); ++i) {
        Segment2d pSeg = p.getSegment(i);
        if (distanceBetweenSegments(seg, pSeg) <= TOLERANCE) {
            return true;
        }
    }
    return false;
}

std::vector<Vector2dd> getIntersectionPoints(const Polygon& p, const Segment2d& seg) {
    std::vector<Vector2dd> result;
    for (int i = 0; i < p.size(); ++i) {
        Segment2d pSeg = p.getSegment(i);
        if (seg.a != pSeg.a && seg.b != pSeg.b) {
            Vector2dd intersection;
            if (segmentsIntersect(seg, pSeg, &intersection)) {
                result.push_back(intersection);
            }
        }
    }
    return result;
}

std::map<double, Vector2dd> getIntersectionPoints(const Polygon& p1, const Polygon& p2) {
    std::map<double, Vector2dd> intersectionPoints;
    double distance = 0.0;
    for (int i = 0; i < p1.size(); ++i) {
        Segment2d seg = p1.getSegment(i);
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

Polygon doOffset(const Polygon& p, int offset) {
    Polygon shifted;
    for (int i = 0; i < p.size(); ++i) {
        Vector2dd p1 = p.getPoint(i);
        Vector2dd p2 = p.getNextPoint(i);
        Vector2dd normal = p.getNormal(i).normalised();

        shifted.push_back(p1 + normal * offset);
        shifted.push_back(p2 + normal * offset);
    }
    return shifted;
}

/**
 * https://hal.inria.fr/inria-00518005/document
 */
Polygon alg1(const Polygon& p) {
    Polygon result;

    for (int i = 0; i < p.size(); i += 2) {
        Line2d firstLine = p.getLine(i);
        Line2d secondLine = p.getLine((i + 2) % p.size());
        Segment2d firstSeg = p.getSegment(i);
        Segment2d secondSeg = p.getSegment((i + 2) % p.size());

        if (linesOverlap(firstLine, secondLine)) {
            // case 1: lines overlapping
            cout << "case 1 \n";
            result.push_back(p.getPoint(i + 2));
        } else {
            //case 2: line have one intersection point
            Vector2dd intersectionPoint;
            if (segmentsIntersect(firstSeg, secondSeg, &intersectionPoint)) {
                // case 2a: intersection point is TIP for both segments
                cout << "case 2a \n";
                result.push_back(intersectionPoint);
            } else {
                if (raysIntersect(firstLine.toRay(), secondLine.toRay(), &intersectionPoint) &&
                        !pointLiesOnSeg(firstSeg, intersectionPoint) && !pointLiesOnSeg(secondSeg, intersectionPoint)) {
                    // case 2b
                    cout << "case 2b 1 \n";
                    // for both lines point is on ray, i.e. is FIP
                    // and intersection point lies on first ray, i.e. is PFIP
                    result.push_back(intersectionPoint);
                } else if (raysIntersect(*inverseRay(firstLine.toRay()), secondLine.toRay(), &intersectionPoint) &&
                        !pointLiesOnSeg(firstSeg, intersectionPoint) && !pointLiesOnSeg(secondSeg, intersectionPoint)) {
                    // case 2b
                    cout << "case 2b 2 \n";
                    // intersection point doesn't lay on first ray, i.e. is NFIP
                    result.push_back(p.getPoint(i + 1));
                    result.push_back(p.getPoint(i + 2));
                } else {
                    intersectionPoint = firstLine.intersectWith(secondLine);
                    if ((pointLiesOnSeg(firstSeg, intersectionPoint) && !pointLiesOnSeg(secondSeg, intersectionPoint)) ||
                        (pointLiesOnSeg(secondSeg, intersectionPoint) && !pointLiesOnSeg(firstSeg, intersectionPoint))) {
                        // case 2c
                        cout << "case 2c \n";
                        // if intersection point is TIP for first segment and FIP for second
                        // or if intersection point is TIP for second segment and FIP for first
                        result.push_back(p.getPoint(i + 1));
                        result.push_back(p.getPoint(i + 2));
                    }
                }
            }
        }
    }
    // case 3 is not considered because polygon is closed polyline
    return result;
}

std::vector<Segment2d> clipping(const Polygon& original, const Polygon& untrimmed, int offset) {
    std::vector<Segment2d> segments;
    // step 1
    Polygon dual = alg1(doOffset(original, -offset));
    std::map<double, Vector2dd> intersectionPoints;
    std::map<double, Vector2dd> intersectionPointsWithOriginal = getIntersectionPoints(untrimmed, original);
    std::map<double, Vector2dd> intersectionPointsWithDual = getIntersectionPoints(untrimmed, dual);
    std::map<double, Vector2dd> selfIntersectionPoints = getIntersectionPoints(untrimmed, untrimmed);

    intersectionPoints.insert(intersectionPointsWithOriginal.begin(), intersectionPointsWithOriginal.end());
    intersectionPoints.insert(intersectionPointsWithDual.begin(), intersectionPointsWithDual.end());
    intersectionPoints.insert(selfIntersectionPoints.begin(), selfIntersectionPoints.end());

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
        if (intersect(original, seg)) {
            segments.erase(itSeg);
        } else {
            ++itSeg;
        }
    }

    return segments;
}

/**
 *
 * @param p
 * @param offset positive if outset, negative if inset
 */
std::vector<Segment2d> shiftPolygon(const Polygon& p, int offset) {
    Polygon shifted = doOffset(p, offset);
    Polygon untrimmed = alg1(shifted);

    return clipping(p, untrimmed, offset);
}

#endif //CORECVS_INSETOUTSET_H

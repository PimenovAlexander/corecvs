//
// Created by Ivan Kylchik on 24.11.2019.
//

#ifndef CORECVS_INSETOUTSET_H
#define CORECVS_INSETOUTSET_H

#include <iostream>
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
    result.push_back(p.getPoint(0));

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
    return result;
}

/**
 *
 * @param p
 * @param offset positive if outset, negative if inset
 */
Polygon shiftPolygon(const Polygon& p, int offset) {
    Polygon shifted = doOffset(p, offset);

    return alg1(shifted);
}

#endif //CORECVS_INSETOUTSET_H

#ifndef BEZIERRASTERIZER_H
#define BEZIERRASTERIZER_H

#include "math/vector/vector2d.h"

#include <utility>
#include <iostream>
#include <cmath>
#include <tuple>
#include <stack>
#include <memory>

namespace corecvs {


#if DEPRICATED
struct Point {
    double x;
    double y;

    bool operator==(const Point &p) const {
        return x == p.x && y == p.y;
    }
};
#endif

struct Segment1d {
    double left;
    double right;

    inline bool is_intersect(const Segment1d &s) const {
        return !(((left < s.left) && right < s.left) || ((s.left < left) && (s.right < left)));
    }

    inline bool inside_of(const Segment1d &s) const {
        return left >= s.left && left <= s.right && right <= s.right;
    }

    inline bool left_intersect_of(const Segment1d &s) const {
        return left < s.left && right > s.left && right < s.right;
    }

    inline bool right_intersect_of(const Segment1d &s) const  {
        return right > s.right && left > s.left && left < s.right;
    }
};

/**
 *  Class that would store Cubic Bézier curve
 *
 *
 *  \f[ {B} (t) = (1-t)^{3}     \mathbf P_0
 *             + 3(1-t)^{2}t    \mathbf P_1
 *             + 3(1-t)    t^{2}\mathbf P_2
 *             +           t^{3}\mathbf P_3 \f]
 *
 *
 *
 **/
struct Curve {
    Vector2dd p1;
    Vector2dd p2;
    Vector2dd p3;
    Vector2dd p4;

    /**
     * Compute Vector2dd on cubic bezier curve given value 0<=t<=1
     **/
    inline Vector2dd getEvaluatedPoint(double t) const {
        double t1 = t;
        double m1 = 1 - t;
        double t2 = t1 * t1;
        double m2 = m1 * m1;
        double t3 = t2 * t1;
        double m3 = m2 * m1;

        return       m3      * p1
               + 3 * m2 * t1 * p2
               + 3 * m1 * t2 * p3
                        + t3 * p4;
    }

    /*
     * determine weather curve is flat algo by Roger Willcocks
     * */
    inline bool isCurveFlat(double flatnessCriterion) const {

        Vector2dd a = 3 * p2 - 2 * p1 -     p4;
        Vector2dd b = 3 * p3 -     p1 - 2 * p4;

        Vector2dd a2 = a * a; /* per component product */
        Vector2dd b2 = b * b;

        return ((std::max(a2.x(), b2.x()) + std::max(a2.y(), b2.y())) < flatnessCriterion);
    }


    inline std::tuple<Curve, Curve> splitByHalfT() const {
        // split on two cubic curves
        Vector2dd mid_p_12 = (p1 + p2) / 2.0;
        Vector2dd mid_p_23 = (p2 + p3) / 2.0;
        Vector2dd mid_p_34 = (p3 + p4) / 2.0;

        Vector2dd mid_p_123  = (mid_p_12  + mid_p_23 ) / 2.0;
        Vector2dd mid_p_234  = (mid_p_23  + mid_p_34 ) / 2.0;
        Vector2dd mid_p_1234 = (mid_p_123 + mid_p_234) / 2.0;

        Curve curve1 = {p1, mid_p_12, mid_p_123, mid_p_1234};
        Curve curve2 = {mid_p_1234, mid_p_234, mid_p_34, p4};

        return std::make_tuple(curve1, curve2);
    }

    /*
     * split by t on two cubic curves
     * */
    inline std::tuple<Curve, Curve> splitByArbitraryT(double t) const {
        // split on two cubic curves
        Vector2dd p_12  = p1 + (p2 - p1) * t;
        Vector2dd p_23  = p2 + (p3 - p2) * t;
        Vector2dd p_34  = p3 + (p4 - p3) * t;
        Vector2dd p_123 = p_12 + (p_23 - p_12) * t;

        Vector2dd p_234 =  p_23  + (p_34  - p_23 ) * t;
        Vector2dd p_1234 = p_123 + (p_234 - p_123) * t;

        Curve curve1 = {p1, p_12, p_123, p_1234};
        Curve curve2 = {p_1234, p_234, p_34, p4};
        return std::make_tuple(curve1, curve2);
    }

    /*
     * split by flatness to linear segment and second segment is further reduced
     * return evaluated parameter t', on which must be split
     */
    inline double splitByFlatness(double flatness, double maximum) const {

        double safeStep = 0.005;
        double directVectorLen = (p2 - p1).l2Metric();

        Vector2dd dp21 = p2 - p1;
        Vector2dd dp31 = p3 - p1;

        if (p3 == p1 || p2 == p1) {
            // take care of cases when modulos is zero
            return std::min(maximum, safeStep);
        }

        /* p3_in_new_coordinate */
        Vector2dd p3nc = Vector2dd(
                ((dp31.x()) * (dp21.x()) + (dp31.y()) * (dp21.y())),
                ((dp31.x()) * (dp21.y()) - (dp31.y()) * (dp21.x()))
        ) / directVectorLen;
        return std::min(maximum, 2 * sqrt(flatness / (3 * abs(p3nc.y()))));
    }

    /**
     * Utility functions
     **/

    friend std::ostream& operator << (std::ostream &out, const Curve &toSave)
    {
        out << toSave.p1 << " "
            << toSave.p2 << " "
            << toSave.p3 << " "
            << toSave.p4 << " ";
        return out;
    }

};

template<class BufferType,class Rasterizer>
class BezierRasterizer {

public:
    BufferType &buffer;
    Rasterizer &rasterizer;
    typename BufferType::InternalElementType color;

    BezierRasterizer(BufferType &buffer, Rasterizer &rasterizer, const typename BufferType::InternalElementType &color)
            : buffer(buffer),rasterizer(rasterizer), color(color) {}



    static double cross(const Vector2dd &a, const Vector2dd &b)
    {
        return a.x() * b.y() - a.y() * b.x();
    }


    /*
     * dummy implementation of cubicBezier
     * used incremental method with static step
     * use for testing
     */
    void cubicBezierDummy(const Curve &p) {
        const double step = 0.001;
        double t = 0;
        Vector2dd left = p.getEvaluatedPoint(0);
        Vector2dd right;
        while (t <= 1) {
            right = p.getEvaluatedPoint(t);
            drawLine(left, right);
            left = right;
            t += step;
        }
    }


    /*
     * De Casteljau divide and conquer algo + flatness criterion by Roger Willcocks
     * detail -  https://jeremykun.com/2013/05/11/bezier-curves-and-picasso/
     */

    void cubicBezierCasteljauApproximationByFlatness(Curve p, double flatnessConstant = 0.5) {

        // since we know that sqrt(1/16*(max(ax^2,bx^2)+max(ay^2,by^2)<=dist
        // we could optimize evaluation
        double flatness = flatnessConstant * flatnessConstant * 16;

        // emulate recursion calls
        std::stack<Curve> callStack;

        callStack.push(p);
        while (true) {

            Curve curve = callStack.top();
            if (curve.isCurveFlat(flatness)) {
                Vector2dd &p1 = curve.p1;
                Vector2dd &p4 = curve.p4;

                if (callStack.size() == 1) {
                    drawLine(p1, p4);
                    return;
                }
                drawLine(p1, p4);
                callStack.pop();
            } else {
                callStack.pop();
                Curve curve1, curve2;

                std::tie(curve1, curve2) = curve.splitByHalfT();
                callStack.push(curve2);
                callStack.push(curve1);

            }
        }
    }

    /*
     * algo detail https://pdfs.semanticscholar.org/8963/c06a92d6ca8868348b0930bbb800ff6e7920.pdf
     * use math to optimize curve approximation
     */
    void cubicBezierCasteljauArticle(Curve p, double flatnessConstant = 0.0005) {

        if ((p.p1 == p.p2 || p.p1 == p.p3)) {
            circular_approximation(p,0,1,flatnessConstant);
            return;
        }

        if (p.p2 == p.p4) {
            //handle bad case when p2==p4
            circular_approximation(p, 0, 1, flatnessConstant);
            return;
        }

        // for start we need find up to 2 inflection points(cubic parametric curve have at most 2 such Points) t1 and t2 t1<t2
        //[t1-, t1+]  t1-<t1<t1+ is section surround inflection Vector2dd could be approximated by line  if flatness criterion ok
        // similar to [t2-,t2+]
        // we have [t1-,t1+], [t2-,t2+] , other segment will be approximated by circle approximation

        // if we define x(t) and y(t) as x(t) = a*t^3 + b*t^2 + c*t + d then coefficients would be as follows:
        Vector2dd A = -p.p1   + 3 * p.p2   - 3 * p.p3   + p.p4;
        Vector2dd B =  3 * p.p1   - 6 * p.p2   + 3 * p.p3;
        Vector2dd C = -3 * p.p1   + 3 * p.p2;
        //Vector2dd D = p.p1;

        // y coordinate of last point  in new system
        double directVectorLen = (p.p2 - p.p1).l2Metric();
        double s4 = cross(p.p4 - p.p1, p.p2 - p.p1) / directVectorLen;
        // at inflection Vector2dd  only derivative of acceleration has component perpendicular  to velocity vector =>
        // s(t) =  t^3 * s4 , i.e to achieve desired flatness we set s(t) = flatness constant =>>  t^3 = flatness/s4

        if (s4 == 0) {
            // i.e tf will be large => no inflection Vector2dd in [0,1]
            circular_approximation(p, 0, 1, flatnessConstant);
            return;
        }

        double tf = cbrt(flatnessConstant / s4);

        //at inflection Points component of acceleration = 0 =>
        // cross product of acceleration *velocity = 0
        // solve quadratic equation for t

        double crossBA = cross(B,A);
        double t_cusp = -0.5 * cross(C,A) / crossBA;
        double t_cusp2 = t_cusp * t_cusp;
        double discriminant = t_cusp2 - cross(C, B) / crossBA / 3;


        if (discriminant < 0 || crossBA == 0) {
            //no inflexion points
            circular_approximation(p, 0, 1, flatnessConstant);
            return;
        }

        if (discriminant == 0) {
            // only one inflection Point
            double t = t_cusp;
            double t1_left  = t - (1 - t) * tf;
            double t1_right = t + (1 - t) * tf;
            Segment1d center = {t1_left, t1_right};
            Segment1d segment = {0, 1};
            if (center.inside_of(segment)) {
                circular_approximation(p, 0, center.left, flatnessConstant);
                lineApproximation(p, center.left, center.right, flatnessConstant);
                circular_approximation(p, center.right, 1, flatnessConstant);
            } else if (center.left_intersect_of(segment)) {
                lineApproximation(p, 0, center.right, flatnessConstant);
                circular_approximation(p, center.right, 1, flatnessConstant);
            } else {
                circular_approximation(p, 0, center.left, flatnessConstant);
                lineApproximation(p, center.left, 1, flatnessConstant);
            }

            return;

        }
        // now we have two inflection Points

        double t1 = t_cusp - sqrt(discriminant);
        double t2 = t_cusp + sqrt(discriminant);
        if (t1 > t2) std::swap(t1, t2);

        // [-tf,0] and [0,tf] < flatness and we can merge them to one segment and then transform to parametric range

        double t1_left  = t1 - (1 - t1) * tf;
        double t1_right = t1 + (1 - t1) * tf;

        double t2_left  = t2 - (1 - t2) * tf;
        double t2_right = t2 + (1 - t2) * tf;

        Segment1d left = {t1_left, t1_right};
        Segment1d right = {t2_left, t2_right};
        Segment1d segment = {0.0, 1.0};

        if ((!left.is_intersect(segment)) && (!right.is_intersect(segment))) {
            // left and right out of [0,1]
            circular_approximation(p, 0, 1.0, flatnessConstant);
            return;
        }

        if (left.is_intersect(right)) {
            // left intersect right

            if (left.is_intersect(segment) && (!right.is_intersect(segment))) {

                if (left.inside_of(segment)) {
                    // right out of [0,1] and left inside  of [0,1]
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    lineApproximation(p, left.left, left.right, flatnessConstant);
                    circular_approximation(p, left.right, 1, flatnessConstant);

                } else {
                    // right out of [0,1] and left on right edge  of [0,1]
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    lineApproximation(p, left.left, 1, flatnessConstant);

                }

            } else if (right.is_intersect(segment) && (!left.is_intersect(segment))) {

                if (right.inside_of(segment)) {
                    // left out of [0,1] and right inside of [0,1]
                    circular_approximation(p, 0, right.left, flatnessConstant);
                    lineApproximation(p, right.left, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);

                } else {
                    // left out of [0,1] and right on left edge of [0,1]
                    lineApproximation(p, 0, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);
                }

            } else {

                if (left.inside_of(segment) && right.inside_of(segment)) {
                    // both inside of [0,1]
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    lineApproximation(p, left.left, t_cusp, flatnessConstant);
                    lineApproximation(p, t_cusp, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);

                } else if (left.left_intersect_of(segment) && right.left_intersect_of(segment)) {
                    // both on edge left
                    if (t_cusp > 0) {
                        lineApproximation(p, 0, t_cusp, flatnessConstant);
                        lineApproximation(p, t_cusp, right.right, flatnessConstant);
                    } else lineApproximation(p, 0, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);

                } else if (left.right_intersect_of(segment) && right.right_intersect_of(segment)) {
                    //both on edge right
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    if (t_cusp < 1) {
                        lineApproximation(p, left.left, t_cusp, flatnessConstant);
                        lineApproximation(p, t_cusp, 1, flatnessConstant);
                    } else lineApproximation(p, left.left, 1, flatnessConstant);

                } else if (left.left_intersect_of(segment) && right.inside_of(segment)) {
                    //left on edge  right inside
                    if (t_cusp > 0) {
                        lineApproximation(p, 0, t_cusp, flatnessConstant);
                        lineApproximation(p, t_cusp, right.right, flatnessConstant);
                    } else lineApproximation(p, 0, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);

                } else if (left.inside_of(segment) && right.right_intersect_of(segment)) {
                    // left inside right on edge
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    if (t_cusp < 1) {
                        lineApproximation(p, left.left, t_cusp, flatnessConstant);
                        lineApproximation(p, t_cusp, 1, flatnessConstant);
                    } else lineApproximation(p, left.left, 1, flatnessConstant);

                } else {
                    circular_approximation(p, 0, 1, flatnessConstant);

                }

            }

        } else {
            //case when left right disjoint
            if (left.is_intersect(segment) && (!right.is_intersect(segment))) {
                subroutine_if(p, left, flatnessConstant);

            } else if ((!left.is_intersect(segment)) && (right.is_intersect(segment))) {
                subroutine_if(p, right, flatnessConstant);

            } else {

                if (left.inside_of(segment) && right.inside_of(segment)) {
                    // left and right inside [0,1]
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    lineApproximation(p, left.left, left.right, flatnessConstant);
                    circular_approximation(p, left.right, right.left, flatnessConstant);
                    lineApproximation(p, right.left, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);

                } else if (left.left_intersect_of(segment) && right.left_intersect_of(segment)) {
                    // left and right on left edge of [0,1]
                    //impossible because would be intesection between left and right

                } else if (left.right_intersect_of(segment) && right.right_intersect_of(segment)) {
                    // left and right on right edge of [0,1]
                    //impossible because would be intesection between left and right

                } else if (left.inside_of(segment) && right.right_intersect_of(segment)) {
                    // left inside and right on edge
                    circular_approximation(p, 0, left.left, flatnessConstant);
                    lineApproximation(p, left.left, left.right, flatnessConstant);
                    circular_approximation(p, left.right, right.left, flatnessConstant);
                    lineApproximation(p, right.left, 1, flatnessConstant);

                } else if (left.left_intersect_of(segment) && right.inside_of(segment)) {
                    //left on edge and right inside
                    lineApproximation(p, 0, left.right, flatnessConstant);
                    circular_approximation(p, left.right, right.left, flatnessConstant);
                    lineApproximation(p, right.left, right.right, flatnessConstant);
                    circular_approximation(p, right.right, 1, flatnessConstant);

                } else if (left.left_intersect_of(segment) && right.right_intersect_of(segment)) {
                    // both on edge
                    lineApproximation(p, 0, left.right, flatnessConstant);
                    circular_approximation(p, left.right, right.left, flatnessConstant);
                    lineApproximation(p, right.left, 1, flatnessConstant);
                } else {
                    circular_approximation(p, 0, 1, flatnessConstant);
                }

            }

        }

    }

private:

    void drawLine(double x1, double y1, double x4, double y4) {
       rasterizer.drawLine(buffer,round(x1), round(y1), round(x4), round(y4),color);
    }

    void drawLine(const Vector2dd &p1, const Vector2dd &p4) {
       rasterizer.drawLine(buffer,round(p1.x()), round(p1.y()), round(p4.x()), round(p4.y()),color);
    }

    inline void circular_approximation(Curve p, double start, double end, double flatness) {
        if (end > 1) end = 1;
        if (start < 0) start = 0;

        Curve initialCurve = p;

        double t = start;
        Curve curCurve = p;
        Vector2dd leftPoint = initialCurve.getEvaluatedPoint(t);
        Vector2dd rightPoint;
        Curve unused;
        while (t < end) {
            // we get t that attached to new curve not initial curve
            double t_new_coordinate = curCurve.splitByFlatness(flatness, 1.0);
            t += t_new_coordinate;
            t = std::min(t, end);

            rightPoint = initialCurve.getEvaluatedPoint(t);
            drawLine(leftPoint, rightPoint);
            leftPoint = rightPoint;

            std::tie(unused, curCurve) = initialCurve.splitByArbitraryT(t);


        }
    }


    inline void subroutine_if(Curve curve, Segment1d &seg, double flatnessConstant) {
        Segment1d segment = {0.0, 1.0};// our [0,1];
        if (seg.left_intersect_of(segment)) {
            // seg on left edge of [0,1]
            lineApproximation(curve, 0.0, seg.right, flatnessConstant);
            circular_approximation(curve, seg.right, 1.0, flatnessConstant);
        } else if (seg.inside_of(segment)) {
            // seg inside [0,1]
            circular_approximation(curve, 0, seg.left, flatnessConstant);
            lineApproximation(curve, seg.left, seg.right, flatnessConstant);
            circular_approximation(curve, seg.right, 1.0, flatnessConstant);
        } else {
            // seg on right edge of [0,1]
            circular_approximation(curve, 0, seg.left, flatnessConstant);
            lineApproximation(curve, seg.left, 1, flatnessConstant);
        }

    }

    inline void lineApproximation(Curve curve, double t_min, double t_max, double /*flatness*/) {

        if (t_min >= t_max || t_min < 0 || t_max > 1) {
            //already draw line
            return;
        }

        Vector2dd a = curve.getEvaluatedPoint(t_min);
        Vector2dd b = curve.getEvaluatedPoint(t_max);
        drawLine(a, b);
    }

};

} // namespace corecvs


#endif // BEZIERRASTERIZER_H

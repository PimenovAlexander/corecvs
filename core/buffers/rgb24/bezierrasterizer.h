#ifndef BEZIERRASTERIZER_H
#define BEZIERRASTERIZER_H



#include <utility>
#include <iostream>
#include <cmath>
#include <tuple>
#include <stack>
#include <memory>

struct Point {
    double x;
    double y;

    bool operator==(const Point &p) const {
        return x == p.x && y == p.y;
    }
};

struct Segment {
    double left;
    double right;

    inline bool is_intersect(Segment &s) {
        return !(((left < s.left) && right < s.left) || ((s.left < left) && (s.right < left)));
    }

    inline bool inside_of(Segment &s) {
        return left >= s.left && left <= s.right && right <= s.right;
    }

    inline bool left_intersect_of(Segment &s) {
        return left < s.left && right > s.left && right < s.right;
    }

    inline bool right_intersect_of(Segment &s) {
        return right > s.right && left > s.left && left < s.right;
    }
};

struct Curve {
    Point p1;
    Point p2;
    Point p3;
    Point p4;

    /*
     * compute point on cubic bezier curve given value 0<=t<=1
     */
    inline Point get_evaluated_point(double t) {
        double m3 = (1 - t) * (1 - t) * (1 - t);
        double t3 = t * t * t;
        double m2 = (1 - t) * (1 - t);
        double t2 = t * t;
        double t1 = t;
        double m1 = 1 - t;
        return {
                (p1.x * m3 + 3 * m2 * t1 * p2.x + 3 * t2 * m1 * p3.x + t3 * p4.x),
                (p1.y * m3 + 3 * m2 * t1 * p2.y + 3 * t2 * m1 * p3.y + t3 * p4.y)
        };
    }

    /*
     * determine weather curve is flat algo by Roger Willcocks
     * */
    inline bool is_curve_flat(double flatness_criterion) {

        double ax2 = pow(3 * p2.x - 2 * p1.x - p4.x, 2);
        double ay2 = pow(3 * p2.y - 2 * p1.y - p4.y, 2);
        double bx2 = pow(3 * p3.x - p1.x - 2 * p4.x, 2);
        double by2 = pow(3 * p3.y - p1.y - 2 * p4.y, 2);

        return ((std::max(ax2, bx2) + std::max(ay2, by2)) < flatness_criterion);

    }


    inline std::tuple<Curve, Curve> splitByHalfT() {
        // split on two cubic curves
        Point mid_p_12 = {(p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0};
        Point mid_p_23 = {(p2.x + p3.x) / 2.0, (p2.y + p3.y) / 2.0};
        Point mid_p_34 = {(p3.x + p4.x) / 2.0, (p3.y + p4.y) / 2.0};
        Point mid_p_123 = {(mid_p_12.x + mid_p_23.x) / 2.0, (mid_p_12.y + mid_p_23.y) / 2.0};
        Point mid_p_234 = {(mid_p_23.x + mid_p_34.x) / 2.0, (mid_p_23.y + mid_p_34.y) / 2.0};
        Point mid_p_1234 = {(mid_p_123.x + mid_p_234.x) / 2.0, (mid_p_123.y + mid_p_234.y) / 2.0};

        Curve point1 = {p1, mid_p_12, mid_p_123, mid_p_1234};
        Curve point2 = {mid_p_1234, mid_p_234, mid_p_34, p4};

        return std::make_tuple(point1, point2);
    }

    /*
     * split by t on two cubic curves
     * */
    inline std::tuple<Curve, Curve> splitByArbitraryT(double t) {
        // split on two cubic curves
        Point p_12 = {(p1.x + (p2.x - p1.x)) * t, p1.y + (p2.y - p1.y) * t};
        Point p_23 = {p2.x + (p3.x - p2.x) * t, p2.y + (p3.y - p2.y) * t};
        Point p_34 = {p3.x + (p4.x - p3.x) * t, p3.y + (p4.y - p3.y) * t};
        Point p_123 = {p_12.x + (p_23.x - p_12.x) * t, p_12.y + (p_23.y - p_12.y) * t};

        Point p_234 = {p_23.x + (p_34.x - p_23.x) * t, p_23.y + (p_34.y - p_23.y) * t};
        Point p_1234 = {p_123.x + (p_234.x - p_123.x) * t, p_123.y + (p_234.y - p_123.y) * t};

        Curve point1 = {p1, p_12, p_123, p_1234};
        Curve point2 = {p_1234, p_234, p_34, p4};
        return std::make_tuple(point1, point2);
    }

    /*
     * split by flatness to linear segment and second segment is further reduced
     * return evaluated parameter t', on which must be split
     */
    inline double splitByFlatness(double flatness, double maximum) {

        double safeStep = 0.005;
        double direct_vector_modulo = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));

        if (p3 == p1 || p2 == p1) {
            // take care of cases when modulos is zero
            return std::min(maximum, safeStep);
        }
        Point p3_in_new_coordinate = {
                ((p3.x - p1.x) * (p2.x - p1.x) + (p3.y - p1.y) * (p2.y - p1.y)) / direct_vector_modulo,
                ((p3.x - p1.x) * (p2.y - p1.y) - (p3.y - p1.y) * (p2.x - p1.x)) / direct_vector_modulo,
        };
        double p3_in_new_coordinate_modulo = sqrt(pow(p3_in_new_coordinate.x, 2) + pow(p3_in_new_coordinate.y, 2));
        return std::min(maximum, 2 * sqrt(flatness / (3 * p3_in_new_coordinate_modulo)));

    }

};

template<class BufferType,class Rasterizer>
class BezierRasterizer {

public:
    BufferType &buffer;
    Rasterizer &rasterizer;
    typename BufferType::InternalElementType color;

    BezierRasterizer(BufferType &buffer,Rasterizer &rasterizer,const typename BufferType::InternalElementType &color)
            : buffer(buffer),rasterizer(rasterizer), color(color) {}




    /*
     * dummy implementation of cubicBezier
     * used incremental method with static step
     * use for testing
     */
    void cubicBezierDummy(Curve p) {
        const double step = 0.001;
        double t = 0;
        Point left = p.get_evaluated_point(0);
        Point right;
        while (t <= 1) {

            double m3 = (1 - t) * (1 - t) * (1 - t);
            double t3 = t * t * t;
            double m2 = (1 - t) * (1 - t);
            double t2 = t * t;
            double t1 = t;
            double m1 = 1 - t;

            right = {p.p1.x * m3 + 3 * m2 * t1 * p.p2.x + 3 * t2 * m1 * p.p3.x + t3 * p.p4.x,
                     p.p1.y * m3 + 3 * m2 * t1 * p.p2.y + 3 * t2 * m1 * p.p3.y + t3 * p.p4.y};

            drawLine(left.x, left.y, right.x, right.y);
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
        std::unique_ptr<std::stack<Curve>> callStack(new std::stack<Curve>());

        callStack->push(p);
        while (true) {

            auto curve = callStack->top();
            if (curve.is_curve_flat(flatness)) {
                auto p1 = curve.p1;
                auto p4 = curve.p4;
                if (callStack->size() == 1) {
                    drawLine(p1.x, p1.y, p4.x, p4.y);
                    return;
                }

                drawLine(p1.x, p1.y, p4.x, p4.y);
                callStack->pop();

            } else {
                callStack->pop();
                Curve curve1, curve2;

                std::tie(curve1, curve2) = curve.splitByHalfT();
                callStack->push(curve2);
                callStack->push(curve1);

            }
        }
    }

    /*
     * algo detail https://pdfs.semanticscholar.org/8963/c06a92d6ca8868348b0930bbb800ff6e7920.pdf
     * use math to optimize curve approximation
     */
    void cubicBezierCasteljauArticle(Curve p, double flatnessConstant = 0.0005) {

        if ((p.p1 == p.p2 || p.p1 == p.p3)) {
            //handle bad cases when modulos is zero
            Curve l, r;
            std::tie(l, r) = p.splitByArbitraryT(0.5);
            cubicBezierCasteljauArticle(l);
            cubicBezierCasteljauArticle(r);
            return;
        }

        if (p.p2 == p.p4) {
            //handle bad case when p2==p4
            circular_approximation(p, 0, 1, flatnessConstant);
            return;
        }

        // for start we need find up to 2 inflection points(cubic parametric curve have at most 2 such points) t1 and t2 t1<t2
        //[t1-, t1+]  t1-<t1<t1+ is section surround inflection point could be approximated by line  if flatness criterion ok
        // similar to [t2-,t2+]
        // we have [t1-,t1+], [t2-,t2+] , other segment will be approximated by circle approximation

        // if we define x(t) and y(t) as x(t) = a*t^3 + b*t^2 + c*t + d then coefficients would be as follows:
        double ax = -p.p1.x + 3 * p.p2.x - 3 * p.p3.x + p.p4.x;
        double bx = 3 * p.p1.x - 6 * p.p2.x + 3 * p.p3.x;
        double cx = -3 * p.p1.x + 3 * p.p2.x;
        double dx = p.p1.x;
        double ay = -p.p1.y + 3 * p.p2.y - 3 * p.p3.y + p.p4.y;
        double by = 3 * p.p1.y - 6 * p.p2.y + 3 * p.p3.y;
        double cy = -3 * p.p1.y + 3 * p.p2.y;
        double dy = p.p1.y;

        // y coordinate of last point  in new system
        double direct_vector_modulo = sqrt(pow(p.p2.x - p.p1.x, 2) + pow(p.p2.y - p.p1.y, 2));
        double s4 =
                ((p.p4.x - p.p1.x) * (p.p2.y - p.p1.y) - (p.p4.y - p.p1.y) * (p.p2.x - p.p1.x)) / direct_vector_modulo;
        // at inflection point  only derivative of acceleration has component perpendicular  to velocity vector =>
        // s(t) =  t^3 * s4 , i.e to achieve desired flatness we set s(t) = flatness constant =>>  t^3 = flatness/s4

        if (s4 == 0) {
            // i.e tf will be large => no inflection point in [0,1]
            circular_approximation(p, 0, 1, flatnessConstant);
            return;
        }

        double tf = cbrt(flatnessConstant / s4);

        //at inflection points component of acceleration = 0 =>
        // cross product of acceleration *velocity = 0
        // solve quadratic equation for t
        double t_cusp = -0.5 * (ay * cx - ax * cy) / (ay * bx - ax * by);
        double t_cusp2 = t_cusp * t_cusp;
        double discriminant = t_cusp2 - (by * cx - bx * cy) / (ay * bx - ax * by) / 3;


        if (discriminant < 0 || (ay * bx - ax * by) == 0) {
            //no inflexion points
            circular_approximation(p, 0, 1, flatnessConstant);
            return;
        }

        if (discriminant == 0) {
            // only one inflection point
            double t = t_cusp;
            double t1_left = t - (1 - t) * tf;
            double t1_right = t + (1 - t) * tf;
            Segment center = {t1_left, t1_right};
            Segment segment = {0, 1};
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
        // now we have two inflection points

        double t1 = t_cusp - sqrt(discriminant);
        double t2 = t_cusp + sqrt(discriminant);
        if (t1 > t2) std::swap(t1, t2);

        // [-tf,0] and [0,tf] < flatness and we can merge them to one segment and then transform to parametric range

        double t1_left = t1 - (1 - t1) * tf;
        double t1_right = t1 + (1 - t1) * tf;
        double t2_left = t2 - (1 - t2) * tf;
        double t2_right = t2 + (1 - t2) * tf;
        Segment left = {t1_left, t1_right};
        Segment right = {t2_left, t2_right};
        Segment segment = {0.0, 1.0};

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

    inline void circular_approximation(Curve p, double start, double end, double flatness) {
        if (end > 1) end = 1;
        if (start < 0) start = 0;

        Curve initialCurve = p;

        double t = start;
        Curve curCurve = p;
        Point leftPoint = initialCurve.get_evaluated_point(t);
        Point rightPoint;
        Curve unused;
        while (t < end) {
            // we get t that attached to new curve not initial curve
            double t_new_coordinate = curCurve.splitByFlatness(flatness, end);
            t += t_new_coordinate;
            t = std::min(t, end);

            rightPoint = initialCurve.get_evaluated_point(t);
            drawLine(leftPoint.x, leftPoint.y, rightPoint.x, rightPoint.y);
            leftPoint = rightPoint;

            std::tie(unused, curCurve) = curCurve.splitByArbitraryT(t);


        }
    }


    inline void subroutine_if(Curve curve, Segment &seg, double flatnessConstant) {
        Segment segment = {0.0, 1.0};// our [0,1];
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

    inline void lineApproximation(Curve curve, double t_min, double t_max, double flatness) {

        if (t_min >= t_max || t_min < 0 || t_max > 1) {
            //already draw line
            return;
        }

        Point a = curve.get_evaluated_point(t_min);
        Point b = curve.get_evaluated_point(t_max);
        drawLine(a.x, a.y, b.x, b.y);
    }

};








#endif // BEZIERRASTERIZER_H

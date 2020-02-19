#ifndef ORIENTEDBOX_H
#define ORIENTEDBOX_H

#include <geometry/ellipticalApproximation.h>
#include <geometry/axisAlignedBox.h>
#include <math/matrix/matrix33.h>

namespace corecvs {


class OrientedBox
{
public:
    Vector3dd position;
    Matrix33 axis;

    OrientedBox(const AxisAlignedBox3d& box)
    {
        position = box.getCenter();
        axis = Matrix33::FromDiagonal(box.measure() / 2.0);
    }

    bool isInside(const Vector3dd &point) const
    {
        Vector3dd o = (axis.transposed().inv() * point) - position;
        return (o.isInHypercube(Vector3dd(-1,-1,-1), Vector3dd(1, 1, 1)));
    }

    Vector3dd getCenter() const
    {
        return position;
    }

    template<class Transfromation>
    void transform(const Transfromation &T) {
        position = T * position;

        axis = Matrix33::FromRows(
            T * axis.row(0) - T * Vector3dd::Zero(),
            T * axis.row(1) - T * Vector3dd::Zero(),
            T * axis.row(2) - T * Vector3dd::Zero());

        /*Not operations are overloaded, so we would do it like this*/

    }

    template<class Transfromation>
    OrientedBox transformed(const Transfromation &T) {
        OrientedBox toReturn = *this;
        toReturn.transform<Transfromation>(T);
        return toReturn;
    }


    vector<Vector3dd> getPoints() const {
        vector<Vector3dd> toReturn;
        toReturn.reserve(8);
        toReturn.push_back(position - axis.row(0) + axis.row(1) + axis.row(2));
        toReturn.push_back(position - axis.row(0) - axis.row(1) + axis.row(2));
        toReturn.push_back(position + axis.row(0) + axis.row(1) + axis.row(2));
        toReturn.push_back(position + axis.row(0) - axis.row(1) + axis.row(2));

        toReturn.push_back(position - axis.row(0) + axis.row(1) - axis.row(2));
        toReturn.push_back(position - axis.row(0) - axis.row(1) - axis.row(2));
        toReturn.push_back(position + axis.row(0) + axis.row(1) - axis.row(2));
        toReturn.push_back(position + axis.row(0) - axis.row(1) - axis.row(2));

        return toReturn;
    }



    static OrientedBox FromStatistics(const EllipticalApproximation3d &approx, double sigmas = 3.0)
    {
        OrientedBox toReturn;
        toReturn.axis = Matrix33::FromRows(
            approx.mAxes[0] * approx.mValues[0],
            approx.mAxes[1] * approx.mValues[1],
            approx.mAxes[2] * approx.mValues[2]) * sigmas;

        toReturn.position = approx.getCenter();

        return toReturn;
    }

    /* Not yet implemented methods */
    // void toConvexPolyhedron();
    // intersect(Ray3d)


    /**
     * This method doesn't assume that box is square
     *
     * You could speedup around 40% if you know that box is square
     *
     * This is also generic method for any convex polyherdron, optimised version
     * could use box symmetry
     **/
    bool hasIntersection(const OrientedBox &other)
    {
        vector<Vector3dd> tp = getPoints();
        vector<Vector3dd> op = other.getPoints();

        const Matrix33 &axis0 = axis;
        const Matrix33 &axis1 = other.axis;

        vector<Vector3dd> directions;
        directions.reserve((3 + 3) * 2 + 3 * 3);
        directions.push_back(axis0.row(0));
        directions.push_back(axis0.row(1));
        directions.push_back(axis0.row(2));
        directions.push_back(axis0.row(0) ^ axis0.row(1));
        directions.push_back(axis0.row(1) ^ axis0.row(2));
        directions.push_back(axis0.row(2) ^ axis0.row(0));

        directions.push_back(axis1.row(0));
        directions.push_back(axis1.row(1));
        directions.push_back(axis1.row(2));
        directions.push_back(axis1.row(0) ^ axis1.row(1));
        directions.push_back(axis1.row(1) ^ axis1.row(2));
        directions.push_back(axis1.row(2) ^ axis1.row(0));

        /* Cross products between */

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                directions.push_back(axis0.row(i) ^ axis1.row(j));
            }
        }

        /*Actual checks*/

        for (Vector3dd & dir: directions)
        {
            // dir.normalise(); /* optional */

            double min0 = numeric_limits<double>::max();
            double max0 = numeric_limits<double>::lowest();
            for (Vector3dd &p: tp)
            {
                double v = p & dir;
                min0 = std::min(v, min0);
                max0 = std::max(v, max0);
            }


            double min1 = numeric_limits<double>::max();
            double max1 = numeric_limits<double>::lowest();
            for (Vector3dd &p: op)
            {
                double v = p & dir;
                min1 = std::min(v, min1);
                max1 = std::max(v, max1);
            }

            if (min1 > max0) return false;
            if (min0 > max1) return false;
        }
        return true;
    }


    OrientedBox();
};


}

#endif // ORIENTEDBOX_H

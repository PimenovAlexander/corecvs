#include <set>

#include "core/math/vector/vector3d.h"
#include "core/geometry/polygons.h"


namespace corecvs {


class ConvexHull
{
public:
    /**
     *  Helper functions
     **/
    static double ccw(const Vector2dd& p1, const Vector2dd& p2, const Vector2dd& p3)
    {
        return (p2 - p1) & (p3 - p1).leftNormal();
    }


    static double ccwProjective(const Vector3dd& p1, const Vector3dd& p2, const Vector3dd& p3)
    {
        return -(p1 ^ p2) & p3;
    }


    /**
     * Most trivial and slow algorithms
     *
     * These methods need a lot of additional testing
     ***/
    static Polygon GiftWrap  (const std::vector<Vector2dd> &list);
    static Polygon GrahamScan(std::vector<Vector2dd> points);

    static vector<uint> GiftWrapId(const std::vector<Vector2dd> &list);
    static ProjectivePolygon GrahamScan(std::vector<Vector3dd> points);


    /* */
    static int initalPoint(ProjectivePolygon &points);
    static int secondPoint(ProjectivePolygon &points, int initalPoint);
    static bool GiftWrap(ProjectivePolygon &points, ProjectivePolygon &output, Mesh3D *debug = NULL);



/* To Do Make this */
template<typename PointType>
    static Polygon GrahamScan(std::vector<PointType> points);



    enum ConvexHullMethod {
        GIFT_WARP,
        GRAHAM_SCAN,
        LAST
    };

    static Polygon ConvexHullCompute(std::vector<Vector2dd> points, ConvexHullMethod &method);
};

} // namespace


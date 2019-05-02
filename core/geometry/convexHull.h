#include <set>

#include "core/math/vector/vector3d.h"
#include "core/geometry/polygons.h"


namespace corecvs {


class ConvexHull
{
public:
    /**
     * Most trivial and slow algorithms
     *
     * These methods need a lot of additional testing
     ***/
    static Polygon GiftWrap(const std::vector<Vector2dd> &list);
    static Polygon GrahamScan(std::vector<Vector2dd> points);

    static ProjectivePolygon GrahamScan(std::vector<Vector3dd> points);


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


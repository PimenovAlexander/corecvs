#pragma once
/*
 * \file swarmPoint.h
 *
 * \date Mar 15, 2013
 */

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include "core/buffers/rgb24/rgbColor.h"

namespace corecvs {

class SwarmPoint
{
public:
    Vector3dd point;
    Vector3dd speed;
    Vector2dd texCoor;
    RGBColor color;
    bool is6D;
    int flag;
    int cluster;

    enum FlagEnum {
        IS_3D = 0,
        IS_6D = 1,
        USER_1 = 0x2,
        USER_2 = 0x4
    };

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(point,   Vector3dd(0.0), "point");
        visitor.visit(speed,   Vector3dd(0.0), "speed");
        visitor.visit(texCoor, Vector3dd(0.0), "imagePoint");
        visitor.visit(color,   RGBColor(0),    "color");
    }

    SwarmPoint() :
        point(0.0),
        speed(0.0),
        texCoor(0.0),
        color(RGBColor::gray(0xFF)),
                is6D(false),
                flag(0),
                cluster(0)
    {}

    double distTo(SwarmPoint other);
};


struct SortSwarmPointX
{
    bool operator()(SwarmPoint const & one, SwarmPoint const & another) const
    {
        return one.point.x() < another.point.x();
    }
};

struct SortSwarmPointY
{
    bool operator()(SwarmPoint const & one, SwarmPoint const & another) const
    {
        return one.point.y() < another.point.y();
    }
};

struct SortSwarmPointZ
{
    bool operator()(SwarmPoint const & one, SwarmPoint const & another) const
    {
        return one.point.z() < another.point.z();
    }
};

struct SortSwarmPointTexY
{
    bool operator()(SwarmPoint const & one, SwarmPoint const & another) const
    {
        return one.texCoor.y() < another.texCoor.y();
    }
};

struct SortSwarmPointTexX
{
    bool operator()(SwarmPoint const & one, SwarmPoint const & another) const
    {
        return one.texCoor.x() < another.texCoor.x();
    }
};

} /* namespace corecvs */

/* EOF */

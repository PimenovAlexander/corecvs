#ifndef DEFORMMAP_H
#define DEFORMMAP_H

#include "vector2d.h"

namespace corecvs {

/**
 * This class holds the mapping function from coordinate domain
 * to the other coordinate domain.
 *
 * We probably should use only static poly-morphism.
 * So map should be made non-virtual
 **/
template<typename InputIndexType, typename OutputIndexType>
class DeformMap
{
public:

    typedef Vector2d<InputIndexType>   InputPoint;
    typedef Vector2d<OutputIndexType> OutputPoint;

    inline OutputPoint map(const InputIndexType & /*y*/, const InputIndexType & /*x*/) {}

    //inline OutputPoint map(const InputPoint &point)
    //{
    //    return map (point.y, point.x);
    //}

    virtual ~DeformMap() {}
};

} // namespace corecvs


#endif // DEFORMMAP_H

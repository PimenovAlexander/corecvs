#ifndef FIXEDPOINTBLMAPPER_H_
#define FIXEDPOINTBLMAPPER_H_
/**
 * \file fixedPointBlMapper.h
 * \brief A generic function to map buffer according to FixedPointDisplace
 *
 *
 *
 * \date Mar 1, 2012
 * \author alexander
 * \ingroup cppcorefiles
 */

#include "core/buffers/fixeddisp/fixedPointDisplace.h"

namespace corecvs {

/**
 *  This class provides a parallel execution of pixel mapping based on the precomputed bilinear displacement buffer.
 *  Basically it moves pixels to new position based on the FixedPointDisplace buffer that should implement
 *  the map(i,j) function
 *
 **/
template<class InputType, class OutputType, class InternalIndexType, class InternalElementType>
class FixedPointBlMapper {
    //typedef typename InputType::InternalIndexType InternalIndexType;

public:

    class ParallelDoReverseDeformationBlPrecomp
    {
        OutputType *toReturn;
        const FixedPointDisplace *map;
        InputType *buf;

    public:
        ParallelDoReverseDeformationBlPrecomp(
            OutputType *_toReturn,
            const FixedPointDisplace *_map,
            InputType *_buf
            ) :
        toReturn(_toReturn), map(_map), buf(_buf)
        {}

        void operator()( const BlockedRange<InternalIndexType>& r ) const
        {
            InternalIndexType j;
            InternalIndexType newW = toReturn->getW();

            for (InternalIndexType i = r.begin(); i < r.end(); i++)
            {
                for (j = 0; j < newW - 1; j++)
                {
                    BilinearMapPoint p = map->map(i,j);
                    if (buf->isValidCoordBlPrecomp(p))
                        toReturn->element(i,j) = buf->elementBlPrecomp(p);
                    else
                        toReturn->element(i,j) = InternalElementType(0x0);
                }
            }
        }

    }; // ParallelDoReverseDeformationBlPrecomp

    inline bool isValidCoordBlPrecomp(const BilinearMapPoint &point) const
    {
        return /*(point.x >= 0 ) &&*/ (point.x < static_cast<const InputType *>(this)->getW() - 1) &&
               /*(point.y >= 0 ) &&*/ (point.y < static_cast<const InputType *>(this)->getH() - 1);
    }

    /**
     * This function template is responsible for applying reverse
     * transformation to the abstract buffer.
     *
     *
     * \param map
     *         The map to apply
     * \param newH
     *         Output Buffer Height
     * \param newW
     *         Output Buffer Width
     **/
    OutputType *doReverseDeformationBlPrecomp(const FixedPointDisplace *map, InternalIndexType newH = -1, InternalIndexType newW = -1)
    {
        InputType *realThis =  static_cast<InputType *>(this);

        if (newH == -1) newH = realThis->getH();
        if (newW == -1) newW = realThis->getW();

        OutputType *toReturn = new OutputType(newH, newW);
        DOTRACE(("Starting transform to %d %d...\n", newW - 1, newH - 1));
        parallelable_for(0, newH - 1, ParallelDoReverseDeformationBlPrecomp(toReturn, map, realThis));
        return toReturn;
    }

};

} // namespace corecvs

#endif  /* #ifndef FIXEDPOINTBLMAPPER_H_ */

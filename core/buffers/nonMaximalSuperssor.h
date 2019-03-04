#ifndef NONMAXIMALSUPERSSOR_H
#define NONMAXIMALSUPERSSOR_H

#include "core/math/vector/vector2d.h"

namespace corecvs {

using std::vector;

template<class BufferType>
class NonMaximalSuperssor
{
public:
    typedef typename BufferType::InternalElementType ElementType;
    typedef typename BufferType::InternalIndexType   IndexType;

    typedef Vector2d<IndexType> CoordType;


    NonMaximalSuperssor() {}

    /**
     * Non-maximum supression with avg. complexity < 2.4 ops / pixel and worst-case complexity < 4-4/(w+1) ops / pixel
     * (naive implementation costs 1.3 + 2log(w) avg. and n^2 worst-case)
     *
     * Check out http://homes.esat.kuleuven.be/~konijn/publications/2006/eth_biwi_00446.pdf for details
     *
     * \param windowHalf - half of maximum-search window
     * \param threshold  - threshold for maximum-candidates
     * \param maximas    - vector of local-maximum points (output)
     * \param skip       - skip border
     **/
    void nonMaximumSupression(const BufferType &image, const IndexType &windowHalf, const ElementType &threshold, vector<CoordType> &maximas, const IndexType &skip = 0);

    inline bool nonMaximumSupressionHelper(const BufferType &image, IndexType &top, IndexType &bottom, IndexType &left, IndexType &right, ElementType &mx)
    {
        for (IndexType i = top; i < bottom; i++)
        {
            for (IndexType j = left; j < right; j++)
            {
                if (image.element(i, j) > mx)
                    return true;
            }
        }
        return false;
    }

};

} //namespace corecvs


#endif // NONMAXIMALSUPERSSOR_H

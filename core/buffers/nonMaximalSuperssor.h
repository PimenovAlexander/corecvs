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

    /*
     * Non-maximum supression with avg. complexity < 2.4 ops / pixel and worst-case complexity < 4-4/(w+1) ops / pixel
     * (naive implementation costs 1.3 + 2log(w) avg. and n^2 worst-case)
     *
     * Check out http://homes.esat.kuleuven.be/~konijn/publications/2006/eth_biwi_00446.pdf for details
     *
     * \param windowHalf - half of maximum-search window
     * \param threshold  - threshold for maximum-candidates
     * \param maximas    - vector of local-maximum points (output)
     * \param skip       - skip border
     */

    void nonMaximumSupression(const BufferType &image, const IndexType &windowHalf, const ElementType &threshold, vector<CoordType> &maximas, const IndexType &skip = 0)
    {
        maximas.clear();
        IndexType a = windowHalf;
        IndexType s = skip;

        for (IndexType i = s + a; i + a + s < image.w; i += a + 1)
        {
            for (IndexType j = s + a; j + a + s < image.h; j += a + 1)
            {
                IndexType mi = i;
                IndexType mj = j;

                ElementType mx = image.element(j, i);

                for (auto i2 = i; i2 < i + a + 1; i2++)
                {
                    for (auto j2 = j; j2 < j + a + 1; j2++)
                    {
                        if (image.element(j2, i2) > mx)
                        {
                            mx = image.element(j2, i2);
                            mi = i2;
                            mj = j2;
                        }
                    }
                }

                // Now we are sure, that A[mj, mi] is best in [j; j+w]x[i; i+w], need to check if it is best in [mj-w; mj+w]x[mi-w; mi+w]
                bool failed = mx < threshold;

                IndexType top    = mj - a;
                IndexType bottom = std::min(mj + a + 1, j);
                IndexType left   = mi - a;
                IndexType right  = mi + a + 1;

#define TRY_NMS(tv, bv, lv, rv) \
                if (!failed) \
                { \
                    top = tv; \
                    bottom = bv; \
                    left = lv; \
                    right = rv; \
                    failed |= nonMaximumSupressionHelper(image, top, bottom, left, right, mx); \
                }

                TRY_NMS(      top,                          bottom,      left, right)
                TRY_NMS(        j, std::min(mj + a + 1, j + a + 1),    mi - a, std::min(mi + a + 1, i))
                TRY_NMS(      top,                          bottom, i + a + 1, mi + a + 1)
                TRY_NMS(j + a + 1,                      mj + a + 1,    mi - a, mi + a)
#undef TRY_NMS
                if (!failed)
                    maximas.push_back(Vector2d<int>(mi, mj));
            }
        }
    }

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

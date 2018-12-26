#include "core/buffers/nonMaximalSuperssor.h"
#include "core/buffers/convolver/convolver.h"

namespace corecvs {

template<class BufferType>
void NonMaximalSuperssor<BufferType>::nonMaximumSupression(
        const BufferType &image,
        const IndexType &windowHalf,
        const ElementType &threshold,
        vector<CoordType> &maximas,
        const IndexType &skip)
{
    //SYNC_PRINT(("NonMaximalSuperssor<BufferType>::nonMaximumSupression([%dx%d], windowHalf=%d, threshold=%lf, _, skip=%d): called\n", image.w, image.h, windowHalf, threshold, skip ));

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
    //SYNC_PRINT(("NonMaximalSuperssor::nonMaximumSupression(): found %d corners\n", (int)maximas.size() ));

}

template class NonMaximalSuperssor<FpImage>;




} // namespace corecvs

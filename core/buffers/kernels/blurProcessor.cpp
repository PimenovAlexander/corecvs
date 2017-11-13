/**
 * \file blurProcessor.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 19, 2010
 * \author alexander
 */

#include "core/utils/global.h"

#include "core/buffers/kernels/blurProcessor.h"
#include "core/tbbwrapper/tbbWrapper.h"
namespace corecvs {

/*TODO: Hide this*/
const int BlurProcessor::xshift[4] = {0,1,0,1};
const int BlurProcessor::yshift[4] = {0,0,1,1};



class ParallelPrepareBlurBuffers
{
    G12IntegralBuffer *integral;
    G12Buffer *blur;
    unsigned dy;
    unsigned dx;
public:

    ParallelPrepareBlurBuffers(
            G12IntegralBuffer *_integral,
            G12Buffer *_blur,
            unsigned _dy,
            unsigned _dx) :
       integral(_integral), blur(_blur), dy(_dy), dx(_dx)
       {}


    void operator()( const BlockedRange<int>& r ) const
    {
        for( int i = r.begin(); i < r.end(); i ++ )
        {
            G12Buffer::InternalElementType* outoffset = &(blur->element(i, 0));
            for (int j = 0; j < blur->w; j ++)
            {
                int y = i * 2 + dy;
                int x = j * 2 + dx;

                unsigned tmp = integral->rectangle(x, y, x + BlurProcessor::BLUR_RADIUS * 2, y + BlurProcessor::BLUR_RADIUS * 2);
                *outoffset = (uint16_t)(tmp / 25);
                outoffset++;
            }
        }
    }

};

/**
 *  The image below represents the geometry of the things to happen
 *  A, B, C, D - elements of the blur buffers
 *
 *
 *
 *  \code
 *
 *                  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
 *                          0       1       2       3       4       5       6
 *                          0   1   2   3   4   5   6   7   8   9   10  11
 *              +---------------------------------------------------------------+
 *  0           |   +   +   +   +   +                       +   +   +   +   +   |
 *  1           |   +   +   +   +   +                       +   +   +   +   +   |
 *  2   0   0   |   +   +   A   B   A   B   A   B   A   B   A   B   A   +   +   |
 *  3       1   |   +   +   C   D   C   D   C   D   C   D   C   D   C   +   +   |
 *  4   1   2   |   +   +   A   B   A  \B/  A   B   A   B   A   B   A   +   +   |
 *  5       3   |           C   D   C  -D-  C   D   C   D   C   D   C           |
 *  6   2   4   |           A   B   A  /B\  A   B   A   B   A   B   A           |
 *  7       5   |           C   D   C   D   C   D   C   D   C   D   C           |
 *  8   3   6   |           A   B   A   B   A   B   A   B   A   B   A           |
 *  9       7   |   +   +   C   D   C   D   C   D   C   D   C   D   C   +   +   |
 *  10  4   8   |   +   +   A   B   A   B   A   B   A   B   A   B   A   +   +   |
 *  11      9   |   +   +   C   D   C   D   C   D   C   D   C   D   C   +   +   |
 *  12  5   10  |   +   +   +   +   +                       +   +   +   +   +   |
 *  13          |   +   +   +   +   +                       +   +   +   +   +   |
 *  14          +---------------------------------------------------------------+
 *
 *  \endcode
 **/
void BlurProcessor::prepareBlurBuffers(
        G12IntegralBuffer *integral,
        G12Buffer* blur[4])
{
    const unsigned h = integral->getEffectiveH();
    const unsigned w = integral->getEffectiveW();
    unsigned g;

    // shifts of the coordinate system for 4 types of points 2*2

    const unsigned patternsH = h - BLUR_RADIUS * 2;
    const unsigned patternsW = w - BLUR_RADIUS * 2;

    const unsigned patternHhalf = patternsH  / 2;// (h-4)/2
    const unsigned patternWhalf = patternsW  / 2;
    int patternLowerH = patternHhalf;
    int patternLowerW = patternWhalf;

    if (patternsH & 0x1)
        patternLowerH++;
    if (patternsW & 0x1)
        patternLowerW++;

    blur[0] = new G12Buffer(patternLowerH, patternLowerW, false);
    blur[1] = new G12Buffer(patternLowerH, patternWhalf,  false);
    blur[2] = new G12Buffer(patternHhalf,  patternLowerW, false);
    blur[3] = new G12Buffer(patternHhalf,  patternWhalf,  false);

    /*Accurately fill blurred buffers with data */

    for (g = 0; g < 4; g++)
    {
        parallelable_for<int, ParallelPrepareBlurBuffers>
          (0, blur[g]->h, ParallelPrepareBlurBuffers(integral, blur[g], yshift[g], xshift[g]));
    }
}


void BlurProcessor::prepareBlurBuffers(G12IntegralBuffer *integral,
        unsigned blurH,
        unsigned blurW,
        unsigned stepH,
        unsigned stepW,
        G12Buffer** blur)
{
    G12Buffer *blurred = integral->rectangularBlur<G12Buffer>(blurH, blurW);
    for (unsigned i = 0; i < stepH; i++)
    {
        for (unsigned j = 0; j < stepW; j++)
        {
            int h = (blurred->h / stepH);
            int w = (blurred->w / stepW);
            if ((h * stepH + i) < (unsigned)blurred->h) h++;
            if ((w * stepW + j) < (unsigned)blurred->w) w++;
            G12Buffer *small = new G12Buffer(h, w);

            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++)
                {
                    small->element(y,x) = blurred->element(i + y * stepH, j + x * stepW);
                }
            }
            blur[i * stepW + j] = small;
        }
    }
    delete blurred;
}


#ifdef WITH_SSE

class ParallelPrepareBlurBuffersSSE
{
    G12IntegralBuffer *integral;
    G12Buffer **blur;

public:
    ParallelPrepareBlurBuffersSSE(
        G12IntegralBuffer *_integral,
        G12Buffer *_blur[4]) :
    integral(_integral),
    blur(_blur)
    {}

    void operator()( const BlockedRange<unsigned>& r ) const
    {
        unsigned j;

        const unsigned w = integral->getEffectiveW();

        for( unsigned i = r.begin(); i < r.end(); i++ )
        {
            int t = i >> 1;
            int gUp = (i & 0x1) << 1;
            uint16_t *outoffset1 = &(blur[gUp]    ->element(t,0));
            uint16_t *outoffset2 = &(blur[gUp + 1]->element(t,0));


            Float32x4 div25(1.0f / 25.0f);
            for (j = 0; j < w - 4 - 7; j += 8)
            {
                /* Take first 4 sums of 5x5 areas*/
                Int32x4 tmp = integral->rectangle_sse_new(j, i, j + 4, i + 4);
                Float32x4 float_tmp(tmp);
                Float32x4 float_mult = float_tmp * div25;
                Int32x4 dv = float_mult.trunc();


                dv.shuffle<_MM_SHUFFLE(3,1,2,0)>();
                /* Now the order is 3 1 2 0*/

                /* Take second 4 sums of 5x5 areas*/
                tmp = integral->rectangle_sse_new(j + 4, i, j + 8, i + 4);
                float_tmp = Float32x4(tmp);
                float_mult = float_tmp * div25;
                Int32x4 dv2 = float_mult.trunc();

                dv2.shuffle<_MM_SHUFFLE(3,1,2,0)>();
                /* Now the order is 7 5 6 4*/

                /*Pack together 8 results */
                Int16x8 pv = Int16x8::pack(dv,dv2);

                /* Now the order is 3 1 2 0 7 5 6 4 */
                Int16x8 sv0 = pv.shuffled<_MM_SHUFFLE(3,1,2,0)>();
                //__m128i sv0 = _mm_shuffle_epi32(pv,_MM_SHUFFLE(3,1,2,0));
                /**
                 * Now the order is 6 4 2 0 7 5 3 1
                 * You can see odd and even now come together
                 * */
                sv0.saveLower(outoffset1);
                //_mm_storel_epi64((__m128i*)outoffset1, sv0);

                Int16x8 sv1 = pv.shuffled<_MM_SHUFFLE(2,0,3,1)>();
                //__m128i sv1 = _mm_shuffle_epi32(pv,_MM_SHUFFLE(2,0,3,1));
                /* Now the order is 7 5 3 1 6 4 2 0 */
                sv1.saveLower(outoffset2);
                //_mm_storel_epi64((__m128i*)outoffset2, sv1);

                outoffset1 += 4;
                outoffset2 += 4;
            }

            for (; j < w - 4; j++)
            {
                unsigned tmp = integral->rectangle(j, i, j + 4, i + 4);
                uint16_t result = (uint16_t)(tmp / 25);
                if (j & 1) {
                    *outoffset2 = result;
                     outoffset2++;
                } else {
                    *outoffset1 = result;
                    outoffset1++;
                }
            }
        }
    }
};


ALIGN_STACK_SSE void BlurProcessor::prepareBlurBuffersSSE(G12IntegralBuffer *integral, G12Buffer* blur[4])
{
    const unsigned h = integral->getEffectiveH();
    const unsigned w = integral->getEffectiveW();

    // shifts of the coordinate system for 4 types of points 2*2
    const unsigned patternsH = h - BLUR_RADIUS * 2;
    const unsigned patternsW = w - BLUR_RADIUS * 2;

    const unsigned patternHhalf = patternsH  / 2;
    const unsigned patternWhalf = patternsW  / 2;
    int patternLowerH = patternHhalf;
    int patternLowerW = patternWhalf;

    if (patternsH & 0x1)
        patternLowerH++;
    if (patternsW & 0x1)
        patternLowerW++;

    blur[0] = new G12Buffer(patternLowerH, patternLowerW, false);
    blur[1] = new G12Buffer(patternLowerH, patternWhalf,  false);
    blur[2] = new G12Buffer(patternHhalf,  patternLowerW, false);
    blur[3] = new G12Buffer(patternHhalf,  patternWhalf,  false);

    parallelable_for(0u, h - 4, ParallelPrepareBlurBuffersSSE(integral, blur));
}

#endif // WITH_SSE

} //namespace corecvs


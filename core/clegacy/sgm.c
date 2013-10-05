/**
 * \file sgm.c
 * \brief this file implements the fast and the research version of the SGM algorithm
 *
 *
 * \ingroup corefiles
 * \date Apr 14, 2009
 * \author Alexander Pimenov
 */


#include <stdlib.h>

// This is a temporary. This is a C file and should be rewritten anyway.
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#undef __STDC_LIMIT_MACROS
#include <string.h>
#include <stdio.h>

#include "global.h"


#ifdef TRACE
#include <stdio.h>
#endif

#include "sgm.h"

typedef struct PathDescription_TAG {
        int dy;
        int dx;
        int isVertical;
        int pathClass;

} PathDescription;

#define PC_PATH_LIST_SIZE 12

PathDescription pathList[PC_PATH_LIST_SIZE] =
   {{ 1, 0, 1, PATH_TOP},
        {-1, 0, 1, PATH_BOTTOM},
        { 0, 1, 0, PATH_LEFT},
        { 0,-1, 0, PATH_RIGHT},

        { 1, 1, 1, PATH_TOP_LEFT},
        {-1, 1, 1, PATH_BOTTOM_LEFT},
    { 1,-1, 1, PATH_TOP_RIGHT},
    {-1,-1, 1, PATH_BOTTOM_RIGHT},

        { 1, 1, 0, PATH_TOP_LEFT},
        {-1, 1, 0, PATH_BOTTOM_LEFT},
    { 1,-1, 0, PATH_TOP_RIGHT},
        {-1,-1, 0, PATH_BOTTOM_RIGHT}
    };


#define ADDR(x,y,d) ((d) * I->h * I->w + (y) * I->w + (x))

/** \todo TODO Use subpixel distance, or filter out the image or use MI*/
/**
 *
 */
static inline uint32_t C(G12Buffer *I, G12Buffer *J, int x, int y, int dx, int dy)
{
        int offsetI = (I->w * y + x);
        int newx = x + dx;
        if (newx < 0) newx = 0;
        if (newx > I->w - 1) newx = I->w - 1;

//        if (newx < 0) return 0x1FFFF;
//        if (newx > I->w - 1) return 0x1FFFF;

        int offsetJ = (J->w * y + newx);

        if (I->data[offsetI] > J->data[offsetJ])
                return (I->data[offsetI] - J->data[offsetJ]);
        else
                return (J->data[offsetJ] - I->data[offsetI]);
        //uint32_t diff = I->data[offsetI] - J->data[offsetJ];

        //return fabs( ((double)I->data[offsetI]) - ((double)J->data[offsetJ])) / 100.0;
}


/**
 * This function counts a minimum of the 4 unsigned 32 bit values.
 */
static inline uint32_t min4 (uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        if (a > b) a = b;
        if (c > d) c = d;
        if (a > c) return c;
        return a;
}

/**
 * This function counts a minimum of the 4 unsigned 16 bit values.
 */
static inline uint16_t min4_16 (uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
        if (a > b) a = b;
        if (c > d) c = d;
        if (a > c) return c;
        return a;
}

/**
 * This function counts a minimum of the 4 unsigned 32 bit values.
 */
static inline uint32_t min4_32 (uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        if (a > b) a = b;
        if (c > d) c = d;
        if (a > c) return c;
        return a;
}

/**
 * This function counts a minimum of the 4 unsigned 32 bit values.
 */
static inline uint32_t min4_32_special (uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t add) {
        if (a > b) a = b;
        a += add;
        if (c > d) c = d;
        if (a > c) return c;
        return a;
}


/**
 * This function counts a minimum of the 4 unsigned 32 bit values.
 */
static inline uint32_t min3_32 (uint32_t a, uint32_t b, uint32_t c) {
        if (a > b) a = b;
        if (a > c) return c;
        return a;
}

#ifdef DEPRICATED
DisparityBuffer *countDisparity( G12Buffer* I, G12Buffer* J, int dMax, int closePenlty, int longPenalty )
{
        G12Buffer *d1 = sgm_PC16(I, J,  dMax, closePenlty, longPenalty, NULL, NULL, NULL, 0xFFFF, NULL);
        G12Buffer *d2 = sgm_PC16(I, J, -dMax, closePenlty, longPenalty, NULL, NULL, NULL, 0xFFFF, NULL);
        DisparityBuffer *result = disparityBufferCreate(d1->h, d1->w);
        int i,j;
        for (i = 0; i <  d1->h; i++)
        {
                for (j = 0; j <  d1->w; j++)
                {
                    int leftDisparity = d1->data[i * d1->w + j] / (16.0 * 255.0) * dMax;
                    if (j + leftDisparity > d1->w)
                    {
                            continue;
                    }
                        int rightDisparity = d2->data[i * d1->w + j + leftDisparity] / (16.0 * 255.0) * dMax;
                        if (leftDisparity == rightDisparity)
                        {
                                result->data[i * d1->w + j] = d1->data[i * d1->w + j];
                        }
                }
        }
        result->maxValue = 16 * 255;

        return result;

}
#endif

/**
 * This function uses a lot of space. We should invent how not to use it
 *
 */
static uint32_t *computeCostArray(G12Buffer* I, G12Buffer* J, int dMax, int isInverted)
{
        uint32_t *Carray = (uint32_t *)dv_malloc(dMax * I->h * I->w * sizeof(uint32_t));
        int d,i,j;
        // Fill the cost array and Sarray
        for (d = 0; d < dMax; d ++)
        {
                for (i = 0; i  < I->h; i++)
                {
                        for (j = 0; j  < I->w; j++)
                        {
                                int offset = ADDR(j,i,d);
                                if (!isInverted)
                                {
                                        Carray[offset] = C(I,J, j,i, d ,0);
                                }
                                else
                                {
                                        Carray[offset] = C(J,I, j,i,-d ,0);
                                }
                        }
                }
        }
        return Carray;
}

/**
 * This function implements the classical SGM algorithm (slow).
 *
 **/
G12Buffer* sgm_PC16 (
                G12Buffer* I,
                G12Buffer* J,
                int dMax,
                int closePenlty,
                int longPenalty,
                uint32_t **sArrayTest,
                uint32_t **cArrayTest,
                uint32_t **pArrayTest,
                uint32_t mask,
                SGM_Progress_Callback callback)
{
#ifdef ASSERTS
        if (I == NULL || J == NULL)
                ASSERT_FAIL("null parameters not supported");
        if (I->h != J->h || I->w != J->w)
                ASSERT_FAIL("Buffers have different sizes");
#endif

    uint32_t *Carray;
    uint32_t *Sarray;
    uint32_t *Parray;

        int i,j,d;

        int isInverted = dMax < 0;
        dMax = isInverted ? -dMax : dMax;


        uint32_t *Lcost = (uint32_t *) dv_malloc (dMax * sizeof(uint32_t *));
        uint32_t *Lcost1 = (uint32_t *) dv_malloc (dMax * sizeof(uint32_t *));
        uint32_t *costs;
        uint32_t *costs1;
        uint32_t *tmp;

        uint32_t min;
        uint32_t oldmin;

        int dx, dy;
        int x,y;


        int arraySize = dMax * I->h * I->w * sizeof(uint32_t);

        if (pArrayTest != NULL)
        {

                int storageAlloc = arraySize * PATH_LIMIT;
#ifdef ASSERTS
                if (storageAlloc / PATH_LIMIT != arraySize)
                        ASSERT_FAIL("An integer overflow selecting the debug buffer. Sorry.");
#endif

                printf("%d bits will be used to store the debug data.\n", storageAlloc);
                Parray = (uint32_t *) dv_malloc (storageAlloc);
        }

        Carray = computeCostArray(I, J, dMax, isInverted);
        Sarray = (uint32_t *) dv_calloc (arraySize, 1);


        // Traverse all the paths types

        int pathcount;
        for (pathcount = 0; pathcount < PATH_LIMIT; pathcount++)
        {
                if (!(pathList[pathcount].pathClass & mask))
                        continue;

                dy = pathList[pathcount].dy;
                dx = pathList[pathcount].dx;
                int isVertical = pathList[pathcount].isVertical;
#ifdef TRACE
                printf("Path Group %d [%d;%d]\n", pathcount, dy, dx);
#endif

                int startx = 0;
                int starty = 0;
                if (isVertical)
                {
                        if (dy < 0) starty = I->h - 1;
                }
                else
                {
                        if (dx < 0) startx = I->w - 1;
                }

                if (callback != NULL)
                        callback(pathcount, 0);
                // For every path in the group
                while (1)
                {
                        x = startx;
                        y = starty;

                        min = UINT32_MAX;
                        for (d = 0; d < dMax; d++)
                        {
                                uint32_t result = Carray[ADDR(x,y,d)];
                                if (result < min)
                                        min = result;
                                Lcost[d] = result;
                                Lcost1[d] = 0;
                        }

                        costs = Lcost;
                        costs1 = Lcost1;
                        oldmin = min;

                        while (x < I->w && x >= 0 && y < I->h && y >= 0)
                        {

                                min = UINT32_MAX;
                                for (d = 0; d < dMax; d++)
                                {
                                        uint32_t cost = Carray[ADDR(x,y,d)];
                                        uint32_t l1 = (d > 0        ? (costs[d - 1] + closePenlty) : UINT32_MAX);
                                        uint32_t l2 = (d < dMax - 1 ? (costs[d + 1] + closePenlty) : UINT32_MAX);
                                        uint32_t l3 = oldmin + longPenalty;
                                        uint32_t l4 = costs[d];
#ifdef ASSERTS
                                        uint32_t sum =  cost + min4 (l1, l2, l3, l4);
                                        if (sum < cost || sum < min4 (l1, l2, l3, l4))
                                                ASSERT_FAIL("TROUBLE: OVERFLOW IN SGM");

                                        if (oldmin > sum )
                                                ASSERT_FAIL("TROUBLE: INTERNAL BUG IN SGM");
#endif
                                        uint32_t result =
                                                cost + min4 (l1, l2, l3, l4) - oldmin;

                                        if (result < min) {
                                                min = result;
                                        }
                                        costs1[d] = result;
#ifdef ASSERTS
                                        sum = Sarray[ADDR(x,y,d)] + result;
                                        if (sum < Sarray[ADDR(x,y,d)] || sum < result)
                                                ASSERT_FAIL("TROUBLE: OVERFLOW IN SGM");
#endif
                                        if (pArrayTest != NULL) {
                                                Parray[(ADDR(x,y,d)) * PATH_LIMIT + pathcount] = result;
                                        }
                                        Sarray[ADDR(x,y,d)] += result;
                                        //Sarray[ADDR(x,y,d)]++;
                                }

                                oldmin = min;
                                tmp = costs;
                                costs = costs1;
                                costs1 = tmp;
                                x += dx;
                                y += dy;
                        }

                        if (isVertical)
                        {
                                startx++;
                                if (startx >=  I->w) {
                                        break;
                                }
                        }
                        else
                        {
                                starty++;
                                if (starty >=  I->h) {
                                        break;
                                }
                        }

                }
        }
#ifdef TRACE
        printf("Arrays done %d\n", Sarray[56]);
#endif
#ifdef ASSERTS
        uint32_t globalMax = 0;
#endif

        G12Buffer *toReturn = g12BufferCreate(I->h, I->w);

        for (i = 0; i < I->h; i++ )
        {
                for (j = 0; j < I->w; j++ )
                {
                        double min = 2 * (double)UINT32_MAX;
                        double minpos = -1;
                        double pos;
                        for (d = 1; d < dMax - 1; d++)
                        {
#ifdef ASSERTS
                                if ((uint32_t)Sarray[ADDR(j,i,d)] >= (1 << 16))
                                {
                                        char s[100];
                                        snprintf(s, 100, "Overflow of 2^16 in SGM global cost buffer at (%d,%d) disp = %d value=%u", j, i, d, (uint32_t)Sarray[ADDR(j,i,d)] );
                                        int p;
/*                                        for (p = 0; p < PATH_LIMIT; p++)
                                                printf( "Path %d cost: %d\n", p, Parray[(ADDR(j,i,d)) * PATH_LIMIT + p]);*/
                                        printf("%s\n", s);
                                        //DOASSERT(s);

                                }

#endif


//#define SIMPLE_DIST
#ifdef SIMPLE_DIST
                                uint32_t result = Sarray[ADDR(j,i,d)];
                                if (result < min)
                                {
                                        min = result;
                                        minpos = d;
                                }
#else
                                // Parbolic Interpolation
                                int E = Sarray[ADDR(j,i,d - 1)];
                                int F = Sarray[ADDR(j,i,d)];
                                int G = Sarray[ADDR(j,i,d + 1)];

                                double c = F;
                                double a = (E + G - 2 * F) / 2;
                                double b = (G - E) / 2;
                                double result;

                                // Parabola goes down. minimum is on the ends
                                do {
                                        if (a > 0) {
                                                pos = - b / (2 * a);
                                                if ( pos >= -1 && pos <= 1) {
                                                        result = a * pos * pos + b * pos + c;
                                                        pos += d;
                                                        break;
                                                }
                                        }

                                        if ( E > G ) {
                                                result = G;
                                                pos = d + 1;
                                        }
                                        else
                                        {
                                                result = E;
                                                pos = d - 1;
                                        }

                                } while (0);

                                if (result < min)
                                {
                                        min = result;
                                        minpos = pos;
                                }
#endif
                        }
#ifdef ASSERTS
                        if (minpos < 0 || minpos > dMax) {
                                ASSERT_FAIL("INTERNAL ERROR");
                        }
#endif

                        toReturn->data[toReturn->w * i + j] = ((minpos / dMax) * 255.0) * 16.0;
//                        toReturn->data[(toReturn->w * i + j) * toReturn->bpp] = ((255.0 / (minpos + 1)));
                }
        }

        if (cArrayTest != NULL) {
                *cArrayTest = Carray;
        } else {
                dv_free(Carray);
        }
        if (sArrayTest != NULL) {
                *sArrayTest = Sarray;
        } else {
                dv_free(Sarray);
        }
        if (pArrayTest != NULL) {
                        *pArrayTest = Parray;
        }
        dv_free(Lcost);
        dv_free(Lcost1);
        return toReturn;

}

/* TODO: Finish this function */
int getMinimum (uint32_t *data, int length, int stride, int maximium)
{




        return 0;
}

#define FAST_SGM
#ifdef FAST_SGM
G12Buffer* sgmFastx86 (G12Buffer* left, G12Buffer* right, int dMax, int closePenlty, int longPenalty)
{
#ifdef ASSERTS
        if (left == NULL || right == NULL)
                ASSERT_FAIL("null parameters not supported");
        if (left->h != right->h || left->w != right->w)
                ASSERT_FAIL("Buffers have different sizes");
#endif

    uint16_t *Carray;

        int i,j,d;
        int h = left->h;
        int w = left->w;
        int isInverted = dMax < 0;
        dMax = isInverted ? -dMax : dMax;

        int arraySize = dMax * left->h * left->w * sizeof(uint16_t);
        Carray = (uint16_t *) dv_malloc (arraySize);

        // Fill the cost array. In fact we don't need this
        int offset = 0;
        if (isInverted)
        {
                for (d = 0; d < dMax; d ++)
                        for (i = 0; i < h; i++)
                                for (j = 0; j < w; j++)
                                        Carray[offset++] = C(right, left, j,i, -d ,0);
        }
        else
        {
                for (d = 0; d < dMax; d ++)
                        for (i = 0; i < h; i++)
                                for (j = 0; j < w; j++)
                                        Carray[offset++] = C(left,right, j,i, d ,0);

        }

        /*Allocate memory for the process*/
        uint32_t *Sline = (uint32_t *) dv_malloc (dMax * w * sizeof(uint32_t));

        uint16_t **(Ltop[3]);
        for (i = 0; i < 3; i++)
                Ltop[i] = (uint16_t **) dv_malloc (w * sizeof(uint16_t *));

        for (i = 0; i < 3; i++)
                for (j = 0; j < w; j++ )
                        Ltop[i][j] = (uint16_t *)dv_malloc (dMax * sizeof(uint16_t));

        uint16_t *(Ltnew[5]);
        for (i = 0; i < 5; i++)
                Ltnew[i] = (uint16_t *) dv_malloc (dMax * sizeof(uint16_t));

        uint16_t *(Ltmin[3]);
        for (i = 0; i < 3; i++)
        Ltmin[i] = (uint16_t *) dv_malloc (w * sizeof(uint16_t));

        uint16_t *Lleft  = (uint16_t *) dv_malloc(sizeof(uint16_t) * dMax);
        uint16_t *Lright = (uint16_t *) dv_malloc(sizeof(uint16_t) * dMax);

        G12Buffer *toReturn = g12BufferCreate(h, w);

        for (j = 0; j < w ; j++)
        {
                Ltmin[0][j] = UINT16_MAX;
                Ltmin[1][j] = UINT16_MAX;
                Ltmin[2][j] = UINT16_MAX;

                for (d = 0; d < dMax; d++)
                {
                        Ltop[0][j][d] = Carray[d * h * w + 0 * w + j];
                        Ltop[1][j][d] = Carray[d * h * w + 0 * w + j];
                        Ltop[2][j][d] = Carray[d * h * w + 0 * w + j];
                        if (Ltmin[0][j] > Ltop[0][j][d])
                                Ltmin[0][j] = Ltop[0][j][d];

                        if (Ltmin[1][j] > Ltop[1][j][d])
                                Ltmin[1][j] = Ltop[1][j][d];

                        if (Ltmin[2][j] > Ltop[2][j][d])
                                Ltmin[2][j] = Ltop[2][j][d];
                }
        }

        for (i = 1; i < h; i++)
        {
                uint16_t l1, l2, l3, l4, result;
                uint16_t *tmp;

                uint16_t LleftMin = UINT16_MAX;
                uint16_t LrightMin = UINT16_MAX;
                for (j = 0; j < dMax * w; j++)
                        Sline[j] = 0;

                for (d = 0; d < dMax; d++) {
                        Lleft [d] = Carray[d * h * w + i * w + 0];
                        if (Lleft [d] < LleftMin)
                                LleftMin = Lleft[d];
                        Lright[d] = Carray[d * h * w + i * w + w - 1];
                        if (Lright [d] < LrightMin)
                                LrightMin = Lright[d];
                }

                // Fill the line
                for (j = 1; j < w - 1; j++) {
                        uint16_t minTop = UINT16_MAX;
                        uint16_t minLU = UINT16_MAX;
                        uint16_t minRU = UINT16_MAX;
                        uint16_t minL = UINT16_MAX;

                        uint16_t oldminTop = Ltmin[0][j];
                        uint16_t oldminLU  = Ltmin[1][j - 1];
                        uint16_t oldminRU  = Ltmin[2][j + 1];

                        for (d = 0; d < dMax; d++) {
                                uint16_t cost = Carray[d * h * w + i * w + j];

                                // Vertical
                                l1 = (d > 0        ? (Ltop[0][j][d - 1] + closePenlty) : UINT16_MAX);
                                l2 = (d < dMax - 1 ? (Ltop[0][j][d + 1] + closePenlty) : UINT16_MAX);
                                l3 = Ltop[0][j][d];
                                l4 = oldminTop + longPenalty;
                                result =  min4_16(l1, l2, l3, l4) - oldminTop + cost;

                                if (result < minTop) {
                                        minTop = result;
                                }
                                Ltnew[0][d] = result;
                                Sline[j * dMax +  d] += result;
#ifdef ASSERTS
                                if (Sline[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow1");
#endif


                                // LU
                                l1 = (d > 0        ? (Ltop[1][j - 1][d - 1] + closePenlty) : UINT16_MAX);
                                l2 = (d < dMax - 1 ? (Ltop[1][j - 1][d + 1] + closePenlty) : UINT16_MAX);
                                l3 = Ltop[1][j - 1][d];
                                l4 = oldminLU + longPenalty;
                                result = min4_16 (l1, l2, l3, l4) - oldminLU + cost;

                                if (result < minLU) {
                                        minLU = result;
                                }
                                Ltnew[1][d] = result;
                                Sline[j * dMax +  d] += result;
#ifdef ASSERTS
                                if (Sline[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow2");
#endif


                                // RU
                                l1 = (d > 0        ? (Ltop[2][j + 1][d - 1] + closePenlty) : UINT16_MAX);
                                l2 = (d < dMax - 1 ? (Ltop[2][j + 1][d + 1] + closePenlty) : UINT16_MAX);
                                l3 = Ltop[2][j + 1][d];
                                l4 = oldminRU + longPenalty;
                                result = min4_16 (l1, l2, l3, l4) - oldminRU + cost;

                                if (result < minRU) {
                                        minRU = result;
                                }
                                Ltnew[2][d] = result;
                                Sline[j * dMax +  d] += result;
#ifdef ASSERTS
                                if (Sline[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow3");
#endif

                                // Also From Left
                                l1 = (d > 0        ? (Lleft[d - 1] + closePenlty) : UINT16_MAX);
                                l2 = (d < dMax - 1 ? (Lleft[d + 1] + closePenlty) : UINT16_MAX);
                                l3 = Lleft[d];
                                l4 = LleftMin + longPenalty;
                                result = min4_16 (l1, l2, l3, l4) - LleftMin + cost;

                                if (result < minL) {
                                        minL = result;
                                }
                                Ltnew[3][d] = result;
                                Sline[j * dMax +  d] += result;
#ifdef ASSERTS
                                if (Sline[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow4");
#endif
                        }

                        tmp = Ltop[0][j];
                        Ltop[0][j] = Ltnew[0];
                        Ltnew[0] = tmp;

                        tmp = Ltop[1][j];
                        Ltop[1][j] = Ltnew[1];
                        Ltnew[1] = tmp;

                        tmp = Ltop[2][j];
                        Ltop[2][j] = Ltnew[2];
                        Ltnew[2] = tmp;

                        tmp = Lleft;
                        Lleft = Ltnew[3];
                        Ltnew[3] = tmp;

                        Ltmin[0][j] = minTop;
                        Ltmin[1][j] = minLU;
                        Ltmin[2][j] = minRU;
                        LleftMin = minL;
                }

                // Also From Right
                for (j = w - 1; j >= 1; j--) {
                        uint16_t minR = UINT16_MAX;

                        for (d = 0; d < dMax; d++) {
                                uint16_t cost = Carray[d * h * w + i * w + j];

                                l1 = (d > 0        ? (Lright[d - 1] + closePenlty) : UINT16_MAX);
                                l2 = (d < dMax - 1 ? (Lright[d + 1] + closePenlty) : UINT16_MAX);
                                l3 = Lright[d];
                                l4 = LrightMin + longPenalty;
                                result = min4_16 (l1, l2, l3, l4) - LrightMin + cost;

                                if (result < minR) {
                                        minR = result;
                                }
                                Ltnew[4][d] = result;
                                Sline[j * dMax +  d] += result;
#ifdef ASSERTS
                                if (Sline[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow5");
#endif
                        }
                        tmp = Lright;
                        Lright = Ltnew[4];
                        Ltnew[4] = tmp;
                        LrightMin = minR;
                }



                for (j = 1; j < w - 1; j++) {
                        double min = 2 * (double)UINT32_MAX;
                        double minpos = -1;
                        for (d = 1; d < dMax - 2; d++)
                        {
#ifdef ASSERTS
                                if (Sline[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow5");
#endif

                                uint16_t result = Sline[j * dMax + d];
                                if (result < min)
                                {
                                        min = result;
                                        minpos = d;
                                }
                        }
#ifdef ASSERTS
                        if (minpos < 0 || minpos > dMax)
                                ASSERT_FAIL("INTERNAL ERROR");
#endif
                        toReturn->data[w * i + j] = ((minpos / dMax) * 255.0) * 16.0;
                }


        }

        dv_free(Carray);


        /*Allocate memory for the process*/
        dv_free (Sline);

        for (i = 0; i < 3; i++)
        {
                for (j = 0; j < w; j++ )
                        dv_free(Ltop[i][j]);
                dv_free(Ltop[i]);
        }

        for (i = 0; i < 5; i++)
                dv_free(Ltnew[i]);

        for (i = 0; i < 3; i++)
                dv_free(Ltmin[i]);

        dv_free(Lleft);
        dv_free(Lright);
        return toReturn;
}
#endif


#ifdef FAST_SGM
G12Buffer* sgmFastx86v1 (G12Buffer* left, G12Buffer* right, int dMax, int closePenalty, int longPenalty)
{
#ifdef ASSERTS
        if (left == NULL || right == NULL)
                ASSERT_FAIL("null parameters not supported");
        if (left->h != right->h || left->w != right->w)
                ASSERT_FAIL("Buffers have different sizes");
#endif

           int i,j,d;
        int h = left->h;
        int w = left->w;
        int isInverted = dMax < 0;
        dMax = isInverted ? -dMax : dMax;

        /*Allocate memory for the process*/
        uint32_t *SLine = (uint32_t *) dv_malloc (dMax * w * sizeof(uint32_t));

        uint32_t *LTopBlock;
        LTopBlock = (uint32_t *) dv_malloc (3 * dMax * w * sizeof(uint32_t));
        uint32_t **LTopPointersBlock;
        LTopPointersBlock = (uint32_t **) dv_malloc (3 * w * sizeof(uint32_t *));

        int p;
        for (p = 0; p < 3 * w; p++)
                LTopPointersBlock[p] = LTopBlock + dMax * p;


        uint32_t **LTopLeft  = LTopPointersBlock;
        uint32_t **LTop      = LTopPointersBlock + w;
        uint32_t **LTopRight = LTopPointersBlock + w * 2;


        uint32_t *LNewBlock;
        LNewBlock = (uint32_t *) dv_malloc (5 * dMax * sizeof(uint32_t));
        uint32_t *LNewTop      = LNewBlock;
        uint32_t *LNewTopLeft  = LNewBlock + dMax;
        uint32_t *LNewTopRight = LNewBlock + dMax * 2;
        uint32_t *LNewLeft     = LNewBlock + dMax * 3;
        uint32_t *LNewRight    = LNewBlock + dMax * 4;


        uint32_t *LTopMinBlock;
        LTopMinBlock = (uint32_t *) dv_malloc (3 * w * sizeof(uint32_t));

        uint32_t *LTopLeftMin  = LTopMinBlock;
        uint32_t *LTopMin      = LTopMinBlock + w;
        uint32_t *LTopRightMin = LTopMinBlock + 2 * w;


        uint32_t *LLeftBlock  = (uint32_t *) dv_malloc(sizeof(uint32_t) * dMax);
        uint32_t *LRightBlock = (uint32_t *) dv_malloc(sizeof(uint32_t) * dMax);

        uint32_t *LLeft  = LLeftBlock;
        uint32_t *LRight = LRightBlock;

        G12Buffer *toReturn = g12BufferCreate(h, w);

        /**
         *   For a top row compute the lowest cost for all the disparities
         **/
        for (j = 0; j < w ; j++)
        {
                LTopLeftMin[j]  = UINT32_MAX;
                LTopMin[j]      = UINT32_MAX;
                LTopRightMin[j] = UINT32_MAX;

                uint32_t min = UINT32_MAX;
                uint32_t cost;
                for (d = 0; d < dMax; d++)
                {
                        if (isInverted)
                                cost = C(right, left, j, 0, -d ,0);
                        else
                                cost = C(left, right, j, 0, d ,0);

                        LTop[j][d] = cost;
                        LTopLeft[j][d] = cost;
                        LTopRight[j][d] = cost;

                        if (min > cost)
                                min = cost;
                }

                LTopLeftMin[j]  = min;
                LTopMin[j]      = min;
                LTopRightMin[j] = min;
        }


        /**
         *  Iterate - row by row
         **/
        for (i = 1; i < h; i++)
        {
                uint32_t l1, l2, l3, l4, result;
                uint32_t *tmp;

                uint32_t LLeftMin = UINT32_MAX;
                uint32_t LRightMin = UINT32_MAX;

                memset(SLine, 0, dMax * w * sizeof(uint32_t));

                uint32_t cost;


                for (d = 0; d < dMax; d++)
                {
                        uint32_t tmpLeft;
                        uint32_t tmpRight;

                        if (isInverted)
                                tmpLeft = C(right, left, 0, i, -d ,0);
                        else
                                tmpLeft = C(left, right, 0, i, d ,0);

                        if (isInverted)
                                tmpRight = C(right, left, w - 1, i, -d ,0);
                        else
                                tmpRight = C(left, right, w - 1, i, d ,0);

                        LLeft [d] = tmpLeft;
                        if (tmpLeft < LLeftMin) LLeftMin = tmpLeft;

                        LRight[d] = tmpRight;
                        if (tmpRight < LRightMin) LRightMin = tmpRight;
                }

                // Fill the line
                for (j = 1; j < w - 1; j++) {
                        uint32_t minTop      = UINT32_MAX;
                        uint32_t minTopLeft  = UINT32_MAX;
                        uint32_t minTopRight = UINT32_MAX;
                        uint32_t minLeft     = UINT32_MAX;

                        uint32_t oldMinTop      = LTopMin[j];
                        uint32_t oldMinTopLeft  = LTopLeftMin[j - 1];
                        uint32_t oldMinTopRight  = LTopRightMin[j + 1];
                        uint32_t cost;
                        //uint32_t *costOffset = Carray + i * w * dMax + j * dMax;

                        uint32_t *LTopLine = LTop[j];
                        uint32_t *LTopRightLine = LTopRight[j + 1];
                        uint32_t *LTopLeftLine = LTopLeft[j - 1];


                        /* d = 0 */
                        //cost = *costOffset;
                        if (isInverted)
                                cost = C(right, left, j,i, 0 ,0);
                        else
                                cost = C(left, right, j,i, 0 ,0);
                        //costOffset++;

                        // Vertical
                        l2 = LTopLine[1] + closePenalty;
                        l3 = LTopLine[0];
                        l4 = oldMinTop + longPenalty;
                        result =  min3_32(l2, l3, l4) - oldMinTop + cost;
                        if (result < minTop) minTop = result;
                        LNewTop[0] = result;
                        SLine[j * dMax] += result;

                        // Left-Top direction
                        l2 = LTopLeftLine[1] + closePenalty;
                        l3 = LTopLeftLine[0];
                        l4 = oldMinTopLeft + longPenalty;
                        result = min3_32 (l2, l3, l4) - oldMinTopLeft + cost;
                        if (result < minTopLeft) minTopLeft = result;
                        LNewTopLeft[0] = result;
                        SLine[j * dMax] += result;

                        // Right-Top direction
                        l2 = LTopRightLine[1] + closePenalty;
                        l3 = LTopRightLine[0];
                        l4 = oldMinTopRight + longPenalty;
                        result = min3_32 (l2, l3, l4) - oldMinTopRight + cost;
                        if (result < minTopRight) minTopRight = result;
                        LNewTopRight[0] = result;
                        SLine[j * dMax] += result;

                        // Also From Left
                        l2 = LLeft[1] + closePenalty;
                        l3 = LLeft[0];
                        l4 = LLeftMin + longPenalty;
                        result = min3_32 (l2, l3, l4) - LLeftMin + cost;
                        if (result < minLeft) minLeft = result;
                        LNewLeft[0] = result;
                        SLine[j * dMax ] += result;

                        /* Conventional d*/

                        for (d = 1; d < dMax - 1; d++)
                        {
                                uint32_t addition = 0;
                                //cost = *costOffset;
                                //costOffset++;
                                if (isInverted)
                                        cost = C(right, left, j,i, -d ,0);
                                else
                                        cost = C(left, right, j,i, d ,0);

                                // Vertical
                                l1 = LTopLine[d - 1];
                                l2 = LTopLine[d + 1];
                                l3 = LTopLine[d];
                                l4 = oldMinTop + longPenalty;
                                result =  min4_32_special(l1, l2, l3, l4, closePenalty) - oldMinTop + cost;
                                if (result < minTop) minTop = result;
                                LNewTop[d] = result;
                                addition += result;
                                // Left-Top direction

                                l1 = LTopLeftLine[d - 1];
                                l2 = LTopLeftLine[d + 1];
                                l3 = LTopLeftLine[d];
                                l4 = oldMinTopLeft + longPenalty;
                                result = min4_32_special(l1, l2, l3, l4, closePenalty) - oldMinTopLeft + cost;
                                if (result < minTopLeft) minTopLeft = result;
                                LNewTopLeft[d] = result;
                                addition  += result;

                                // Right-Top direction
                                l1 = LTopRightLine[d - 1];
                                l2 = LTopRightLine[d + 1];
                                l3 = LTopRightLine[d];
                                l4 = oldMinTopRight + longPenalty;
                                result = min4_32_special(l1, l2, l3, l4, closePenalty) - oldMinTopRight + cost;
                                if (result < minTopRight) minTopRight = result;
                                LNewTopRight[d] = result;
                                addition  += result;

                                // Also From Left
                                l1 = LLeft[d - 1];
                                l2 = LLeft[d + 1];
                                l3 = LLeft[d];
                                l4 = LLeftMin + longPenalty;
                                result = min4_32_special(l1, l2, l3, l4, closePenalty) - LLeftMin + cost;
                                if (result < minLeft) minLeft = result;
                                LNewLeft[d] = result;
                                addition += result;

                                SLine[j * dMax +  d] += addition;
                        }

                        d = dMax - 1;
                        //cost = *costOffset;
                        //costOffset++;
                        if (isInverted)
                                cost = C(right, left, j,i, -d ,0);
                        else
                                cost = C(left, right, j,i, d ,0);


                        // Vertical
                        l1 = LTopLine[d - 1] + closePenalty;
                        l3 = LTopLine[d];
                        l4 = oldMinTop + longPenalty;
                        result =  min3_32(l1, l3, l4) - oldMinTop + cost;
                        if (result < minTop) minTop = result;
                        LNewTop[d] = result;
                        SLine[j * dMax +  d] += result;

                        // Left-Top direction

                        l1 = LTopLeftLine[d - 1] + closePenalty;
                        l3 = LTopLeftLine[d];
                        l4 = oldMinTopLeft + longPenalty;
                        result = min3_32 (l1, l3, l4) - oldMinTopLeft + cost;

                        if (result < minTopLeft) {
                                minTopLeft = result;
                        }
                        LNewTopLeft[d] = result;
                        SLine[j * dMax +  d] += result;

                        // Right-Top direction
                        l1 = LTopRightLine[d - 1] + closePenalty;
                        l3 = LTopRightLine[d];
                        l4 = oldMinTopRight + longPenalty;
                        result = min3_32 (l1, l3, l4) - oldMinTopRight + cost;
                        if (result < minTopRight) minTopRight = result;
                        LNewTopRight[d] = result;
                        SLine[j * dMax +  d] += result;

                        // Also From Left
                        l1 = LLeft[d - 1] + closePenalty;
                        l3 = LLeft[d];
                        l4 = LLeftMin + longPenalty;
                        result = min3_32 (l1, l3, l4) - LLeftMin + cost;
                        if (result < minLeft) minLeft = result;
                        LNewLeft[d] = result;
                        SLine[j * dMax +  d] += result;

                        tmp = LTop[j];
                        LTop[j] = LNewTop;
                        LNewTop = tmp;

                        tmp = LTopLeft[j];
                        LTopLeft[j] = LNewTopLeft;
                        LNewTopLeft = tmp;

                        tmp = LTopRight[j];
                        LTopRight[j] = LNewTopRight;
                        LNewTopRight = tmp;

                        tmp = LLeft;
                        LLeft = LNewLeft;
                        LNewLeft = tmp;

                        LTopMin[j] = minTop;
                        LTopLeftMin[j] = minTopLeft;
                        LTopRightMin[j] = minTopRight;
                        LLeftMin = minLeft;
                }

                // Also From Right
                for (j = w - 1; j >= 1; j--) {
                        uint32_t minRight = UINT32_MAX;

                        for (d = 0; d < dMax; d++) {
                                if (isInverted)
                                        cost = C(right, left, j,i, -d ,0);
                                else
                                        cost = C(left, right, j,i, d ,0);

                                l1 = (d > 0        ? (LRight[d - 1] + closePenalty) : UINT32_MAX);
                                l2 = (d < dMax - 1 ? (LRight[d + 1] + closePenalty) : UINT32_MAX);
                                l3 = LRight[d];
                                l4 = LRightMin + longPenalty;
                                result = min4_32 (l1, l2, l3, l4) - LRightMin + cost;

                                if (result < minRight) {
                                        minRight = result;
                                }
                                LNewRight[d] = result;
                                SLine[j * dMax +  d] += result;
                        }
                        tmp = LRight;
                        LRight = LNewRight;
                        LNewRight = tmp;

                        LRightMin = minRight;
                }



                for (j = 1; j < w - 1; j++) {
                        double min = 2 * (double)UINT32_MAX;
                        double minpos = -1;
                        for (d = 1; d < dMax - 2; d++)
                        {
#ifdef ASSERTS
                                if (SLine[j * dMax + d] >= (1 << 16))
                                        ASSERT_FAIL("Fast SGM overflow5");
#endif

                                uint16_t result = SLine[j * dMax + d];
                                if (result < min)
                                {
                                        min = result;
                                        minpos = d;
                                }
                        }
#ifdef ASSERTS
                        if (minpos < 0 || minpos > dMax)
                                ASSERT_FAIL("INTERNAL ERROR");
#endif
                        toReturn->data[w * i + j] = ((minpos / dMax) * 255.0) * 16.0;
                }


        }

        /*Deallocate memory after the process*/
        dv_free(SLine);
        dv_free(LTopBlock);
        dv_free(LTopPointersBlock);
        dv_free(LNewBlock);
        dv_free(LTopMinBlock);
        dv_free(LLeftBlock);
        dv_free(LRightBlock);
        return toReturn;
}
#endif


/**
 * \file algebra.c
 * \brief This files contains the fast implementation for algebraic functions, such as sqrt(), atan2(), etc.
 *
 *
 *  \ingroup corefiles
 *  \date Jul 10, 2009
 *  \author Alexander Pimenov
 */
#include "algebra.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

uint16_t rootLUT[128] = {
            0 /*   0 */,     1 /*   1 */,     4 /*   2 */,     9 /*   3 */,
           16 /*   4 */,    25 /*   5 */,    36 /*   6 */,    49 /*   7 */,
           64 /*   8 */,    81 /*   9 */,   100 /*  10 */,   121 /*  11 */,
          144 /*  12 */,   169 /*  13 */,   196 /*  14 */,   225 /*  15 */,
          256 /*  16 */,   289 /*  17 */,   324 /*  18 */,   361 /*  19 */,
          400 /*  20 */,   441 /*  21 */,   484 /*  22 */,   529 /*  23 */,
          576 /*  24 */,   625 /*  25 */,   676 /*  26 */,   729 /*  27 */,
          784 /*  28 */,   841 /*  29 */,   900 /*  30 */,   961 /*  31 */,
         1024 /*  32 */,  1089 /*  33 */,  1156 /*  34 */,  1225 /*  35 */,
         1296 /*  36 */,  1369 /*  37 */,  1444 /*  38 */,  1521 /*  39 */,
         1600 /*  40 */,  1681 /*  41 */,  1764 /*  42 */,  1849 /*  43 */,
         1936 /*  44 */,  2025 /*  45 */,  2116 /*  46 */,  2209 /*  47 */,
         2304 /*  48 */,  2401 /*  49 */,  2500 /*  50 */,  2601 /*  51 */,
         2704 /*  52 */,  2809 /*  53 */,  2916 /*  54 */,  3025 /*  55 */,
         3136 /*  56 */,  3249 /*  57 */,  3364 /*  58 */,  3481 /*  59 */,
         3600 /*  60 */,  3721 /*  61 */,  3844 /*  62 */,  3969 /*  63 */,
         4096 /*  64 */,  4225 /*  65 */,  4356 /*  66 */,  4489 /*  67 */,
         4624 /*  68 */,  4761 /*  69 */,  4900 /*  70 */,  5041 /*  71 */,
         5184 /*  72 */,  5329 /*  73 */,  5476 /*  74 */,  5625 /*  75 */,
         5776 /*  76 */,  5929 /*  77 */,  6084 /*  78 */,  6241 /*  79 */,
         6400 /*  80 */,  6561 /*  81 */,  6724 /*  82 */,  6889 /*  83 */,
         7056 /*  84 */,  7225 /*  85 */,  7396 /*  86 */,  7569 /*  87 */,
         7744 /*  88 */,  7921 /*  89 */,  8100 /*  90 */,  8281 /*  91 */,
         8464 /*  92 */,  8649 /*  93 */,  8836 /*  94 */,  9025 /*  95 */,
         9216 /*  96 */,  9409 /*  97 */,  9604 /*  98 */,  9801 /*  99 */,
        10000 /* 100 */, 10201 /* 101 */, 10404 /* 102 */, 10609 /* 103 */,
        10816 /* 104 */, 11025 /* 105 */, 11236 /* 106 */, 11449 /* 107 */,
        11664 /* 108 */, 11881 /* 109 */, 12100 /* 110 */, 12321 /* 111 */,
        12544 /* 112 */, 12769 /* 113 */, 12996 /* 114 */, 13225 /* 115 */,
        13456 /* 116 */, 13689 /* 117 */, 13924 /* 118 */, 14161 /* 119 */,
        14400 /* 120 */, 14641 /* 121 */, 14884 /* 122 */, 15129 /* 123 */,
        15376 /* 124 */, 15625 /* 125 */, 15876 /* 126 */, 16129 /* 127 */
};


uint8_t getDirection5bit(int16_t dx, int16_t dy)
{
        uint8_t accumulator = 0;
        int16_t rx;
        int16_t ry;
        /* \todo TODO: Make this faster*/
        // Reduce the circle to the 1st quadrant
        if (dx < 0)
        {
                if (dy < 0)
                {
                        accumulator = 16;
                        rx = -dx;
                        ry = -dy;
                }
                else
                {
                        accumulator = 8;
                        rx =  dy;
                        ry = -dx;
                }
        }
        else
        {
                if (dy < 0)
                {
                        accumulator = 24;
                        rx = -dy;
                        ry = dx;
                }
                else
                {
                        accumulator = 0;
                        rx = dx;
                        ry = dy;
                }
        }

        // Do the tree search
        if (ry <= rx)
        {
                /// @todo So far use double. Later no double should be used.
                if (rx <= 0.414213562 * ry)
                {
                        if (rx <= 0.198912367 * ry)
                                return accumulator;
                        else
                                return accumulator + 1;
                }
                else
                {
                        if (rx <= 0.668178638 * ry)
                                return accumulator + 2;
                        else
                                return accumulator + 3;
                }
        }
        else
        {
                if (ry <= 0.414213562 * rx)
                {
                        if (ry <= 0.198912367 * rx)
                                return accumulator + 7;
                        else
                                return accumulator + 6;
                }
                else
                {
                        if (ry <= 0.668178638 * rx)
                                return accumulator + 5;
                        else
                                return accumulator + 4;
                }
        }
}

/**
 * Expecting two Sobel derivatives of the 12bit image
 *   This will make the range 12 + 3 bits for the
 *   derivatives (due to Sobel matrix coefficients)
 *
 *  \param dx - X Sobel derivative
 *  \param dy - Y Sobel derivative
 *
 **/
uint8_t getMagnitude7bit(int16_t dx, int16_t dy)
{
    //                        24bit      +       24bit          = 25bit
        uint32_t distsq = ((int32_t)dx * dx) + ((int32_t)dy * dy);
        //   reduce to 14bits
        distsq >>= 11;
        //   And now use search in LUT
        uint8_t start = 0, end = 127;
        while (start != end) {
                uint8_t middle = (start + end) >> 1;
                if (rootLUT[middle] == distsq)
                        return middle;
                if (rootLUT[middle] > distsq)
                {
                        end = middle - 1;
                }
                else
                {
                        start = middle + 1;
                }
        }
        return start;
}


double getLengthStable (double a, double b)
{
        double ma = (a > 0) ? a : -a;
        double mb = (b > 0) ? b : -b;

        if (ma == 0 && mb == 0) return 0;
        if (ma > mb)
        {
                double div = (mb / ma);
                return ma * sqrt(1.0 + div * div );
        }
        else
        {
                double div = (ma / mb);
                return mb * sqrt(1.0 + div * div );
        }
}


/**
 *
 *  The fast approximation of the square root with the method similar to Newton's
 *
 *
 *  */
inline uint32_t getIntegerRootFast(uint32_t input)
{
        uint32_t x;
        x = (input / 0x3f + 0x3f) >> 1;
        x = (input / x + x) >> 1;
        x = (input / x + x) >> 1;
/*        x = (input / x + x) >> 1;
        x = (input / x + x) >> 1;
        x = (input / x + x) >> 1;
        x = (input / x + x) >> 1;*/
        return x;
}

/**
 * As 1/25 is
 *   0b0.(00001010001111010111)
 *
 * The period is presented as follows:
 *
 * |1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20|21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
 * |0  0  0  0  1  0  1  0  0  0  1  1  1  1  0  1  0  1  1  1| 0  0  0  0  1  0  1  0  0  0  1  1  1  1  0  1  0  1  1  1
 *
 *
 * the division result could be computed as the addition of the shifted values.
 *
 * This version is for 16bit
 *
 */
inline uint16_t divideBy25_16 (uint16_t in)
{
        uint16_t in0_101 = (in >> 1)  + (in >> 3);
        uint16_t in0_111 = in0_101 + (in >> 2);
        uint16_t result = ((in0_101) + (in0_101 >> 9) + (in0_111 >> 6) + (in0_111 >> 13)) >> 4;

        uint16_t r = in - result * 25;
        if (r >= 25) result++;
        return result;
}

/**
 * As 1/25 is
 *   0b0.(00001010001111010111)
 *
 * The period is presented as follows:
 *
 * |1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20|21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
 * |0  0  0  0  1  0  1  0  0  0  1  1  1  1  0  1  0  1  1  1| 0  0  0  0  1  0  1  0  0  0  1  1  1  1  0  1  0  1  1  1
 *
 *
 * the division result could be computed as the addition of the shifted values.
 *
 * This version is for 32bit
 *
 */
inline uint32_t divideBy25_32 (uint32_t in)
{
        uint32_t in0_101 = (in >> 1)  + (in >> 3);
        uint32_t in0_111 = in0_101 + (in >> 2);
        uint32_t in0_00001010001111010111 = ((in0_101) + (in0_101 >> 9) + (in0_111 >> 6) + (in0_111 >> 13)) >> 4;
        uint32_t result = (in0_00001010001111010111) + (in0_00001010001111010111 >> 20);

        uint16_t r = in - result * 25;
        if (r >= 25) result++;
        if (r >= 50) result++;
        return result;
}

#define TABLE_SIZE 512
#define MAX_FUCTION_VALUE  (0xFFFFFF)
#define MIN_FUCTION_VALUE  (0)

#define SLOPE_MULTIPLIER_SHIFT (23)


uint32_t sqare_root_lut[TABLE_SIZE];
/*
void squareRootLUTPrepare ()
{
        uint32_t i;
        for (i = 0; i < TABLE_SIZE; i++)
        {
                uint32_t start = i * (MAX_SQRT_VALUE / TABLE_SIZE);
                uint32_t end   = (i + 1)  * (MAX_SQRT_VALUE / TABLE_SIZE);

                uint32_t sqrtstart = sqrt((double) start);
                uint32_t sqrtend = sqrt((double) end);
                double slope = ((double)sqrtend - sqrtstart) / ((double)MAX_SQRT_VALUE / TABLE_SIZE);
                uint32_t packedStart = (uint32_t) sqrtstart;
                uint32_t packedSlope = (uint32_t) (slope * (1 << SLOPE_MULTIPLIER_SHIFT));
                sqare_root_lut[i] = (packedStart << 16) | packedSlope;

                printf("Start = %u, End = %u, slope = %2.5lf, 0x%x\n ", start, end, slope, packedSlope);
        }

}

uint32_t squareRoot (uint32_t value)
{
        if (value > MAX_SQRT_VALUE)
                return 0;
        uint32_t packedId = (value >> (24 - 9));
        uint32_t packed = sqare_root_lut[packedId];
        uint32_t packedStart = packed >> 16;
        uint32_t packedSlope = packed & 0xFFFF;
        uint32_t result = packedStart + ((packedSlope * (value & 0x7FFF)) >> SLOPE_MULTIPLIER_SHIFT);
        printf("packedId = %u\n", packedId);
        return result;
}*/
/*
int main (void)
{
        squareRootLUTPrepare();
        unsigned i;
        for (i = 0; i < TABLE_SIZE; i++)
        {
                uint32_t st = sqare_root_lut[i];
                printf("%u %u => %u %u %u\n", i, st, i * (0xFFF / TABLE_SIZE), st >> 16, st & 0xFFFF);
        }

        for (i = 0; i < 0xFFFFFF; i+=0xFFFFF )
        {
                printf("%u %u %u\n", i, (uint32_t)sqrt(i), squareRoot(i));

        }

        return 0;
}*/

/*
int main (void)
{
        uint32_t i;
        for (i = 0; i < 0xFFFFFFFF; i+=1)
        {
                uint32_t real = i / 25;
                uint32_t ours = divideBy25_32(i);
                if (real != ours)
                        printf("!!!%u (%u : %u) %u\n", i, ours, i - ours * 25, real);
                if ((i % 1000000) == 0)
                        printf("Processing %u (%lf)\n", i, (double)i / (0xFFFFFFFF) * 100.0);
        }
        return 0;
}
*/
/*
#define main_test_root main
#define TEST_CYCLES 1000

int main_test_root (void)
{
        int i,j;
        long int time, startTime;
        printf("Testing quality...\n");
        int maxdiff = 0;
        double meandiff = 0;
        for (i = 1; i < 0xFFFF; i++)
        {
                int res = getRootFast(i);
                double root = sqrt(i);
                int real = round(root);
                int diff = res > real ? res - real : real - res;
                if (diff > maxdiff)
                        maxdiff = diff;
                meandiff += diff;
                printf("%d ", diff);
        }

        meandiff /= 0xFFFF;

        printf("Max diff %d mean = %lf\n", maxdiff, meandiff);

        int saver = 1;
        printf("Speed test...\n");
        startTime = clock();
        for (j = 0; j < TEST_CYCLES ; j++)
        {
                for (i = 1; i < 0xFFFF - TEST_CYCLES; i++)
                {
                        int res = getRootFast(i+j);
                        saver += res;
                }
        }
        time = clock() - startTime;
        printf("Our version   : %d\n", time);



        startTime = clock();
        for (j = 0; j < TEST_CYCLES ; j++)
        {
                for (i = 1; i < 0xFFFF - TEST_CYCLES; i++)
                {
                        double real = sqrt(i+j);
                        saver += real;
                }
        }
        time = clock() - startTime;
        printf("Double version: %d\n", time);

        return saver > 0 ? 1 : 0;
}
*/

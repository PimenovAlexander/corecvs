#pragma once
/**
 * \file localHistogram.h
 *
 * \date Oct 18, 2013
 **/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "abstractBuffer.h"

using corecvs::AbstractBuffer;

class LocalHistogram
{
public:
    static const int SIZE = 8;
    uint8_t data[SIZE];

    LocalHistogram()
    {
        memset(data, 0, SIZE * sizeof(data[0]));
    }

    inline void inc(uint8_t argument)
    {
        int val = (int)argument * SIZE / 255;
        if (val >= 0 && val < SIZE)
        {
            if (data[val] != 255)
            {
    //            printf("%d ", val);
                data[val]++;
            }
        }
    }

};

class LocalHistogram2D
{
public:
    static const int SIZE1 = 8;
    static const int SIZE2 = 4;

    uint8_t data[SIZE1 * SIZE2];
    bool isSet;

    LocalHistogram2D()
    {
        memset(this, 0, sizeof(LocalHistogram2D));
    }

    inline void inc(uint8_t argument1, uint8_t argument2)
    {
        int val1 = (int)argument1 * SIZE1 / 255;
        int val2 = (int)argument2 * SIZE2 / 255;

        if (val1 >= 0 && val1 < SIZE1 && val2 >= 0 && val2 < SIZE2 )
        {
            int offset = val1 * SIZE2 + val2;
            if (data[offset] != 255)
            {
    //            printf("%d ", val);
                data[offset]++;
            }
        }
    }
};


typedef AbstractBuffer<LocalHistogram>   HistogramBuffer;
typedef AbstractBuffer<LocalHistogram2D> Histogram2DBuffer;



/* EOF */

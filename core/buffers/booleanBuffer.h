#pragma once

/**
 * \file booleanBuffer.h
 *
 * \date Sep 28, 2013
 **/

#include "abstractContiniousBuffer.h"
#include "g8Buffer.h"

namespace corecvs
{

#if 0
class BooleanAccessor {
private:
    uint8_t *mData;
    int mBit;

    BooleanAccessor(
            uint8_t *mData;
            int mBit;
    )

};
#endif

typedef AbstractContiniousBuffer<uint8_t, int32_t> BooleanBufferBase;


class BooleanBuffer : public BooleanBufferBase
{
public:
    int effectiveH;
    int effectiveW;


    static int bytesFromBits (int bitNumber)
    {
        return (bitNumber - 1) / 8 + 1;
    }

    static void printByte(uint8_t byte)
    {
        for(int i = 0; i < 8; i++)
        {
            printf("%c", (byte & 0x80) ? '1' : '0');
            byte <<= 1;
        }
    }


    BooleanBuffer(int32_t h, int32_t w, bool shouldInit = true) :
          BooleanBufferBase (h, bytesFromBits(w), shouldInit)
        , effectiveH(h)
        , effectiveW(w)
    {};

    BooleanBuffer(G8Buffer *that) :
          BooleanBufferBase (that->h, bytesFromBits(that->w), false)
        , effectiveH(that->h)
        , effectiveW(that->w)
    {
        for (int i = 0; i < that->h; i++ )
        {
            uint8_t *resultLine = &this->element(i,0);
            uint8_t acc = 0;
            int subcount = 0;

            for (int j = 0; j < that->w; j++, subcount = ((subcount + 1) & 0x7))
            {
                bool bit = (that->element(i,j) == G8Buffer::InternalElementType(0));
                acc <<= 1;
                acc |= (bit ? 0x0 : 0x1);
                if (subcount == 0x7) {
                    *resultLine = acc;
                    resultLine++;
                    acc = 0;
                }
            }
            if (subcount != 0x7) {
                acc <<= 8 - subcount;
                *resultLine = acc;
            }

            /* TODO: Finish line */
        }
    }

    G8Buffer *unpack(G8Buffer::InternalElementType false_id =  G8Buffer::InternalElementType(0),
                     G8Buffer::InternalElementType true_id  = ~G8Buffer::InternalElementType(0) )
    {
        G8Buffer *toReturn = new G8Buffer(effectiveH, effectiveW, false);
        for (int i = 0; i < effectiveH; i++ )
        {
           uint8_t *inputLine = &this->element(i,0);
           for (int j = 0; j < effectiveW; j++ )
           {
               toReturn->element(i, j) = (((*inputLine) << (j % 8)) & 0x80) ? true_id : false_id;
               if ((j & 0x7) == 0x7)
               {
                   inputLine++;
               }

           }
        }
        return toReturn;
    }

    void printBuffer()
    {
        for (int i = 0; i < h; i++ )
        {
           uint8_t *inputLine = &this->element(i,0);
           for (int j = 0; j < w; j++ )
           {
               printByte(*inputLine);
               printf(" ");
               inputLine++;
           }
           printf("\n");
        }
    }

};

} /* namespace corecvs */
/* EOF */

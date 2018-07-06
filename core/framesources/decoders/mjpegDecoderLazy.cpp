/**
 * \file MjpegDecoderLazy.cpp
 * \brief Add Comment Here
 *
 * \date Nov 4, 2010
 * \author alexander
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core/utils/global.h"

//#define _TRACE
#ifdef _TRACE
#define _DOTRACE(X) printf X
#else
#define _DOTRACE(X)
#endif

#include "mjpegDecoderLazy.h"

uint8_t MjpegDecoderLazy::JPEGHuffmanTable[MjpegDecoderLazy::JPG_HUFFMAN_TABLE_LENGTH] = {
    0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0A, 0x0B, 0x01, 0x00, 0x03,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01,
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x10,
    0x00, 0x02, 0x01, 0x03, 0x03,
    0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
    0x01, 0x02, 0x03, 0x00, 0x04,
    0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81,
    0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
    0x24, 0x33, 0x62, 0x72, 0x82,
    0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x53, 0x54, 0x55, 0x56,
    0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3,
    0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA,
    0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA, 0x11, 0x00, 0x02,
    0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00,
    0x01, 0x02, 0x77, 0x00, 0x01,
    0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51,
    0x07, 0x61, 0x71, 0x13, 0x22,
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23,
    0x33, 0x52, 0xF0, 0x15, 0x62,
    0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18,
    0x19, 0x1A, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4A,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65,
    0x66, 0x67, 0x68, 0x69, 0x6A,
    0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84,
    0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2,
    0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
    0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xE2, 0xE3,
    0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5,
    0xF6, 0xF7, 0xF8, 0xF9, 0xFA
};


unsigned char MjpegDecoderLazy::CoefZigZag[64] = {
    0,   1,  5,  6, 14, 15, 27, 28,
    2,   4,  7, 13, 16, 26, 29, 42,
    3,   8, 12, 17, 25, 30, 41, 43,
    9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

unsigned char MjpegDecoderLazy::CoefZigZag2[64] = {
     0,  2,  3,  9, 10, 20, 21, 35,
    14, 16, 25, 31, 39, 46, 50, 57,
     5,  7, 12, 18, 23, 33, 37, 48,
    27, 29, 41, 44, 52, 55, 59, 62,
    15, 26, 30, 40, 45, 51, 56, 58,
     1,  4,  8, 11, 19, 22, 34, 36,
    28, 42, 43, 53, 54, 60, 61, 63,
     6, 13, 17, 24, 32, 38, 47, 49
};


#define IFIX(a) ((int)((a) * (1 << 11) + .5))
int MjpegDecoderLazy::aaidct[8] = {
    IFIX(0.3535533906), IFIX(0.4903926402),
    IFIX(0.4619397663), IFIX(0.4157348062),
    IFIX(0.3535533906), IFIX(0.2777851165),
    IFIX(0.1913417162), IFIX(0.0975451610)
};

#define S22 ((int)IFIX(2 * 0.382683432))
#define C22 ((int)IFIX(2 * 0.923879532))
#define IC4 ((int)IFIX(1 / 0.707106781))


MjpegDecoderLazy::MjpegDecoderLazy()
    : comps(NULL)
    , scans(NULL)
{}

int MjpegDecoderLazy::huffman_init()
{
    int tc, th, tt;
    const unsigned char *ptr= JPEGHuffmanTable ;
    int i, l;
    unsigned j;
    l = JPG_HUFFMAN_TABLE_LENGTH ;
    while (l > 0) {
        unsigned hufflen[16], k;
        unsigned char huffvals[256];

        tc = *ptr++;
        th = tc & 15;
        tc >>= 4;
        tt = tc * 2 + th;
        if (tc > 1 || th > 1)
            return -ERR_BAD_TABLES;
        for (i = 0; i < 16; i++)
            hufflen[i] = *ptr++;
        l -= 1 + 16;
        k = 0;
        for (i = 0; i < 16; i++) {
            for (j = 0; j < hufflen[i]; j++)
                huffvals[k++] = *ptr++;
            l -= hufflen[i];
        }
        _DOTRACE(("Creating predefined huffman lookup table:"));
        dec_makehuff(dhuff + tt, hufflen, huffvals);
    }
    return 0;
}

int  MjpegDecoderLazy::readtables(int till, int *isDHT)
{
    int m, l, i, lq, pq, tq;
    unsigned j;
    int tc, th, tt;

    for (;;) {
        if (parser.getbyte() != 0xff) {
            return -1;
        }
        nextbyte:
        if ((m = parser.getbyte()) == till)
            break;

        switch (m) {
        case 0xc2:
            return 0;

        case M_DQT:
            //printf("find DQT \n");
            lq = parser.getword();
            while (lq > 2) {
                pq = parser.getbyte();
                tq = pq & 0xF;
                if (tq > 3)
                {
                    return -1;
                }
                pq >>= 4;
                if (pq != 0)
                {
                    return -1;
                }
                for (i = 0; i < 64; i++)
                    quants[tq][i] = parser.getbyte();
                lq -= 64 + 1;
            }
            break;

    case M_DHT:
            //printf("find DHT \n");
            l = parser.getword();
            while (l > 2) {
                unsigned hufflen[16], k;
                unsigned char huffvals[256];

                tc = parser.getbyte();
                th = tc & 15;
                tc >>= 4;
                tt = tc * 2 + th;
                if (tc > 1 || th > 1)
                {
                    return -1;
                }
                for (i = 0; i < 16; i++)
                    hufflen[i] = parser.getbyte();
                l -= 1 + 16;
                k = 0;
                for (i = 0; i < 16; i++) {
                    for (j = 0; j < hufflen[i]; j++)
                        huffvals[k++] = parser.getbyte();
                    l -= hufflen[i];
                }
                dec_makehuff(dhuff + tt, hufflen, huffvals);
            }
            *isDHT= 1;
            break;

    case M_DRI:
            _DOTRACE(("find DRI \n"));
            l = parser.getword();
            info.dri = parser.getword();
            break;
    case 0xff:
            goto nextbyte;
            break;

    default:
            l = parser.getword();
            while (l-- > 2)
                parser.getbyte();
            break;
        }
    }

    return 0;
}

void MjpegDecoderLazy::idctqtab(uint8_t *qin, int *qout)
{
    int i, j;

    const uint8_t *offset = CoefZigZag;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            qout[*offset] = qin[*offset] * ((aaidct[i] * aaidct[j]) >> 11);
            offset++;
        }
    }
}

/**
 *  Construct the huffman lookup tables form the
 *  length and values.
 *
 *  hufflen will hold 16 entries that hold the number of the codes coded to particular
 *  length
 *
 *  huffvals - will hold the array of the values sorted lexicographically by the code
 *
 * */
void MjpegDecoderLazy::dec_makehuff(HuffmanTable *hu, unsigned * hufflen, unsigned char *huffvals)
{
    _DOTRACE(("Creating Huffman lookup table %x\n", hu->table));
    memset(hu->table, 0, (1 << 16) * sizeof(HuffmanTableEntry));

    unsigned code = 0;

#ifdef _TRACE
    printf("Raw length:\n");
    for (unsigned i = 1; i < 16; i++, code <<= 1)
    {
        printf("%d ", hufflen[i]);
    }
    printf("\n");
#endif

    int count = 0;
    for (unsigned i = 1; i <= 16; i++, code <<= 1) {  /* sizes */
        /* Go through the codes of particular length */
        for (unsigned j = 0; j < hufflen[i-1]; j++) {

            unsigned char currentValue = *huffvals++;
            uint32_t c = code << (16 - i);

            unsigned d;

            /*TODO: make a cycle simpler */
            for (d = 0; d < (unsigned)(1 << (16 - i)); d++) {
                hu->table[c | d].length = i;
                hu->table[c | d].value  = currentValue;
                count++;
            }

#ifdef _TRACE
            printf("%4d -> ", currentValue);
            for (int tst = 0; tst < i; tst++)
            {
                printf("%s ", (c << tst) & 0x8000 ? "1" : "0");
            }
            printf("\n");
#endif
            code++;
        }
    }

#ifdef DEEP_TRACE
    printf("Totally filled %d lines\n", code);

    printf("Table dump\n");
    for (unsigned i = 0; i < (1 << 16); i++)
    {
        unsigned c = i;
        unsigned length = hu->table[i].length;
        for (int tst = 0; tst < 16; tst++)
        {
            printf("%s%s", (c << tst) & 0x8000 ? "1" : "0", length == (tst + 1)? "|" : " ");
        }
        printf("  %d %d %d\n", i, hu->table[i].value, hu->table[i].length);
    }
    printf("\n");
#endif
}


inline int MjpegDecoderLazy::HuffmanParser::fillbits()
{
   if (marker)
   {
       return left;
   }
   /* while the buffer is not yet filled */
   while (left <= 24) {
       /*get new byte*/
       uint8_t b = *datap++;
       uint8_t m;
       /* Check if it is a marker */
       if (b == 0xff) {
           if ((m = *datap++) != 0) {
               //printf("Marker %d\n", marker);
               marker = m;
               break;
           }
       }
       bits |= (b << (24 - left));
       left += 8;
   }
    return left;
}


inline int MjpegDecoderLazy::HuffmanParser::getValue(HuffmanTable *tbl)
{
    fillbits();
    uint16_t code = bits >> 16;
    uint8_t value  = tbl->table[code].value;
    uint8_t length = tbl->table[code].length;

#ifdef _TRACE
    printf("%8x : ", tbl->table);
    for (int tst = 0; tst < length; tst++)
    {
       printf("%s ", (code << tst) & 0x8000 ? "1" : "0");
    }
    printf(" -> %d\n", value);
#endif

    left -= length;
    bits <<= length;
    return value;
}

inline int MjpegDecoderLazy::HuffmanParser::getBits(int number)
{
    fillbits();
    if (number == 0)
        return 0;
    uint32_t value = bits >> (32 - number);
    bits <<= number;
    left -=  number;

#ifdef _TRACE
    printf("Addbits  : ");
    for (int tst = 0; tst < number; tst++)
    {
        printf("%s ", (value << tst) & (1 << (number - 1)) ? "1" : "0");
    }
#endif
    if (!(value & (1 << (number - 1)))) {
        value += (-1 << number) + 1;
    }
#ifdef _TRACE
    printf(" to %d\n", value);
#endif
    return value;
}


inline int MjpegDecoderLazy::HuffmanParser::readMarker()
{
    fillbits();
    int toReturn = marker;
    marker = 0;
    left = 0;
    bits = 0;
    fillbits();
    return toReturn;
}

int MjpegDecoderLazy::dec_checkmarker(void)
{
    int i;
    int marker = hparser.readMarker();
    if (marker != info.rm)
        return -1;
    info.nm = info.dri;
    info.rm = (info.rm + 1) & ~(0x8);
    for (i = 0; i < info.ns; i++)
        scans[i].dc = 0;
    return 0;
}


/**
 *
 *  This functions decodes   JPEG Minimum Coded Unit  form  huffman compressed stream
 *
 *
 * */
void MjpegDecoderLazy::decode_mcus(int *dct, int n, ScanData *sc)
{
    int t;

    memset(dct, 0, n * 64 * sizeof(*dct));

    while (n-- > 0) {

        int *dctend = dct + 64;

        int dcBits = hparser.getValue(sc->hudc);
        int dcValue = hparser.getBits(dcBits);
        sc->dc += dcValue;
        _DOTRACE(("DC %d\n", dcValue));

        *dct++ = sc->dc;

        while (dct < dctend) {
            t = hparser.getValue(sc->huac);
            unsigned r = t >> 4;
            unsigned s = t & 0xF;
            if (r == 0 && s == 0)
            {
                dct = dctend;
                _DOTRACE(("block Ended\n"));
                break;
            }
            dct += r;
            int acValue = hparser.getBits(s);
            *dct++ = acValue;
            _DOTRACE(("YAC %d skipped %d\n", acValue, r));
        }


#ifdef _TRACE
        printf("Raw block\n");
          for (unsigned j = 0; j < 8; j++) {
              for (unsigned k = 0; k < 8; k++) {
                  printf("%4d ",((unsigned *)(dct - 64))[j * 8 + k]);
              }
              printf("\n");
          }
#endif

        if (n == sc->next)
            sc++;
    }


}


#define IMULT(a, b) (((a) * (b)) >> 11)
#define ITOINT(a) ((a) >> 11)

void MjpegDecoderLazy::idct(int *in, int *out, int *quant, long off)
{

     long t0, t1, t2, t3, t4, t5, t6, t7;
     long tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
     long tmp[64], *tmpp;

    /*
     int32_t t0, t1, t2, t3, t4, t5, t6, t7; // t ;
     int32_t tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
     int32_t tmp[64], *tmpp;
    */

     int i, j, te;
     unsigned char *zig2p;

     t0 = off;
/*     if (max == 1) {
         t0 += in[0] * quant[0];
         for (i = 0; i < 64; i++)
             out[i] = ITOINT(t0);
         return;
     }*/
     zig2p = CoefZigZag2;
     tmpp = tmp;
     for (i = 0; i < 8; i++) {
         j = *zig2p++;
         t0 += in[j] * (long) quant[j];
         j = *zig2p++;
         t5 = in[j] * (long) quant[j];
         j = *zig2p++;
         t2 = in[j] * (long) quant[j];
         j = *zig2p++;
         t7 = in[j] * (long) quant[j];
         j = *zig2p++;
         t1 = in[j] * (long) quant[j];
         j = *zig2p++;
         t4 = in[j] * (long) quant[j];
         j = *zig2p++;
         t3 = in[j] * (long) quant[j];
         j = *zig2p++;
         t6 = in[j] * (long) quant[j];


         if ((t1 | t2 | t3 | t4 | t5 | t6 | t7) == 0) {

             tmpp[0 * 8] = t0;
             tmpp[1 * 8] = t0;
             tmpp[2 * 8] = t0;
             tmpp[3 * 8] = t0;
             tmpp[4 * 8] = t0;
             tmpp[5 * 8] = t0;
             tmpp[6 * 8] = t0;
             tmpp[7 * 8] = t0;

             tmpp++;
             t0 = 0;
             continue;
         }
         //IDCT;
         tmp0 = t0 + t1;
         t1 = t0 - t1;
         tmp2 = t2 - t3;
         t3 = t2 + t3;
         tmp2 = IMULT(tmp2, IC4) - t3;
         tmp3 = tmp0 + t3;
         t3 = tmp0 - t3;
         tmp1 = t1 + tmp2;
         tmp2 = t1 - tmp2;
         tmp4 = t4 - t7;
         t7 = t4 + t7;
         tmp5 = t5 + t6;
         t6 = t5 - t6;
         tmp6 = tmp5 - t7;
         t7 = tmp5 + t7;
         tmp5 = IMULT(tmp6, IC4);
         tmp6 = IMULT((tmp4 + t6), S22);
         tmp4 = IMULT(tmp4, (C22 - S22)) + tmp6;
         t6 = IMULT(t6, (C22 + S22)) - tmp6;
         t6 = t6 - t7;
         t5 = tmp5 - t6;
         t4 = tmp4 - t5;

         tmpp[0 * 8] = tmp3 + t7;    //t0;
         tmpp[1 * 8] = tmp1 + t6;    //t1;
         tmpp[2 * 8] = tmp2 + t5;    //t2;
         tmpp[3 * 8] = t3 + t4;  //t3;
         tmpp[4 * 8] = t3 - t4;  //t4;
         tmpp[5 * 8] = tmp2 - t5;    //t5;
         tmpp[6 * 8] = tmp1 - t6;    //t6;
         tmpp[7 * 8] = tmp3 - t7;    //t7;
         tmpp++;
         t0 = 0;
     }

     for (i = 0, j = 0; i < 8; i++) {
         t0 = tmp[j + 0];
         t1 = tmp[j + 1];
         t2 = tmp[j + 2];
         t3 = tmp[j + 3];
         t4 = tmp[j + 4];
         t5 = tmp[j + 5];
         t6 = tmp[j + 6];
         t7 = tmp[j + 7];
         if ((t1 | t2 | t3 | t4 | t5 | t6 | t7) == 0) {
             te = ITOINT(t0);
             out[j + 0] = te;
             out[j + 1] = te;
             out[j + 2] = te;
             out[j + 3] = te;
             out[j + 4] = te;
             out[j + 5] = te;
             out[j + 6] = te;
             out[j + 7] = te;
             j += 8;
             continue;
         }
         //IDCT;
         tmp0 = t0 + t1;
         t1 = t0 - t1;
         tmp2 = t2 - t3;
         t3 = t2 + t3;
         tmp2 = IMULT(tmp2, IC4) - t3;
         tmp3 = tmp0 + t3;
         t3 = tmp0 - t3;
         tmp1 = t1 + tmp2;
         tmp2 = t1 - tmp2;
         tmp4 = t4 - t7;
         t7 = t4 + t7;
         tmp5 = t5 + t6;
         t6 = t5 - t6;
         tmp6 = tmp5 - t7;
         t7 = tmp5 + t7;
         tmp5 = IMULT(tmp6, IC4);
         tmp6 = IMULT((tmp4 + t6), S22);
         tmp4 = IMULT(tmp4, (C22 - S22)) + tmp6;
         t6 = IMULT(t6, (C22 + S22)) - tmp6;
         t6 = t6 - t7;
         t5 = tmp5 - t6;
         t4 = tmp4 - t5;

         out[j + 0] = ITOINT(tmp3 + t7);
         out[j + 1] = ITOINT(tmp1 + t6);
         out[j + 2] = ITOINT(tmp2 + t5);
         out[j + 3] = ITOINT(t3 + t4);
         out[j + 4] = ITOINT(t3 - t4);
         out[j + 5] = ITOINT(tmp2 - t5);
         out[j + 6] = ITOINT(tmp1 - t6);
         out[j + 7] = ITOINT(tmp3 - t7);
         j += 8;
     }
}

#define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xFF : (((color) < 0) ? 0 : (color)))

void MjpegDecoderLazy::yuv422ptoG12(int *out, G12Buffer *pic, int y, int x)
{
    int j, k;
#ifdef _TRACE
    printf("Macroblock\n");
    printf("Y\n");
    for (j = 0; j < 8; j++) {
        for (k = 0; k < 8; k++) {
            printf("%4d ",((unsigned *)out)[j * 8 + k]);
        }
        printf("\n");
    }

    for (j = 0; j < 8; j++) {
        for (k = 0; k < 8; k++) {
            printf("%4d ",((unsigned *)out)[j * 8 + k + 64]);
        }
        printf("\n");
    }
    printf("\n");
#endif
    for (j = 0; j < 8; j++) {
        for (k = 0; k < 8; k++) {
            int value;
            value = out[j * 8 + k];
            pic->element(y + j, x + k) = CLIP(value) << 4;
            value = out[j * 8 + k + 64];
            pic->element(y + j, x + k + 8) = CLIP(value) << 4;
        }
    }
}

void MjpegDecoderLazy::yuv422ptoRGB(int *out, RGB24Buffer *pic, int y, int x)
{
    int j, k, idx = 0;
    for (j = 0; j < 8; j++) {
        for (k = 0; k < 8; k++, idx++) {
            int value, y8, u, v;
            value = out[idx];
            y8 = CLIP(value);
            u  = 128;
            v  = 128;
            pic->element(y + j, x + k) = RGBColor::FromYUV(y8, u, v);

            value = out[idx + 64];
            y8 = CLIP(value);
            u  = 128;
            v  = 128;
            pic->element(y + j, x + k + 8) = RGBColor::FromYUV(y8, u, v);
        }
    }
}

template<class ResultBuffer>
ResultBuffer *MjpegDecoderLazy::decodeData(unsigned char *buf, bool isRGB)
{
    int intwidth;
    int intheight;
    int i, j, m, tac, tdc;
    int mcusx, mcusy, mx, my;
    int ypitch,xpitch,bpp,pitch,x,y;
    int mb;
    int err = 0;
    int isInitHuffman = 0;
    JpegDecdata *decdata = new JpegDecdata();

    ResultBuffer *toReturn = NULL;

    memset(&info, 0, sizeof(Jpginfo));

    {
        if (!decdata) {
            err = -1; goto error;
        }
        if (buf == NULL) {
            err = -1; goto error;
        }

        parser = StreamParser(buf);
        if (parser.getbyte() != 0xff) {
            err = ERR_NO_SOI; goto error;
        }

        if (parser.getbyte() != M_SOI) {
            err = ERR_NO_SOI; goto error;
        }

        if (readtables(M_SOF0, &isInitHuffman)) {
            err = ERR_BAD_TABLES; goto error;
        }
        parser.getword();
        i = parser.getbyte();

        if (i != 8) {
            err = ERR_NOT_8BIT; goto error;
        }

        intheight = parser.getword();
        intwidth = parser.getword();

        if ((intheight & 7) || (intwidth & 7)) {
            err = ERR_BAD_WIDTH_OR_HEIGHT; goto error;
        }

        _DOTRACE(("File metrics are [%dx%d]\n",intwidth, intheight));

        info.nc = parser.getbyte();
        if (info.nc > MAXCOMP) {
            err = ERR_TOO_MANY_COMPPS; goto error;
        }

        _DOTRACE(("File has %d components\n", info.nc));

        comps = new ComponentData[info.nc];
        scans = new ScanData[info.nc];

        for (i = 0; i < info.nc; i++) {
            int h, v;
            comps[i].cid = parser.getbyte();
            comps[i].hv  = parser.getbyte();
            v = comps[i].hv & 0xF;
            h = comps[i].hv >> 4;

            comps[i].tq = parser.getbyte();

            if (h > 3 || v > 3) {
                err = ERR_ILLEGAL_HV; goto error;
            }
            if (comps[i].tq > 3) {
                err = ERR_QUANT_TABLE_SELECTOR; goto error;
            }
        }

        if (readtables(M_SOS, &isInitHuffman)) {
            err = ERR_BAD_TABLES; goto error;
        }

        parser.getword();
        info.ns = parser.getbyte();

        if (info.ns == 0) {
            _DOTRACE(("info ns %d/n",info.ns));
            err = ERR_NOT_YCBCR_221111;  goto error;
        }

        for (i = 0; i < info.ns; i++) {
            scans[i].cid = parser.getbyte();
            tdc = parser.getbyte();
            tac = tdc & 15;
            tdc >>= 4;
            if (tdc > 1 || tac > 1) {
                err = ERR_QUANT_TABLE_SELECTOR;
                goto error;
            }
            for (j = 0; j < info.nc; j++)
                if (comps[j].cid == scans[i].cid)
                    break;
            if (j == info.nc) {
                err = ERR_UNKNOWN_CID_IN_SCAN;
                goto error;
            }
            scans[i].hv = comps[j].hv;
            scans[i].tq = comps[j].tq;
            scans[i].hudc = dhuff + tdc;
            scans[i].huac = dhuff + 2 + tac;
        }

        i = parser.getbyte();
        j = parser.getbyte();
        m = parser.getbyte();

        if (i != 0 || j != 63 || m != 0) {
            _DOTRACE(("hmm FW error,not seq DCT ??\n"));
        }
        // printf("ext huffman table %d \n",isInitHuffman);
        if (!isInitHuffman) {
            if (huffman_init() < 0) {
                //return -ERR_BAD_TABLES;
                return NULL;
            }
        }

        toReturn = new ResultBuffer(intheight, intwidth, false);
        int width  = intwidth;
        int height = intheight;

        if (scans[0].hv != 0x21)
        {
            _DOTRACE(("Formats other than 422 are temporary unsupported\n"));
            return NULL;
            //return -1;
        }

        _DOTRACE(("Format is 422\n"));
        mb=4;
        mcusx = width  >> 4;
        mcusy = height >> 3;
        bpp=2;
        xpitch = 16    * bpp;
        pitch  = width * bpp;       // YUYV out
        ypitch = 8     * pitch;

        idctqtab(quants[scans[0].tq], decdata->dquant[0]);
        idctqtab(quants[scans[1].tq], decdata->dquant[1]);
        idctqtab(quants[scans[2].tq], decdata->dquant[2]);

        hparser = HuffmanParser(parser.datap);

        info.nm = info.dri + 1;
        info.rm = M_RST0;
        for (i = 0; i < info.ns; i++) {
            scans[i].dc = 0;
        }
        scans[0].next = 2;
        scans[1].next = 1;
        scans[2].next = 0; /* 4xx encoding */

#define CONV_PROLOG     \
            for (my = 0, y = 0; my < mcusy; my++, y += ypitch) {            \
                for (mx = 0, x = 0; mx < mcusx; mx++, x += xpitch) {        \
                    if (info.dri && !--info.nm)                             \
                        if (dec_checkmarker()) {                            \
                            err = ERR_WRONG_MARKER; goto error;             \
                        }                                                   \
                    decode_mcus(decdata->dcts, mb, scans);                  \
                    idct(decdata->dcts      , decdata->out      , decdata->dquant[0], IFIX(128.5));         \
                    idct(decdata->dcts + 64 , decdata->out + 64 , decdata->dquant[0], IFIX(128.5));         \
                  /*idct(decdata->dcts + 128, decdata->out + 256, decdata->dquant[1], IFIX(0.5), max[4]);*/ \
                  /*idct(decdata->dcts + 192, decdata->out + 320, decdata->dquant[2], IFIX(0.5), max[5]);*/ \

#define CONV_EPILOG     \
                }       \
            }           \

        if (isRGB)
        {
            CONV_PROLOG
                    yuv422ptoRGB(decdata->out, (RGB24Buffer *)toReturn, my * 8, mx * 16);
            CONV_EPILOG
        }
        else
        {
            CONV_PROLOG
                    yuv422ptoG12(decdata->out, (G12Buffer *)toReturn, my * 8, mx * 16);
            CONV_EPILOG
        }

        m = hparser.readMarker();
        if (m != M_EOI) {
            err = ERR_NO_EOI; goto error;
        }

        err = 0;
        /*delete[] comps;
        delete[] scans;
        delete decdata;
        return toReturn;*/
    }
error:

    if (err != 0)
    {
        printf("FJPEG decoder: error code: %d\n", err);
        if (1)
        {
            FILE *out = fopen("precrash.mjpeg", "wb");
            fwrite(buf, 320 * 200 * 2, 1, out);
            fclose(out);
        }

        if (err != ERR_NO_EOI)
        {
            if (toReturn != NULL)
            {
                delete toReturn;
                toReturn = NULL;
            }
        }
    }

    delete[] comps;
    delete[] scans;
    delete decdata;
    return toReturn;
}

G12Buffer *MjpegDecoderLazy::decode(unsigned char *buf)
{
    return decodeData<G12Buffer>(buf, false);
}

RGB24Buffer *MjpegDecoderLazy::decodeRGB24(unsigned char *buf)
{
    return decodeData<RGB24Buffer>(buf, true);
}

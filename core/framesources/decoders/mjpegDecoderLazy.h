/**
 * \file MjpegDecoderLazy.h
 * \brief Add Comment Here
 *
 * This file is heavily based on GPL code. Should rework it to remove viral license taint.
 *
 * \date Nov 4, 2010
 * \author alexander
 */

#ifndef MJPEGDECODERLAZY_H_
#define MJPEGDECODERLAZY_H_

#include <stdint.h>
#include "buffers/g12Buffer.h"
#include "buffers/rgb24/rgb24Buffer.h"

using namespace corecvs;

class MjpegDecoderLazy
{
public:

    static const unsigned DECBITS = 10;
    static const int MAXCOMP = 4;

    class StreamParser {
    public:
        uint8_t *datap;

        StreamParser(uint8_t *_datap) : datap(_datap) {}
        /*TODO: Remove this ASAP*/
        StreamParser() : datap(NULL) {}

        int getbyte(void)
        {
            return *datap++;
        }

        int getword(void)
        {
            int c1, c2;
            c1 = *datap++;
            c2 = *datap++;
            return (c1 << 8) | c2;
        }
    };

    class HuffmanTableEntry {
    public:
        uint8_t value;
        uint8_t length;
    };

    class HuffmanTable {
    public:
        HuffmanTableEntry table[1 << 16];
    };

    class HuffmanParser {
    public:

        uint8_t *datap; /**< pointer to the data in stream*/
        uint32_t bits;  /**< currently processed bits */
        int left;       /**< number of bits left to process untill we will need to pop more form stream */
        int marker;     /**< flag if marker was found */

        HuffmanParser(uint8_t *_datap) : datap(_datap), bits(0), left(0), marker(0) {}
        HuffmanParser() {}

        int getValue(HuffmanTable *tbl);
        int getBits(int number);
        int readMarker();

    private:
        int fillbits();
    };

    class Jpginfo {
    public:
        int nc;         /**< number of components */
        int ns;         /**< number of scans */
        int dri;        /**< restart interval */
        int nm;         /**< mcus til next marker */
        int rm;         /**< next restart marker */
    };

    class ComponentData {
    public:
        int cid;
        int hv;
        int tq;
    };

    /**
     *  Huffman decode table
     *
     *
     * */
    class ScanData {
    public:
        int dc;         /* old dc value */

        HuffmanTable *hudc;
        HuffmanTable *huac;
        int next;           /* when to switch to next scan */

        int cid;            /* component id */
        int hv;         /* horiz/vert, copied from comp */
        int tq;         /* quant tbl, copied from comp */
    };

    class JpegDecdata {
    public:
        int dcts[6 * 64 + 16];
        int out[64 * 6];
        int dquant[3][64];
    };


    /* Special constants */
    const static int  M_SOI   = 0xd8;
    const static int  M_APP0  = 0xe0;
    const static int  M_DQT   = 0xdb;
    const static int  M_SOF0  = 0xc0;
    const static int  M_DHT   = 0xc4;
    const static int  M_DRI   = 0xdd;
    const static int  M_SOS   = 0xda;
    const static int  M_RST0  = 0xd0;
    const static int  M_EOI   = 0xd9;
    const static int  M_COM   = 0xfe;
    const static int  M_EOF   = 0x80;


    /* ERROR Codes */
    const static int ERR_NO_SOI               = 1;
    const static int ERR_NOT_8BIT             = 2;
    const static int ERR_HEIGHT_MISMATCH      = 3;
    const static int ERR_WIDTH_MISMATCH       = 4;
    const static int ERR_BAD_WIDTH_OR_HEIGHT  = 5;
    const static int ERR_TOO_MANY_COMPPS      = 6;
    const static int ERR_ILLEGAL_HV           = 7;
    const static int ERR_QUANT_TABLE_SELECTOR = 8;
    const static int ERR_NOT_YCBCR_221111     = 9;
    const static int ERR_UNKNOWN_CID_IN_SCAN  = 10;
    const static int ERR_NOT_SEQUENTIAL_DCT   = 11;
    const static int ERR_WRONG_MARKER         = 12;
    const static int ERR_NO_EOI               = 13;
    const static int ERR_BAD_TABLES           = 14;
    const static int ERR_DEPTH_MISMATCH       = 15;

    const static int JPG_HUFFMAN_TABLE_LENGTH = 0x1A0;

    static uint8_t JPEGHuffmanTable[];
    static uint8_t CoefZigZag[];
    static uint8_t CoefZigZag2[];
    static int aaidct[];

    MjpegDecoderLazy();

    G12Buffer   *decode     (unsigned char *buf);
    RGB24Buffer *decodeRGB24(unsigned char *buf);

private:
    template<class ResultBuffer> ResultBuffer *decodeData(unsigned char *buf, bool isRGB);

    int  huffman_init();
    void dec_makehuff(HuffmanTable *hu, unsigned * hufflen, unsigned char *huffvals);
    int  readtables(int till, int *isDHT);

    void idctqtab(uint8_t *qin, int *qout);

    int  dec_readmarker(void);
    int  dec_checkmarker(void);

    void decode_mcus(int *dct, int n, ScanData *sc);
    void idct(int *in, int *out, int *quant, long off);

    void yuv422ptoG12(int *out, G12Buffer   *pic, int y, int x);
    void yuv422ptoRGB(int *out, RGB24Buffer *pic, int y, int x);

    StreamParser    parser;
    HuffmanParser   hparser;

    Jpginfo         info;
    HuffmanTable    dhuff[4];
    ComponentData  *comps;
    ScanData       *scans;

    uint8_t         quants[4][64];
};

#endif /* MJPEGDECODERLAZY_H_ */

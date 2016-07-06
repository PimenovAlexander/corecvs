#ifndef READERS_H_
#define READERS_H_
/**
 * \file readers.h
 * \brief a header for different structure readers
 *
 * \date May 7, 2011
 * \author alexander
 */

#ifdef WITH_SSE

#include <stdint.h>

#include "sseWrapper.h"
#include "fixedVector.h"


namespace corecvs {

using std::endl;

/**
 *   Reads a structure of 4 byte fields into SSE registers.
 *
 **/
class SSEReader16BBBB_BBBB {
public:

    static const int BYTE_STEP = 64;

    /**
     *  This function takes 16 4 byte records packed as
     *
     *  0             4             8              60               16*4
     *  | A0 B0 C0 D0 | A1 B1 C1 D1 | A2 ....   D14 | A15 B15 C15 D15 |
     *
     *  And packs them as
     *   A0 A1 A2 A3 ...  A15
     *   B0 B1 B2 B3 ...  B15
     *   C0 C1 C2 C3 ...  C15
     *   D0 D1 D2 D3 ...  D15
     *
     **/
    static FixedVector<Int8x16, 4> read(uint32_t *ptr)
    {
        FixedVector<Int8x16, 4> result;
        /* Preparing 4 registers that contain...*/
        /* A0 B0 C0 D0 .... A3 B3 C3 D3 */
        Int32x4 a(ptr      );
        /* A4 B4 C4 D4 .... A7 B7 C7 D7 */
        Int32x4 b(ptr + 0x4);
        Int32x4 c(ptr + 0x8);
        Int32x4 d(ptr + 0xC);

        /* Rearranging the data cycling trough the fields of the structure*/
        for (unsigned i = 0; i < 4; i++)
        {
            /* A0  00 00 00 ... A3  00 00 00*/
            Int32x4 al = a & Int32x4(0xFF);
            /* A4  00 00 00 ... A7  00 00 00*/
            Int32x4 bl = b & Int32x4(0xFF);
            /* A8  00 00 00 ... A11 00 00 00*/
            Int32x4 cl = c & Int32x4(0xFF);
            /* A12 00 00 00 ... A15 00 00 00*/
            Int32x4 dl = d & Int32x4(0xFF);

            result[i] = Int8x16::pack(Int16x8::pack(al, bl), Int16x8::pack(cl, dl));

            a.shiftLogical(8);
            b.shiftLogical(8);
            c.shiftLogical(8);
            d.shiftLogical(8);
        }
        return result;
    }

    static void write(const FixedVector<Int8x16, 4> &data, uint32_t *ptr)
    {
        /* A0 B0 A1 B1 ...  A7 B7 */
        Int16x8 mixl1 = Int8x16::unpackLower(data[0], data[1]);
        /* C0 D0 C1 D1 ...  C7 D7 */
        Int16x8 mixl2 = Int8x16::unpackLower(data[2], data[3]);

        /* A8 B8 A8 B8 ...  A15 B15 */
        Int16x8 mixh1 = Int8x16::unpackHigher(data[0], data[1]);
        /* C8 D8 C8 D8 ...  C15 D15 */
        Int16x8 mixh2 = Int8x16::unpackHigher(data[2], data[3]);

        Int32x4 a = Int16x8::unpackLower (mixl1, mixl2);
        Int32x4 b = Int16x8::unpackHigher(mixl1, mixl2);
        Int32x4 c = Int16x8::unpackLower (mixh1, mixh2);
        Int32x4 d = Int16x8::unpackHigher(mixh1, mixh2);

        a.save(ptr);
        b.save(ptr + 0x4);
        c.save(ptr + 0x8);
        d.save(ptr + 0xC);
    }

};


class SSEReader8BBBB_DDDD {
public:

    static const int BYTE_STEP = 32;

    /**
     *  This function takes 8 4 byte records packed as
     *
     *  0             4             8              28           8*4
     *  | A0 B0 C0 D0 | A1 B1 C1 D1 | A2 ....   D6 | A7 B7 C7 D7 |
     *
     *  And packs them as
     *  | 00 A0 | 00 A1 | 00 A2 | 00 A3 ...  | 00 A7 |
     *  | 00 B0 | 00 B1 | 00 B2 | 00 B3 ...  | 00 B7 |
     *  | 00 C0 | 00 C1 | 00 C2 | 00 C3 ...  | 00 C7 |
     *  | 00 D0 | 00 D1 | 00 D2 | 00 D3 ...  | 00 D7 |
     *
     **/
    static FixedVector<Int16x8, 4> read(uint32_t *ptr)
    {

        FixedVector<Int16x8, 4> result;
        /* Preparing 2 registers that contain...*/
        /* | A0 B0 C0 D0 | .... | A3 B3 C3 D3 |*/
        Int32x4 a(ptr      );
        /* | A4 B4 C4 D4 | .... | A7 B7 C7 D7 |*/
        Int32x4 b(ptr + 0x4);

        /* Rearranging the data cycling trough the fields of the structure*/
        for (unsigned i = 0; i < 4; i++)
        {
            Int32x4 al = a & Int32x4(0xFF);
            Int32x4 bl = b & Int32x4(0xFF);
            result[i] = Int16x8::pack(al, bl);
            a.shiftLogical(8);
            b.shiftLogical(8);
        }
        return result;
    }

    /**
     *  Taking packed vector
     *  | 00 A0 | 00 A1 | 00 A2 | 00 A3 ...  | 00 A7 |
     *  | 00 B0 | 00 B1 | 00 B2 | 00 B3 ...  | 00 B7 |
     *  | 00 C0 | 00 C1 | 00 C2 | 00 C3 ...  | 00 C7 |
     *  | 00 D0 | 00 D1 | 00 D2 | 00 D3 ...  | 00 D7 |
     **/

    static void write(const FixedVector<Int16x8, 4> &data, uint32_t *ptr)
    {

        Int16x8 united1 = (data[1] << 8) | data[0];
        Int16x8 united2 = (data[3] << 8) | data[2];

        Int32x4 a = Int16x8::unpackLower (united1.data, united2.data);
        Int32x4 b = Int16x8::unpackHigher(united1.data, united2.data);

        a.save(ptr);
        b.save(ptr + 0x4);
    }

};

/**/
class SSEReaderBBBB_QQQQ {
public:

    static const int SAFE_STEP = 16;
    static const int BYTE_STEP = 4;

    /**
     *  This function reads _16_ bytes, takes 4 first and extends them to UInt32x4
     **/
    static Int32x4 read(uint8_t *ptr)
    {
        Int8x16 input(ptr);
        Int16x8 extended = Int8x16::unpackLower(input, Int8x16((int8_t)0));
        return Int16x8::unpackLower(extended, Int16x8((int16_t)0));
    }
};

class SSEReader2BBBB_QQQQ {
public:

    static const int SAFE_STEP = 16;
    static const int BYTE_STEP = 8;

    /**
     *  This function reads _16_ bytes, takes 4 first and 4 second bytes and extends them to UInt32x4 each
     **/
    static FixedVector<Int32x4, 2> read(uint8_t *ptr)
    {
        FixedVector<Int32x4, 2> result;

        Int8x16 input(Int64x2::loadLower((int64_t *)ptr).data);
        Int16x8 extended = Int8x16::unpackLower(input, Int8x16((int8_t)0));
        result[0] = Int16x8::unpackLower(extended, Int16x8((int16_t)0));
        result[1] = Int16x8::unpackHigher(extended, Int16x8((int16_t)0));
        return result;
    }
};


class SSEReader8DD_DD {
public:

    static const int BYTE_STEP = 32;

    /**
     *  This function takes 8 4 byte records packed as
     *
     *  0                      4                      8                              32
     *  | A0.0 A0.1  B0.0 B0.1 | A1.0 A1.1  B1.0 B1.1 | ....  | A7.0 A7.1  B7.0 B7.1 |
     *
     *  And packs them as
     *  | A0.0 A0.1 | A1.0 A1.1 | ... | A7.0 A7.1 |
     *  | B0.0 B0.1 | B1.0 B1.1 | ... | B7.0 B7.1 |
     *
     **/
    static FixedVector<Int16x8, 2> read(uint32_t *ptr)
    {
        FixedVector<Int16x8, 2> result;
        Int32x4 a(ptr      );
        Int32x4 b(ptr + 0x4);

        cout << "a" << a << endl;
        cout << "b" << b << endl;

        cout << "a" << Int16x8(a.data) << endl;
        cout << "b" << Int16x8(b.data) << endl;


        /* Rearranging the data cycling trough the fields of the structure*/
        for (unsigned i = 0; i < 2; i++)
        {
            Int32x4 al = a & Int32x4(0xFFFF);
            Int32x4 bl = b & Int32x4(0xFFFF);

            cout << "al" << al << endl;
            cout << "bl" << bl << endl;

            result[i] = Int16x8::pack(al, bl);

            a.shiftLogical(16);
            b.shiftLogical(16);
        }
        return result;
    }

    /**
     *  Taking packed vector
     *  | A0.0 A0.1 | A1.0 A1.1 | ... | A7.0 A7.1 |
     *  | B0.0 B0.1 | B1.0 B1.1 | ... | B7.0 B7.1 |
     **/

    static void write(const FixedVector<Int16x8, 2> &data, uint32_t *ptr)
    {
        Int32x4 a = Int16x8::unpackLower (data[0], data[1]);
        Int32x4 b = Int16x8::unpackHigher(data[0], data[1]);

        a.save(ptr);
        b.save(ptr + 0x4);
    }

};

class SSEReader8BBBBBBBB_DDDDDDDD {
public:

    static const int BYTE_STEP = 64;

    /**
       *  This function takes 8 8 byte records packed as
       *
       *  0                          8                        16                                        8*8
       *  | A0 B0 C0 D0  E0 F0 G0 H0 | A1 B1 C1 D1 E1 F1 G1 H1 | A2 ....   H7 | A7 B7 C7 D7  E7 F7 G7 H7 |
       *
       *  And packs them as
       *
       *  | 00 A0 | 00 A1 | 00 A2 | 00 A3 ...  | 00 A7 |
       *  | 00 B0 | 00 B1 | 00 B2 | 00 B3 ...  | 00 B7 |
       *  | 00 C0 | 00 C1 | 00 C2 | 00 C3 ...  | 00 C7 |
       *  | 00 D0 | 00 D1 | 00 D2 | 00 D3 ...  | 00 D7 |
       *  | 00 E0 | 00 E1 | 00 E2 | 00 E3 ...  | 00 E7 |
       *  | 00 F0 | 00 F1 | 00 F2 | 00 F3 ...  | 00 F7 |
       *  | 00 G0 | 00 G1 | 00 G2 | 00 G3 ...  | 00 G7 |
       *  | 00 H0 | 00 H1 | 00 H2 | 00 H3 ...  | 00 H7 |
       **/
    static FixedVector<Int16x8, 8> read(int64_t *ptr)
    {
        /* Preparing 2 registers that contain...*/
        /* | A0 B0 C0 D0  E0 F0 G0 H0 | A1 B1 C1 D1 E1 F1 G1 H1 | */
        Int64x2 a(ptr      );
        /* | A2 B2 C2 D2  E2 F2 G2 H2 | A3 B3 C3 D3 E3 F3 G3 H3 | */
        Int64x2 b(ptr + 0x2);
        /* | A4 B4 C4 D4  E4 F4 G4 H4 | A5 B5 C5 D5 E5 F5 G5 H5 | */
        Int64x2 c(ptr + 0x4);
        /* | A6 B6 C6 D6  E6 F6 G6 H6 | A7 B7 C7 D7 E7 F7 G7 H7 | */
        Int64x2 d(ptr + 0x6);

//        cout << "a =" << Int8x16(a.data) << endl;
//        cout << "b =" << Int8x16(b.data) << endl;
//        cout << "c =" << Int8x16(c.data) << endl;
//        cout << "d =" << Int8x16(d.data) << endl;

//        cout << " " << endl;
        Int64x2 e = Int32x4::unpackLower(Int32x4(a.data), Int32x4(b.data));
//        cout << "upackl =" << Int8x16(e.data) << endl;
        Int64x2 g = Int32x4::unpackHigher(Int32x4(a.data), Int32x4(b.data));
//        cout << "upackh =" << Int8x16(g.data) << endl;
        Int64x2 e1 = Int32x4::unpackLower(Int32x4(c.data), Int32x4(d.data));
//        cout << "upackl =" << Int8x16(e1.data) << endl;
        Int64x2 g1 = Int32x4::unpackHigher(Int32x4(c.data), Int32x4(d.data));
//        cout << "upackh =" << Int8x16(g1.data) << endl;

//        cout << "interl 8" << endl;
        Int16x8 u = Int8x16::unpackLower(Int8x16(e.data), Int8x16(g.data));
//        cout << "interl =" << Int8x16(u.data) << endl;
        Int16x8 v = Int8x16::unpackHigher(Int8x16(e.data), Int8x16(g.data));
//        cout << "interh =" << Int8x16(v.data) << endl;
        Int16x8 u1 = Int8x16::unpackLower(Int8x16(e1.data), Int8x16(g1.data));
//        cout << "interl =" << Int8x16(u1.data) << endl;
        Int16x8 v1 = Int8x16::unpackHigher(Int8x16(e1.data), Int8x16(g1.data));
//        cout << "interh =" << Int8x16(v1.data) << endl;

//        cout << "unpack again" << endl;
        Int64x2 k = Int32x4::unpackLower(Int32x4(u.data), Int32x4(v.data));
//        cout << "upackl =" << Int8x16(k.data) << endl;
        Int64x2 l = Int32x4::unpackHigher(Int32x4(u.data), Int32x4(v.data));
//        cout << "upackh =" << Int8x16(l.data) << endl;
        Int64x2 k1 = Int32x4::unpackLower(Int32x4(u1.data), Int32x4(v1.data));
//        cout << "upackl =" << Int8x16(k1.data) << endl;
        Int64x2 l1 = Int32x4::unpackHigher(Int32x4(u1.data), Int32x4(v1.data));
//        cout << "upackh =" << Int8x16(l1.data) << endl;

//        cout << "interl 16" << endl;
        Int32x4 s = Int16x8::unpackLower(Int16x8(k.data), Int16x8(l.data));
//        cout << "interl =" << Int8x16(s.data) << endl;
        Int32x4 t = Int16x8::unpackHigher(Int16x8(k.data), Int16x8(l.data));
//        cout << "interh =" << Int8x16(t.data) << endl;
        Int32x4 s1 = Int16x8::unpackLower(Int16x8(k1.data), Int16x8(l1.data));
//        cout << "interl =" << Int8x16(s1.data) << endl;
        Int32x4 t1 = Int16x8::unpackHigher(Int16x8(k1.data), Int16x8(l1.data));
//        cout << "interh =" << Int8x16(t1.data) << endl;

//        cout << "interl 32" << endl;
        Int64x2 x = Int32x4::unpackLower(Int32x4(s.data), Int32x4(s1.data));
//        cout << "interl =" << Int8x16(x.data) << endl;
        Int64x2 y = Int32x4::unpackHigher(Int32x4(s.data), Int32x4(s1.data));
//        cout << "interh =" << Int8x16(y.data) << endl;
        Int64x2 x1 = Int32x4::unpackLower(Int32x4(t.data), Int32x4(t1.data));
//        cout << "interl =" << Int8x16(x1.data) << endl;
        Int64x2 y1 = Int32x4::unpackHigher(Int32x4(t.data), Int32x4(t1.data));
//        cout << "interh =" << Int8x16(y1.data) << endl;

        FixedVector<Int16x8, 8> result;
        result[0] =  Int8x16::unpackLower (Int8x16(x.data), Int8x16((int8_t)0));
        result[1] =  Int8x16::unpackHigher(Int8x16(x.data), Int8x16((int8_t)0));
        result[2] =  Int8x16::unpackLower (Int8x16(x1.data), Int8x16((int8_t)0));
        result[3] =  Int8x16::unpackHigher(Int8x16(x1.data), Int8x16((int8_t)0));
        result[4] =  Int8x16::unpackLower (Int8x16(y.data), Int8x16((int8_t)0));
        result[5] =  Int8x16::unpackHigher(Int8x16(y.data), Int8x16((int8_t)0));
        result[6] =  Int8x16::unpackLower (Int8x16(y1.data), Int8x16((int8_t)0));
        result[7] =  Int8x16::unpackHigher(Int8x16(y1.data), Int8x16((int8_t)0));

        return result;

    }

    /**
     *  Taking packed vector
       *  | 00 A0 | 00 A1 | 00 A2 | 00 A3 ...  | 00 A7 |
       *  | 00 B0 | 00 B1 | 00 B2 | 00 B3 ...  | 00 B7 |
       *  | 00 C0 | 00 C1 | 00 C2 | 00 C3 ...  | 00 C7 |
       *  | 00 D0 | 00 D1 | 00 D2 | 00 D3 ...  | 00 D7 |
       *  | 00 E0 | 00 E1 | 00 E2 | 00 E3 ...  | 00 E7 |
       *  | 00 F0 | 00 F1 | 00 F2 | 00 F3 ...  | 00 F7 |
       *  | 00 G0 | 00 G1 | 00 G2 | 00 G3 ...  | 00 G7 |
       *  | 00 H0 | 00 H1 | 00 H2 | 00 H3 ...  | 00 H7 |
     **/

    static void write(const FixedVector<Int16x8, 8> &data, uint32_t *ptr)
    {

        /* | A0 B0 A1 B1 A2 B2 A3 B3 A4 B4 A5 B5 A6 B6 A7 B7 | */
        Int16x8 united1 = (data[1] << 8) | data[0];
        /* | C0 D0 C1 D1 C2 D2 C3 D3 C4 D4 C5 D5 C6 D6 C7 D7 | */
        Int16x8 united2 = (data[3] << 8) | data[2];
        Int16x8 united3 = (data[5] << 8) | data[4];
        Int16x8 united4 = (data[7] << 8) | data[6];

        /* | A0 B0 C0 D0 A2 B2 C2 D2 A4 B4 C4 D4 A6 B6 C4 D4 | */
        Int32x4 a = Int16x8::unpackLower (united1.data, united2.data);
        /* | A1 B1 C1 D1 A3 B3 C3 D3 A5 B5 C5 D5 A7 B7 C7 D7 | */
        Int32x4 b = Int16x8::unpackHigher(united1.data, united2.data);
        Int32x4 c = Int16x8::unpackLower (united3.data, united4.data);
        Int32x4 d = Int16x8::unpackHigher(united3.data, united4.data);

//        cout << "a =" << Int8x16(a) << endl;
//        cout << "b =" << Int8x16(b) << endl;
//        cout << "c =" << Int8x16(c) << endl;
//        cout << "d =" << Int8x16(d) << endl;

        Int64x2 e = Int32x4::unpackLower (a.data, c.data);
        Int64x2 f = Int32x4::unpackHigher(a.data, c.data);
        Int64x2 g = Int32x4::unpackLower (b.data, d.data);
        Int64x2 h = Int32x4::unpackHigher(b.data, d.data);

        uint64_t *outptr = (uint64_t *)ptr;
        e.save(outptr);
        f.save(outptr + 0x2);
        g.save(outptr + 0x4);
        h.save(outptr + 0x6);
    }

};


} //namespace corecvs
#endif



#endif  //READERS_H_


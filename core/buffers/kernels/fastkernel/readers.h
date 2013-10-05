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
        /* A0 B0 C0 D0 .... A3 B3 C3 D3 */
        Int32x4 a(ptr      );
        /* A4 B4 C4 D4 .... A7 B7 C7 D7 */
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

} //namespace corecvs
#endif



#endif  //READERS_H_


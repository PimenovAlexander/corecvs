#include <bitset>
#include <memory.h>
#include <vector>

#include "frSkyMultimodule.h"

using namespace std;

/**

    Ti - are CH0 (throttle) bits
    Ri - are CH1 (Roll)     bits
    Pi - are CH2 (Pitch)    bits
    Yi - are CH3 (Yaw)      bits

    A, B, C, D - are CH4, CH5, CH6, CH7

    In 32 bit values

       HEADER:
       | 31         [0x00]        24 | 23       [0x19]         16  |  15        [0x0F]        8  | 7         [0x55]        0  |
       |  0  0  0  0     0  0  0  0  |  0  0  0  1     1  0  0  1  |  0  0  0  0     1  1  1  1  | 0  1  0  1     0  1  0  1  |


       DATA:
       | 63                       56 | 55                       48 |  47                      40 | 39                      32 |
       |  P8 P7 P6 P5    P4 P3 P2 P1 |  P0 0  R9 R8    R7 R6 R5 R4 |  R3 R2 R1 R0    0  T9 T8 T7 | T6 T5 T4 T3    T2 T1 T0 0  |

       | 95                       88 | 87                       80 |  79                      72 | 71                      64 |
       |  B7 B6 B5  B4   B3 B2 B1 B0 |  0  A9 A8 A7    A6 A5 A4 A3 |  A2 A1 A0 00    Y9 Y8 Y7 Y6 | Y5 Y4 Y3 Y2    Y1 Y0 0  P9 |

       | 127        [0x00]       120 | 119                     112 |  111                    104 | 103                     96 |
       |  0  0  0  0     0  0  0  0  | D9 D8  D7 D6    D5 D4 D3 D2 |  D1 D0 0  C9    C8 C7 C6 C5 | C4 C3 C2 C1    C0  0 B9 B8 |

       FOOTER:
       | 159        [0x01]       152 | 151       [0x00]        144 |  143       [0x20]       136 | 135      [0x04]        128 |
       |  0  0  0  0     0  0  0  1  | 0  0  0   0     0  0  0  0  |  0  0  1  0     0  0  0  0  | 0  0  0  0     0  1  0  0  |

       | 191        [0x02]       184 | 183       [0x00]        176 |  175        [0x40]      168 | 167      [0x08]        160 |
       |  0  0  0  0     0  0  1  0  | 0  0  0   0     0  0  0  0  |  0  1  0  0     0  0  0  0  | 0  0  0  0     1  0  0  0  |

                                                                   |  207       [0x80]       200 | 199      [0x10]        192 |
                                                                   |  1  0  0  0     0  0  0  0  | 0  0  0  1     0  0  0  0  |


        0x55| 0x0F| 0x19| 0x00|
        0x00| 0x04| 0x20| 0x00|
        0x00| 0x0F| 0x10| 0x00|
        0x02| 0x10| 0x80| 0x00|
        0x04| 0x20| 0x00| 0x01|
        0x08| 0x40| 0x00| 0x02|
        0x10| 0x80


        More reference code for this message format can be found in OpenTX -
        https://github.com/opentx/opentx/blob/17b4aa24668bc4b223dd0613a48d2cc050e39358/radio/src/pulses/multi_arm.cpp

 **/

void FrSkyMultimodule::pack(const CopterInputs &channels, uint8_t message[])
{
    memset(message, 0, MESSAGE_SIZE * sizeof(uint8_t));

    uint32_t *mptr32 = (uint32_t *)&message[0];
    int axis[8];
    for (int i = 0; i < 8; i++)
    {
        axis[i] = (channels.axis[i] - MAGIC_DIFFERENCE)  * 8 / 10;
        axis[i] += 5;
    }
    mptr32[0] = 0x00000F55U;

    // byte 2
    mptr32[0] |= (modelId & 0x0F) << 16;
    mptr32[0] |= (subtype & 0x07) << 20;
    mptr32[0] |= lowPower ? (0x1 << 23) : 0x0;


    mptr32[1] = ((axis[0] & MASK) <<  1) |
                ((axis[1] & MASK) << 12) |
                ((axis[2] & MASK) << 23) ;

    mptr32[2] = ((axis[2] & MASK) >>  9) |
                ((axis[3] & MASK) <<  2) |
                ((axis[4] & MASK) << 13) |
                ((axis[5] & MASK) << 24) ;

    mptr32[3] = ((axis[5] & MASK) >>  8) |
                ((axis[6] & MASK) <<  3) |
                ((axis[7] & MASK) << 14);

    mptr32[4] = 0x01002004U;
    mptr32[5] = 0x02004008U;

    message[24] = 0x10;
    message[25] = 0x80;

}

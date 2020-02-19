/**
 * \file lutAlgebra.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 4, 2010
 * \author alexander
 */

#include "math/lutAlgebra.h"
#include "math/vector/vector3d.h"
#include <iostream>
#include <stdlib.h>
namespace corecvs {

using std::hex;
using std::istream;
using std::ostream;
using std::cout;

void LUTable::generateCCode()
{
    cout << "/*************************************************************/\n";
    cout << "/* This is an auto-generated LUT for function " << name << " */\n";
    cout << "/*************************************************************/\n";
    cout << "\n";
    cout << "#include <stdint.h>\n";
    cout << "\n";
    cout << "\n";
    cout << "uint32_t LUT" << name << "data[] = {\n";

    for (unsigned i = 0; i < data.size();i++)
    {
        cout << hex << data[i] << ", \n";
    }
    cout << "}\n";
    cout << "\n";

}

int LUTable::approxF (int value)
{
    if (value > high - low + 1)
        return 0;

    uint32_t valueId   = (value >> tailLen);
    uint32_t valueLeftMask = ((~(uint32_t)0) >> (32 - tailLen));
    uint32_t valueLeft =  value & valueLeftMask;
    uint32_t packed = data[valueId];

    uint32_t packedStart = packed >> division;
    uint32_t packedSlopeMask = ((~(uint32_t)0) >> (32 - division));
    uint32_t packedSlope = packed & packedSlopeMask;

    uint32_t result = packedStart + ((packedSlope * valueLeft) >> shift);
    return result + low;
}

LUTable *LUTGenerator::generate(SpeedupFunction &input, string name, unsigned tableSize)
{
    LUTable *luttable = new LUTable();
    luttable->name = name;
    luttable->data.resize(tableSize);

    unsigned low  = input.getDomainLow();
    unsigned high = input.getDomainHigh();
    unsigned interval = high - low;

    luttable->shift = 22;
    luttable->division = 16;
    luttable->tailLen = 15;

    printf("interval = %u\n", interval);
    for (unsigned i = 0; i < tableSize; i++)
    {
        uint32_t start = i * (interval / tableSize);
        uint32_t end   = (i + 1)  * (interval / tableSize);

        uint32_t sqrtstart = input.f(start);
        uint32_t sqrtend   = input.f(end);
        double slope = ((double)sqrtend - sqrtstart) / ((double)interval / tableSize);

        //uint32_t middle = (start + end) / 2;
        //double


        uint32_t packedStart = (uint32_t) sqrtstart;
        uint32_t packedSlope = (uint32_t) ceil(slope * (1 << luttable->shift));
        if (packedSlope > (1U << luttable->division))
        {
            printf("Overflow in slope, reduce SHIFT\n");
            exit(0);
        }

        luttable->data[i] = (packedStart << luttable->division) | packedSlope;

        printf("Start = %u, End = %u, slope = %2.5lf, 0x%x\n ", start, end, slope, packedSlope);
    }

    luttable->high = high;
    luttable->low  = low;


    return luttable;
}


LUTGenerator::Deviations LUTGenerator::deviations(LUTable *table, SpeedupFunction &input)
{
    double sumDevs = 0;
    double sumSqDevs = 0;
    double maxDevs = 0;
    int valMax = 0;

    for (int i = table->low; i <= table->high; i++)
    {
        double dev = table->approxF(i) - input.f(i);
        dev = fabs(dev);

        sumDevs += dev;
        sumSqDevs += dev * dev;
        if (dev > maxDevs)
        {
            valMax = i;
            maxDevs = dev;
        }
    }

    Deviations devs = {sumDevs, sumSqDevs, maxDevs, valMax, table->high - table->low};
    return devs;
}


int test1Main ( void )
{
    //LUTable lut;
    LUTGenerator *generator = new LUTGenerator();
    SqrtFunction *sqrtF = new SqrtFunction();
    LUTable *lut =  generator->generate(*sqrtF, string("sqrt"), 512);

    lut->approxF(7056);

    for (int i = 0; i < 0xFFFFFF; i+=0x7FF65 )
    {
        printf("%u %u %u\n", i, (uint32_t)sqrt((float)i), lut->approxF(i));
    }
    //lut->generateCCode();
    LUTGenerator::Deviations a = generator->deviations(lut, *sqrtF);

    printf("[%lf %lf %lf %u (of %u)]\n", a.devSum, a.devSqSum, a.devMax, a.valueMax, a.size);

    return 0;
}

} //namespace corecvs


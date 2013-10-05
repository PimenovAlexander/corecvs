#ifndef LUTALGEBRA_H_
#define LUTALGEBRA_H_

/**
 * \file lutAlgebra.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 4, 2010
 * \author alexander
 */

#include <string>
#include <vector>
#include <stdint.h>

#include "vector3d.h"
#include "stdio.h"
#include "math.h"

namespace corecvs {

using std::string;
using std::vector;

class SpeedupFunction
{
public:
    virtual int f(int) = 0;
    virtual int getDomainLow() = 0;
    virtual int getDomainHigh() = 0;
};

class SqrtFunction : public SpeedupFunction
{

    virtual int f (int in)
    {
        return sqrt((double)in);
    }

    virtual int getDomainLow(){ return 0x0; };
    virtual int getDomainHigh(){ return 0xFFFFFF; };


};

/**
 *  Almost POCO
 **/
class LUTable
{
public:
    string name;

    int low;
    int high;

    int shift;
    int tailLen;
    int division;

    vector<uint32_t> data;

    LUTable(){};

    void generateCCode();
    int approxF (int value);
};

class LUTGenerator
{
public:
    typedef struct
    {
        double devSum;
        double devSqSum;
        double devMax;
        int valueMax;
        int size;
    } Deviations;

    LUTable *generate(SpeedupFunction &input, string name, unsigned tableSize);
    Deviations deviations(LUTable *table, SpeedupFunction &input);
};


} //namespace corecvs
#endif /* LUTALGEBRA_H_ */


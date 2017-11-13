#ifndef GENERICFASTKERNEL_H_
#define GENERICFASTKERNEL_H_
/**
 * \file genericFastKernel.h
 * \brief a header for GenericFastKernel.c
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */
#include "core/utils/global.h"

#include "core/buffers/kernels/fastkernel/fastKernel.h"
namespace corecvs {

template <typename Algebra>
class GenericFastKernel
{
public:

    Algebra *algebra;
    inline static int getCenterX(){ return 1; };
    inline static int getCenterY(){ return 1; };
    inline static int getSizeX(){ return 3; };
    inline static int getSizeY(){ return 3; };

    typedef typename Algebra::Type BaseType;

    GenericFastKernel(){};
    GenericFastKernel(Algebra *_algebra)
    {
        algebra = _algebra;
    }

    void process()
    {
        BaseType a00 = algebra->getMainInput(0,0);
        BaseType a01 = algebra->getInput(0,1);
        BaseType a02 = algebra->getInput(0,2);
        BaseType a10 = algebra->getInput(1,0);
        BaseType a11 = algebra->getInput(1,1);
        BaseType a12 = algebra->getInput(1,2);
        BaseType a20 = algebra->getInput(2,0);
        BaseType a21 = algebra->getInput(2,1);
        BaseType a22 = algebra->getInput(2,2);

        BaseType center = (a11 << 1);
        BaseType cross  = (a01 + a10 + a12 + a21 + center) << 1;
        BaseType result = (a22 + a00 + a02 + a20 + cross) >> 4;
        algebra->putMainOutput(0,0,result);
    }
};




} //namespace corecvs
#endif  //GENERICFASTKERNEL_H_


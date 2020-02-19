/**
 * \file copyKernel.h
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 24, 2010
 * \author alexander
 */

#ifndef COPYKERNEL_H_
#define COPYKERNEL_H_

#include "utils/global.h"

#include "buffers/abstractKernel.h"
namespace corecvs {


template <typename Algebra>
class CopyKernel
{
public:
    static const int inputNumber  = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; }
    inline static int getCenterY(){ return 0; }
    inline static int getSizeX(){ return 1; }
    inline static int getSizeY(){ return 1; }

    typedef typename Algebra::InputType Type;


template<typename OtherAlgebra>
    CopyKernel(const CopyKernel<OtherAlgebra> &/* other*/ ) {}

    CopyKernel() {}

    void process(Algebra &algebra) const
    {
          algebra.putOutput(0,0,algebra.getInput(0,0));
    }
};


} //namespace corecvs
#endif


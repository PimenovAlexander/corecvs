#ifndef BASEKERNEL_H_
#define BASEKERNEL_H_
/**
 * \file baseKernel.h
 * \brief a header for BaseKernel.c
 *
 * \ingroup cppcorefiles
 * \date Sep 26, 2010
 * \author alexander
 */

#include "utils/global.h"

namespace corecvs {

/**
 *  \brief A base class for FastKernel kernels.
 *
 *  FastKernels use static polymorphism and that you may not inherit this class,
 *  but it's a demo of what methods are expected.
 *
 **/
template <typename Algebra>
class BaseKernel
{
public:
    typedef Algebra AlgebraType;

    Algebra *algebra;

    inline static int getCenterX(){ return 0; }
    inline static int getCenterY(){ return 0; }
    inline static int getSizeX()  { return 1; }
    inline static int getSizeY()  { return 1; }

    typedef typename Algebra::Type BaseType;

    /**
     * Every implementation should contain copy constructor
     * To be used with OpenMP (this reason is currently outdated)
     **/
    BaseKernel(BaseKernel &other) {
        setAlgebra(other.algebra);
    }

    BaseKernel() {
        setAlgebra(NULL);
    }

    void setAlgebra(Algebra *_algebra) {
        this->algebra = _algebra;
    }

    void process() {
    }

};


} //namespace corecvs
#endif  //BASEKERNEL_H_

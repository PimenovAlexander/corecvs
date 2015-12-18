/**
 * \file threshold.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Oct 8, 2010
 * \author alexander
 */

#ifndef THRESHOLD_H_
#define THRESHOLD_H_

#include <stdint.h>
namespace corecvs {

template <typename Algebra>
class ThresholdBinariseKernel
{
public:
    static const int inputNumber  = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; }
    inline static int getCenterY(){ return 0; }
    inline static int getSizeX()  { return 1; }
    inline static int getSizeY()  { return 1; }

    typedef typename Algebra::InputType Type;

    int16_t threshold;

template<typename OtherAlgebra>
    ThresholdBinariseKernel(const ThresholdBinariseKernel<OtherAlgebra> &other)
    {
        threshold = other.threshold;
    }

    ThresholdBinariseKernel() : threshold(0x8FF) {}

    ThresholdBinariseKernel(int16_t _threshold) : threshold(_threshold)  {}

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0);
        Type result = Algebra::branchlessMask(a > Type(threshold));
        algebra.putOutput(0,0,result);
    }
};



} //namespace corecvs
#endif /* THRESHOLD_H_ */


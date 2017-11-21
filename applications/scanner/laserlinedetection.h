/*#ifndef LASERLINEDETECTION_H
#define LASERLINEDETECTION_H
#include <cmath>
#include <vector>
#include "core/buffers/abstractBuffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

struct Kernel1D {
    std::vector<double> kernel;
    int offset;
    double divisor;
};

class KernelGenerator
{
public:
    virtual generate(Kernel1D &out) = 0;
};

class SecondOrderGauss : public KernelGenerator
{
    int from;
    int to;
    double mu;
    double sigma;

    double secondDerivativeNormalDistribution(int point)
    {
        return (exp(-pow(point-mu,2)/(2*pow(sigma,2))) * (-mu-sigma+point) * (-mu+sigma+point)) / (sqrt(2*M_PI)*pow(sigma,5));
    }

public:

    SecondOrderGauss(int from, int to, double sigma = 5, double mu = 0) :
        from  (from),
        to      (to),
        sigma(sigma),
        mu      (mu)
    {}

    virtual generate(Kernel1D &out) override
    {
        out.kernel.clear();
        out.divisor = 0.0;

        for (int i = from; i <= to; i++)
        {
            out.kernel.push_back(secondDerivativeNormalDistribution(i));
            out.divisor += out.kernel.back();
        }

        out.offset = out.size() / 2;
    }

};

class LaserLineDetection
{
private:

    RGB24Buffer *frame;
    KernelGenerator *generator;

public:
    LaserLineDetection(RGB24Buffer *frame, KernelGenerator *generator);
    void setFrame(RGB24Buffer *frame);
    void setGenerator(KernelGenerator *generator);
    AbstractBuffer<double> *calculateConvolutionNaive();
    vector<double> extractLaserPoints();
};

#endif // LASERLINEDETECTION_H*/

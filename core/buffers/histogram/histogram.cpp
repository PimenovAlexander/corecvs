/**
 * \file histogram.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 1, 2010
 * \author alexander
 */

#include "global.h"

#include "histogram.h"
namespace corecvs {

Histogram::~Histogram()
{
    // TODO Auto-generated destructor stub
}

/*
 * This probably should be speeded up
 * by some sort of optimization
 * */
int Histogram::getLowerPersentile(double persentile)
{
    /* TODO:Should this be int or unsigned */
    unsigned limit = (unsigned)(this->totalSum * persentile);
    unsigned sum = 0;
    for (unsigned i = 0; i < data.size(); i++)
    {
        sum += data[i];
        if (sum > limit)
            return i;
    }
    return (int)data.size() - 1 + min;
}

int Histogram::getHigherPersentile(double persentile)
{
    unsigned limit = (unsigned)(this->totalSum * persentile);
    unsigned sum = 0;
    for (int i = (int)data.size() - 1; i >= 0; i--)
    {
        sum += data[i];
        if (sum > limit)
            return i;
    }
    return min;
}

/**
 *   Gets the mean of the image
 **/
int Histogram::getMeanThreshold()
{
    double sumMoment = 0;
    for (unsigned t = 0; t < data.size(); t++)
        sumMoment += t * data[t];

    sumMoment /= totalSum;

    return min + sumMoment;
}

/**
 *    Returns the 50% quantile
 **/
int Histogram::getMedianThreshold()
{
    return getLowerPersentile(0.5);
}

/**
 *
 *  Gets Otsu threshold (http://en.wikipedia.org/wiki/Otsu%27s_method) for the histogram
 *
 *  We what to minimize the in-group variance
 *  \f{eqnarray}
 *    \sigma^2_w(t)=\omega_1(t)\sigma^2_1(t)+\omega_2(t)\sigma^2_2(t) \\
 *    \omega_1(t)=\sum_{i=min}^t P(i) \\
 *    \omega_2(t)=\sum_{i=t+1}^{max} P(i) \\
 *    \mu_1(t)= {1 \over \omega_1(t)} \sum_{i=min}^t {i P(i)}  \\
 *    \mu_2(t)= {1 \over \omega_2(t)} \sum_{i=t+1}^{max} {i P(i)} \\
 *    \sigma^2_1(t) = {1 \over \omega_1(t)} \sum_{i=min}^t {(i - \mu_1(t))^2 P(i)}
 *
 *
 * \f}
 * This is equal to maximizing between-groups variance, which can be computed iteratively
 *
 * \f{eqnarray}
 *  \sigma^2_b(t)=\sigma^2-\sigma^2_w(t)=\omega_1(t)\omega_2(t) (\mu_1(t)-\mu_2(t))^2
 * \f}
 *
 **/
int Histogram::getOtsuThreshold()
{
    double sum = 0;
    for (unsigned t = 0; t < data.size();t++)
        sum += t * data[t];

    double sumBack = 0;
    int weightBack = 0;
    int weightForward = 0;

    double maxVarience = 0;
    int result = 0;

    for (unsigned t = 0; t < data.size(); t++)
    {
       weightBack += data[t];
       if (weightBack == 0)
           continue;

       weightForward = totalSum - weightBack;
       if (weightForward == 0)
           break;

       sumBack += t * data[t];

       double meanBack    = sumBack / weightBack;
       double meanForward = (sum - sumBack) / weightForward;

       double diffMean = meanBack - meanForward;

       double mutalVarience = (double)weightBack * weightForward * diffMean * diffMean;

       // Check if new maximum found
       if (mutalVarience > maxVarience) {
           maxVarience = mutalVarience;
           result = t;
       }
    }
    return result + min;
}

double Histogram::getMeanValue(double *dev)
{
    double meanColor = 0.0;
    double meanSquare = 0.0;
    for (unsigned i = 0; i < data.size(); i++)
    {
        double c = data[i];
        meanColor += c * i;
        meanSquare += c * i * i;
    }

    meanColor  /= totalSum;
    meanSquare /= totalSum;

    if (dev != NULL) {
        (*dev) = sqrt(meanSquare - meanColor * meanColor);
    }

    return (meanColor + min);
}

} //namespace corecvs


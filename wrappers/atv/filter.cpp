#include "filter.h"

void filterLum(fftwf_complex *input, fftwf_complex * &output, u_short buffSize)
{
    auto *result = (fftwf_complex *) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    for (u_short freqNumber = 0; freqNumber < (buffSize / 2); freqNumber++)
    {
        result[freqNumber][0] = input[freqNumber][0];
        result[freqNumber][1] = input[freqNumber][1];
    }
    for (u_short freqNumber = (buffSize / 2); freqNumber < buffSize; freqNumber++)
    {
        result[freqNumber][0] = 0;
        result[freqNumber][1] = 0;
    }
    output = result;
}

void filterColor(fftwf_complex *input, fftwf_complex * &output, u_short buffSize)
{
    auto *result = (fftwf_complex *) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    for (u_short freqNumber = 0; freqNumber < (buffSize / 2); freqNumber++)
    {
        result[freqNumber][0] = 0;
        result[freqNumber][1] = 0;
    }
    for (u_short freqNumber = (buffSize / 2); freqNumber < buffSize; freqNumber++)
    {
        result[freqNumber][0] = input[freqNumber][0];
        result[freqNumber][1] = input[freqNumber][1];
    }
    output = result;
}

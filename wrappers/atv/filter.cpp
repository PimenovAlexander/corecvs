#include "filter.h"

void filterLum(fftwf_complex *input, fftwf_complex *output, u_short buffSize)
{
    for (u_short freqNumber = 0; freqNumber < (buffSize / 2); freqNumber++)
    {
        output[freqNumber][0] = input[freqNumber][0];
        output[freqNumber][1] = input[freqNumber][1];
    }
    for (u_short freqNumber = (buffSize / 2); freqNumber < buffSize; freqNumber++)
    {
        output[freqNumber][0] = 0;
        output[freqNumber][1] = 0;
    }
}

void filterColor(fftwf_complex *input, fftwf_complex *output, u_short buffSize)
{
    for (u_short freqNumber = 0; freqNumber < (buffSize / 2); freqNumber++)
    {
        output[freqNumber][0] = 0;
        output[freqNumber][1] = 0;
    }
    for (u_short freqNumber = (buffSize / 2); freqNumber < buffSize; freqNumber++)
    {
        output[freqNumber][0] = input[freqNumber][0];
        output[freqNumber][1] = input[freqNumber][1];
    }
}

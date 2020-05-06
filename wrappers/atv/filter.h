#ifndef ATV_FILTER_H
#define ATV_FILTER_H

#include <cstdlib>
#include <fftw3.h>


void filterLum(fftwf_complex *input, fftwf_complex* &output, u_short buffSize);

void filterColor(fftwf_complex *input, fftwf_complex* &output, u_short buffSize);

#endif //ATV_FILTER_H

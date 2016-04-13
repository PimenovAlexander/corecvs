/**
* FFTW wrapper definitions
*
* \author  pavel.vasilev
* \date    Nov 23, 2015
*/

#ifndef FFTWWRAPPER_H_
#define FFTWWRAPPER_H_

#include "global.h"

#include <vector>
#include <cstdint>

#include "abstractBuffer.h"

struct fftw_plan_s;
typedef double fftw_complex[2];

namespace corecvs {

class FFTW
{
public:
    enum Direction
    {
        Forward = 1,
        Backward = -1
    };

private:
    std::vector<int> mDimensions;
    fftw_plan_s*     mPlan = nullptr;
    Direction        mDirection;

public:

    FFTW();

    ~FFTW();

    /**
    * One-dimensional Fast Fourier Transform on complex data yielding complex data.
    *
    * \param   size            Data length.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    * \param   direction       Transform direction
    *
    */
    void transform(int size, fftw_complex *input, fftw_complex *output, Direction direction);

    /**
    * One-dimensional forward Fast Fourier Transform on real data yielding complex data.
    *
    * \param   size            Data length.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformForward(int size, double* input, fftw_complex *output);

    /**
    * One-dimensional forward Fast Fourier Transform on complex data yielding complex data.
    *
    * \param   size            Data length.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformForward(int size, fftw_complex *input, fftw_complex *output);

    /**
    * One-dimensional forward Fast Fourier Transform on real data yielding complex data.
    *
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformForward(std::vector<double>& input, fftw_complex *output);

    /**
    * One-dimensional backward Fast Fourier Transform.
    *
    * \param   size            Data length.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformBackward(int size, fftw_complex *input, fftw_complex *output);

    /**
    * One-dimensional backward Fast Fourier Transform yielding real data.
    *
    * \param   size            Data length.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformBackward(int size, fftw_complex *input, double *output);

    /**
    * Two-dimensional forward Fast Fourier Transform on real data yielding complex data.
    *
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformForward2D(AbstractBuffer<uint16_t, int32_t> *input, fftw_complex *output);

    /**
    * Two-dimensional forward Fast Fourier Transform on complex data yielding complex data.
    *
    * \param   sizeX            Data length in the first dimension.
    * \param   sizeY            Data length in the second dimension.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    * \param   direction       Transform direction
    *
    */
    void transform2D(int sizeX, int sizeY, fftw_complex *input, fftw_complex *output, Direction direction);

    /**
    * Two-dimensional backward Fast Fourier Transform on complex data yielding real data.
    *
    * \param   sizeX           Data length in the first dimension.
    * \param   sizeY           Data length in the second dimension.
    * \param   [in]  input     Input data.
    * \param   [out] output    Transform result.
    *
    */
    void transformBackward2D(int sizeX, int sizeY, fftw_complex *input, double *output); 

};

} // namespace corecvs

#endif // FFTWWRAPPER_H_

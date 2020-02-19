/**
 * \file gaussian.cpp
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 24, 2010
 * \author alexander
 */

#include "utils/global.h"

#include "buffers/kernels/gaussian.h"
namespace corecvs {

uint32_t Gaussian3x3int::data[9] = {
        1 , 2 , 1,
        2 , 4 , 2,
        1 , 2 , 1 };

Gaussian3x3int *Gaussian3x3int::instance = new Gaussian3x3int();


GaussianKernel::GaussianKernel(int h, int w, int x, int y, double sigma):
     DpKernel(h, w, x, y)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            element(i, j) = normalPDF<double>(i - y, sigma) * normalPDF<double>(j - x, sigma);
        }
    }
}

} //namespace corecvs


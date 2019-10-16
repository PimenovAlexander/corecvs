#include "laplace.h"

namespace corecvs {


LaplacianKernel::LaplacianKernel(int h, int w, int x, int y, double sigma):
     DpKernel(h, w, x, y)
{
    double sigma2 = sigma * sigma;
    double scale = -1.0 / (M_PI * sigma2 * sigma2);
    double denum = 2 * sigma2;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            double r2  = (i - y) * (i - y) + (j - x) * (j - x);
            double rsc = r2 / denum;
            element(i, j) = scale * (1 - rsc) * exp(rsc);
        }
    }
}

} // namespace corecvs

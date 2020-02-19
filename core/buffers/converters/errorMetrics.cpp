#include "buffers/converters/errorMetrics.h"
#include "buffers/converters/debayer.h"
#include "fileformats/ppmLoader.h"

using namespace corecvs;

double ErrorMetrics::mse(RGB48Buffer *img1, RGB48Buffer *img2, int border)
{
    if (img1->w != img2->w || img1->h != img2->h)
        return -1;

    double err = 0;

    for (int i = border; i < img1->h - border; i++)
    {
        for (int j = border; j < img1->w - border; j++)
        {
            for (int c = 0; c < 2; c++)
                err += pow((int32_t)img1->element(i, j)[c] - img2->element(i, j)[c], 2);
        }
    }

    return err / ((img1->h - 2 * border) * (img1->w - 2 * border) * 3);
}

double ErrorMetrics::mse(G12Buffer *img1, G12Buffer *img2, int border)
{
    if (img1->w != img2->w || img1->h != img2->h)
        return -1;

    double err = 0;

    for (int i = border; i < img1->h - border; i++)
    {
        for (int j = border; j < img1->w - border; j++)
        {
            err += pow((int32_t)img1->element(i, j) - img2->element(i, j), 2);
        }
    }

    return err / ((img1->h - 2 * border) * (img1->w - 2 * border));
}

double ErrorMetrics::psnr(RGB48Buffer *img1, RGB48Buffer *img2, int border)
{
    double MSE = mse(img1, img2, border);

    // handle the infinity
    if (MSE == 0)
        return 1;

    return MSE == -1 ? -1 : 20 * log10((1 << 8) - 1) - 10 * log10(MSE);
}

double ErrorMetrics::psnr(G12Buffer *img1, G12Buffer *img2, int border)
{
    double MSE = mse(img1, img2, border);

    if (MSE == 0)
        return 1;

    return MSE == -1 ? -1 : 20 * log10((1 << 12) - 1) - 10 * log10(MSE);
}

double ErrorMetrics::rmsd(RGB48Buffer *img1, RGB48Buffer *img2, int border)
{
    double MSE = mse(img1, img2, border);
    return MSE == -1 ? -1 : sqrt(MSE);
}

double ErrorMetrics::rmsd(G12Buffer *img1, G12Buffer *img2, int border)
{

    double MSE = mse(img1, img2, border);
    return MSE == -1 ? -1 : sqrt(MSE);
}

double ErrorMetrics::Ymse(G12Buffer *bayer, RGB48Buffer *debayer, int border, int bits)
{
    if (bayer->w != debayer->w || bayer->h != debayer->h)
        return -1;

    double err = 0;

    // this can be slow
    Debayer d(bayer, bits);
    AbstractBuffer<double, int> *y = new AbstractBuffer<double, int>(bayer->getSize());
    d.getYChannel(y);

    for (int i = border; i < bayer->h - border; i++)
    {
        for (int j = border; j < bayer->w - border; j++)
        {
            double orig = y->element(i, j);
            RGBTColor<uint16_t> elem = debayer->element(i, j);
            double yh = elem.yh();
            err += pow(orig - yh, 2);
        }
    }

    delete_safe(y);

    return err / ((bayer->h - 2 * border) * (bayer->w - 2 * border));
}

double ErrorMetrics::Yrmsd(G12Buffer *bayer, RGB48Buffer *debayer, int border, int bits)
{
    double MSE = Ymse(bayer, debayer, border, bits);
    return sqrt(MSE);
}
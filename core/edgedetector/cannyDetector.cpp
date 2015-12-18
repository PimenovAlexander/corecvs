/**
 * \file cannyDetector.cpp
 *
 * \date Oct 19, 2013
 **/

#include "cannyDetector.h"
#if 0
void CannyDetector::recursiveEdgeProver(G12Buffer *buffer, int h, int w)
{
    if ( h <= 0 || h >= buffer->h -1 || w <= 0 || w >= buffer->w - 1 ) return;

    int l = buffer->element(h,w - 1);
    int r = buffer->element(h,w + 1);
    int u = buffer->element(h - 1, w);
    int d = buffer->element(h + 1, w);

    int lu = buffer->element(h - 1, w - 1);
    int ld = buffer->element(h + 1, w - 1);
    int ru = buffer->element(h - 1, w + 1);
    int rd = buffer->element(h + 1, w + 1);
    if (l == gray)
    {
        buffer->element(h, w - 1) = white;
        recursiveEdgeProver(buffer,h, w - 1);
    }
    if (r == gray)
    {
        buffer->element(h, w + 1) = white;
        recursiveEdgeProver(buffer,h, w + 1);
    }
    if (u == gray)
    {
        buffer->element(h - 1, w) = white;
        recursiveEdgeProver(buffer,h - 1, w);
    }
    if (d == gray)
    {
        buffer->element(h + 1, w) = white;
        recursiveEdgeProver(buffer, h + 1, w);
    }
    if (lu == gray)
    {
        buffer->element(h - 1, w - 1) = white;
        recursiveEdgeProver(buffer, h - 1, w - 1);
    }
    if (ld == gray)
    {
        buffer->element(h + 1, w - 1) = white;
        recursiveEdgeProver(buffer, h + 1, w - 1);
    }
    if (ru == gray)
    {
        buffer->element(h - 1, w + 1) = white;
        recursiveEdgeProver(buffer,h - 1, w + 1);
    }
    if (rd == gray)
    {
        buffer->element(h + 1, w + 1) = white;
        recursiveEdgeProver(buffer, h + 1, w + 1);
    }

    return;
}

static G12Buffer *CannyDetector::doFilter(G12Buffer *input, const CannyParameters &mCannyParameters)
{
    if (input == NULL) {
        return NULL;
    }

    // non-maximum suppression
    DerivativeBuffer derBuf(input);
    G12Buffer *suppressedBuffer = derBuf.nonMaximalSuppression();

    // double thresholding
    for (int i = 0; i < suppressedBuffer->h; i++)
    {
        for (int j = 0; j < suppressedBuffer->w; j++ )
        {
             uint16_t pixel = suppressedBuffer->element(i,j);
             if (pixel < mCannyParameters.minimumThreshold())
             {
                 suppressedBuffer->element(i,j) = 0;
                 continue;
             }
             if (pixel > mCannyParameters.maximumThreshold())
             {
                 suppressedBuffer->element(i,j) = CannyFilter::white;
                 continue;
             }
             suppressedBuffer->element(i,j) = CannyFilter::gray;
        }
    }

    // edge tracking
    if (mCannyParameters.shouldEdgeDetect())
    {
        for (int i = 0; i < suppressedBuffer->h; i++)
        {
            for (int j = 0; j < suppressedBuffer->w; j++ )
            {
                if (suppressedBuffer->element(i,j) == CannyFilter::white)
                    CannyFilter::recursiveEdgeProver(suppressedBuffer, i, j);
            }
        }
        for (int i = 0; i < suppressedBuffer->h; i++)
            for (int j = 0; j < suppressedBuffer->w; j++ )
                if (suppressedBuffer->element(i,j) == CannyFilter::gray) suppressedBuffer->element(i,j) = 0;
    }

    return suppressedBuffer;
}
#endif

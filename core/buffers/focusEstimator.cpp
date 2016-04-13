#include <string.h> // NULL

#include "focusEstimator.h"

namespace corecvs {

FocusEstimator::Result FocusEstimator::calc(unsigned char* data, int h, int w, int stride, int x1, int y1, int x2, int y2, const int edgeScoreThr)
{
    if (x1 <= 1) x1 = 1;
    if (y1 <= 1) y1 = 1;
    if (x2 >= w) x2 = w - 1;
    if (y2 >= h) y2 = h - 1;

    if (data == NULL || h <= 2 || w <= 2 || stride <= 0)
        return { 0, 0 };

    double score = 0, scoreF = 0;
    unsigned char* line = data + stride;

    for (int y = 1; y < h - 1; ++y, line += stride)
    {
        int inRoi = y1 <= y && y < y2;
        unsigned char* pixel = line + 1;

        for (int x = 1; x < w - 1; ++x, ++pixel)
        {
            int sobelH = pixel[-1]      - pixel[1];
            int sobelV = pixel[-stride] - pixel[stride];

            int edgeScore = sobelH * sobelH + sobelV * sobelV;
            if (edgeScore > edgeScoreThr) {
                scoreF += edgeScore;
                if (inRoi && x1 <= x && x < x2)
                    score += edgeScore;
            }
        }
    }

    Result ret;
    ret.score     = (int)((double)score  / ((x2 - x1) * (y2 - y1)) + 0.5);
    ret.fullScore = (int)((double)scoreF / ((w  -  2) * (h  -  2)) + 0.5);
    return ret;
}

} // namespace corecvs

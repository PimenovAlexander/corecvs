#pragma once
/**
 * \file focusEstimator.h
 * \brief a header for focusEstimator.cpp
 *
 * \ingroup cppcorefiles
 * \date Sept 21, 2015
 * \author alexander
 */

namespace corecvs {

class FocusEstimator
{
public:
    /** \brief  Structure to describe result of the focus estimation
    */
    struct Result {
        int score;                                          // result for the chosen rectangular ROI
        int fullScore;                                      // result for the full image
    };

    /** \brief  Returns focus score that's integral over the image
     */
    static Result calc(unsigned char* data, int h, int w, int stride, int x1, int y1, int x2, int y2, const int edgeScoreThr = 10);

#if defined(RGB24Buffer) || defined(CRGB24BUFFER_H_)
    static Result calc(RGB24Buffer* buffer, int x1, int y1, int x2, int y2)
    {
        if (buffer == NULL)
            return { 0, 0 };

        G8Buffer *gray = buffer->getChannel(ImageChannel::GRAY);

        Result ret = calc(gray->data, gray->h, gray->w, gray->stride, x1, y1, x2, y2);

        delete_safe(gray);

        return ret;
    }
#endif

};

} //namespace corecvs

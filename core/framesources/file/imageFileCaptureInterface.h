#pragma once

#include <string>
#include "framesources/imageCaptureInterface.h"

class ImageFileCaptureInterface : public virtual ImageCaptureInterface
{
public:
    ImageFileCaptureInterface(const std::string *pathFmt = NULL, bool isVerbose = false);

    virtual ~ImageFileCaptureInterface() {}

    /** Aux function to get an image file name, which path name is given by additional parameters.
     *
     * \param imageNumber       - image file number in a series (since 1-st)
     * \param channelNumber     - image file channel number in a series (usual 0 or 1)
     */
    std::string   getImageFileName(uint imageNumber, uint channelNumber = 0);

    void     increaseImageFileCounter()  { mCount++;   }
    void     resetImageFileCounter()     { mCount = 1; }

    static   std::string  sPgmImageFileFmt;                 // commonly used format of series from pgm files

protected:
    std::string   mPathFmt;                                 // relative/absolute path format to the series of images that's given by app param
    bool     mVerbose;                                      // the verbose mode of action reporting into console
    unsigned mCount;                                        // active file counter

private:
    std::string   mPathPrefix;                              // automatically detected prefix to the path above to reduce dependence of the current dir
};

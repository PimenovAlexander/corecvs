#pragma once

#include <string>

#include "imageCaptureInterface.h"

using namespace std;

class ImageFileCaptureInterface : public ImageCaptureInterface
{
public:
    ImageFileCaptureInterface(const string *pathFmt = NULL, bool isVerbose = false);

    virtual ~ImageFileCaptureInterface() {}

    /** Aux function to get an image file name, which path name is given by additional parameters.
     *
     * \param imageNumber       - image file number in a series (since 1-st)
     * \param channelNumber     - image file channel number in a series (usual 0 or 1)
     */
    string   getImageFileName(uint imageNumber, uint channelNumber = 0);

    static   string  sPgmImageFileFmt;                      // commonly used format of series from pgm files

    void     increaseImageFileCounter()  { mCount++;   }
    void     resetImageFileCounter()     { mCount = 1; }

protected:
    string   mPathFmt;                                      // relative/absolute path format to the series of images that's given by app param
    bool     mVerbose;                                      // the verbose mode of action reporting into console
    unsigned mCount;                                        // active file counter

private:
    string   mPathPrefix;                                   // automatically detected prefix to the path above to reduce dependence of the current dir
};

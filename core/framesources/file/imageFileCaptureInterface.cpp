/**
 * \file imageFileCaptureInterface.cpp
 * \brief TODO
 *
 * \date Nov 20, 2012
 * \author s.fedorenko
 */
#include "utils/global.h"
#include "utils/utils.h"
#include "filesystem/folderScanner.h"

#include "imageFileCaptureInterface.h"


string ImageFileCaptureInterface::sPgmImageFileFmt = "image%04u_c%d.pgm";       // commonly used format of series from pgm files

ImageFileCaptureInterface::ImageFileCaptureInterface(const string *pathFmt, bool isVerbose)
    : mVerbose(isVerbose)
    , mPathPrefix("")
{
    if (pathFmt != NULL) {
        mPathFmt = *pathFmt;                                // if the path is given then store it for the further usage
    }
    resetImageFileCounter();
}

string ImageFileCaptureInterface::getImageFileName(uint imageNumber, uint channelNumber)
{
    // TODO: How to replace this with C++ construct? I.e. MFC's CString has a method Format() for this, but std::string doesn't. :(
    char pathName[256];

    if (mPathFmt.find("%0.0s") != string::npos)             // format comes from GUI with constant pair for debugging
    {
        snprintf2buf(pathName, mPathFmt.c_str(), "", channelNumber);// str + int (the way to disable pair switching over series)
    }
    else if (mPathFmt.find("%") != string::npos)            // format comes from GUI with series processing
    {
        // TODO: it would be good here to check that format expects exactly 2 integers
        snprintf2buf(pathName, mPathFmt.c_str(), imageNumber, channelNumber); // expects 2 ints
    }
    else if (!mPathFmt.empty())                             // format comes as a ready path, only from some tests
    {
        // one exclusion is possible here: when ctor got a direct path to the file!
        if (FolderScanner::pathExists(mPathFmt)) {
            return mPathFmt;                                // return immediately the initial path as it's not a format
        }

        // mPathFmt has a real folder path
        if (mPathFmt[mPathFmt.length() - 1] != '/') {       // append slash at the end if absent
            mPathFmt += '/';
        }
        mPathFmt += sPgmImageFileFmt;                       // add default format to the pathFmt

        return getImageFileName(imageNumber, channelNumber);
    }

    if (imageNumber == 1 && mPathPrefix.empty() && !FolderScanner::pathExists(pathName))
    {
        /**
         * We're to check correctness of the current directory, but we need a path to "data" folder with image files.
         *
         *          "../"  - Bin directory curDir
         * "../../../../"  - MSVC sets curDir to the project dir (= "host_soft")
         * "../../../../"  - OpenCL project curDir
         *
         * \note  It's slightly dangerously to check 4 levels up for a requested file,\n
         *        especially if there's a special folder structure with different series...
         *        Therefore for Unix it's checked one level up as we've run from Bin dir,\n
         *        and usually we use path as "data/pair" for test images.
         */
        if (FolderScanner::pathExists((string("../") + pathName).c_str()))
        {
            mPathPrefix = "../";
        }
#ifdef WIN32
        else if (FolderScanner::pathExists((string("../../../../") + pathName).c_str()))
        {
            mPathPrefix = "../../../../";
        }
#endif
    }

    return mPathPrefix + pathName;
}

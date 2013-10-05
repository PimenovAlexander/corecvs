/**
 * \file imageFileCaptureInterface.cpp
 * \brief TODO
 *
 * \date Nov 20, 2012
 * \author s.fedorenko
 */
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "global.h"

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
        QFileInfo fi(mPathFmt.c_str());
        if (fi.isFile() && fi.exists()) {
            return mPathFmt;                                // return immediately the initial path as it's not a format
        }

        // mPathFmt has a real folder path
        if (mPathFmt[mPathFmt.length() - 1] != '/') {       // append slash at the end if absent
            mPathFmt += '/';
        }
        mPathFmt += sPgmImageFileFmt;                       // add default format to the pathFmt

        return getImageFileName(imageNumber, channelNumber);
    }

    if (imageNumber == 1 && mPathPrefix.empty() && !QFile::exists(pathName))
    {
        /**
         * We're to check correctness of the current directory, but we need a path to "data" folder with image files.
         *
         *       "../"  - Bin directory curDir
         *    "../../"  - OpenCL project curDir
         * "../../../"  - MSVC sets curDir to the project dir (= "hostSoft")
         *
         * \note  But from another side it's dangerously to scan up to 3 levels by trying to find a requested file.\n
         *        especially if there's a special folder structure with different series...
         *        Therefore for Unix it's applied only for one parent as we run from the bin directory, \n
         *        but usually use images by path like this: "data/pair".
         */
        string path(pathName);
        string prefix;

#ifdef WIN32
        cuint numRecursiveFoldersToCheck = 3;
#else
        cuint numRecursiveFoldersToCheck = 1;               // one check for parent dir is ok or also dangerously?
#endif
        for (unsigned k = 0; k < numRecursiveFoldersToCheck; k++)
        {
            path  .insert(0, "../");
            prefix.insert(0, "../");                        // try at folder one step up

            if (QFile::exists(path.c_str()))
            {
                mPathPrefix.assign(prefix);
                break;
            }
        }
    }

    return mPathPrefix + pathName;
}

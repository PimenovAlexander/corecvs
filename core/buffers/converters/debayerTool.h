/**
 * \file    buffers/converters/debayerTool.h
 *
 * Declares the DebayerTool class.
 */

#ifndef DEBAYER_TOOL_H
#define DEBAYER_TOOL_H

#include <string>

#include "core/utils/global.h"

#include "core/reflection/commandLineSetter.h"
#include "core/buffers/converters/debayer.h"

namespace corecvs {

class DebayerTool
{
protected:
    Debayer::Parameters  debayerParams;     // Debayer params obtained from command line params

    virtual void         usage(cchar* progName);
                         
    int                  doCompare(CommandLineSetter &s, const std::string &source);
    int                  doConvertToBayer(const std::string &src, const std::string &dst, int outBits, int bpos);
    bool                 endsWith(const std::string &src, const std::string &ext) const;

    virtual bool         saveRgb24(const std::string &dst, const RGB24Buffer& rgb24)
    {
        // bmp, ppm are processed on the upper level, but standard jpg/png require core/utils
        CORE_UNUSED(dst);
        CORE_UNUSED(rgb24);
        return false;
    }

    virtual RGB24Buffer* loadRgb24(const std::string &src);
    virtual RGB48Buffer* loadRgb48(const std::string &src, int inBits, const std::string &bayer8);

public:
    DebayerTool() {}
   ~DebayerTool() {}

    int                  proceed(int argc, const char **argv);
};

} // namespace corecvs

#endif // DEBAYER_TOOL_H

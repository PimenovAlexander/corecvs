#ifndef DUMMY_VIDEO_ENCODER_INTERFACE_H
#define DUMMY_VIDEO_ENCODER_INTERFACE_H

#include "buffers/rgb24/rgb24Buffer.h"
#include <string>

namespace corecvs {

class DummyVideoEncoderInterface
{
public:
    bool trace = true;

    DummyVideoEncoderInterface(bool trace = false);

    virtual int startEncoding(const std::string &name, int h, int w, int codec_id = -1)
    {
        if (trace) SYNC_PRINT(("DummyVideoEncoderInterface::startEncoding(%s, %d, %d, %d)\n", name.c_str(), h, w, codec_id));
        return 0;
    }

    virtual void addFrame(corecvs::RGB24Buffer * /*input*/)
    {
        if (trace) SYNC_PRINT(("DummyVideoEncoderInterface::addFrame()\n"));
    }

    virtual void endEncoding()
    {
        if (trace) SYNC_PRINT(("DummyVideoEncoderInterface::endEncoding()\n"));
    }
};

} // namespace corecvs

#endif // DUMMY_VIDEO_ENCODER_INTERFACE_H

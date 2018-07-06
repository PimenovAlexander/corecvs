#include "swScaler.h"

#include "core/math/mathUtils.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
}


using namespace corecvs;

template<typename BufferType, int swScaleFormatId>
BufferType *SWScaler::scale(BufferType *input, double scaler)
{
    // make parameter snapshot
    int scaledWidth  = fround(input->w * scaler);
    int scaledHeight = fround(input->h * scaler);

    BufferType *toReturn = new BufferType(scaledHeight, scaledWidth, false);

    uint8_t *inData[1]  = { (uint8_t *)input->data    };
    uint8_t *outData[1] = { (uint8_t *)toReturn->data };


    int inStride [1] = { input->stride    * sizeof(typename BufferType::InternalElementType) };
    int outStride[1] = { toReturn->stride * sizeof(typename BufferType::InternalElementType) };

    SwsContext *scaleContext = sws_getContext(
                input->w,
                input->h,
                (AVPixelFormat)swScaleFormatId,
                scaledWidth,
                scaledHeight,
                (AVPixelFormat)swScaleFormatId,
                SWS_BILINEAR , NULL, NULL, NULL);


    sws_scale(scaleContext,
        inData,
        inStride,
        0,
        input->h,
        outData,
        outStride);

    sws_freeContext(scaleContext);

    return toReturn;
}

RGB24Buffer *SWScaler::scale (RGB24Buffer *input, double scaler)
{
    return scale<RGB24Buffer, AV_PIX_FMT_RGBA>(input, scaler);
}

G8Buffer *SWScaler::scale (G8Buffer *input, double scaler)
{
    return scale<G8Buffer, AV_PIX_FMT_GRAY8>(input, scaler);
}

G12Buffer *SWScaler::scale (G12Buffer *input, double scaler)
{
    return scale<G12Buffer, AV_PIX_FMT_GRAY16>(input, scaler);
}

SWScaler::SWScaler()
{

}

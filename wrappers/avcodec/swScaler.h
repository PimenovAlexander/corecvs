#ifndef SWSCALER_H
#define SWSCALER_H

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/g8Buffer.h"

class SWScaler
{
public:
    SWScaler();

    static corecvs::RGB24Buffer *scale (corecvs::RGB24Buffer *input, double scaler);
    static corecvs::G8Buffer    *scale (corecvs::G8Buffer    *input, double scaler);
    static corecvs::G12Buffer   *scale (corecvs::G12Buffer   *input, double scaler);


private:

template<typename BufferType, int swScaleFormatId>
    static BufferType *scale(BufferType *input, double scale);

};

#endif // SWSCALER_H

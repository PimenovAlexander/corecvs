#pragma once

class MjpegDecoder
{
public:
    MjpegDecoder();

    int decode(unsigned char **pic
            , unsigned char *buf, int *width, int *height);
};



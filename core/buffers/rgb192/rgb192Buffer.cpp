#include "rgb192Buffer.h"



RGB192Buffer::RGB192Buffer(RGB24Buffer *that) :
    RGB192BufferBase (that->h, that->w, false)

{
     fillWith(that);
}

RGB24Buffer *RGB192Buffer::toRGB24Buffer()
{
    RGB24Buffer *toReturn = new RGB24Buffer(h,w, false);
    copyTo(toReturn);
    return toReturn;
}

RGB192Buffer *RGB192Buffer::FromRGB24Buffer(RGB24Buffer *that)
{
    RGB192Buffer *toReturn = new RGB192Buffer(h,w, false);
    toReturn->fillWith(that);
    return toReturn;
}

void RGB192Buffer::fillWith(RGB24Buffer *that)
{
    int minH = std::min(h, that->h);
    int minW = std::min(w, that->w);

    for (int i = 0; i < minH; i++)
    {
        const RGB24Buffer ::InternalElementType   *inRunner = &that->element(i, 0);
        RGB192Buffer::InternalElementType *outRunner = &this->element(i, 0);

        for (int j = 0; j < minW; j++)
        {
            *outRunner = RGB192Color::fromRGBColor(*inRunner);
            inRunner++;
            outRunner++;
        }
    }
}


void RGB192Buffer::copyTo(RGB24Buffer *that)
{
    int minH = std::min(h, that->h);
    int minW = std::min(w, that->w);

    for (int i = 0; i < minH; i++)
    {
        RGB24Buffer ::InternalElementType *outRunner = &that->element(i, 0);
        const RGB192Buffer::InternalElementType *inRunner = &this->element(i, 0);

        for (int j = 0; j < minW; j++)
        {
            *outRunner = RGB24Buffer::InternalElementType::FromDouble(*inRunner);
            inRunner++;
            outRunner++;
        }
    }
}


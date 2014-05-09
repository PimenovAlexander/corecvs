#include "commonStructures.h"


LRImage::LRImage(int newNumberInImageCollection, double newShiftX, double newShiftY, double newAngleDegree, double newCoefficient)
{
    numberInImageCollection_ = newNumberInImageCollection;
    shiftX_ = newShiftX;
    shiftY_ = newShiftY;
    angleDegree_ = newAngleDegree;
    coefficient_ = newCoefficient;
}

#if 0
RGB192Color::RGB192Color(double red, double green, double blue)
{
    r_ = red;
    g_ = green;
    b_ = blue;
}

RGB192Color::RGB192Color()
{
    r_ = 0;
    g_ = 0;
    b_ = 0;
}
#endif


RGB192Buffer::RGB192Buffer(int height, int width)
{
    h_ = height;
    w_ = width;
    data_ = new RGB192Color*[h_];
    for (int i = 0; i < h_; i++)
        data_[i] = new RGB192Color[w_];
}

RGB192Buffer::RGB192Buffer(RGB24Buffer *imageToCopy)
{
    h_ = imageToCopy -> getH();
    w_ = imageToCopy -> getW();
    data_ = new RGB192Color*[h_];
    for (int i = 0; i < h_; i++)
        data_[i] = new RGB192Color[w_];
    for (int i = 0; i < h_; i++)
        for (int j = 0; j < w_; j++)
        {
            data_[i][j].r() = (double)imageToCopy -> element(i, j).r();
            data_[i][j].g() = (double)imageToCopy -> element(i, j).g();
            data_[i][j].b() = (double)imageToCopy -> element(i, j).b();
        }
}

int RGB192Buffer::getH()
{
    return h_;
}


int RGB192Buffer::getW()
{
    return w_;
}

RGB192Buffer::~RGB192Buffer()
{
    for (int i = 0; i < h_; i++)
        delete [] data_[i];
    delete [] data_;
}

void RGB192Buffer::copy192to24(RGB24Buffer *image24)
{
    for (int i = 0; i < image24 -> getH(); i++)
        for (int j = 0; j < image24 -> getW(); j++)
        {
            image24 -> element(i, j).r() = (int)data_[i][j].r();
            image24 -> element(i, j).g() = (int)data_[i][j].g();
            image24 -> element(i, j).b() = (int)data_[i][j].b();
        }
}

void RGB192Buffer::copy24to192(RGB24Buffer *image24)
{
    for (int i = 0; i < image24 -> getH(); i++)
        for (int j = 0; j < image24 -> getW(); j++)
        {
            data_[i][j].r() = (double)image24 -> element(i, j).r();
            data_[i][j].g() = (double)image24 -> element(i, j).g();
            data_[i][j].b() = (double)image24 -> element(i, j).b();
        }
}

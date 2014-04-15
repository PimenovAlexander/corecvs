#include <rgb24Buffer.h>

#pragma once

class LRImage
{
public:
    int numberInImageCollection_;
    double shiftX_;
    double shiftY_;
    double angleDegree_;
    double coefficient_;
public:
    LRImage(int newNumberInImageCollection, double newShiftX, double newShiftY, double newAngleDegree, double newCoefficient);
};

struct RGBmask
{
    bool rUp;
    bool rDown;
    bool gUp;
    bool gDown;
    bool bUp;
    bool bDown;
};



class RGB192Color
{
private:
    double r_;
    double g_;
    double b_;
public:
    RGB192Color(double red, double green, double blue);

    RGB192Color();
    inline double & r()
    {
            return (*this).r_;
    }

    inline double & g()
    {
            return (*this).g_;
    }

    inline double & b()
    {
            return (*this).b_;
    }
};

class RGB192Buffer
{
private:
    int h_;
    int w_;
    RGB192Color **data_;
public:
    int getH();

    int getW();

    RGB192Buffer(int height, int width);

    RGB192Buffer(RGB24Buffer *imageToCopy);

    inline RGB192Color & element(int i, int j)
    {
        return data_[i][j];
    }

    void copy24to192(RGB24Buffer *image24);

    void copy192to24(RGB24Buffer *image24);

    ~RGB192Buffer();
};

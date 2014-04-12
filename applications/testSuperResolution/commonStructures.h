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

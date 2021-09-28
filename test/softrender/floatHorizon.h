#ifndef FLOATHORIZON_H
#define FLOATHORIZON_H

#include <core/buffers/rgb24/rgb24Buffer.h>




class FloatHorizon
{
public:
    FloatHorizon();
    corecvs::RGB24Buffer *output = NULL;
    double phi;
    double psi;

    int stopAt = -1;
    bool shouldHorison = false;

    void render0();
    void render1();
    void render2();

    vector<double> top;
    vector<double> bottom;
    void init();
    void drawDebug();

    void render3();

};

#endif // FLOATHORIZON_H

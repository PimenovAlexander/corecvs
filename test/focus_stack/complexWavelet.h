#ifndef COMPLEXWAVELET_H
#define COMPLEXWAVELET_H

#include "core/buffers/abstractBuffer.h"
#include "core/buffers/float/dpImage.h"
#include "fsAlgorithm.h"
using namespace std;

class ComplexWavelet : public FSAlgorithm
{
public:
    ComplexWavelet(){}
    void doStacking(vector<corecvs::RGB24Buffer*> & imageStack, corecvs::RGB24Buffer * result) override;
    ~ComplexWavelet() override {}

private:
    static corecvs::AbstractBuffer<double> * merge(corecvs::AbstractBuffer<double> * in, int type1, int type2);
    static corecvs::AbstractBuffer<double> * split(corecvs::AbstractBuffer<double> * in, int type1, int type2);

    static pair<corecvs::AbstractBuffer<double> *, corecvs::AbstractBuffer<double> *> analysis(corecvs::DpImage * in, int n, int length);
    static void subtract(corecvs::AbstractBuffer<double> * im1, corecvs::AbstractBuffer<double> * im2);
    static void add(corecvs::AbstractBuffer<double> * im1, corecvs::AbstractBuffer<double> * im2);

    static corecvs::AbstractBuffer<double> * synthesis(corecvs::AbstractBuffer<double> * inRe, corecvs::AbstractBuffer<double> * inIm, int n);
};

#endif //COMPLEXWAVELET_H

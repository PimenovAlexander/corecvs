#ifndef COMPLEXWAVELET_H
#define COMPLEXWAVELET_H

#include "FSAlgorithm.h"
using namespace std;

class ComplexWavelet : public FSAlgorithm
{
public:
    ComplexWavelet(){}
    void doStacking(vector<RGB24Buffer*> & imageStack, RGB24Buffer * result) override;
    ~ComplexWavelet() override {}

private:
    static AbstractBuffer<double> * merge(AbstractBuffer<double> * in, int type1, int type2);
    static AbstractBuffer<double> * split(AbstractBuffer<double> * in, int type1, int type2);
    static pair<AbstractBuffer<double> *, AbstractBuffer<double> *> analysis(DpImage * in, int n, int length);
    static void subtract(AbstractBuffer<double> * im1, AbstractBuffer<double> * im2);
    static void add(AbstractBuffer<double> * im1, AbstractBuffer<double> * im2);
    static AbstractBuffer<double> * synthesis(AbstractBuffer<double> * inRe, AbstractBuffer<double> * inIm, int n);
};

#endif //COMPLEXWAVELET_H
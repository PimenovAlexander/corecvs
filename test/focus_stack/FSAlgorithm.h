#ifndef FSALGORITHM_H
#define FSALGORITHM_H

#include "core/buffers/rgb24/rgb24Buffer.h"

using namespace corecvs;

class FSAlgorithm {
public:
    virtual void doStacking(std::vector<RGB24Buffer*> & imageStack, RGB24Buffer * result) = 0;
    virtual ~FSAlgorithm(){}
};

#endif // FSALGORITHM_H

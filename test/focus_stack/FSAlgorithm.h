#ifndef FSALGORITHM_H
#define FSALGORITHM_H

#include "core/buffers/rgb24/rgb24Buffer.h"

using namespace corecvs;

class FSAlgorithm {
public:
    virtual void doStacking(std::vector<RGB24Buffer*> & imageStack, RGB24Buffer * result)
    {
        SYNC_PRINT(("No focus stacking realisation found!\n"));
    }
    virtual ~FSAlgorithm(){}
};

#endif // FSALGORITHM_H

#ifndef FSALGORITHM_H
#define FSALGORITHM_H

#include "core/buffers/rgb24/rgb24Buffer.h"

using namespace corecvs;

/**
*    This class is a base for Focus Stacking algorithms.
**/

class FSAlgorithm {
public:
    /**
     *   Method which implements focus stacking.
     *   @param imageStack Stack with input images.
     *   @param result Pointer to image where the result of stacking should be saved.
     **/
    virtual void doStacking(std::vector<RGB24Buffer*> & imageStack, RGB24Buffer * result) = 0;
    virtual ~FSAlgorithm(){}
};

#endif // FSALGORITHM_H

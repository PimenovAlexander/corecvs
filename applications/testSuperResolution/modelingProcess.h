#pragma once
#include "rgb24Buffer.h"
#include "commonStructures.h"
#include <deque>

RGB24Buffer *simpleModelingProcess(RGB24Buffer *startImage, double coefficient, int shiftX, int shiftY, double angleDegree);
RGB24Buffer *squareBasedResamplingRotate(RGB24Buffer *startImage, double coefficient, int shiftX, int shiftY, double angleDegree);
RGB24Buffer *simpleModelingProcessWithList(std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages);

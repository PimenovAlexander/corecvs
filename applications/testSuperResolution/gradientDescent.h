#pragma once

#include "rgb24Buffer.h"
#include "commonStructures.h"
#include <deque>

double differenceBetweenImages(RGB24Buffer* image1, RGB24Buffer* image2);

//double diffFunc(RGB24Buffer *startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages);

void improve(RGB192Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages,
               std::deque<RGB192Buffer*> listOfImagesFromUpsampled, std::deque<double> *results,
               double step,
               double minQualityImprovement, int numberOfIterations);

bool iteration(RGB192Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages,
               std::deque<RGB192Buffer*> listOfImagesFromUpsampled, std::deque<double> *results,
               double step,
               double minQualityImprovement, RGBmask *mask,
               int rX,
               int rY,
               int rColor,
               int rDir);

void getNewCoordinates(double oldX, double oldY, double coefficient, double shiftX, double shiftY, double angle, double oldCenterX, double oldCenterY, double newCenterX, double newCenterY, double *newX, double *newY);

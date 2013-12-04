#include "resamples.h"

RGB24Buffer *nearestNeighbourVersion1(RGB24Buffer *startImage) {
    RGB24Buffer *result = new RGB24Buffer(startImage -> getH()/2, startImage -> getW()/2, false);
    for (int i = 0 ; i < result -> getH(); i++)
        for (int j = 0; j < result -> getW(); j++)
            result -> element(i,j) = startImage -> element(2*i,2*j);
    return result;
}


RGB24Buffer *nearestNeighbourVersion2(RGB24Buffer *startImage) {
    RGB24Buffer *result = new RGB24Buffer(startImage -> getH()/2, startImage -> getW()/2, false);
    for (int i = 0 ; i < result -> getH(); i++)
        for (int j = 0; j < result -> getW(); j++)
            result -> element(i,j) = RGBColor((startImage -> element(2*i,2*j).r() + startImage -> element(2*i+1,2*j).r() +startImage -> element(2*i,2*j+1).r() + startImage -> element(2*i+1,2*j+1).r())/4,
                                              (startImage -> element(2*i,2*j).g() + startImage -> element(2*i+1,2*j).g() +startImage -> element(2*i,2*j+1).g() + startImage -> element(2*i+1,2*j+1).g())/4,
                                              (startImage -> element(2*i,2*j).b() + startImage -> element(2*i+1,2*j).b() +startImage -> element(2*i,2*j+1).b() + startImage -> element(2*i+1,2*j+1).b())/4);
    return result;
}

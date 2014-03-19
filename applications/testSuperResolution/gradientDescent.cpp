#include "gradientDescent.h"
#include <cmath>
#include "transformations.h"
#include "resamples.h"

double differenceBetweenImages(RGB24Buffer* image1, RGB24Buffer* image2)
{
    double sum = 0;
    for (int i = 0 ; (i < image1 -> getH()) && (i < image2 -> getH()); i++)
       for (int j = 0 ; (j < image1 -> getW()) && (j < image2 -> getW()); j++)
       {
           sum += sqrt((
                       (double)((int)(image1 -> element(i,j).r()) - (int)(image2 -> element(i,j).r()))/255
                     * (double)((int)(image1 -> element(i,j).r()) - (int)(image2 -> element(i,j).r()))/255
                     + (double)((int)(image1 -> element(i,j).g()) - (int)(image2 -> element(i,j).g()))/255
                     * (double)((int)(image1 -> element(i,j).g()) - (int)(image2 -> element(i,j).g()))/255
                     + (double)((int)(image1 -> element(i,j).b()) - (int)(image2 -> element(i,j).b()))/255
                     * (double)((int)(image1 -> element(i,j).b()) - (int)(image2 -> element(i,j).b()))/255
                       )/3);
       }
    sum = sqrt (sum/((image1 -> getH()) * (image1 -> getW())));
    return sum;
}

double diffFunc(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages)
{
    double sum = 0;
    for (int i = 0; i < (int)LRImages.size(); i++)
    {
        RGB24Buffer *rotatedImage = rotate(startImage, LRImages.at(i).angleDegree_);
        RGB24Buffer *result = squareBasedResampling(rotatedImage,LRImages.at(i).coefficient_,
                                                    LRImages.at(i).shiftX_,LRImages.at(i).shiftY_,LRImages.at(i).angleDegree_);
        sum += differenceBetweenImages(result, imageCollection.at(LRImages.at(i).numberInImageCollection_));
        delete_safe(rotatedImage);
        delete_safe(result);
    }
    return sum;
}

#include "gradientDescent.h"
#include <cmath>
#include "transformations.h"
#include "resamples.h"
#include <cstdlib>
#include <time.h>

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
    sum /= (double)LRImages.size();
    return sum;
}

void iteration(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages, double step,
               double minQualityImprovement)
{
    double oldValue = diffFunc(startImage,imageCollection,LRImages);
    srand (time(NULL));
    int rX = rand() % (int)(startImage -> getH());
    int rY = rand() % (int)(startImage -> getW());
    int rColor = rand() % 3;
    int rDir = rand() % 2;
    if (rDir == 0)
        rDir = -1;

    if (rColor == 0)
    {
        if ((int)(startImage -> element(rX,rY).r()) > 255 - step)
            rDir = -1;
        if ((int)(startImage -> element(rX,rY).r()) < step)
            rDir = 1;
        startImage -> element(rX,rY).r() += rDir * step;
        double newValue = diffFunc(startImage,imageCollection,LRImages);
        if (oldValue - newValue < minQualityImprovement)
            startImage -> element(rX,rY).r() -= rDir * step;
    }

    if (rColor == 1)
    {
        if ((int)(startImage -> element(rX,rY).g()) > 255 - step)
            rDir = -1;
        if ((int)(startImage -> element(rX,rY).g()) < step)
            rDir = 1;
        startImage -> element(rX,rY).g() += rDir * step;
        double newValue = diffFunc(startImage,imageCollection,LRImages);
        if (oldValue - newValue < minQualityImprovement)
            startImage -> element(rX,rY).g() -= rDir * step;
    }

    if (rColor == 2)
    {
        if ((int)(startImage -> element(rX,rY).b()) > 255 - step)
            rDir = -1;
        if ((int)(startImage -> element(rX,rY).b()) < step)
            rDir = 1;
        startImage -> element(rX,rY).b() += rDir * step;
        double newValue = diffFunc(startImage,imageCollection,LRImages);
        if (oldValue - newValue < minQualityImprovement)
            startImage -> element(rX,rY).b() -= rDir * step;
    }
}

#include "resamples.h"
#include "modelingProcess.h"
#include "transformations.h"
#include "listsOfLRImages.h"
RGB24Buffer *simpleModelingProcess(RGB24Buffer *startImage, double coefficient, int shiftX, int shiftY, double angleDegree)
{
    int shiftX1, shiftY1, shiftX2, shiftY2, shiftX3, shiftY3, shiftX4, shiftY4;
    shiftX1 = shiftX;
    shiftY1 = shiftY;
    shiftX2 = 2 * shiftX;
    shiftY2 = 2 * shiftY;
    shiftX3 = 3 * shiftX;
    shiftY3 = 3 * shiftY;
    shiftX4 = 4 * shiftX;
    shiftY4 = 4 * shiftY;

    RGB24Buffer *image1 = squareBasedResampling(startImage, coefficient, shiftX1, shiftY1, angleDegree);
    RGB24Buffer *image2 = squareBasedResampling(startImage, coefficient, shiftX2, shiftY2, angleDegree);
    RGB24Buffer *image3 = squareBasedResampling(startImage, coefficient, shiftX3, shiftY3, angleDegree);
    RGB24Buffer *image4 = squareBasedResampling(startImage, coefficient, shiftX4, shiftY4, angleDegree);

    image1 = resampleWithBilinearInterpolation(image1, 1 / coefficient);
    image2 = resampleWithBilinearInterpolation(image2, 1 / coefficient);
    image3 = resampleWithBilinearInterpolation(image3, 1 / coefficient);
    image4 = resampleWithBilinearInterpolation(image4, 1 / coefficient);

    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()),(int)(startImage -> getW()), false);
    for (int i = shiftX4; i < result -> getH(); i++)
        for (int j = shiftY4 ; j < result -> getW(); j++) {

            result -> element(i, j).r() =
                    (image1 -> element(i - shiftX1, j - shiftY1).r() +
                     image2 -> element(i - shiftX2, j - shiftY2).r() +
                     image3 -> element(i - shiftX3, j - shiftY3).r() +
                     image4 -> element(i - shiftX4, j - shiftY4).r()) / 4;

            result -> element(i,j).g() =
                    (image1 -> element(i - shiftX1, j - shiftY1).g() +
                     image2 -> element(i - shiftX2, j - shiftY2).g() +
                     image3 -> element(i - shiftX3, j - shiftY3).g() +
                     image4 -> element(i - shiftX4, j - shiftY4).g()) / 4;

            result -> element(i,j).b() =
                    (image1 -> element(i - shiftX1, j - shiftY1).b() +
                     image2 -> element(i - shiftX2, j - shiftY2).b() +
                     image3 -> element(i - shiftX3, j - shiftY3).b() +
                     image4 -> element(i - shiftX4, j - shiftY4).b()) / 4;
        }
    return result;
}

RGB24Buffer *squareBasedResamplingRotate(RGB24Buffer *startImage, double coefficient, int shiftX, int shiftY, double angleDegree)
{
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()),(int)(startImage -> getW()), false);
    RGB24Buffer *image = squareBasedResampling(startImage, coefficient, shiftX, shiftY, angleDegree);
    result = rotate(image, angleDegree);
    return result;
}

RGB24Buffer *simpleModelingProcessWithList(std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages)
{
    /*RGB24Buffer *result = new RGB24Buffer((int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getH()),(int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getW()), false);
    RGB24Buffer *image = new RGB24Buffer((int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getH()),(int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getW()), false);
    std::deque<RGB24Buffer*> imageCollection2;
    imageCollection2 = imageCollection;
    for(int k = 0; k < LRImages.size(); k++)
    {
        imageCollection2.at(LRImages.at(k).numberInImageCollection_) = rotate(imageCollection.at(LRImages.at(k).numberInImageCollection_), -LRImages.at(k).angleDegree_);
        imageCollection2.at(LRImages.at(k).numberInImageCollection_) = resampleWithBilinearInterpolation(imageCollection.at(LRImages.at(k).numberInImageCollection_), 1/LRImages.at(k).coefficient_);
    }
    for(int i = 0; i < imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getH(); i++)
        for(int j = 0; j < imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getW(); j++)
        {
            for(int k = 0; k < LRImages.size(); k++)
            {
                image = imageCollection2.at(LRImages.at(k).numberInImageCollection_);
                result -> element(i, j).r() += (image -> element(i, j).r()/LRImages.size());
                result -> element(i, j).g() += (image -> element(i, j).g()/LRImages.size());
                result -> element(i, j).b() += (image -> element(i, j).b()/LRImages.size());
            }
            //result -> element(i, j).r() /= LRImages.size();
            //result -> element(i, j).g() /= LRImages.size();
            //result -> element(i, j).b() /= LRImages.size();
        }
    return result;*/
}

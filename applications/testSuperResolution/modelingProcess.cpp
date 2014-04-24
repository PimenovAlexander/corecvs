#include "resamples.h"
#include "modelingProcess.h"
#include "transformations.h"
#include "commonStructures.h"
#include "polygons.h"
#include "gradientDescent.h"

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
    RGB24Buffer *result = new RGB24Buffer((double)((int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getH()) / (double)LRImages.at(0).coefficient_)
                                          ,(double)((int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getW()) / (double)LRImages.at(0).coefficient_), false);
    //RGB24Buffer *image = new RGB24Buffer((double)((int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getH()) / (double)LRImages.at(0).coefficient_)
     //                                     ,(double)((int)(imageCollection.at(LRImages.at(0).numberInImageCollection_) -> getW()) / (double)LRImages.at(0).coefficient_), false);
    //std::deque<RGB24Buffer*> imageCollection2;
    /*for(int k = 0; k < (int)LRImages.size(); k++)
    {
        RGB24Buffer *rotatedImage = rotate(imageCollection.at(LRImages.at(k).numberInImageCollection_), -LRImages.at(k).angleDegree_);
        imageCollection2.push_back(squareBasedResampling(rotatedImage, 1 /LRImages.at(k).coefficient_, - LRImages.at(k).coefficient_ * LRImages.at(k).shiftX_, - LRImages.at(k).coefficient_ * LRImages.at(k).shiftY_, 0));
        delete_safe(rotatedImage);
    }*/
    for(int i = 0; i < result -> getW(); i++)
        for(int j = 0; j < result -> getH(); j++)
        {
            double red = 0;
            double green = 0;
            double blue = 0;

            double summarySquare = 0;

            for(int k = 0; k < (int)LRImages.size(); k++)
            {
                double sizeX = (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> getW();
                double sizeY = (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> getH();


                double x1;
                double y1;
                getNewCoordinates(i,j,LRImages.at(k).coefficient_, LRImages.at(k).shiftX_, LRImages.at(k).shiftY_, LRImages.at(k).angleDegree_ * M_PI/180,
                                  (double)result -> getW()/2, (double)result -> getH()/2,
                                  sizeX/2, sizeY/2, &x1, &y1);

                double x2;
                double y2;
                getNewCoordinates(i+1,j,LRImages.at(k).coefficient_, LRImages.at(k).shiftX_, LRImages.at(k).shiftY_, LRImages.at(k).angleDegree_ * M_PI/180,
                                  (double)result -> getW()/2, (double)result -> getH()/2,
                                  sizeX/2, sizeY/2, &x2, &y2);

                double x3;
                double y3;
                getNewCoordinates(i+1,j+1,LRImages.at(k).coefficient_, LRImages.at(k).shiftX_, LRImages.at(k).shiftY_, LRImages.at(k).angleDegree_ * M_PI/180,
                                  (double)result -> getW()/2, (double)result -> getH()/2,
                                  sizeX/2, sizeY/2, &x3, &y3);

                double x4;
                double y4;
                getNewCoordinates(i,j+1,LRImages.at(k).coefficient_, LRImages.at(k).shiftX_, LRImages.at(k).shiftY_, LRImages.at(k).angleDegree_ * M_PI/180,
                                  (double)result -> getW()/2, (double)result -> getH()/2,
                                  sizeX/2, sizeY/2, &x4, &y4);




                double minX = min(min(x1, x2), min(x3, x4));
                double maxX = max(max(x1, x2), max(x3, x4));

                double minY = min(min(y1, y2), min(y3, y4));
                double maxY = max(max(y1, y2), max(y3, y4));

                for (int x = max( minX, 0.0); (x < sizeX) && (x <= maxX); x++)
                    for (int y = max( minY, 0.0); (y < sizeY) && (y <= maxY); y++)
                    {
                        double square = areaForPixels(x1,y1,x2,y2,x3,y3,x4,y4,x,y);

                        //cout<<i<<" "<<j<<" "<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<x3<<" "<<y3<<" "<<x4<<" "<<y4<<" "<<square<<endl;

                        if (square > 0)
                        {
                            red   = (red   * summarySquare + square * (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> element(y, x).r()) / (summarySquare + square);
                            green = (green * summarySquare + square * (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> element(y, x).g()) / (summarySquare + square);
                            blue  = (blue  * summarySquare + square * (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> element(y, x).b()) / (summarySquare + square);
                            summarySquare += square;
                        }
                    }

            }

            result -> element(j,i) = RGBColor((uint8_t)red,(uint8_t)green,(uint8_t)blue);
        }
    return result;
}

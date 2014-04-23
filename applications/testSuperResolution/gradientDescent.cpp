#include "gradientDescent.h"
#include <cmath>
#include "transformations.h"
#include "resamples.h"
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <random>
#include "polygons.h"
#include <fstream>
using namespace std;


/*double differenceBetweenImages(RGB24Buffer* image1, RGB24Buffer* image2)
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
}*/

double differenceBetweenImages(RGB24Buffer* image1, RGB24Buffer* image2)
{
    double sum = 0;
    for (int i = 0 ; (i < image1 -> getH()) && (i < image2 -> getH()); i++)
       for (int j = 0 ; (j < image1 -> getW()) && (j < image2 -> getW()); j++)
       {
           sum += (
                       (double)((int)(image1 -> element(i,j).r()) - (int)(image2 -> element(i,j).r()))/255
                     * (double)((int)(image1 -> element(i,j).r()) - (int)(image2 -> element(i,j).r()))/255
                     + (double)((int)(image1 -> element(i,j).g()) - (int)(image2 -> element(i,j).g()))/255
                     * (double)((int)(image1 -> element(i,j).g()) - (int)(image2 -> element(i,j).g()))/255
                     + (double)((int)(image1 -> element(i,j).b()) - (int)(image2 -> element(i,j).b()))/255
                     * (double)((int)(image1 -> element(i,j).b()) - (int)(image2 -> element(i,j).b()))/255
                       )/3;
       }
    sum = sum/((image1 -> getH()) * (image1 -> getW()));
    return sum;
}

/*double diffFunc(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages)
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
}*/


/*void getNewCoordinates(double oldX, double oldY, double coefficient, double shiftX, double shiftY, double angle, double newCenterX, double newCenterY, double *newX, double *newY)
{
    double shiftedX = oldX - shiftX;
    double shiftedY = oldY - shiftY;
    double newCoordX = shiftedX * coefficient;
    double newCoordY = shiftedY * coefficient;
    double x = newCoordX - newCenterX;
    double y = newCoordY - newCenterY;
    *newX = newCenterX + x * cos(angle) - y * sin(angle);
    *newY = newCenterY + x * sin(angle) + y * cos(angle);
}*/
void getNewCoordinates(double oldX, double oldY, double coefficient, double shiftX, double shiftY, double angle, double oldCenterX, double oldCenterY, double newCenterX, double newCenterY, double *newX, double *newY)
{
    double newXShifted = (oldX - shiftX - oldCenterX) * coefficient;
    double newYShifted = (oldY - shiftY - oldCenterY) * coefficient;

    double xShifted = newXShifted * cos(angle) - newYShifted * sin(angle);
    double yShifted = newXShifted * sin(angle) + newYShifted * cos(angle);

    *newX = xShifted + newCenterX;
    *newY = yShifted + newCenterY;
}

bool iteration(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages,
               std::deque<RGB192Buffer*> listOfImagesFromUpsampled, std::deque<double> *results,
               double step,
               double minQualityImprovement, RGBmask *mask,
               int rX,
               int rY,
               int rColor,
               int rDir)
{
    bool result = true;
    if ((rColor == 0) && ( ((rDir == 1) && (mask[startImage -> getH() * rX + rY].rUp)) || ((rDir == -1) && (mask[startImage -> getH() * rX + rY].rDown)) ))
    {
        std::deque<double> newValues;
        std::deque<int> minXs;
        std::deque<int> maxXs;
        std::deque<int> minYs;
        std::deque<int> maxYs;
        std::deque<double > shifts;

        for (int i = 0; i < 9 * (int)LRImages.size(); i++)
            shifts.push_back(0);
        for (int k = 0; k < (int)LRImages.size(); k++)
        {
            double coefficient = LRImages.at(k).coefficient_;
            double angle = (LRImages.at(k).angleDegree_)*M_PI/180;
            int number = LRImages.at(k).numberInImageCollection_;
            double shiftX = LRImages.at(k).shiftX_;
            double shiftY = LRImages.at(k).shiftY_;

            double newX1;
            double newY1;
            getNewCoordinates((double)rX, (double)rY, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX1, &newY1);

            double newX2;
            double newY2;
            getNewCoordinates((double)rX+1, (double)rY, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX2, &newY2);

            double newX3;
            double newY3;
            getNewCoordinates((double)rX+1, (double)rY+1, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX3, &newY3);


            double newX4;
            double newY4;
            getNewCoordinates((double)rX, (double)rY+1, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX4, &newY4);

            minXs.push_back(min(min((int)newX1, (int)newX2), min((int)newX3, (int)newX4)));
            maxXs.push_back(max(max((int)newX1, (int)newX2), max((int)newX3, (int)newX4)));

            minYs.push_back(min(min((int)newY1, (int)newY2), min((int)newY3, (int)newY4)));
            maxYs.push_back(max(max((int)newY1, (int)newY2), max((int)newY3, (int)newY4)));

            newValues.push_back((*results).at(k));

            for (int i = minXs.at(k); i <= maxXs.at(k); i++)       //Improve this part
                for (int j = minYs.at(k); j <= maxYs.at(k); j++)
                {
                    if ((i >= 0) && (i < listOfImagesFromUpsampled.at(k) -> getW()) && (j >= 0) && (j < listOfImagesFromUpsampled.at(k) -> getH()))
                    {
                        double square = areaForPixels(newX1,newY1,newX2,newY2,newX3,newY3,newX4,newY4,i,j);

                        double shift = square * (double)rDir * (double)step;

                        shifts.at(9*k + (i - minXs.at(k)) * 3 + (j - minYs.at(k)) ) = shift;

                        double oldELement = (double)listOfImagesFromUpsampled.at(k) -> element(j,i).r() -
                            (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> element(j, i).r();

                        double newELement = oldELement + shift;

                        double coefficient = 255 * 255 * 3 * (double)listOfImagesFromUpsampled.at(k) -> getH() * (double)listOfImagesFromUpsampled.at(k) -> getW();

                        newValues.at(k) +=  (newELement * newELement - oldELement * oldELement)/coefficient;
                    }
                }
        }
        double diff = 0;
        for (int k = 0; k < (int)LRImages.size();k++)
            diff += (*results).at(k) - newValues.at(k);

        if (diff > minQualityImprovement)               //Need to improve this part
        {
            startImage -> element(rY,rX).r() += rDir * step;
            for (int k = 0; k < (int)LRImages.size(); k++)
            {
                (*results).at(k) = newValues.at(k);
                for (int i = minXs.at(k); i <= maxXs.at(k); i++)
                    for (int j = minYs.at(k); j <= maxYs.at(k); j++)
                    {
                        if ((i >= 0) && (i < listOfImagesFromUpsampled.at(k) -> getW()) && (j >= 0) && (j < listOfImagesFromUpsampled.at(k) -> getH()))
                        {
                            double shift = shifts.at(9*k + (i - minXs.at(k)) * 3 + (j - minYs.at(k)) );
                            listOfImagesFromUpsampled.at(k) -> element(j, i).r() += shift;
                        }
                    }
            }
        }else
        {
            result = false;
            if (rDir == 1)
                mask[startImage -> getH() * rX + rY].rUp = false;
            else
                mask[startImage -> getH() * rX + rY].rDown = false;
        }
    }

    if ((rColor == 1) && ( ((rDir == 1) && (mask[startImage -> getH() * rX + rY].gUp)) || ((rDir == -1) && (mask[startImage -> getH() * rX + rY].gDown)) ))
    {
        std::deque<double> newValues;
        std::deque<int> minXs;
        std::deque<int> maxXs;
        std::deque<int> minYs;
        std::deque<int> maxYs;
        std::deque<double > shifts;

        for (int i = 0; i < 9 * (int)LRImages.size(); i++)
            shifts.push_back(0);
        for (int k = 0; k < (int)LRImages.size(); k++)
        {
            double coefficient = LRImages.at(k).coefficient_;
            double angle = (LRImages.at(k).angleDegree_)*M_PI/180;
            int number = LRImages.at(k).numberInImageCollection_;
            double shiftX = LRImages.at(k).shiftX_;
            double shiftY = LRImages.at(k).shiftY_;

            double newX1;
            double newY1;
            getNewCoordinates((double)rX, (double)rY, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX1, &newY1);

            double newX2;
            double newY2;
            getNewCoordinates((double)rX+1, (double)rY, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX2, &newY2);

            double newX3;
            double newY3;
            getNewCoordinates((double)rX+1, (double)rY+1, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX3, &newY3);


            double newX4;
            double newY4;
            getNewCoordinates((double)rX, (double)rY+1, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX4, &newY4);

            minXs.push_back(min(min((int)newX1, (int)newX2), min((int)newX3, (int)newX4)));
            maxXs.push_back(max(max((int)newX1, (int)newX2), max((int)newX3, (int)newX4)));

            minYs.push_back(min(min((int)newY1, (int)newY2), min((int)newY3, (int)newY4)));
            maxYs.push_back(max(max((int)newY1, (int)newY2), max((int)newY3, (int)newY4)));

            newValues.push_back((*results).at(k));

            for (int i = minXs.at(k); i <= maxXs.at(k); i++)       //Improve this part
                for (int j = minYs.at(k); j <= maxYs.at(k); j++)
                {
                    if ((i >= 0) && (i < listOfImagesFromUpsampled.at(k) -> getW()) && (j >= 0) && (j < listOfImagesFromUpsampled.at(k) -> getH()))
                    {
                        double square = areaForPixels(newX1,newY1,newX2,newY2,newX3,newY3,newX4,newY4,i,j);

                        double shift = square * (double)rDir * (double)step;

                        shifts.at(9*k + (i - minXs.at(k)) * 3 + (j - minYs.at(k)) ) = shift;

                        double oldELement = (double)listOfImagesFromUpsampled.at(k) -> element(j,i).g() -
                            (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> element(j, i).g();

                        double newELement = oldELement + shift;

                        double coefficient = 255 * 255 * 3 * (double)listOfImagesFromUpsampled.at(k) -> getH() * (double)listOfImagesFromUpsampled.at(k) -> getW();


                        newValues.at(k) +=  (newELement * newELement - oldELement * oldELement)/coefficient;
                    }
                }
        }
        double diff = 0;
        for (int k = 0; k < (int)LRImages.size();k++)
            diff += (*results).at(k) - newValues.at(k);

        if (diff > minQualityImprovement)               //Need to improve this part
        {
            startImage -> element(rY,rX).g() += rDir * step;
            for (int k = 0; k < (int)LRImages.size(); k++)
            {
                (*results).at(k) = newValues.at(k);
                for (int i = minXs.at(k); i <= maxXs.at(k); i++)
                    for (int j = minYs.at(k); j <= maxYs.at(k); j++)
                    {
                        if ((i >= 0) && (i < listOfImagesFromUpsampled.at(k) -> getW()) && (j >= 0) && (j < listOfImagesFromUpsampled.at(k) -> getH()))
                        {
                            double shift = shifts.at(9*k + (i - minXs.at(k)) * 3 + (j - minYs.at(k)) );
                            listOfImagesFromUpsampled.at(k) -> element(j, i).g() += shift;
                        }
                    }
            }
        }else
        {
            result = false;
            if (rDir == 1)
                mask[startImage -> getH() * rX + rY].gUp = false;
            else
                mask[startImage -> getH() * rX + rY].gDown = false;
        }
    }

    if ((rColor == 2) && ( ((rDir == 1) && (mask[startImage -> getH() * rX + rY].bUp)) || ((rDir == -1) && (mask[startImage -> getH() * rX + rY].bDown)) ))
    {
        std::deque<double> newValues;
        std::deque<int> minXs;
        std::deque<int> maxXs;
        std::deque<int> minYs;
        std::deque<int> maxYs;
        std::deque<double > shifts;

        for (int i = 0; i < 9 * (int)LRImages.size(); i++)
            shifts.push_back(0);
        for (int k = 0; k < (int)LRImages.size(); k++)
        {
            double coefficient = LRImages.at(k).coefficient_;
            double angle = (LRImages.at(k).angleDegree_)*M_PI/180;
            int number = LRImages.at(k).numberInImageCollection_;
            double shiftX = LRImages.at(k).shiftX_;
            double shiftY = LRImages.at(k).shiftY_;

            double newX1;
            double newY1;
            getNewCoordinates((double)rX, (double)rY, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX1, &newY1);

            double newX2;
            double newY2;
            getNewCoordinates((double)rX+1, (double)rY, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX2, &newY2);

            double newX3;
            double newY3;
            getNewCoordinates((double)rX+1, (double)rY+1, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX3, &newY3);


            double newX4;
            double newY4;
            getNewCoordinates((double)rX, (double)rY+1, coefficient, shiftX, shiftY, angle, (double)startImage -> getW()/2, (double)startImage -> getH()/2, (double)imageCollection.at(number) -> getW()/2, (double)imageCollection.at(number) -> getH()/2, &newX4, &newY4);

            minXs.push_back(min(min((int)newX1, (int)newX2), min((int)newX3, (int)newX4)));
            maxXs.push_back(max(max((int)newX1, (int)newX2), max((int)newX3, (int)newX4)));

            minYs.push_back(min(min((int)newY1, (int)newY2), min((int)newY3, (int)newY4)));
            maxYs.push_back(max(max((int)newY1, (int)newY2), max((int)newY3, (int)newY4)));

            newValues.push_back((*results).at(k));

            for (int i = minXs.at(k); i <= maxXs.at(k); i++)       //Improve this part
                for (int j = minYs.at(k); j <= maxYs.at(k); j++)
                {
                    if ((i >= 0) && (i < listOfImagesFromUpsampled.at(k) -> getW()) && (j >= 0) && (j < listOfImagesFromUpsampled.at(k) -> getH()))
                    {
                        double square = areaForPixels(newX1,newY1,newX2,newY2,newX3,newY3,newX4,newY4,i,j);

                        double shift = square * (double)rDir * (double)step;

                        shifts.at(9*k + (i - minXs.at(k)) * 3 + (j - minYs.at(k)) ) = shift;

                        double oldELement = (double)listOfImagesFromUpsampled.at(k) -> element(j,i).b() -
                            (double)imageCollection.at(LRImages.at(k).numberInImageCollection_) -> element(j, i).b();

                        double newELement = oldELement + shift;

                        double coefficient = 255 * 255 * 3 * (double)listOfImagesFromUpsampled.at(k) -> getH() * (double)listOfImagesFromUpsampled.at(k) -> getW();


                        newValues.at(k) +=  (newELement * newELement - oldELement * oldELement)/coefficient;
                    }
                }
        }
        double diff = 0;
        for (int k = 0; k < (int)LRImages.size();k++)
            diff += (*results).at(k) - newValues.at(k);

        if (diff > minQualityImprovement)               //Need to improve this part
        {
            startImage -> element(rY,rX).b() += rDir * step;
            for (int k = 0; k < (int)LRImages.size(); k++)
            {
                (*results).at(k) = newValues.at(k);
                for (int i = minXs.at(k); i <= maxXs.at(k); i++)
                    for (int j = minYs.at(k); j <= maxYs.at(k); j++)
                    {
                        if ((i >= 0) && (i < listOfImagesFromUpsampled.at(k) -> getW()) && (j >= 0) && (j < listOfImagesFromUpsampled.at(k) -> getH()))
                        {
                            double shift = shifts.at(9*k + (i - minXs.at(k)) * 3 + (j - minYs.at(k)) );
                            listOfImagesFromUpsampled.at(k) -> element(j, i).b() += shift;
                        }
                    }
            }
        }else
        {
            result = false;
            if (rDir == 1)
                mask[startImage -> getH() * rX + rY].bUp = false;
            else
                mask[startImage -> getH() * rX + rY].bDown = false;
        }
    }
    return result;
}


void improve(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages,
             std::deque<RGB192Buffer*> listOfImagesFromUpsampled, std::deque<double> *results,
             double step,
             double minQualityImprovement, int numberOfIterations)
{
    RGBmask *mask = new RGBmask[(int)(startImage -> getH()) * (int)(startImage -> getW())];

    for (int i = 0; i < startImage -> getH(); i++)
        for (int j = 0; j < startImage -> getW(); j++)
        {
            mask[i * startImage -> getW() + j].bDown = true;
            mask[i * startImage -> getW() + j].bUp = true;
            mask[i * startImage -> getW() + j].gDown = true;
            mask[i * startImage -> getW() + j].gUp = true;
            mask[i * startImage -> getW() + j].rDown = true;
            mask[i * startImage -> getW() + j].rUp = true;
        }

    std::random_device rd;
    std::mt19937 mt(rd());

    std::uniform_int_distribution<int> genRX(0, startImage -> getW() - 1);
    std::uniform_int_distribution<int> genRY(0, startImage -> getH() - 1);
    std::uniform_int_distribution<int> genRColor(0, 2);
    std::uniform_int_distribution<int> genRDir(0, 1);


    for (int i = 1; i <= numberOfIterations; i++)
    {

        int rX = genRX(mt);
        int rY = genRY(mt);
        int rColor = genRColor(mt);
        int rDir = genRDir(mt);

        if (rDir == 0)
            rDir = -1;

        if (rColor == 0)
        {
            if (((rDir == 1) && ((int)startImage -> element(rY,rX).r() > 255 - step)) || ((rDir == -1) && ((int)startImage -> element(rY,rX).r() < 255)))
                rDir = 0;
        }

        if (rColor == 1)
        {
            if (((rDir == 1) && ((int)startImage -> element(rY,rX).g() > 255 - step)) || ((rDir == -1) && ((int)startImage -> element(rY,rX).g() < 255)))
                rDir = 0;
        }

        if (rColor == 2)
        {
            if (((rDir == 1) && ((int)startImage -> element(rY,rX).b() > 255 - step)) || ((rDir == -1) && ((int)startImage -> element(rY,rX).b() < 255)))
                rDir = 0;
        }

        if (rDir != 0)
            iteration(startImage, imageCollection, LRImages, listOfImagesFromUpsampled, results, step, minQualityImprovement, mask, rX, rY, rColor, rDir);

        if (i % 10000 == 0)
            cout<<i<<endl;
    }

    delete [] mask;
}

/*void improve(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages,
             std::deque<RGB192Buffer*> listOfImagesFromUpsampled, std::deque<double> *results,
             double step,
             double minQualityImprovement, int numberOfIterations)
{
    RGBmask *mask = new RGBmask[(int)(startImage -> getH()) * (int)(startImage -> getW())];

    for (int i = 0; i < startImage -> getH(); i++)
        for (int j = 0; j < startImage -> getW(); j++)
        {
            mask[i * startImage -> getW() + j].bDown = true;
            mask[i * startImage -> getW() + j].bUp = true;
            mask[i * startImage -> getW() + j].gDown = true;
            mask[i * startImage -> getW() + j].gUp = true;
            mask[i * startImage -> getW() + j].rDown = true;
            mask[i * startImage -> getW() + j].rUp = true;
        }
    for (int i = 0; i < 12; i++)
    {
        for (int rX = 0; rX < startImage -> getW(); rX++)
            for (int rY = 0; rY < startImage -> getH(); rY++)
            {
                for (int rColor = 0; rColor < 3; rColor++)
                    iteration(startImage, imageCollection, LRImages, listOfImagesFromUpsampled, results, step, minQualityImprovement, mask, rX, rY, rColor, 1);
            }
        cout<<i<<endl;
    }
    delete [] mask;
}*/

/*void improve(RGB24Buffer* startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages,
             std::deque<RGB192Buffer*> listOfImagesFromUpsampled, std::deque<double> *results,
             double step,
             double minQualityImprovement, int numberOfIterations)
{
    RGBmask *mask = new RGBmask[(int)(startImage -> getH()) * (int)(startImage -> getW())];

    for (int i = 0; i < startImage -> getH(); i++)
        for (int j = 0; j < startImage -> getW(); j++)
        {
            mask[i * startImage -> getW() + j].bDown = true;
            mask[i * startImage -> getW() + j].bUp = true;
            mask[i * startImage -> getW() + j].gDown = true;
            mask[i * startImage -> getW() + j].gUp = true;
            mask[i * startImage -> getW() + j].rDown = true;
            mask[i * startImage -> getW() + j].rUp = true;
        }
    for (int rX = 0; rX < startImage -> getW(); rX++)
        for (int rY = 0; rY < startImage -> getH(); rY++)
        {
            for (int rColor = 0; rColor < 3; rColor++)
            {
                int count = 0;
                while(iteration(startImage, imageCollection, LRImages, listOfImagesFromUpsampled, results, step, minQualityImprovement, mask, rX, rY, rColor, 1) && (numberOfIterations > count))
                    count++;

                if (count)
                    while(iteration(startImage, imageCollection, LRImages, listOfImagesFromUpsampled, results, step, minQualityImprovement, mask, rX, rY, rColor, -1) && (numberOfIterations > count));
            }

            cout<<rX<<" "<<rY<<endl;
        }
    delete [] mask;
}*/

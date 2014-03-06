#include "resamples.h"
#include <cmath>
#include <random>

RGB24Buffer *resampleWithBilinearInterpolation(RGB24Buffer *startImage, double coefficient)
{
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()*coefficient),(int)(startImage -> getW()*coefficient),false);
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0 ; j < result -> getW(); j++)
        {
            int xLeft = floor(((double)i) / coefficient);
            int yLeft = floor(((double)j) / coefficient);
            int xRight = min (xLeft+1,startImage -> getH()-1);
            int yRight = min (yLeft+1,startImage -> getW()-1);
            double k1 = ((double)i)/coefficient - (double)xLeft;
            double k2 = ((double)j)/coefficient - (double)yLeft;

            result -> element(i,j).r() =
                    startImage -> element(xLeft,yLeft).r() * (1 - k1) * (1 - k2) +
                    startImage -> element(xLeft,yRight).r() * (1 - k1) * k2 +
                    startImage -> element(xRight,yLeft).r() * k1 * (1 - k2) +
                    startImage -> element(xRight,yRight).r() * k1 * k2;

            result -> element(i,j).g() =
                    startImage -> element(xLeft,yLeft).g() * (1 - k1) * (1 - k2) +
                    startImage -> element(xLeft,yRight).g() * (1 - k1) * k2 +
                    startImage -> element(xRight,yLeft).g() * k1 * (1 - k2) +
                    startImage -> element(xRight,yRight).g() * k1 * k2;

            result -> element(i,j).b() =
                    startImage -> element(xLeft,yLeft).b() * (1 - k1) * (1 - k2) +
                    startImage -> element(xLeft,yRight).b() * (1 - k1) * k2 +
                    startImage -> element(xRight,yLeft).b() * k1 * (1 - k2) +
                    startImage -> element(xRight,yRight).b() * k1 * k2;
            /*result -> element(i,j) = startImage -> element(xLeft,yLeft) * (1 - k1) * (1 - k2) +
                    startImage -> element(xLeft,yRight) * (1 - k1) * k2 +
                    startImage -> element(xRight,yLeft) * k1 * (1 - k2) +
                    startImage -> element(xRight,yRight) * k1 * k2;*/
        }
    return result;
}



double LancsozFilter(double dist, int a)
{
    if (dist == 0)
        return 1;
    if (abs(dist) >= a)
        return 0;
    return (double)a * sin (M_PI * dist) * sin (M_PI * dist/(double)a) / (M_PI*M_PI*(double)a * (double)a);
}


RGB24Buffer *resampleWithLancsozFilter(RGB24Buffer *startImage, double coefficient, int a)
{
    RGB24Buffer *tempHorImage = new RGB24Buffer((int)(startImage -> getH()),(int)(startImage -> getW()*coefficient),false);
    for (int i = 0 ; i < tempHorImage -> getH(); i++)
        for (int j = 0; j < tempHorImage -> getW(); j++)
        {
            double nearY = floor(j / coefficient);
            uint8_t sumR = 0;
            uint8_t sumG = 0;
            uint8_t sumB = 0;
            double weight = 0;
            for (int y = nearY - a + 1; y < nearY + a; y++) {
                if ((y >= 0) && (y < startImage -> getW()))
                {
                    double lanc = LancsozFilter(nearY - y, a);
                    sumR += startImage -> element(i,y).r() * lanc;
                    sumG += startImage -> element(i,y).g() * lanc;
                    sumB += startImage -> element(i,y).b() * lanc;
                    weight += lanc;
                }
            }
            tempHorImage -> element(i,j).r() = sumR/weight;
            tempHorImage -> element(i,j).g() = sumG/weight;
            tempHorImage -> element(i,j).b() = sumB/weight;
        }

    RGB24Buffer *result = new RGB24Buffer((int)(tempHorImage -> getH()*coefficient),tempHorImage -> getW(),false);
    for (int i = 0 ; i < result -> getH(); i++)
        for (int j = 0; j < result -> getW(); j++)
        {
            double nearX = floor( i / coefficient);
            uint8_t sumR = 0;
            uint8_t sumG = 0;
            uint8_t sumB = 0;
            double weight = 0;
            for (int x = nearX - a + 1; x < nearX + a; x++) {
                if ((x >= 0) && (x < tempHorImage -> getH()))
                {
                    double lanc = LancsozFilter(nearX - x, a);
                    sumR += tempHorImage -> element(x,j).r() * lanc;
                    sumG += tempHorImage -> element(x,j).g() * lanc;
                    sumB += tempHorImage -> element(x,j).b() * lanc;
                    weight += lanc;
                }
            }
            result -> element(i,j).r() = sumR/weight;
            result -> element(i,j).g() = sumG/weight;
            result -> element(i,j).b() = sumB/weight;
        }
    delete_safe(tempHorImage);
    return result;
}


RGB24Buffer *resampleWithNearestNeighbour(RGB24Buffer *startImage, double coefficient) {
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()*coefficient),(int)(startImage -> getW()*coefficient),false);
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0 ; j < result -> getW(); j++)
        {
            int xLeft = floor(((double)i) / coefficient);
            int yLeft = floor(((double)j) / coefficient);
            int xRight = min (xLeft+1,startImage -> getH()-1);
            int yRight = min (yLeft+1,startImage -> getW()-1);
            double k1 = ((double)i)/coefficient - (double)xLeft;
            double k2 = ((double)j)/coefficient - (double)yLeft;

            if (k1 > 0.5)
            {
                if (k2 > 0.5)
                    result -> element(i,j) = startImage -> element(xRight,yRight);
                else
                    result -> element(i,j) = startImage -> element(xRight,yLeft);
            }
            else
            {
                if (k2 > 0.5)
                    result -> element(i,j) = startImage -> element(xLeft,yRight);
                else
                    result -> element(i,j) = startImage -> element(xLeft,yLeft);
            }
        }
    return result;
}



int isInside(double A_x, double A_y, double B_x, double B_y,double C_x, double C_y,double D_x, double D_y, double X, double Y)
{
    int result = 0;
    if ((B_x - A_x)*(X - A_x) + (B_y - A_y)*(Y - A_y) >= 0)
    if ((C_x - B_x)*(X - B_x) + (C_y - B_y)*(Y - B_y) >= 0)
        if ((D_x - C_x)*(X - C_x) + (D_y - C_y)*(Y - C_y) >= 0)
        if ((A_x - D_x)*(X - D_x) + (A_y - D_y)*(Y - D_y) >= 0)
            result = 1;
    return result;
}


double getIntersectionOfSquares(double A_x, double A_y, double B_x, double B_y,double C_x, double C_y,double D_x, double D_y, int startPoint_x, int startPoint_y)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0, 0.1);
    //some magic with new random from C++11


    int sum = 0;
    for (double i = (double) startPoint_x; i + 0.000001 < (double) startPoint_x + 1; i+= 0.1)
    for (double j = (double) startPoint_y; j + 0.000001 < (double) startPoint_y + 1; j+= 0.1)
    {
        double X = i + dist(mt);
        double Y = j + dist(mt);
        sum += isInside(A_x, A_y, B_x, B_y,C_x, C_y,D_x,D_y,X,Y);
    }

    return (double)sum/100;

}




RGB24Buffer *squareBasedResampling(RGB24Buffer *startImage, double coefficient, double shiftX, double shiftY, double angle)
{
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()*coefficient),(int)(startImage -> getW()*coefficient),false);
    double cellSize = 1/coefficient;
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0; j < result -> getW(); j++)
        {
            double summarySquare = 0;
            uint8_t sumR = 0;
            uint8_t sumG = 0;
            uint8_t sumB = 0;
            double A_x = shiftX + i * cellSize * cos (angle/180 * MATH_PI) + j * cellSize * sin (angle/180 * MATH_PI);
            double A_y = shiftY + j * cellSize * cos (angle/180 * MATH_PI) + i * cellSize * sin (angle/180 * MATH_PI);
            double B_x = shiftX + (i+1) * cellSize * cos (angle/180 * MATH_PI) + j * cellSize * sin (angle/180 * MATH_PI);
            double B_y = shiftY + j * cellSize * cos (angle/180 * MATH_PI) + (i+1) * cellSize * sin (angle/180 * MATH_PI);
            double C_x = shiftX + (i+1) * cellSize * cos (angle/180 * MATH_PI) + (j+1) * cellSize * sin (angle/180 * MATH_PI);
            double C_y = shiftY + (j+1) * cellSize * cos (angle/180 * MATH_PI) + (i+1) * cellSize * sin (angle/180 * MATH_PI);
            double D_x = shiftX + i * cellSize * cos (angle/180 * MATH_PI) + (j+1) * cellSize * sin (angle/180 * MATH_PI);
            double D_y = shiftY + (j+1) * cellSize * cos (angle/180 * MATH_PI) + i * cellSize * sin (angle/180 * MATH_PI);

            int min_X = floor(min(min(min(A_x,B_x),min(C_x,D_x)),0));
            int min_Y = floor(min(min(min(A_y,B_y),min(C_y,D_y)),0));
            int max_X = floor(max(max(max(A_x,B_x),max(C_x,D_x)),startImage -> getH()-1));
            int max_Y = floor(max(max(max(A_y,B_y),max(C_y,D_y)),startImage -> getW()-1));


            for (int i = min_X; i <= max_X; i++)
                for (int j = min_Y; j <= max_Y; j++)
                {
                    double square = getIntersectionOfSquares(A_x,A_y,B_x,B_y,C_x,C_y,D_x,D_y,i,j);
                    sumR = (sumR * summarySquare + square * startImage -> element(i,j).r()) / (summarySquare + square);
                    sumG = (sumG * summarySquare + square * startImage -> element(i,j).g()) / (summarySquare + square);
                    sumB = (sumB * summarySquare + square * startImage -> element(i,j).b()) / (summarySquare + square);
                    summarySquare += square;
                }
            /*for (int iX = (int)startPointX; iX < ceil(endPointX); iX++)
                for (int iY = (int)startPointY; iY < ceil(endPointY); iY++)
                    if ((iX < startImage -> getH()) && (iY < startImage -> getW()) && (iX >= 0) && (iY >= 0)) {
                        double firstX = max(startPointX,(double)iX);
                        double lastX = min(endPointX,(double)iX+1);
                        double firstY = max(startPointY, (double)iY);
                        double lastY = min(endPointY,(double)iY+1);
                        if ((firstX <= lastX) && (firstY <= lastY)) {
                                double X = lastX - firstX;
                                double Y = lastY - firstY;
                                summarySquare += X*Y;
                        }
                    }
            for (int iX = (int)startPointX; iX < ceil(endPointX); iX++)
                for (int iY = (int)startPointY; iY < ceil(endPointY); iY++)
                    if ((iX < startImage -> getH()) && (iY < startImage -> getW()) && (iX >= 0) && (iY >= 0)) {
                        double firstX = max(startPointX,(double)iX);
                        double lastX = min(endPointX,(double)iX+1);
                        double firstY = max(startPointY, (double)iY);
                        double lastY = min(endPointY,(double)iY+1);
                        if ((firstX <= lastX) && (firstY <= lastY)) {
                                double X = lastX - firstX;
                                double Y = lastY - firstY;
                                sumR += (X*Y/summarySquare)*startImage -> element(iX,iY).r();
                                sumG += (X*Y/summarySquare)*startImage -> element(iX,iY).g();
                                sumB += (X*Y/summarySquare)*startImage -> element(iX,iY).b();
                        }
                    }*/
            /*sumR /= summarySquare;
            sumG /= summarySquare;
            sumB /= summarySquare;*/
            result -> element(i,j) = RGBColor(sumR,sumG,sumB);
            /*result -> element(i,j).r() = sumR;
            result -> element(i,j).g() = sumG;
            result -> element(i,j).b() = sumB;*/
        }
    return result;
}

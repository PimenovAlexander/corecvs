#include "convolution.h"
RGB24Buffer *multiplyByMatrix3x3(RGB24Buffer *startImage,   double a1, double a2, double a3,
                                                            double a4, double a5, double a6,
                                                            double a7, double a8, double a9,
                                                            double div)
{
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()),(int)(startImage -> getW()), false);
    for (int i = 1; i < result -> getH() - 1; i++)
        for (int j = 1 ; j < result -> getW() - 1; j++) {

            result -> element(i,j).r() =
                   (startImage -> element(i, j).r() * a5 +
                    startImage -> element(i - 1, j).r() * a2 +
                    startImage -> element(i, j - 1).r() * a4 +
                    startImage -> element(i + 1, j).r() * a8 +
                    startImage -> element(i, j + 1).r() * a6 +
                    startImage -> element(i - 1, j - 1).r() * a1 +
                    startImage -> element(i - 1, j + 1).r() * a3 +
                    startImage -> element(i + 1, j + 1).r() * a9 +
                    startImage -> element(i + 1, j - 1).r() * a7) / div;

            result -> element(i,j).g() =
                   (startImage -> element(i, j).g() * a5 +
                    startImage -> element(i - 1, j).g() * a2 +
                    startImage -> element(i, j - 1).g() * a4 +
                    startImage -> element(i + 1, j).g() * a8 +
                    startImage -> element(i, j + 1).g() * a6 +
                    startImage -> element(i - 1, j - 1).g() * a1 +
                    startImage -> element(i - 1, j + 1).g() * a3 +
                    startImage -> element(i + 1, j + 1).g() * a9 +
                    startImage -> element(i + 1, j - 1).g() * a7) / div;

            result -> element(i,j).b() =
                   (startImage -> element(i, j).b() * a5 +
                    startImage -> element(i - 1, j).b() * a2 +
                    startImage -> element(i, j - 1).b() * a4 +
                    startImage -> element(i + 1, j).b() * a8 +
                    startImage -> element(i, j + 1).b() * a6 +
                    startImage -> element(i - 1, j - 1).b() * a1 +
                    startImage -> element(i - 1, j + 1).b() * a3 +
                    startImage -> element(i + 1, j + 1).b() * a9 +
                    startImage -> element(i + 1, j - 1).b() * a7) / div;
        }
    return result;
}

RGB24Buffer *convolution(RGB24Buffer *startImage)
{
    /**
     *  Matrix for multyplying:
     *  (a1,a2,a3)
     *  (a4,a5,a6)
     *  (a7,a8,a9)
     **/

    double a1 = 0.5;
    double a2 = 0.75;
    double a3 = 0.5;
    double a4 = 0.75;
    double a5 = 1;
    double a6 = 0.75;
    double a7 = 0.5;
    double a8 = 0.75;
    double a9 = 0.5;

    double div = 6;
    RGB24Buffer *result = multiplyByMatrix3x3(startImage,   a1, a2, a3,
                                                            a4, a5, a6,
                                                            a7, a8, a9,
                                                            div);
    return result;
}

RGB24Buffer *sharpening(RGB24Buffer *startImage)
{
    double a1 = -0.1;
    double a2 = -0.1;
    double a3 = -0.1;
    double a4 = -0.1;
    double a5 = 1.8;
    double a6 = -0.1;
    double a7 = -0.1;
    double a8 = -0.1;
    double a9 = -0.1;

    double div = 1;
    RGB24Buffer *result = multiplyByMatrix3x3(startImage,   a1, a2, a3,
                                                            a4, a5, a6,
                                                            a7, a8, a9,
                                                            div);
    return result;
}

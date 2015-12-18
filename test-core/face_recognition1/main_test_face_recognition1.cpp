/**
 * \file main_test_face_recogition.cpp
 * \brief Add Comment Here
 *
 * \date Jun 28, 2010
 * \author alexander
 */

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"

#include "global.h"
#include "vjPattern.h"
#include "g12Buffer.h"
#include "integralBuffer.h"
#include "mipmapPyramid.h"
#include "bmpLoader.h"
#include "bufferFactory.h"

using namespace std;
using namespace corecvs;


void processPoint(VJAdaBoostedClassifier * /*classifier*/, G12IntegralBuffer * /*integral*/, int /*x*/, int /*y*/)
{


}

//int main (int /*argC*/, char *argV[])
TEST(FaceRecognition1, main)
{
    const double UP_FACTOR = 5;
    const double UP_STEP   = 1.2;

//    const double DOWN_FACTOR = 0.25;
//    const double DOWN_STEP = 1.0 / 1.2;


    fstream patternFile;
    patternFile.open("boost-result.txt", fstream::in);
    VJAdaBoostedClassifier *classifier = new VJAdaBoostedClassifier();
    patternFile >> (*classifier);

    for (unsigned i = 0; i < classifier->children.size(); i++)
    {
        char name[100];
        snprintf2buf(name, "pattern%d.bmp", i);

        VJSimpleClassifier* partClass = classifier->children.at(i);
        RGB24Buffer *buffer = partClass->drawPattern();
        (BMPLoader()).save(string(name), buffer);
        delete buffer;
    }
//    classifier->print();
    patternFile.close();
    G12Buffer *inputPicture = BufferFactory::getInstance()->loadG12Bitmap(argV[1]);
    if (inputPicture == NULL)
    {
        printf("Problem loading input %s\n", argV[1]);
    }
    RGB24Buffer *output = new RGB24Buffer(inputPicture);

    G12IntegralBuffer *integral = new G12IntegralBuffer(inputPicture);

    int total = 0;
    int positive = 0;

    double factor;

    for (factor = 1.0; factor < UP_FACTOR; factor *= UP_STEP)
    {
        VJAdaBoostedClassifier *scaled = classifier->scale(factor);
        scaled->initLimits();
        double x = scaled->leftMargin;
        double y = scaled->topMargin;

        double patw = scaled->rightMargin + scaled->leftMargin;
        double path = scaled->topMargin + scaled->bottomMargin;

        int hlimit = inputPicture->h - scaled->bottomMargin;
        int wlimit = inputPicture->w - scaled->rightMargin;

        for (; y < hlimit ; y += factor)
        {
            for (; x < wlimit ; x += factor)
            {
                bool hasMatch = scaled->applyToPoint(integral, y, x);
                if (hasMatch)
                {
                    //printf("Match Found! at %d %d level %d\n",j ,i, l);
                    int h = path;
                    int w = patw;
                    output->drawCrosshare1(x,     y      , RGBColor(0xFF0000));
                    output->drawCrosshare1(x + w, y      , RGBColor(0x7F0000));
                    output->drawCrosshare1(x,     y + h  , RGBColor(0x00FF00));
                    output->drawCrosshare1(x + w, y + h  , RGBColor(0x007F00));
                    positive++;
                }
                total++;
            }
        }
        delete scaled;
    }


    delete integral;

    printf("Found %d faces among %d objects %2.2lf%%\n", positive, total, (double) 100.0 * positive / total );
    (BMPLoader()).save("output.bmp", output);
//    return 0;
}


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

#include "core/utils/global.h"
#include "core/boosting/vjPattern.h"
#include "core/buffers/g12Buffer.h"
#include "core/fileformats/ppmLoader.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/integralBuffer.h"
#include "core/buffers/mipmapPyramid.h"
#include "core/buffers/bufferFactory.h"

using namespace std;
using namespace corecvs;

//int main (int /*argC*/, char *argV[])
TEST(FaceRecognition, main)
{
    const double FACTOR = 1.25;

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

    string basename = string(argV[1]).substr(string(argV[1]).rfind("/") + 1);
    RGB24Buffer *output = new RGB24Buffer(inputPicture);

    AbstractMipmapPyramid<G12Buffer> *pyramid = new AbstractMipmapPyramid<G12Buffer>(inputPicture, 16, FACTOR);

    int total = 0;
    int positive = 0;
    double factor = 1.0;
    for (unsigned l = 0; l < pyramid->levels.size(); l++)
    {
        G12Buffer *data = pyramid->levels.at(l);
        G12IntegralBuffer *integral = new G12IntegralBuffer(data);


        for (int i = 0; i < data->h - 112; i+=3 )
        {
            for (int j = 0; j < data->w - 92; j+=3)
            {
                bool hasMatch = classifier->applyToPoint(integral, i, j);
                if (hasMatch)
                {
                    //printf("Match Found! at %d %d level %d\n",j ,i, l);
                    int x = j * factor;
                    int y = i * factor;
                    int h = 112 * factor;
                    int w = 92 * factor;
                    output->drawCrosshare1(x,     y      , RGBColor(0xFF0000));
                    output->drawCrosshare1(x + w, y      , RGBColor(0x7F0000));
                    output->drawCrosshare1(x,     y + h  , RGBColor(0x00FF00));
                    output->drawCrosshare1(x + w, y + h  , RGBColor(0x007F00));

                    G12Buffer *subface = new G12Buffer(inputPicture, x, y, x + w, y + h);
                //    CRGB24Buffer *imgToSave = new CRGB24Buffer(subface);
                    stringstream facename;

                    facename << "faces/" << basename << "_" << positive << ".bmp";
                    cout << "Saving face:" << facename.str() << "\n";
                    //(BMPLoader()).save(facename.str(), imgToSave);
                    (PPMLoader()).save(facename.str(), subface);
                //    delete imgToSave;

                    positive++;
                }
                total++;
            }
        }

        RGB24Buffer *pyrToSave = new RGB24Buffer(data);
        stringstream name;
        name << "pyr" << l << ".bmp";
        (BMPLoader()).save(name.str(), pyrToSave);
        delete pyrToSave;

        delete integral;
        factor *= FACTOR;
    }

    printf("Found %d faces among %d objects %2.2lf%%\n", positive, total, (double) 100.0 * positive / total );
    (BMPLoader()).save("output.bmp", output);    
    //    return 0;
}


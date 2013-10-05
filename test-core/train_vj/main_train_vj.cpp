#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "utils.h"
#include "g12Buffer.h"
#include "integralBuffer.h"
#include "mipmapPyramid.h"
#include "bufferFactory.h"
#include "vjPattern.h"
#include "adaBoost.h"
#include "bmpLoader.h"

using namespace std;
using namespace corecvs;


void calculateMeans (AdaBoostInput<VJInputImage> &inputs)
{
    unsigned h = inputs[0].x.buffer->h;
    unsigned w = inputs[0].x.buffer->w;

    AbstractContiniousBuffer<uint64_t> *posMean = new AbstractContiniousBuffer<uint64_t>(h,w);

    AbstractContiniousBuffer<uint64_t> *negMean = new AbstractContiniousBuffer<uint64_t>(h,w);

    int pos = 0 , neg = 0;

    for (unsigned k = 0; k < inputs.size(); k++)
    {
        G12Buffer *image = inputs[k].x.buffer;
        if (inputs[k].y)
        {
            pos++;
            for (unsigned i = 0; i < h; i++)
                for (unsigned j = 0; j < w; j++)
                    posMean->element(i,j) += image->element(i,j);
        } else {
            neg++;
            for (unsigned i = 0; i < h; i++)
                for (unsigned j = 0; j < w; j++)
                    negMean->element(i,j) += image->element(i,j);
        }
    }

    RGB24Buffer *meanRGB = new RGB24Buffer(h,w);
    for (unsigned i = 0; i < h; i++)
        for (unsigned j = 0; j < w; j++)
        {
            uint32_t gray = (uint8_t)((posMean->element(i,j) / pos) >> 4);
            meanRGB->element(i,j) = RGBColor(gray,gray,gray);
        }
    (BMPLoader()).save("mean_pos.bmp", meanRGB);
    for (unsigned i = 0; i < h; i++)
        for (unsigned j = 0; j < w; j++)
        {
            uint32_t gray = (uint8_t)((negMean->element(i,j) / neg) >> 4);
            meanRGB->element(i,j) = RGBColor(gray,gray,gray);
        }
    (BMPLoader()).save("mean_neg.bmp", meanRGB);

    delete meanRGB;
    delete posMean;
    delete negMean;
}


int main (int argC, char *argV[])
{
    unsigned PATTERNS_NUMBER = 1000;
    unsigned CLASSIFIER_SIZE_LIMIT = 41;

    unsigned POSITIVE_SAMPLES_LIMIT = 300000;
    unsigned NEGATIVE_SAMPLES_LIMIT = 300000;

    setSegVHandler();
    setStdTerminateHandler();


    AdaBoostInput<VJInputImage> inputs;



    if (argC < 2)
    {
        printf("We need at least two arguments \n");
        exit(1);
    }

    if (argC > 3)
        sscanf(argV[3],"%u", &PATTERNS_NUMBER);

    if (argC > 4)
        sscanf(argV[4],"%u", &CLASSIFIER_SIZE_LIMIT);

    if (argC > 5)
    {
        sscanf(argV[5],"%u", &POSITIVE_SAMPLES_LIMIT);
        NEGATIVE_SAMPLES_LIMIT = POSITIVE_SAMPLES_LIMIT;
    }

    char name[256];
    unsigned long count = 0;

    while (count < POSITIVE_SAMPLES_LIMIT)
    {
        snprintf2buf(name, argV[1], count);
        G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap(name);
        if (image == NULL)
            break;
        inputs.push_back(AdaBoostInputData<VJInputImage>(VJInputImage(image, NULL), true));
        count++;
        if (count % 100 == 0)
            printf("Loaded %lu positive samples...\n", count);
    }
    count = 0;

    while (count < NEGATIVE_SAMPLES_LIMIT)
    {
        snprintf2buf(name, argV[1], count);
        G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap(name);
        if (image == NULL)
            break;

        inputs.push_back(AdaBoostInputData<VJInputImage>(VJInputImage(image, NULL), false));
        count++;
        if (count % 100 == 0)
            printf("Loaded %lu negative samples...\n", count);
    }

    fflush(stdout);
    printf("Checking sizes\n");
    AdaBoostInput<VJInputImage>::iterator it;

    printf("Will search for boosted classifier of %d size. For each will iterate though %d patterns\n",
            CLASSIFIER_SIZE_LIMIT, PATTERNS_NUMBER);


    /*Check the size */
    int h = inputs.at(0).x.buffer->h;
    int w = inputs.at(0).x.buffer->w;
    for (unsigned k = 0; k < inputs.size(); k++)
    {
        VJInputImage *input = &(inputs[k].x);
        G12Buffer *image = input->buffer;
        if (image->h != h || image->w != w)
        {
            printf("problem with image size\n");
            exit(1);
        }
        G12IntegralBuffer *integral = new G12IntegralBuffer(image);
        input->integral = integral;
    }

#define CALCULATE_MEANS
#ifdef CALCULATE_MEANS
    calculateMeans(inputs);
#endif

    for (unsigned k = 0; k < inputs.size(); k++)
    {
        VJInputImage *input = &(inputs[k].x);
        delete_safe(input->buffer);
    }

    printf("Totally loaded and created integrals %d samples of size %dx%d\n", (int)inputs.size(), w, h);

    // Init weights. All equal so far
    inputs.initWeigths(1.0, 1.0);

    fstream weights;
    weights.open("weights-current.txt", fstream::in);
    if (weights.is_open())
    {
        printf("Weights file found using it!\n");
        for (unsigned k = 0; k < inputs.size(); k++)
        {
            weights >> inputs[k].weight;
        }
        weights.close();
    } else {
        printf("No weights file. Using equal weights\n");
    }

    fstream patternFile;
    patternFile.open("current-boost-result.txt", fstream::in);
    VJAdaBoostedClassifier *classifier = new VJAdaBoostedClassifier();
    if (patternFile.is_open())
    {
        patternFile >> (*classifier);
        printf("Loaded classifier from the file\n");
        patternFile.close();
    } else {
        printf("No file. Staring with empty classifier\n");
    }

    VJSimpleClassifierGenerator *generator = new VJSimpleClassifierGenerator(h,w, PATTERNS_NUMBER);


    double classifierCost;

    VJSimpleClassifier *simpleClassifier =
        generator->getBestClassifier(inputs, &classifierCost);

    double alpha = 0.5 * log ((1.0 - classifierCost) / classifierCost);

    /*Regenerate the results*/
    ClassifierStatistics stats;

    for (it = inputs.begin(); it != inputs.end(); it++)
    {
        bool prediction = simpleClassifier->classify((*it).x);
        double alphaExp = exp(-alpha * (prediction ? 1 : -1) * ((*it).y ? 1 : -1));
        (*it).weight *= alphaExp;
        stats.addValue(prediction, (*it).y);
    }
    inputs.normalizeWeigths();


    printf("Result so far TP=%d TN=%d FP=%d FN=%d\n", stats.truePositive, stats.trueNegative, stats.falsePositive, stats.falseNegative);
    classifier->addClassifier(simpleClassifier, alpha);

    /*Test for total result*/
    ClassifierStatistics totalStats;


    fstream outbreaks;
    outbreaks.open("outbreaks", fstream::out);
    for (unsigned i = 0; i < inputs.size(); i++)
    {
        bool isPositive = inputs[i].y;
        bool theClass = classifier->classify(inputs[i].x);
        totalStats.addValue(theClass, isPositive);

        if (isPositive != theClass)
        {
            if (isPositive)
                outbreaks << "FN " << i << "\n";
            else
                outbreaks << "FP " << i << "\n";
        }
    }
    outbreaks.close();
    printf("Boosted Result %d classifiers so far TP=%d TN=%d FP=%d FN=%d\n",
            (int)classifier->children.size(),
            totalStats.truePositive, totalStats.trueNegative,
            totalStats.falsePositive, totalStats.falseNegative);

    fstream resultStream;
    stringstream boostname;
    boostname << "current-boost-result." << classifier->getSize() << ".txt";
    resultStream.open(boostname.str().c_str(), fstream::out);
    resultStream << (*classifier);
    resultStream.close();
    resultStream.open("current-boost-result.txt", fstream::out);
    resultStream << (*classifier);
    resultStream.close();


    stringstream weightsName;
    weightsName << "weights-current." << classifier->getSize() << ".txt";

    /*Saving weights*/
    weights.open(weightsName.str().c_str(), fstream::out);
    for (unsigned k = 0; k < inputs.size(); k++)
    {
        weights << inputs[k].weight << "\n";
    }
    weights.close();

    weights.open("weights-current.txt", fstream::out);
    for (unsigned k = 0; k < inputs.size(); k++)
    {
        weights << inputs[k].weight << "\n";
    }
    weights.close();



    printf("Result saved...\n");
    for (it = inputs.begin(); it != inputs.end(); it++)
    {
        VJInputImage *input = &((*it).x);
        G12Buffer *image = input->buffer;
        G12IntegralBuffer *integral = input->integral;
        if (image)
            delete image;
        if (integral)
            delete integral;
    }

    delete classifier;
    delete generator;
    printf("Cleanup finish...\n");

}

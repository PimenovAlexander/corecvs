#ifndef OPENCVDEFAULTPARAMS_H
#define OPENCVDEFAULTPARAMS_H

struct SurfParams
{
    double hessianThreshold;
    int octaves;
    int octaveLayers;
    bool extended;
    bool upright;


    SurfParams(
            double hessianThreshold = 300.0,
            int octaves = 4,
            int octaveLayers = 2,
            bool extended = true,
            bool upright = false)
        : hessianThreshold(hessianThreshold),
        octaves(octaves),
        octaveLayers(octaveLayers),
        extended(extended),
        upright(upright)
    {
    }
};

struct SiftParams
{
    double contrastThreshold;
    double edgeThreshold;
    double sigma;
    int nOctaveLayers;

    SiftParams(
            double contrastThreshold = 0.01,
            double edgeThreshold = 16.0,
            double sigma = 1.6,
            int nOctaveLayers = 4) :
        contrastThreshold(contrastThreshold),
        edgeThreshold(edgeThreshold),
        sigma(sigma),
        nOctaveLayers(nOctaveLayers)
    {
    }
};

struct StarParams
{
    int maxSize;
    int responseThreshold;
    int lineThresholdProjected;
    int lineThresholdBinarized;
    int supressNonmaxSize;

    StarParams(
            int maxSize = 16,
            int responseThreshold = 30,
            int lineThresholdProjected = 10,
            int lineThresholdBinarized = 8,
            int supressNonmaxSize = 5) :
        maxSize(maxSize),
        responseThreshold(responseThreshold),
        lineThresholdProjected(lineThresholdProjected),
        lineThresholdBinarized(lineThresholdBinarized),
        supressNonmaxSize(supressNonmaxSize)
    {
    }
};

struct FastParams
{
    int threshold;
    bool nonmaxSuppression;
    int type;

    FastParams(
            int threshold = 1,
            bool nonmaxSuppression = true,
            int type = 2) ://cv::FastFeatureDetector::TYPE_9_16) :
        threshold(threshold), nonmaxSuppression(nonmaxSuppression), type(type)
    {
    }

};

struct OrbParams
{
    double scaleFactor;
    int nLevels;
    int edgeThreshold;
    int firstLevel;
    int WTA_K;
    int scoreType;
    int patchSize;

    OrbParams(double scaleFactor = 1.2, int nLevels = 8, int edgeThreshold = 31,
            int firstLevel = 0, int WTA_K = 2, int scoreType = 0, //cv::ORB::HARRIS_SCORE,
            int patchSize = 31):
        scaleFactor(scaleFactor), nLevels(nLevels), edgeThreshold(edgeThreshold), firstLevel(firstLevel),
        WTA_K(WTA_K), scoreType(scoreType), patchSize(patchSize)
    {
    }
};

struct BriskParams
{
    int thresh;
    int octaves;
    double patternScale;

    BriskParams(int thresh = 30, int octaves = 3, double patternScale = 1.0) :
        thresh(thresh), octaves(octaves), patternScale(patternScale)
    {
    }
};


#endif

#ifndef OPENCVDEFAULTPARAMS_H
#define OPENCVDEFAULTPARAMS_H

#include <string>
#include <map>

namespace
{
    std::map<std::string, double> parse(const std::string &params)
    {
        std::string curr;
        std::vector<std::string> fields;
        size_t i = 0;
        for (; i < params.size(); ++i)
        {
            if (curr.size() == 0 && params[i] == ' ')
                continue;
            if ( params[ i ] == ' ' && !curr.size() )
            {
                fields.push_back(curr);
                curr.resize(0);
            }
            if ( params[ i ] == ';' )
            {
                fields.push_back( curr );
                curr.resize( 0 );
                i++;
            }

            curr = curr + params[i];
        }
        if (curr.size())
            fields.push_back(curr);

        std::map<std::string, double> mapped;
        for (auto& f: fields)
        {
            auto pos = f.find("=");
            if (pos == std::string::npos)
                continue;

            auto key = f.substr(0, pos);
            f.erase(0, pos + 1);
            auto value = f;
            mapped[key] = std::stod(value);
        }
        return mapped;
    }
}

#define PARSED(n, o) \
    if (mp.count(#n)) \
    { \
        this->n = o(mp[#n]); \
    }

struct SurfParams
{
    double hessianThreshold;
    int octaves;
    int octaveLayers;
    bool extended;
    bool upright;


    SurfParams(
            const std::string &params = "",
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
        auto mp = ::parse(params);
        PARSED(hessianThreshold,)
        PARSED(octaves,)
        PARSED(octaveLayers,)
        PARSED(extended, 0.0 !=)
        PARSED(upright, 0.0 !=)
    }
};

struct SiftParams
{
    double contrastThreshold;
    double edgeThreshold;
    double sigma;
    int nOctaveLayers;

    SiftParams(
            const std::string &params = "",
            double contrastThreshold = 0.08,
            double edgeThreshold = 16.0,
            double sigma = 1.6,
            int nOctaveLayers = 3) :
        contrastThreshold(contrastThreshold),
        edgeThreshold(edgeThreshold),
        sigma(sigma),
        nOctaveLayers(nOctaveLayers)
    {
        auto mp = ::parse(params);
        PARSED(contrastThreshold,)
        PARSED(edgeThreshold,)
        PARSED(sigma,)
        PARSED(nOctaveLayers,)
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
            const std::string &params = "",
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
        auto mp = ::parse(params);
        PARSED(maxSize,)
        PARSED(responseThreshold,)
        PARSED(lineThresholdProjected,)
        PARSED(lineThresholdBinarized,)
        PARSED(supressNonmaxSize,)
    }
};

struct FastParams
{
    int threshold;
    bool nonmaxSuppression;
    int type;

    FastParams(
            const std::string& params = "",
            int threshold = 1,
            bool nonmaxSuppression = true,
            int type = 2) ://cv::FastFeatureDetector::TYPE_9_16) :
        threshold(threshold), nonmaxSuppression(nonmaxSuppression), type(type)
    {
        auto mp = ::parse(params);
        PARSED(threshold,)
           PARSED(nonmaxSuppression, 0.0 !=)
           PARSED(type,)
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
    int maxFeatures;

    OrbParams(const std::string& params = "", double scaleFactor = 1.2, int nLevels =16, int edgeThreshold = 31,
            int firstLevel = 0, int WTA_K = 2, int scoreType = 0, //cv::ORB::HARRIS_SCORE,
            int patchSize = 31, int maxFeatures = 2000):
        scaleFactor(scaleFactor), nLevels(nLevels), edgeThreshold(edgeThreshold), firstLevel(firstLevel),
        WTA_K(WTA_K), scoreType(scoreType), patchSize(patchSize), maxFeatures(maxFeatures)
    {
        auto mp = ::parse(params);
        PARSED(scaleFactor,)
        PARSED(nLevels,)
        PARSED(edgeThreshold,)
        PARSED(firstLevel,)
        PARSED(WTA_K,)
        PARSED(scoreType,)
        PARSED(patchSize,)
        PARSED(maxFeatures,)
    }

};

struct BriskParams
{
    int thresh;
    int octaves;
    double patternScale;

    BriskParams(const std::string& params = "", int thresh = 30, int octaves = 3, double patternScale = 1.0) :
        thresh(thresh), octaves(octaves), patternScale(patternScale)
    {
        auto mp = ::parse(params);
        PARSED(thresh,)
        PARSED(octaves,)
        PARSED(patternScale,)
    }
};

struct AkazeParams
{
    int descriptorType;
    int descriptorSize;
    int descriptorChannels;
    float threshold;
    int octaves;
    int octaveLayers;
    int diffusivity;

    AkazeParams(const std::string& params = "", int descriptorType = 5 /* MLDB */,
                int descriptorSize = 0, int descriptorChannels = 3, float threshold = 0.001f,
                int octaves = 4, int octaveLayers = 4, int diffusivity = 1 /* G2 */) :
        descriptorType(descriptorType), descriptorSize(descriptorSize), descriptorChannels(descriptorChannels),
        threshold(threshold), octaves(octaves), octaveLayers(octaveLayers), diffusivity(diffusivity)
    {
        auto mp = ::parse(params);
        PARSED(descriptorType,)
        PARSED(descriptorSize,)
        PARSED(descriptorChannels,)
        PARSED(threshold,)
        PARSED(octaves,)
        PARSED(octaveLayers,)
        PARSED(diffusivity,)
    }
};


#endif // OPENCVDEFAULTPARAMS_H

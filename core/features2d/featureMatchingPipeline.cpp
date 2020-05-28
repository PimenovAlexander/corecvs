#include "features2d/featureMatchingPipeline.h"
#include "features2d/featureDetectorProvider.h"
#include "features2d/descriptorExtractorProvider.h"
#include "features2d/descriptorMatcherProvider.h"
#include "features2d/detectExtractAndMatchProvider.h"
#include "features2d/detectAndExtractProvider.h"
#include "features2d/bufferReaderProvider.h"
#include "features2d/vsfmIo.h"
#include "buffers/bufferFactory.h"
#include "utils/utils.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>  // clock???  Please use PreciseTimer!!!


static const char* KEYPOINT_EXTENSION   = "keypoints";
static const char* DESCRIPTOR_EXTENSION = "descriptors";
static const char* SIFT_EXTENSION       = "sift";

FeatureMatchingPipeline::FeatureMatchingPipeline(const std::vector<std::string> &filenames, const std::vector<void*> &remapCaches, corecvs::StatusTracker* processState)
{
    this->processState = processState;
    images.reserve(filenames.size());
	bool useCache = remapCaches.size() == filenames.size();
    for (size_t i = 0; i < filenames.size(); ++i)
    {
		images.push_back(Image(i, filenames[i], useCache ? remapCaches[i] : 0));
    }
}

FeatureMatchingPipeline::~FeatureMatchingPipeline()
{
    for (std::vector<FeatureMatchingPipelineStage*>::iterator ps = pipeline.begin(); ps != pipeline.end(); ++ps)
    {
        delete *ps;
    }
    pipeline.clear();
}

void FeatureMatchingPipeline::run()
{
	corecvs::StatusTracker::Reset(processState, "Detecting", pipeline.size());

    for (size_t id = 0; id < pipeline.size(); ++id)
    {
		auto boo = corecvs::StatusTracker::CreateAutoTrackerCalculationObject(processState);
        auto sParams = saveParams[id];
        auto lParams = loadParams[id];
        auto ps = pipeline[id];

        if (lParams.first)
        {
            ps->loadResults(this, lParams.second);
        }
        if (runEnable[id])
        {
            ps->run(this);
        }
        if (sParams.first)
        {
            ps->saveResults(this, sParams.second);
        }
    }
}

void FeatureMatchingPipeline::add(FeatureMatchingPipelineStage *ps, bool run, bool saveData, const std::string &saveName, bool loadData, const std::string& loadName)
{
    this->saveParams.push_back(std::make_pair(saveData, saveName));
    this->loadParams.push_back(std::make_pair(loadData, loadName));
    runEnable.push_back(run);
    pipeline.push_back(ps);
}

void FeatureMatchingPipeline::printCaps()
{
    cout << "Current FeatureMatchingPipeline::caps are:" << std::endl;
    FeatureDetectorProvider::getInstance().print("FeatureDetectorProvider");
    DescriptorExtractorProvider::getInstance().print("DescriptorExtractorProvider");
    DescriptorMatcherProvider::getInstance().print("DescriptorMatcherProvider");
}

//---------------------------------------------------------------------------

class ParallelDetector
{
    FeatureMatchingPipeline* pipeline;
    DetectorType detectorType;
    std::string params;
    int maxFeatureCount;
    int downsampleFactor;

public:
    void operator() (const corecvs::BlockedRange<size_t>& r) const
    {
		std::unique_ptr<FeatureDetector> detector(FeatureDetectorProvider::getInstance().getDetector(detectorType, params));
        size_t N = pipeline->images.size();
        size_t id = r.begin();

        std::stringstream ss1, ss2;
        ss1 << "Start detecting keypoints with " << detectorType << " on ";
        size_t cnt = 0;
        pipeline->tic(id, false);
        size_t kpt = 0;

        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            Image& image = pipeline->images[i];
            image.keyPoints.keyPoints.clear();

            ss1 << image.filename << ", ";

			std::unique_ptr<corecvs::RuntimeTypeBuffer> img(corecvs::BufferFactory::getInstance()->loadRuntimeTypeBitmap(image.filename));
            CORE_ASSERT_TRUE_P((bool)img, ("Invalid image to extract features. See log for details."));

			img->downsample(downsampleFactor);

            detector->detect((*img.get()), image.keyPoints.keyPoints, maxFeatureCount, downsampleFactor == 1 ? image.remapCache : 0);

            kpt += image.keyPoints.keyPoints.size();
            cnt++;
            if (cnt % 4 == 0)
            {
                ss2 << kpt << " keypoints"; kpt = 0;
                pipeline->toc(ss1.str(), ss2.str(), N, cnt, id, false); cnt = 0;
                ss1.str("");
                ss2.str("");
                ss1 << "Detecting keypoints with " << detectorType << " on ";
                pipeline->tic(r.begin(), false);
            }
        }

        if (cnt)
        {
            ss2 << kpt << " keypoints"; kpt = 0;
            pipeline->toc(ss1.str(), ss2.str(), N, cnt, id, false);
            cnt = 0;
        }
    }
    ParallelDetector(
        FeatureMatchingPipeline* pipeline,
        DetectorType detectorType,
        int maxFeatureCount,
        int downsampleFactor,
        const std::string &params = "")
    :    pipeline(pipeline)
       , detectorType(detectorType)
       , params(params)
       , maxFeatureCount(maxFeatureCount)
       , downsampleFactor(downsampleFactor)
    {}
};

void KeyPointDetectionStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    std::stringstream ss1, ss2;

    size_t N = pipeline->images.size();

    corecvs::parallelable_for((size_t)0, N, CORE_MAX(N / MAX_CORE_COUNT_ESTIMATE, (size_t)1)
        , ParallelDetector(pipeline, detectorType, maxFeatureCount, downsampleFactor, params)
        , parallelable);

    ss1 << "KeyPointDetectionStage::run(): Detecting keypoints with " << detectorType;
    pipeline->toc(ss1.str(), ss2.str());
}

void KeyPointDetectionStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &_filename) const
{
    std::vector<Image>& images = pipeline->images;
    for (size_t i = 0; i < images.size(); ++i)
    {
        images[i].keyPoints.save(corecvs::HelperUtils::getFullPathWithNewExt(images[i].filename, KEYPOINT_EXTENSION));
    }
    CORE_UNUSED(_filename);
}

void KeyPointDetectionStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &_filename)
{
    std::vector<Image>& images = pipeline->images;
    for (size_t i = 0; i < images.size(); ++i)
    {
        images[i].keyPoints.load(corecvs::HelperUtils::getFullPathWithNewExt(images[i].filename, KEYPOINT_EXTENSION));
    }
    CORE_UNUSED(_filename);
}

KeyPointDetectionStage::KeyPointDetectionStage(DetectorType type, int maxFeatureCount, int downsampleFactor, const std::string &params)
    : detectorType(type)
    , downsampleFactor(downsampleFactor)
    , maxFeatureCount(maxFeatureCount)
    , params(params)
{
    FeatureDetector* detector = FeatureDetectorProvider::getInstance().getDetector(detectorType);
    parallelable = detector->isParallelable();
    delete detector;
}

//---------------------------------------------------------------------------

void DescriptorExtractionStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &_filename) const
{
    auto images = pipeline->images;
    for (size_t i = 0; i < images.size(); ++i)
    {
        images[i].descriptors.save(corecvs::HelperUtils::getFullPathWithNewExt(images[i].filename, DESCRIPTOR_EXTENSION));

        // NOTE: in some cases (e.g. using different detector/descriptor setup) we are not able
        // to compute descriptor for each of keypoint, so some of them are removed.
        // So, we need to overwrite keypoint files!
        //
        images[i].keyPoints.save(corecvs::HelperUtils::getFullPathWithNewExt(images[i].filename, KEYPOINT_EXTENSION));
    }
    CORE_UNUSED(_filename);
}

void DescriptorExtractionStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &_filename)
{
    std::vector<Image> &images = pipeline->images;
    for (size_t i = 0; i < images.size(); ++i)
    {
        Image &image = images[i];

        image.descriptors.load(corecvs::HelperUtils::getFullPathWithNewExt(image.filename, DESCRIPTOR_EXTENSION));

        CORE_ASSERT_TRUE_S(image.descriptors.type == descriptorType);
        CORE_ASSERT_TRUE_S(image.keyPoints.keyPoints.size() == (image.descriptors.mat.getRows()));
    }
    CORE_UNUSED(_filename);
}

class ParallelExtractor
{
    FeatureMatchingPipeline* pipeline;
    DescriptorType descriptorType;
    std::string params;
    int downsampleFactor;
    bool keypointsColor;

public:
    void operator() (const corecvs::BlockedRange<size_t>& r) const
    {
        DescriptorExtractor* extractor = DescriptorExtractorProvider::getInstance().getDescriptorExtractor(descriptorType, params);
        size_t N = pipeline->images.size();
        size_t id = r.begin();

        std::stringstream ss1, ss2;
        ss1 << "Extracting " << descriptorType << " descriptors " << " from ";
        size_t cnt = 0;
        pipeline->tic(id, false);
        size_t kpt = 0;

        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            Image& image = pipeline->images[i];

            ss1 << image.filename << ", ";

			std::unique_ptr<corecvs::RuntimeTypeBuffer> img(corecvs::BufferFactory::getInstance()->loadRuntimeTypeBitmap(image.filename));
            CORE_ASSERT_TRUE_P((bool)img, ("Invalid image to extract descriptors. See log for details."));

            img->downsample(downsampleFactor);

			if (extractor)
                extractor->compute(*img.get(), image.keyPoints.keyPoints, image.descriptors.mat, downsampleFactor == 1 ? image.remapCache : nullptr);

            image.descriptors.type = descriptorType;

            CORE_ASSERT_TRUE_S(image.descriptors.mat.getRows() == image.keyPoints.keyPoints.size());

            if (downsampleFactor == 1 && keypointsColor)
            {
				std::unique_ptr<corecvs::RGB24Buffer> bufferRGB(corecvs::BufferFactory::getInstance()->loadRGB24Bitmap(image.filename));
                CORE_ASSERT_TRUE_P((bool)bufferRGB, ("Invalid image to extract colors of features. See log for details."));

                for (auto& kp : image.keyPoints.keyPoints)
                {
					corecvs::RGB24Buffer::RGBEx32 mean(corecvs::RGBColor::Black());
                    int cnt = 0;
                    int x = kp.position.x();
                    int y = kp.position.y();
                    int sz = kp.size / 2;

                    for (int xx = x - sz; xx <= x + sz; ++xx)
                        for (int yy = y - sz; yy <= y + sz; ++yy)
                        {
                            if (!bufferRGB->isValidCoord(yy, xx))
                                continue;

                            auto color = bufferRGB->element(yy, xx);
						    mean += corecvs::RGB24Buffer::RGBEx32(color);
                            cnt++;
                        }
                    mean /= cnt;
                    kp.color = mean.toRGBColor();
                }
            }

            kpt += image.keyPoints.keyPoints.size();
            cnt++;
            if (cnt % 4 == 0)
            {
                ss2 << kpt << " descriptors"; kpt = 0;
                pipeline->toc(ss1.str(), ss2.str(), N, cnt, id, false); cnt = 0;
                ss1.str("");
                ss2.str("");
                ss1 << "Extracting " << descriptorType << " descriptors " << " from ";
                pipeline->tic(r.begin(), false);
            }
        }

        if (cnt)
        {
            ss2 << kpt << " descriptors"; kpt = 0;
            pipeline->toc(ss1.str(), ss2.str(), N, cnt, id, false); cnt = 0;
        }

        delete extractor;
    }
    ParallelExtractor(FeatureMatchingPipeline* pipeline, DescriptorType descriptorType, const std::string &params, int downsampleFactor, bool keypointsColor) : pipeline(pipeline), descriptorType(descriptorType), params(params), downsampleFactor(downsampleFactor), keypointsColor(keypointsColor){}
};

void DescriptorExtractionStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    std::stringstream ss1, ss2;

    size_t N = pipeline->images.size();

    corecvs::parallelable_for((size_t)0, N, CORE_MAX(N / MAX_CORE_COUNT_ESTIMATE, (size_t)1)
        , ParallelExtractor(pipeline, descriptorType, params, downsampleFactor, keypointsColor)
        , parallelable);

    ss1 << "Extracting " << descriptorType << " descriptors";
    pipeline->toc(ss1.str(), ss2.str());
}

DescriptorExtractionStage::DescriptorExtractionStage(DescriptorType type, int downsampleFactor, const std::string &params, bool keypointsColor) :
    descriptorType(type),
    downsampleFactor(downsampleFactor),
    keypointsColor(keypointsColor),
    params(params)
{
    DescriptorExtractor* extractor = DescriptorExtractorProvider::getInstance().getDescriptorExtractor(descriptorType, params);
	parallelable = extractor ? extractor->isParallelable() : false;
    delete_safe(extractor);
}

//---------------------------------------------------------------------------

void FileNameRefinedMatchingPlanComputationStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    pipeline->matchPlan.save(filename);
}

void FileNameRefinedMatchingPlanComputationStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    pipeline->matchPlan.load(filename);
}

void FileNameRefinedMatchingPlanComputationStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    MatchPlan &matchPlan = pipeline->matchPlan;
    std::vector<Image> &images = pipeline->images;
    size_t N = images.size();

    matchPlan.plan.clear();

    for (size_t i = 0; i < N; ++i)
    {
        std::deque<uint32_t> query(images[i].keyPoints.keyPoints.size());
        for (size_t j = 0; j < images[i].keyPoints.keyPoints.size(); ++j)
        {
            query[j] = (uint32_t)j;
        }

        for (size_t j = 0; j < N; ++j)
        {
            if (i == j)
                continue;
            std::string& name1 = images[i].filename;
            std::string& name2 = images[j].filename;
            int idx1 = (name1[name1.size() - 6]-'0')*10 + name1[name1.size() - 5] - '0';
            int idx2 = (name2[name2.size() - 6]-'0')*10 + name2[name2.size() - 5] - '0';
            int diff  = (24 * 100 + idx1 - idx2) % 24;
            int diff2 = (24 * 100 + idx2 - idx1) % 24;
            diff = CORE_MIN(diff, diff2);
            if (diff > 5)
            {
                continue;
            }
            std::deque<uint32_t> train(images[j].keyPoints.keyPoints.size());
            for (size_t k = 0; k < images[j].keyPoints.keyPoints.size(); ++k)
            {
                train[k] = (uint32_t)k;
            }

            MatchPlanEntry entry = { (uint16_t)i, (uint16_t)j, query, train };
            matchPlan.plan.push_back(entry);
        }
    }
    pipeline->toc("Preparing matching plan", "");
}

void MatchingPlanComputationStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    pipeline->matchPlan.save(filename);
}

void MatchingPlanComputationStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    pipeline->matchPlan.load(filename);
}

void makeMatchingPlan(FeatureMatchingPipeline& pipeline)
{
	MatchPlan &matchPlan = pipeline.matchPlan;
	std::vector<Image> &images = pipeline.images;
	size_t N = images.size();

	matchPlan.plan.clear();

    for (size_t img1Id = 0; img1Id < N; ++img1Id)
    {
        std::deque<uint32_t> query(images[img1Id].keyPoints.keyPoints.size());
        for (size_t j = 0; j < images[img1Id].keyPoints.keyPoints.size(); ++j)
        {
            query[j] = (uint32_t)j;
        }

        for (size_t img2Id = 0; img2Id < N; ++img2Id)
        {
            if (img1Id == img2Id)
                continue;
            std::deque<uint32_t> train(images[img2Id].keyPoints.keyPoints.size());
            for (size_t k = 0; k < images[img2Id].keyPoints.keyPoints.size(); ++k)
            {
                train[k] = (uint32_t)k;
            }

            MatchPlanEntry entry = { (uint16_t)img1Id, (uint16_t)img2Id, query, train };
			matchPlan.plan.push_back(entry);
		}
	}
}

void MatchingPlanComputationStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
	makeMatchingPlan(*pipeline);
    pipeline->toc("Preparing matching plan", "");
}

//---------------------------------------------------------------------------

MatchingStage::MatchingStage(DescriptorType type, MatcherType matcherType, size_t responsesPerPoint)
    : descriptorType(type), matcherType(matcherType), responsesPerPoint(responsesPerPoint)
{
    DescriptorMatcher* matcher = DescriptorMatcherProvider::getInstance().getMatcher(descriptorType, matcherType);
	parallelable = matcher ? matcher->isParallelable() : false;
    delete matcher;
}

class ParallelMatcher
{
    FeatureMatchingPipeline* pipeline;
    DescriptorType descriptorType;
    MatcherType matcherType;
    size_t responsesPerPoint;
public:
    void operator() (const corecvs::BlockedRange<size_t>& r) const
    {
        DescriptorMatcher* matcher = DescriptorMatcherProvider::getInstance().getMatcher(descriptorType, matcherType);
        size_t S = pipeline->matchPlan.plan.size();
        size_t id = r.begin();
        MatchPlan &matchPlan = pipeline->matchPlan;
        RawMatches &rawMatches = pipeline->rawMatches;
        std::vector<Image> &images = pipeline->images;

        std::stringstream ss1, ss2;
        ss1 << "Matched sets ";
        size_t cnt = 0;
        pipeline->tic(id, false);

        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            ss1 << i << ", ";
            size_t s = i;
            size_t I = matchPlan.plan[s].queryImg;
            size_t J = matchPlan.plan[s].trainImg;
            MatchPlanEntry &query = matchPlan.plan[s];

#if 0
            corecvs::RuntimeTypeBuffer qb(images[I].descriptors.mat);
            corecvs::RuntimeTypeBuffer tb(images[J].descriptors.mat);

#   if 0
            for (size_t j = 0; j < query.queryFeatures.size(); ++j)
            {
                memcpy(qb.row<void>(j), images[I].descriptors.mat.row<void>(query.queryFeatures[j] ), qb.getRowSize());
            }
            for (size_t j = 0; j < query.trainFeatures.size(); ++j)
            {
                memcpy(tb.row<void>(j), images[J].descriptors.mat.row<void>(query.trainFeatures[j]), tb.getRowSize());
            }
#   endif

#else
            corecvs::RuntimeTypeBuffer qb(query.queryFeatures.size(), images[I].descriptors.mat.getCols(), images[I].descriptors.mat.getType());
            corecvs::RuntimeTypeBuffer tb(query.trainFeatures.size(), images[J].descriptors.mat.getCols(), images[J].descriptors.mat.getType());

            for (size_t j = 0; j < query.queryFeatures.size(); ++j)
            {
                memcpy(qb.row<void>(j), images[I].descriptors.mat.row<void>(query.queryFeatures[j]), qb.getRowSize());
            }

            for (size_t j = 0; j < query.trainFeatures.size(); ++j)
            {
                memcpy(tb.row<void>(j), images[J].descriptors.mat.row<void>(query.trainFeatures[j]), tb.getRowSize());
            }
#endif

            std::vector<std::vector<RawMatch>> ml;
			if (matcher)
				matcher->knnMatch(qb, tb, ml, responsesPerPoint);

            for (std::vector<std::vector<RawMatch> >::iterator v = ml.begin(); v != ml.end(); ++v)
            {
                std::array<RawMatch, 2> mk;
                for (size_t i = 0; i < v->size(); ++i)
                {
                    mk[i] = (*v)[i];
                }
                rawMatches.matches[s].push_back(mk);
            }

            // It's time to replace indicies
            for (std::deque<std::array<RawMatch, 2> >::iterator v = rawMatches.matches[s].begin(); v != rawMatches.matches[s].end(); ++v)
            {
                for (std::array<RawMatch, 2>::iterator m = v->begin(); m != v->end() && m->isValid(); ++m)
                {
                    m->featureQ = query.queryFeatures[m->featureQ];
                    m->featureT = query.trainFeatures[m->featureT];
                }
            }

            cnt++;
            if (cnt % 16 == 0)
            {
                pipeline->toc(ss1.str(), ss2.str(), S, cnt, id, false); cnt = 0;
                ss1.str("");
                ss1 << "Matched sets ";
                pipeline->tic(r.begin(), false);
            }
        }

        if (cnt)
        {
            pipeline->toc(ss1.str(), ss2.str(), S, cnt, id, false); cnt = 0;
        }
        delete matcher;
    }

    ParallelMatcher(FeatureMatchingPipeline* pipeline, DescriptorType descriptorType, MatcherType matcherType, size_t responsesPerPoint) :
        pipeline(pipeline), descriptorType(descriptorType), matcherType(matcherType), responsesPerPoint(responsesPerPoint)
    {}
};


void MatchingStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    MatchPlan &matchPlan = pipeline->matchPlan;
    RawMatches &rawMatches = pipeline->rawMatches;

    CORE_ASSERT_TRUE_S(matchPlan.plan.size());
    rawMatches.matches.clear();
    rawMatches.matches.resize(matchPlan.plan.size());

    size_t S = matchPlan.plan.size();
	if (S)
		corecvs::parallelable_for ((size_t)0, S, CORE_MAX(S / MAX_CORE_COUNT_ESTIMATE, (size_t)1)
            , ParallelMatcher(pipeline, descriptorType, matcherType, responsesPerPoint)
            , parallelable);

    std::stringstream ss;
    pipeline->toc("Computing raw matches", ss.str());
}

void MatchingStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    pipeline->rawMatches.load(filename);
}

void MatchingStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    pipeline->rawMatches.save(filename);
}

//---------------------------------------------------------------------------

//static void combinationToIndex(size_t in, size_t &out1, size_t out2)
//{
//    double tt = -0.5 + sqrt(1.0 + 8.0 * in) / 2.0;
//    size_t J = 1+(size_t)tt;
//    size_t I = in - (J - 1) * J / 2;
//}

class ParallelMatcherRefiner
{
    FeatureMatchingPipeline* pipeline;
    DescriptorType descriptorType;
    MatcherType matcherType;
    size_t responsesPerPoint;
    double scaleThreshold;
    bool thresholdDistance;
    std::vector<int> *first_ptr, *next_ptr, *idx_ptr;

public:
    void operator() (const corecvs::BlockedRange<size_t>& r) const
    {
        DescriptorMatcher* matcher = DescriptorMatcherProvider::getInstance().getMatcher(descriptorType, matcherType);
        size_t id = r.begin();
        MatchPlan &matchPlan = pipeline->matchPlan;
        RawMatches &rawMatches = pipeline->rawMatches;
        RefinedMatches &refinedMatches = pipeline->refinedMatches;
        std::vector<Image> &images = pipeline->images;
        std::vector<int> &first = *first_ptr;
        std::vector<int> &next = *next_ptr;
        std::vector<int> &idx = *idx_ptr;

        size_t N = images.size();
        size_t S = N*(N-1)/2;
        size_t responsesPerPoint = 2;
        size_t cnt = 0;
        pipeline->tic(id, false);
        std::stringstream ss1, ss2;
        ss1 << "Matched sets ";

        for (size_t perm_id = r.begin(); perm_id < r.end(); ++perm_id)
        {
            // Now let's get image indicies from combination id
            double tt = -0.5 + sqrt(1.0 + 8.0 * perm_id) / 2.0;
            size_t J = 1+(size_t)tt;
            size_t I = perm_id - (J - 1) * J / 2;
            CORE_ASSERT_TRUE_S(I < J);
            ss1 << "(" << I << ", " << J << ")";

            std::vector<size_t> reqs;
            size_t next_id = first[J*(J-1)/2+I+1];
            while (next_id)
            {
                reqs.push_back(idx[next_id]);
                CORE_ASSERT_TRUE_S(matchPlan.plan[idx[next_id]].isBetween((uint16_t)I, (uint16_t)J));
                next_id = next[next_id];
            }

            // step 1: match
            for (size_t si = 0; si < reqs.size(); ++si)
            {
                size_t s = reqs[si];
                size_t Is = matchPlan.plan[s].queryImg;
                size_t Js = matchPlan.plan[s].trainImg;
                MatchPlanEntry &query = matchPlan.plan[s];

                CORE_ASSERT_TRUE_S(Is < N && Js < N);

#if 0
                corecvs::RuntimeTypeBuffer qb(images[Is].descriptors.mat);
                corecvs::RuntimeTypeBuffer tb(images[Js].descriptors.mat);

                for (size_t j = 0; j < query.queryFeatures.size(); ++j)
                {
                    memcpy(qb.row<void>(j), images[Is].descriptors.mat.row<void>(query.queryFeatures[j]), qb.getRowSize());
                }
                for (size_t j = 0; j < query.trainFeatures.size(); ++j)
                {
                    memcpy(tb.row<void>(j), images[Js].descriptors.mat.row<void>(query.trainFeatures[j]), tb.getRowSize());
                }
#else
                corecvs::RuntimeTypeBuffer qb(query.queryFeatures.size(), images[Is].descriptors.mat.getCols(), images[Is].descriptors.mat.getType());
                corecvs::RuntimeTypeBuffer tb(query.trainFeatures.size(), images[Js].descriptors.mat.getCols(), images[Js].descriptors.mat.getType());

                for (size_t j = 0; j < query.queryFeatures.size(); ++j)
                {
                    memcpy(qb.row<void>(j), images[Is].descriptors.mat.row<void>(query.queryFeatures[j]), qb.getRowSize());
                }

                for (size_t j = 0; j < query.trainFeatures.size(); ++j)
                {
                    memcpy(tb.row<void>(j), images[Js].descriptors.mat.row<void>(query.trainFeatures[j]), tb.getRowSize());
                }
#endif

                std::vector<std::vector<RawMatch>> ml;
				if (matcher)
					matcher->knnMatch(qb, tb, ml, responsesPerPoint);

                SYNC_PRINT(("Matcher implementation returned %" PRISIZE_T " matches\n", ml.size()));

                for (std::vector<std::vector<RawMatch> >::iterator it = ml.begin(); it != ml.end(); ++it)
                {
                    std::vector<RawMatch>& v = *it;
                    std::array<RawMatch, 2> mk;
                    for (size_t i = 0; i < v.size() && i < 2 && v[i].isValid(); ++i)
                    {
                        mk[i] = v[i];
                    }
                    rawMatches.matches[s].push_back(mk);
                }

                // It's time to replace indicies
                for (std::deque<std::array<RawMatch, 2>>::iterator it = rawMatches.matches[s].begin(); it != rawMatches.matches[s].end(); ++it)
                {
                    std::array<RawMatch, 2> &v = *it;
                    for (std::array<RawMatch, 2>::iterator m = v.begin(); m != v.end() && m->isValid(); ++m)
                    {
                        m->featureQ = query.queryFeatures[m->featureQ];
                        m->featureT = query.trainFeatures[m->featureT];
                    }
                }
            }

            // step 2: merge and clean temp
            std::deque<std::array<RawMatch, 2> > accumulator[2];

            accumulator[0].resize(images[I].keyPoints.keyPoints.size());
            accumulator[1].resize(images[J].keyPoints.keyPoints.size());

            for (size_t si = 0; si < reqs.size(); ++si)
            {
                size_t s = reqs[si];
                size_t query = matchPlan.plan[s].queryImg;
                query = (query == I) ? 0 : 1;

                for (size_t i = 0; i < rawMatches.matches[s].size(); ++i)
                {
                    for (size_t j = 0; j < rawMatches.matches[s][i].size() && rawMatches.matches[s][i][j].isValid(); ++j)
                    {
                        RawMatch &dm = rawMatches.matches[s][i][j];
                        if (dm.distance < accumulator[query][dm.featureQ][0].distance)
                        {
                            accumulator[query][dm.featureQ][1] = accumulator[query][dm.featureQ][0];
                            accumulator[query][dm.featureQ][0] = dm;
                        }
                        else if (dm.distance < accumulator[query][dm.featureQ][1].distance)
                        {
                            accumulator[query][dm.featureQ][1] = dm;
                        }
                    }
                }
                rawMatches.matches[s].clear();
                rawMatches.matches[s].shrink_to_fit();
            }

            // step 3: refine by distance
            std::deque<RawMatch> ratioInliers[2];
            ratioInliers[0].resize(images[I].keyPoints.keyPoints.size());
            ratioInliers[1].resize(images[J].keyPoints.keyPoints.size());

            for (size_t i = 0; i < 2; ++i)
            {
                for (size_t j = 0; j < ratioInliers[i].size(); ++j)
                {
                    double ratio = thresholdDistance ?
                          accumulator[i][j][0].distance / 256.0
                        : accumulator[i][j][0].distance / accumulator[i][j][1].distance;
                    if ((ratio) < scaleThreshold)
                    {
                        ratioInliers[i][j] = accumulator[i][j][0];
                        ratioInliers[i][j].best2ndBest = ratio;
                    }
                }
            }

            // step 4: refine by symmetry
            std::deque<Match> final_matches;
            for (size_t i = 0; i < images[I].keyPoints.keyPoints.size(); ++i)
            {
                RawMatch &rm = ratioInliers[0][i];
                if (!rm.isValid())
                    continue;
                if (ratioInliers[1][rm.featureT].featureT == rm.featureQ)
                    final_matches.push_back(Match((uint16_t)I, (uint16_t)J, rm.featureQ, rm.featureT, rm.distance, rm.best2ndBest));
            }

			size_t idx = J*(J - 1) / 2 + I;
			refinedMatches.matchSets[idx] = (RefinedMatchSet(I, J, final_matches));
			ss1  << "\t" << refinedMatches.matchSets[idx].matches.size() << " matches, ";
            cnt++;
            if (cnt % 16 == 0)
            {
                pipeline->toc(ss1.str(), ss2.str(), S, cnt, id, false); cnt = 0;
                ss1.str("");
                ss1 << "Matched sets ";
                pipeline->tic(r.begin(), false);
            }
        }

        if (cnt)
        {
            pipeline->toc(ss1.str(), ss2.str(), S, cnt, id, false); cnt = 0;
        }
        delete matcher;
    }

    ParallelMatcherRefiner(FeatureMatchingPipeline* pipeline, DescriptorType descriptorType, MatcherType matcherType, size_t responsesPerPoint
        , std::vector<int> *first, std::vector<int> *next, std::vector<int> *idx, double scaleThreshold = 0.95, bool thresholdDistance = false)
        : pipeline(pipeline), descriptorType(descriptorType), matcherType(matcherType), responsesPerPoint(responsesPerPoint), scaleThreshold(scaleThreshold), thresholdDistance(thresholdDistance)
        , first_ptr(first), next_ptr(next), idx_ptr(idx)
    {}
};

MatchAndRefineStage::MatchAndRefineStage(DescriptorType descriptorType, MatcherType matcherType, double scaleThreshold, bool thresholdDistance) : descriptorType(descriptorType), matcherType(matcherType), scaleThreshold(scaleThreshold), thresholdDistance(thresholdDistance)
{
    DescriptorMatcher* matcher = DescriptorMatcherProvider::getInstance().getMatcher(descriptorType, matcherType);
	parallelable = matcher ? matcher->isParallelable() : false;
    delete matcher;
}

void MatchAndRefineStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    pipeline->refinedMatches.load(filename);
}

void MatchAndRefineStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    pipeline->refinedMatches.save(filename);
}

void MatchAndRefineStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();

    MatchPlan &matchPlan = pipeline->matchPlan;
    RawMatches &rawMatches = pipeline->rawMatches;
    RefinedMatches &refinedMatches = pipeline->refinedMatches;
    std::vector<Image> &images = pipeline->images;
    size_t N = images.size();
    size_t responsesPerPoint = 2;
    size_t P = N*(N-1)/2;
    size_t S = matchPlan.plan.size();
    std::vector<int> first(P+1);
    std::vector<int> next(S+1);
    std::vector<int> idx(S+1);
    std::vector<int> last(P+1);
    size_t last_id = 1;

    for (size_t s = 0; s < matchPlan.plan.size(); ++s)
    {
        size_t I = matchPlan.plan[s].queryImg;
        size_t J = matchPlan.plan[s].trainImg;
        if (I > J)
        {
            std::swap(I, J);
        }
        size_t pair_id = J*(J-1)/2+I;
        if (!last[pair_id+1])
        {
            first[pair_id+1] = (int)last_id;
            idx  [last_id  ] = (int)s;
            last [pair_id+1] = (int)last_id;
            last_id++;
        }
        else {
            next[last[pair_id+1]] = (int)last_id;
            last[pair_id + 1]     = (int)last_id;
            idx [last_id    ]     = (int)s;
            last_id++;
        }
    }

    rawMatches.matches.resize(matchPlan.plan.size());
    refinedMatches.matchSets.resize(N*(N-1)/2);

    corecvs::parallelable_for ((size_t)0, P, CORE_MAX(P / MAX_CORE_COUNT_ESTIMATE, (size_t)1)
        , ParallelMatcherRefiner(pipeline, descriptorType, matcherType, responsesPerPoint, &first, &next, &idx, scaleThreshold, thresholdDistance)
        , parallelable);

    pipeline->toc("Computing & refining matches on-the-fly", "");
}

#ifdef WITH_CLUSTERS

RandIndexStage::RandIndexStage()
{}

void RandIndexStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    std::ofstream os;
    os.open(filename, std::ostream::out);

    size_t N = index.size();
    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j < N; ++j)
        {
            os << std::setprecision(9) <<  index[i][j] << (j + 1 == N ? "" : ",");
        }
        os << std::endl;
    }
}

void drawMatches(const std::string &prefix, size_t idxL, size_t idxR, std::vector<std::vector<size_t>> &cls, std::vector<int> &bestA, std::vector<int> &bestB, FeatureMatchingPipeline* pipeline, bool rl, RefinedMatchSet *set = 0)
{
    cv::Mat A, B, R;
    auto& images = pipeline->images;
    A = cv::imread(images[idxL].filename);
    B = cv::imread(images[idxR].filename);

    auto szA = A.size();
    auto szB = B.size();

    cv::Size sz(szA.width, szB.height + szA.height);
    R.create(sz, CV_MAKETYPE(A.depth(), 3));

    cv::Mat viewA = R(cv::Rect(0,0,szA.width,szA.height));
    cv::Mat viewB = R(cv::Rect(0, szA.height, szB.width, szB.height));
    A.copyTo(viewA);
    B.copyTo(viewB);

    char filename[1000];
    sprintf(filename, "%s_%s-%s.jpg", prefix.c_str(), images[idxL].filename.c_str(), images[idxR].filename.c_str());

    if (rl)
    {
        for (size_t i = 0; i < images[idxL].keyPoints.keyPoints.size(); ++i)
        {
            auto& kp = images[idxL].keyPoints.keyPoints[i];
            size_t idx = cls[idxL][i];
            if (bestA[bestB[idx]] != idx) idx = 0;
            cv::circle(viewA, cv::Point(kp.x, kp.y), 5, cv::Scalar((idx * 67 + 35) % 256, (idx * 7 + 142) % 256, (idx * 217 + 251) % 256), -5);
        }
        for (size_t i = 0; i < images[idxR].keyPoints.keyPoints.size(); ++i)
        {
            auto& kp = images[idxR].keyPoints.keyPoints[i];
            size_t idx = cls[idxR][i];
            if (bestB[bestA[idx]] != idx) { idx = 0; } else { idx = bestA[idx]; }

            cv::circle(viewB, cv::Point(kp.x, kp.y), 5, cv::Scalar((idx * 67 + 35) % 256, (idx * 7 + 142) % 256, (idx * 217 + 251) % 256), -5);
        }
        if (set)
        {
            auto& s = *set;
            for (size_t i = 0; i < set->matches.size(); ++i)
            {
                size_t kpL = idxL < idxR ? set->matches[i].featureA : set->matches[i].featureB;
                size_t kpR = idxL < idxR ? set->matches[i].featureB : set->matches[i].featureA;
                auto& kpA = images[idxL].keyPoints.keyPoints[kpL];
                auto& kpB = images[idxR].keyPoints.keyPoints[kpR];

                size_t idx = cls[idxL][kpL];
                cv::line(R,  cv::Point(kpA.x, kpA.y), cv::Point(kpB.x, kpB.y + szA.height), cv::Scalar((idx * 67 + 35) % 256, (idx * 7 + 142) % 256, (idx * 217 + 251) % 256));
            }
        }
    } else {
        for (size_t i = 0; i < images[idxL].keyPoints.keyPoints.size(); ++i)
        {
            auto& kp = images[idxL].keyPoints.keyPoints[i];
            size_t idx = cls[idxL][i];
            cv::circle(viewA, cv::Point(kp.x, kp.y), 5, cv::Scalar((idx * 67 + 35) % 256, (idx * 7 + 142) % 256, (idx * 217 + 251) % 256), -5);
        }
        for (size_t i = 0; i < images[idxR].keyPoints.keyPoints.size(); ++i)
        {
            auto& kp = images[idxR].keyPoints.keyPoints[i];
            size_t idx = cls[idxR][i];
            cv::circle(viewB, cv::Point(kp.x, kp.y), 5, cv::Scalar((idx * 67 + 35) % 256, (idx * 7 + 142) % 256, (idx * 217 + 251) % 256), -5);
        }
    }
    cv::imwrite(filename, R);
    std::cerr << "Writing output to " << filename << std::endl;
}

class ParallelCl {
public:
    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        RefinedMatches &refinedMatches = pipeline->refinedMatches;
        size_t S = refinedMatches.matchSets.size();
        size_t N = pipeline->images.size();
        std::vector<std::vector<size_t>>&clusters = *cls;
        std::vector<int> clcl;
        size_t id = r.begin();
        std::stringstream ss1, ss2;
        ss1 << "Computed cluster inliers for sets ";
        size_t cnt = 0;
        pipeline->tic(id, false);

        for (size_t s = r.begin(); s < r.end(); ++s)
        {
            size_t eq = 0, neq = 0, total = 0;
            cnt++;
            // A. Get best-best cluster matching.
            auto& v = refinedMatches.matchSets[s].matches;
            size_t kA = clusters[refinedMatches.matchSets[s].imgA].size();
            size_t kB = clusters[refinedMatches.matchSets[s].imgB].size();

            //std::vector<std::vector<int>> clcl(kA);
            //      for (size_t i = 0; i < kA; ++i)
            //          clcl[i].resize(kB);

            if (clcl.size() < kB * kA) clcl.resize(kB * kA);
            for (size_t i = 0; i < kB * kA; ++i) clcl[i] = 0;

            for (auto& m: v)
            {
                //clcl[clusters[m.imgA][m.featureA]][clusters[m.imgB][m.featureB]]++;
                clcl[clusters[m.imgA][m.featureA]*kB  +clusters[m.imgB][m.featureB]]++;
            }
            //      std::vector<int> best_h(K), best_v(K), sum_cl(K), sum_cl2(K);

            std::vector<int> best_A(kB), best_B(kA), sum_A(kA), sum_B(kB);

            for (int i = 0; i < kA; ++i)
            {
                for (int j = 0; j < kB; ++j)
                {
                    if (clcl[i*kB+j] > clcl[i*kB+best_B[i]]) best_B[i] = j;
                    if (clcl[i*kB+j] > clcl[best_A[j]*kB+j]) best_A[j] = i;

                    sum_A[i] += clcl[i*kB+j];
                    sum_B[j] += clcl[i*kB+j];
                }
            }
            drawMatches("features", v[0].imgA, v[0].imgB, *cls, best_A, best_B, pipeline, true, &refinedMatches.matchSets[s]);
            ss1 << v[0].imgA << ":" << v[0].imgB << ", ";
            int idx = 0;
            for (int i = 0; i < v.size(); ++i)
            {
                int clA = clusters[v[i].imgA][v[i].featureA];
                int clB = clusters[v[i].imgB][v[i].featureB];
                if (best_B[clA] != clB)
                    continue;
                if (best_A[clB] != clA)
                    continue;
                if (double(clcl[clA*kB+clB]) / sum_A[clA] < 0.2)
                    continue;
                if (double(clcl[clA*kB+clB]) / sum_B[clB] < 0.2)
                    continue;
                if (clcl[clA*kB+clB]<20)
                    continue;
                v[idx++] = v[i];
            }
            v.resize(idx);
            drawMatches("matches", v[0].imgA, v[0].imgB, *cls, best_A, best_B, pipeline, true, &refinedMatches.matchSets[s]);
            if (cnt % 16 == 0)
            {
                pipeline->toc(ss1.str(), ss2.str(), S, cnt, id, false); cnt = 0;
                ss1.str("");
                ss1 << "Computed cluster inliers for sets  ";
                pipeline->tic(r.begin(), false);
            }
        }
        if (cnt)
        {
            pipeline->toc(ss1.str(), ss2.str(), S, cnt, id, false); cnt = 0;
        }
    }
    std::vector<std::vector<size_t>> *cls;
    FeatureMatchingPipeline *pipeline;

    ParallelCl(std::vector<std::vector<size_t>> *clusters, FeatureMatchingPipeline *pipeline)
        : cls(clusters), pipeline(pipeline)
    {}
};


void RandIndexStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    RefinedMatches &refinedMatches = pipeline->refinedMatches;
    size_t N = pipeline->images.size();
    index.resize(N);
    std::deque<Image>& images = pipeline->images;
    for (size_t i = 0; i < N; ++i) index[i].resize(N);

    std::vector<std::vector<size_t>> clusters(N);
    for (size_t i = 0; i < N; ++i)
    {
        size_t M = images[i].keyPoints.keyPoints.size();
        auto& kp = images[i].keyPoints.keyPoints;
        std::cerr << "Computing clusters for " << images[i].filename << std::endl;
        size_t K = std::max((int)M / 500, 5);

        std::vector<double> cx(K), cy(K);
        std::vector<size_t> cl(M);
        std::vector<size_t> cnt(K);

        for (size_t j = 0; j < M; ++j)
            cl[j] = rand() % K;

        size_t I = 1000000;
        bool stop = false;
        for (size_t j = 0; j < I && !stop; ++j)
        {
            stop = true;
            cx.clear(); cx.resize(K);
            cy.clear(); cy.resize(K);
            cnt.clear();cnt.resize(K);

            for (size_t k = 0; k < M; ++k)
            {
                cnt[cl[k]]++;
                cx[cl[k]] += kp[k].x;
                cy[cl[k]] += kp[k].y;
            }

            for (size_t k = 0; k < K; ++k)
            {
                cx[k] /= cnt[k];
                cy[k] /= cnt[k];
            }

            for (size_t k = 0; k < K; ++k)
            {
                if (cnt[k] == 0)
                {
                    size_t idx;
                    cx[k] = kp[idx = (rand() % M)].x;
                    cy[k] = kp[idx].y;
                    //      std::cerr << "Re-init cluster " << k << std::endl;
                }
            }

            for (size_t k = 0; k < M; ++k)
            {
                double mindist = 1e100;
                size_t minidx = 0;
                for (size_t l = 0; l < K; ++l)
                {
                    double dx = (cx[l] - kp[k].x);
                    double dy = (cy[l] - kp[k].y);
                    double len = dx * dx + dy * dy;
                    if (mindist > len)
                    {
                        mindist = len;
                        minidx = l;
                    }
                }
                if (cl[k] != minidx)
                {
                    cl[k] = minidx;
                    stop = false;
                }
            }
        }
        clusters[i] =(cl);
        std::ofstream os;
        char fname[10000];
        sprintf(fname, "%s.csv", images[i].filename.c_str());
        os.open(fname, std::ofstream::out);
        for (size_t k = 0; k < M; ++k)
        {
            os << clusters[i][k] << ", ";
        }
        CORE_ASSERT_TRUE_S(clusters[i].size() == images[i].keyPoints.keyPoints.size());
    }

    pipeline->toc("Computing clusters", "");

    pipeline->tic();
    size_t S = refinedMatches.matchSets.size();
    tbb::parallel_for(tbb::blocked_range<size_t>(0, S, std::max(S / 16,(size_t)1)), ParallelCl(&clusters,pipeline));
    pipeline->toc("Computing cluster inliers", "");
}

#endif // WITH_CLUSTERS

//---------------------------------------------------------------------------

RefineMatchesStage::RefineMatchesStage(bool symmetric, double scaleThreshold)
    : symmetric(symmetric), scaleThreshold(scaleThreshold)
{}

void RefineMatchesStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    std::vector<Image> &images = pipeline->images;
    MatchPlan &matchPlan = pipeline->matchPlan;
    RawMatches &rawMatches = pipeline->rawMatches;
    RefinedMatches &refinedMatches = pipeline->refinedMatches;

    // [i][j][k][l]
    // i - query index
    // j - train index
    // k - feature index
    // l - match index (<= 2)
    // Yep, it is too ugly
    std::cerr << "Prealloc accumulator" << std::endl;
    std::deque<std::deque<std::deque<std::array<RawMatch,2> > > > accumulator;

    size_t N = pipeline->images.size();

    accumulator.resize(N);
    for (size_t i = 0; i < N; ++i)
    {
        accumulator[i].resize(N);
        for (size_t j = 0; j < N; ++j)
        {
            accumulator[i][j].resize(images[i].keyPoints.keyPoints.size());
        }
    }
    std::cerr << "Accumulator alloc'ed" << std::endl;
    std::cerr << "Populate accumulator" << std::endl;


    size_t S = matchPlan.plan.size();

    for (size_t s = 0; s < S; ++s)
    {
        size_t query = matchPlan.plan[s].queryImg;
        size_t train = matchPlan.plan[s].trainImg;

        size_t L = rawMatches.matches[s].size() , K;

        for (size_t i = 0; i < L; ++i)
        {
            K = rawMatches.matches[s][i].size();
            for (size_t j = 0; j < K && rawMatches.matches[s][i][j].isValid(); ++j)
            {
                RawMatch dm = rawMatches.matches[s][i][j];
                if (dm.distance < accumulator[query][train][dm.featureQ][0].distance)
                {
                    accumulator[query][train][dm.featureQ][1] = accumulator[query][train][dm.featureQ][0];
                    accumulator[query][train][dm.featureQ][0] = dm;
                } else if (dm.distance < accumulator[query][train][dm.featureQ][1].distance) {
                    accumulator[query][train][dm.featureQ][1] = dm;
                }
            }
        }
    }
    std::cerr << "Populated accumulator" << std::endl;
    std::cerr << "Ratio filtering" << std::endl;

    std::deque<std::deque<std::deque<RawMatch> > > ratioInliers(N);
    for (size_t i = 0; i < N; ++i)
    {
        ratioInliers[i].resize(N);
        for (size_t j = 0; j < N; ++j)
        {
            ratioInliers[i][j].resize(images[i].keyPoints.keyPoints.size());
        }
    }

    for (size_t q = 0; q < N; ++q)
    {
        for (size_t t = 0; t < N; ++t)
        {
            if (t == q) continue;

            for (size_t k = 0; k < images[q].keyPoints.keyPoints.size(); ++k)
            {
                if (accumulator[q][t][k][0].distance / accumulator[q][t][k][1].distance < 0.9)
                    ratioInliers[q][t][k] = (accumulator[q][t][k][0]);
            }
            accumulator[q][t].clear();
            accumulator[q][t].shrink_to_fit();
        }
    }
    std::cerr << "Ratio filtered" << std::endl;

    accumulator.clear(); accumulator.shrink_to_fit();
    std::cerr << "Symmetry filtering" << std::endl;

    // The second step is to remove non-symmetric matches
    for (size_t q = 0; q < N; ++q)
    {
        for (size_t t = 0; t < N; ++t)
        {
            for (size_t k = 0; k < images[q].keyPoints.keyPoints.size(); ++k)
            {
                RawMatch& dm = ratioInliers[q][t][k];
                if (dm.featureT != RawMatch::INVALID_MARKER && dm.featureQ != RawMatch::INVALID_MARKER && ratioInliers[t][q][dm.featureT].featureT != k)
                    dm.featureT = dm.featureQ = RawMatch::INVALID_MARKER;
            }
        }
    }
    std::cerr << "Symmetry filtered" << std::endl;

    // Now we can compose final match set
    size_t total_matches = 0;
    refinedMatches.matchSets.clear();
    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = i + 1; j < N; ++j)
        {
            std::deque<Match> matches;
            for (size_t k = 0; k < images[i].keyPoints.keyPoints.size(); ++k)
            {
                RawMatch& dm = ratioInliers[i][j][k];
                if (ratioInliers[i][j][k].featureT != RawMatch::INVALID_MARKER && ratioInliers[i][j][k].featureQ != RawMatch::INVALID_MARKER)
                    matches.push_back(Match((uint16_t)i, (uint16_t)j, dm.featureQ, dm.featureT, dm.distance));
            }
            if (matches.size())
            {
                refinedMatches.matchSets.push_back(RefinedMatchSet(i, j, matches));
                total_matches += matches.size();
            }
        }
    }
    std::stringstream ss;
    ss << total_matches << " matches (symmetric)";
    pipeline->toc("Filtering matches", ss.str());
}

void RefineMatchesStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    pipeline->refinedMatches.save(filename);
}

void RefineMatchesStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    pipeline->refinedMatches.load(filename);
}

VsfmWriterStage::VsfmWriterStage(bool sortFeatures) : sortFeatures(sortFeatures)
{}

void VsfmWriterStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    // 1. Read keypoints from .SIFT
    // 2. Read matches from provided filename

    std::vector<Image> &images = pipeline->images;
    RefinedMatches &refinedMatches = pipeline->refinedMatches;
    size_t N = pipeline->images.size();
    std::vector<SiftFeature> features;

    for (size_t i = 0; i < N; ++i)
    {
        images[i].keyPoints.keyPoints.clear();

        std::ifstream of;
        of.open(corecvs::HelperUtils::getFullPathWithNewExt(images[i].filename, SIFT_EXTENSION).c_str(), std::ifstream::in);
        VsfmSiftIO::readBinary(of, features);

        size_t M;
        images[i].keyPoints.keyPoints.resize(M = features.size());
        for (size_t j = 0; j < M; ++j)
            images[i].keyPoints.keyPoints[j] = KeyPoint(features[i].x, features[i].y, features[i].scale, features[i].orientation);
    }

    std::ifstream ofs;
    ofs.open(filename.c_str(), std::ifstream::in);
    CORE_ASSERT_TRUE_S(ofs);

    while (ofs)
    {
        size_t I, J, K;
        ofs >> I >> J >> K;
        std::vector<size_t> idxA(K), idxB(K);
        for (size_t i = 0; i < K; ++i)
            ofs >> idxA[i];
        for (size_t i = 0; i < K; ++i)
            ofs >> idxB[i];
        std::deque<Match> matches;
        for(size_t i = 0; i < K; ++i)
            matches.push_back(Match((uint16_t)I, (uint16_t)J, (RawMatch::FeatureId)idxA[i], (RawMatch::FeatureId)idxB[i], 0.0));

        RefinedMatchSet set(I, J, matches);
        refinedMatches.matchSets.push_back(set);
    }
}

void VsfmWriterStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    size_t N = pipeline->images.size();
    std::vector<std::vector<SiftFeature> > features(N);

    // 1. assign importance to features
    // 2. sort features by their importance (?)
    // 3. move backwards features without matches (?)
    // 4. write .SIFT
    std::vector<Image> &images = pipeline->images;
    RefinedMatches &refinedMatches = pipeline->refinedMatches;

    uint8_t data[128] = {0}; // Just do not care about data

    for (size_t i = 0; i < N; ++i)
    {
        features[i].resize(images[i].keyPoints.keyPoints.size());
        for (size_t k = 0; k < images[i].keyPoints.keyPoints.size(); ++k)
        {
            features[i][k] = SiftFeature(
                    images[i].keyPoints.keyPoints[k].position.x(),
                    images[i].keyPoints.keyPoints[k].position.y(),
                    data,
                    images[i].keyPoints.keyPoints[k].size,
                    images[i].keyPoints.keyPoints[k].angle,
                    0.0,
                    0.0);

        }
    }

    for (size_t i = 0; i < refinedMatches.matchSets.size(); ++i)
    {
        for (size_t j = 0; j < refinedMatches.matchSets[i].matches.size(); ++j)
        {
            Match& m = refinedMatches.matchSets[i].matches[j];
            features[m.imgA][m.featureA].importance += 1.0;
            features[m.imgB][m.featureB].importance += 1.0;
        }
    }

    std::vector<std::vector<size_t> > reordering(N);
    std::vector<std::vector<size_t> > reordering_rev(N);
    for (size_t i = 0; i < N; ++i)
    {
        size_t M;
        reordering[i].resize(M = images[i].keyPoints.keyPoints.size());
        reordering_rev[i].resize(M);
        for (size_t j = 0; j < M; ++j)
            reordering[i][j] = j;
        if (sortFeatures)
        {
            std::sort(reordering[i].begin(), reordering[i].end(),
                    [&](const size_t &a, const size_t &b)
                    {return features[i][a].importance > features[i][b].importance; });
        }
        for (size_t j = 0; j < M; ++j)
            reordering_rev[i][reordering[i][j]] = j;
    }

    for (size_t i = 0; i < N; ++i)
    {
        std::vector<SiftFeature> write_features(features[i].size());
        for (size_t j = 0; j < features[i].size(); ++j)
        {
            write_features[j] = features[i][reordering[i][j]];
        }

        std::ofstream of;
        of.open(corecvs::HelperUtils::getFullPathWithNewExt(images[i].filename, SIFT_EXTENSION).c_str(), std::ofstream::out);
        VsfmSiftIO::writeAscii(of, write_features);
    }

    std::ofstream ofs;
    ofs.open(filename.c_str(), std::ofstream::out);
    CORE_ASSERT_TRUE_S(ofs);

    for (size_t i = 0; i < refinedMatches.matchSets.size(); ++i)
    {
        RefinedMatchSet& set = refinedMatches.matchSets[i];
        if (!set.matches.size())
            continue;

        ofs << corecvs::HelperUtils::getFileNameFromFilePath(images[set.imgA].filename) << std::endl
            << corecvs::HelperUtils::getFileNameFromFilePath(images[set.imgB].filename) << std::endl;

        ofs << set.matches.size() << std::endl;

        for (std::deque<Match>::iterator m = set.matches.begin(); m != set.matches.end(); ++m)
        {
            CORE_ASSERT_TRUE_S(m->imgA == set.imgA);
            ofs << reordering_rev[m->imgA][m->featureA] << " ";
        }
        ofs << std::endl;

        for (std::deque<Match>::iterator m = set.matches.begin(); m != set.matches.end(); ++m)
        {
            CORE_ASSERT_TRUE_S(m->imgB == set.imgB);
            ofs << reordering_rev[m->imgB][m->featureB] << " ";
        }
        ofs << std::endl;
    }
}

void FeatureMatchingPipeline::tic(size_t thread_id, bool level)
{
#ifdef WITH_TBB
    tbb::spin_mutex::scoped_lock lock(mutex);
#endif
    if (level)
    {
        if (tics.size() == 0)
        {
            for (size_t i = 0; i < 108; ++i)
            {
                std::cerr << "-";
            }
            std::cerr << std::endl;
        }
        tic_data data;
        data.thread_tics[thread_id] = clock();
        tics.push(data);
    } else {
        tics.top().thread_tics[thread_id] = clock();
    }
}

void FeatureMatchingPipeline::toc(const std::string &name, const std::string &evt, size_t thread_id, bool level)
{
#ifdef WITH_TBB
    tbb::spin_mutex::scoped_lock lock(mutex);
#endif
    size_t toc = clock();
    if (level)
    {
        tic_data tic = tics.top();
        tics.pop();

        size_t ns = (toc - tic.thread_tics[thread_id]) / (CLOCKS_PER_SEC / 1000);
        std::cerr << std::setw(64) << name << std::setw(32) << evt << std::setw(10) << ns << "ms" << std::endl;

        if (tics.size() == 0)
        {
            for (size_t i = 0; i < 108;++i)
                std::cerr << "-";
            std::cerr << std::endl;
        }
    } else {
        tic_data& tic = tics.top();
        size_t tict = tic.thread_tics[thread_id];
        size_t ns = (toc - tict)  / (CLOCKS_PER_SEC / 1000);
        std::cerr << std::setw(64) << name << std::setw(32) << evt << std::setw(10) << ns << "ms" << std::endl;
    }
}

void FeatureMatchingPipeline::toc(const std::string &name, const std::string &evt, size_t curr, size_t rem, size_t thread_id, bool level)
{
#ifdef WITH_TBB
    tbb::spin_mutex::scoped_lock lock(mutex);
#endif
    if (level)
    {
        size_t toc = clock();
        tic_data& tic = tics.top();

        size_t ns = (toc - tic.thread_tics[thread_id]) / (CLOCKS_PER_SEC / 1000);
        tics.pop();
        std::cerr << std::setw(64) << name << std::setw(32) << evt << std::setw(10) << ns << "ms"  <<  std::endl;
    } else {
        size_t toc = clock();
        tic_data& tic = tics.top();
        size_t ns = (toc - tic.thread_tics[thread_id]) / (CLOCKS_PER_SEC / 1000);
        size_t total = 0, count = 0;
        tic.thread_totals[thread_id] += ns;
        tic.thread_counts[thread_id]+= rem;
        total = (toc - tic.thread_tics[~(size_t)0]) / (CLOCKS_PER_SEC / 1000);

        for (std::map<size_t,size_t>::iterator v = tic.thread_counts.begin(); v != tic.thread_counts.end(); ++v)
            count += v->second;

        double one = double(total)/double(count);
        int rs = int((curr - count) * one / 1e3);
        int rm = rs / 60; rs = rs % 60;
        int rh = rm / 60; rm = rm % 60;

        std::cerr << std::setw(64) << name << std::setw(32) << evt << std::setw(10) << ns << "ms (" << rh << "h " << rm << "m " << rs << "s left)" << " [ " << one << " ] " <<  std::endl;
    }
}

//---------------------------------------------------------------------------

DetectAndExtractStage::DetectAndExtractStage(DetectorType detectorType, DescriptorType descriptorType, int maxFeatureCount, int downsampleFactor, const std::string &params, bool keypointsColor)
    : detectorType(detectorType)
    , descriptorType(descriptorType)
    , maxFeatureCount(maxFeatureCount)
    , downsampleFactor(downsampleFactor)
    , keypointsColor(keypointsColor)
    , params(params)
{
    DetectAndExtract* detector = DetectAndExtractProvider::getInstance().getDetector(detectorType, descriptorType);
    parallelable = detector ? detector->isParallelable() : false;
    delete detector;
}

class ParallelDetectorExtractor
{
    FeatureMatchingPipeline* pipeline;
    DetectorType detectorType;
    DescriptorType descriptorType;
    std::string params;
    int maxFeatureCount;
    int downsampleFactor;
    bool keypointsColor;
public:
    void operator() (const corecvs::BlockedRange<size_t>& r) const
    {
        std::unique_ptr<DetectAndExtract> detector(DetectAndExtractProvider::getInstance().getDetector(detectorType, descriptorType, params));
        size_t N = pipeline->images.size();
        size_t id = r.begin();

        std::stringstream ss1, ss2;
        ss1 << "Detecting/Extracting keypoints from " << detectorType << " on ";
        size_t cnt = 0;
        pipeline->tic(id, false);
        size_t kpt = 0;

        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            Image& image = pipeline->images[ i ];
            image.keyPoints.keyPoints.clear();
			image.descriptors.mat = corecvs::RuntimeTypeBuffer();

            ss1 << image.filename << ",\n";

            std::unique_ptr<BufferReader> reader(BufferReaderProvider::getInstance().getBufferReader(image.filename));
			corecvs::RuntimeTypeBuffer img = reader->read(image.filename);
            img.downsample(downsampleFactor);
   
            if (detector.get())
                detector->detectAndExtract(img, image.keyPoints.keyPoints, image.descriptors.mat, maxFeatureCount, downsampleFactor == 1 ? image.remapCache : nullptr);

            kpt += image.keyPoints.keyPoints.size();
            image.descriptors.type = descriptorType;
            CORE_ASSERT_TRUE_S(image.descriptors.mat.getRows() == image.keyPoints.keyPoints.size());

            if (downsampleFactor == 1 && keypointsColor)
            {
                corecvs::RGB24Buffer bufferRGB = reader->readRgb(image.filename);
                for (auto& kp : image.keyPoints.keyPoints)
                {
					corecvs::RGB24Buffer::RGBEx32 mean(corecvs::RGBColor::Black());
                    int cnt = 0;
                    int x = kp.position.x();
                    int y = kp.position.y();
                    int sz = kp.size / 2;

                    for (int xx = x - sz; xx <= x + sz; ++xx)
                        for (int yy = y - sz; yy <= y + sz; ++yy)
                        {
                            if (!bufferRGB.isValidCoord(yy, xx))
                                continue;

                            auto color = bufferRGB.element(yy, xx);
							mean += corecvs::RGB24Buffer::RGBEx32(color);
                            cnt++;
                        }
                    mean /= cnt;
                    kp.color = mean.toRGBColor();
                }
            }

            cnt++;
            if (cnt % 4 == 0)
            {
                ss2 << kpt << " keypoints / descriptors"; kpt = 0;
                pipeline->toc(ss1.str(), ss2.str(), N, cnt, id, false); cnt = 0;
                ss1.str("");
                ss2.str("");
                ss1 << "Detecting " << detectorType << " keypoints and " << descriptorType << " descriptors ";
                pipeline->tic(r.begin(), false);
            }
        }

        if (cnt)
        {
            ss2 << kpt << " keypoints / descriptors"; kpt = 0;
            pipeline->toc(ss1.str(), ss2.str(), N, cnt, id, false);
            cnt = 0;
        }
    }

    ParallelDetectorExtractor(FeatureMatchingPipeline* pipeline, DetectorType detectorType, DescriptorType descriptorType,
        int maxFeatureCount, int downsampleFactor, const std::string &params = "", bool keypointsColor = true) :
        pipeline(pipeline), 
        detectorType(detectorType), 
        descriptorType(descriptorType),
        params(params), 
        maxFeatureCount(maxFeatureCount), 
        downsampleFactor(downsampleFactor),
        keypointsColor(keypointsColor)
    {}
};

void DetectAndExtractStage::run(FeatureMatchingPipeline *pipeline)
{
    pipeline->tic();
    std::stringstream ss1, ss2;

    size_t N = pipeline->images.size();

    corecvs::parallelable_for((size_t)0, N, CORE_MAX(N / MAX_CORE_COUNT_ESTIMATE, (size_t)1)
        , ParallelDetectorExtractor(pipeline, detectorType, descriptorType, maxFeatureCount, downsampleFactor, params, keypointsColor)
        , parallelable);

    ss1 << "Detecting by " << detectorType << " and extracting descriptors by " << descriptorType;
    pipeline->toc(ss1.str(), ss2.str());
}

void DetectAndExtractStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &_filename) const
{
    auto images = pipeline->images;
    for (size_t i = 0; i < images.size(); ++i)
    {
        Image &image = images[i];
        image.keyPoints.save  (corecvs::HelperUtils::getFullPathWithNewExt(image.filename, KEYPOINT_EXTENSION));
        image.descriptors.save(corecvs::HelperUtils::getFullPathWithNewExt(image.filename, DESCRIPTOR_EXTENSION));
    }

    CORE_UNUSED(_filename);
}

void DetectAndExtractStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &_filename)
{
    std::vector<Image>& images = pipeline->images;
    for (size_t i = 0; i < images.size(); ++i)
    {
        Image &image = images[i];
        image.keyPoints.load  (corecvs::HelperUtils::getFullPathWithNewExt(image.filename, KEYPOINT_EXTENSION));
        image.descriptors.load(corecvs::HelperUtils::getFullPathWithNewExt(image.filename, DESCRIPTOR_EXTENSION));

        CORE_ASSERT_TRUE_S(image.descriptors.type == descriptorType);
        CORE_ASSERT_TRUE_S(image.keyPoints.keyPoints.size() == (image.descriptors.mat.getRows()));
    }
    CORE_UNUSED(_filename);
}

//---------------------------------------------------------------------------

DetectExtractAndMatchStage::DetectExtractAndMatchStage(DetectorType detectorType, DescriptorType descriptorType, MatcherType matcherType, int maxFeatureCount, int downsampleFactor, size_t responsesPerPoint, const std::string &params)
    : detectorType(detectorType)
    , descriptorType(descriptorType)
    , matcherType(matcherType)   
    , maxFeatureCount(maxFeatureCount)
    , downsampleFactor(downsampleFactor)
    , responsesPerPoint(responsesPerPoint)
    , params(params)
{}

void DetectExtractAndMatchStage::run(FeatureMatchingPipeline *pipeline)
{
    std::unique_ptr<DetectExtractAndMatch> detector(DetectExtractAndMatchProvider::getInstance().getDetector(detectorType, descriptorType, matcherType, params));
    std::stringstream ss1;
    pipeline->tic();

	const size_t numImages = pipeline->images.size();
	if (detector.get())
		detector->detectExtractAndMatch(*pipeline, maxFeatureCount, (int)responsesPerPoint);
	else
	{
		for (uint i = 0; i < numImages; i++)
			pipeline->images[i].keyPoints.keyPoints.clear();
	}

	for (uint i = 0; i < numImages; i++)
		std::cerr << pipeline->images[i].filename << "\t\t\t " << pipeline->images[i].keyPoints.keyPoints.size() << " keypoints " << std::endl;

	ss1 << "Detect " << detectorType << " and match " << matcherType;
    pipeline->toc(ss1.str(), "");
}

void DetectExtractAndMatchStage::loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename)
{
    pipeline->rawMatches.load(filename);
}

void DetectExtractAndMatchStage::saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const
{
    pipeline->rawMatches.save(filename);
}

//---------------------------------------------------------------------------

void addDetectExtractAndMatchStage(FeatureMatchingPipeline& pipeline, DetectorType detectorType, DescriptorType descriptorType, MatcherType matcherType,
    int maxFeatureCount, int downsampleFactor, const std::string &params, size_t responsesPerPoint)
{
#ifndef WITH_OPENCV_3x
    if (std::string::npos != detectorType  .find("_GPU") &&
        std::string::npos != descriptorType.find("_GPU") &&
        std::string::npos != matcherType   .find("_GPU"))
	{
        pipeline.add(new DetectExtractAndMatchStage(detectorType, descriptorType, matcherType, maxFeatureCount, downsampleFactor, responsesPerPoint, params), true);
	}
	else
#endif
	{
        pipeline.add(new KeyPointDetectionStage(detectorType, maxFeatureCount, downsampleFactor, params), true);
        pipeline.add(new DescriptorExtractionStage(descriptorType, downsampleFactor, params), true);
		pipeline.add(new MatchingPlanComputationStage(), true);
		pipeline.add(new MatchingStage(descriptorType, matcherType, responsesPerPoint), true);
	}
}

void addDetectAndExtractStage(FeatureMatchingPipeline& pipeline,
	DetectorType detectorType,
	DescriptorType descriptorType,
	int maxFeatureCount,
        int downsampleFactor,
        const std::string &params,
        bool keypointsColor)
{

	if (std::string::npos != detectorType  .find("_GPU") &&
		std::string::npos != descriptorType.find("_GPU"))
	{
        pipeline.add(new DetectAndExtractStage(detectorType, descriptorType, maxFeatureCount, downsampleFactor, params, keypointsColor), true);
	}
	else if (detectorType == "AKAZE" && descriptorType == "AKAZE")
    //else if (detectorType == descriptorType)
    {
        pipeline.add(new DetectAndExtractStage(detectorType, descriptorType, maxFeatureCount, downsampleFactor, params, keypointsColor), true);
    }
    else
	{
        pipeline.add(new KeyPointDetectionStage(detectorType, maxFeatureCount, downsampleFactor, params), true);
        pipeline.add(new DescriptorExtractionStage(descriptorType, downsampleFactor, params, keypointsColor), true);
	}
}

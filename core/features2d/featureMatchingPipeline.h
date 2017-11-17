#pragma once

#include <string>
#include <stack>
#include <map>

#include "core/utils/global.h"

#include "core/features2d/imageKeyPoints.h"
#include "core/features2d/imageMatches.h"   // RawMatches
#include "core/features2d/matchingPlan.h"   // MatchPlan
#include "core/utils/statusTracker.h"
#include "core/tbbwrapper/tbbWrapper.h"     // tbb::spin_mutex

class FeatureMatchingPipeline;

class FeatureMatchingPipelineStage
{
public:
    virtual void run(FeatureMatchingPipeline *pipeline) = 0;
    virtual void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename) = 0;
    virtual void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const = 0;
    virtual ~FeatureMatchingPipelineStage() {}
    static const int MAX_CORE_COUNT_ESTIMATE = 128;
};

class KeyPointDetectionStage : public FeatureMatchingPipelineStage
{
public:
    KeyPointDetectionStage( DetectorType type, int maxFeatureCount, int downsampleFactor = 1, const std::string &params = "" );
    void run(FeatureMatchingPipeline *pipeline);
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    ~KeyPointDetectionStage() {}
private:
    DetectorType detectorType;
    int downsampleFactor;
    bool parallelable;
    int maxFeatureCount;
    std::string params;
};

class DescriptorExtractionStage : public FeatureMatchingPipelineStage
{
public:
    DescriptorExtractionStage( DescriptorType type, int downsampleFactor = 1, const std::string &params = "", bool keypointsColor = true );
    void run(FeatureMatchingPipeline *pipeline);
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    ~DescriptorExtractionStage() {}
private:
    DescriptorType descriptorType;
    int downsampleFactor;
    bool parallelable;
    bool keypointsColor;
    std::string params;
};

class MatchingPlanComputationStage : public FeatureMatchingPipelineStage
{
public:
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    void run(FeatureMatchingPipeline *pipeline);
    ~MatchingPlanComputationStage() {}
};

class FileNameRefinedMatchingPlanComputationStage : public FeatureMatchingPipelineStage
{
public:
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    void run(FeatureMatchingPipeline *pipeline);
    ~FileNameRefinedMatchingPlanComputationStage() {}
};

class MatchingStage : public FeatureMatchingPipelineStage
{
public:
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    void run(FeatureMatchingPipeline *pipeline);
    ~MatchingStage() {}
    MatchingStage(DescriptorType type, MatcherType matcher, size_t responsesPerPoint = 2);
private:
    DescriptorType descriptorType;
    MatcherType matcherType;
    size_t responsesPerPoint;
    bool parallelable;
};

class RefineMatchesStage : public FeatureMatchingPipelineStage
{
public:
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    void run(FeatureMatchingPipeline *pipeline);
    ~RefineMatchesStage() {}
    RefineMatchesStage(bool symmetric = true, double scaleThreshold = 1.2);
private:
    bool symmetric;
    double scaleThreshold;
};

class MatchAndRefineStage : public FeatureMatchingPipelineStage
{
public:
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    void run(FeatureMatchingPipeline* pipeline);
    ~MatchAndRefineStage() {}
    MatchAndRefineStage(DescriptorType descriptorType, MatcherType matcherType, double scaleThreshold = 0.95, bool thresholdDistance = false);
private:
    DescriptorType descriptorType;
    MatcherType matcherType;
    double scaleThreshold;
    bool thresholdDistance;
    bool parallelable;
};

//class EpipolarRefiner : public FeatureMatchingPipelineStage
//{
//public:
//    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
//    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
//    void run(FeatureMatchingPipeline *pipeline);
//    EpipolarRefiner(double distanceLimit = 100.0);
//private:
//    double distanceLimit;
//
//};

class VsfmWriterStage : public FeatureMatchingPipelineStage
{
public:
    void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
    void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;
    void run(FeatureMatchingPipeline *pipeline) { CORE_UNUSED(pipeline); }
    VsfmWriterStage(bool sortFeatures);
private:
    bool sortFeatures;
};

class DetectAndExtractStage : public FeatureMatchingPipelineStage
{
public:
    DetectAndExtractStage( DetectorType detectorType, DescriptorType descriptorType, int maxFeatureCount, int downsampleFactor = 1, const std::string &params = "", bool keypointsColor = true );
    void run( FeatureMatchingPipeline *pipeline );
    void loadResults( FeatureMatchingPipeline *pipeline, const std::string &filename );
    void saveResults( FeatureMatchingPipeline *pipeline, const std::string &filename ) const;

private:
    DetectorType detectorType;
    DescriptorType descriptorType;
    int maxFeatureCount;
    int downsampleFactor;
    bool parallelable;
    bool keypointsColor;
    std::string params;
};

class DetectExtractAndMatchStage : public FeatureMatchingPipelineStage
{
public:
    DetectExtractAndMatchStage( DetectorType detectorType, DescriptorType descriptorType, MatcherType matcherType, int maxFeatureCount, int downsampleFactor = 1, size_t responsesPerPoint = 2, const std::string &params = "" );
	void run(FeatureMatchingPipeline *pipeline);
	void loadResults(FeatureMatchingPipeline *pipeline, const std::string &filename);
	void saveResults(FeatureMatchingPipeline *pipeline, const std::string &filename) const;

private:
    DetectorType detectorType;
    DescriptorType descriptorType;
    MatcherType matcherType;
    int maxFeatureCount;
    int downsampleFactor;
    size_t responsesPerPoint;
    std::string params;
};

class FeatureMatchingPipeline
{
public:
	FeatureMatchingPipeline(const std::vector<std::string> &filenames, const std::vector<void*> &remapCaches, corecvs::StatusTracker* processState = nullptr);
	~FeatureMatchingPipeline();

    void run();
    void add(FeatureMatchingPipelineStage* stage, bool run, bool saveData = false, const std::string &saveName = "", bool loadData = false, const std::string &loadName = "");
    void tic(size_t thread_id = ~(size_t)0, bool level = true);
    void toc(const std::string &name, const std::string &evt, size_t thread_id = ~(size_t)0, bool level = true);
    void toc(const std::string &name, const std::string &evt, const size_t curr, const size_t rem, size_t thread_id = ~(size_t)0, bool level = true);

    std::vector<Image>  images;
    MatchPlan           matchPlan;
    RawMatches          rawMatches;
    RefinedMatches      refinedMatches;

    DetectorType        detectorType;
    DescriptorType      descriptorType;

#ifdef WITH_TBB
    tbb::spin_mutex     mutex;
#endif

    static void printCaps();

private:
    struct tic_data
    {
        std::map<size_t, size_t> thread_tics;
        std::map<size_t, size_t> thread_totals, thread_counts;
    };
    std::stack<tic_data> tics;


    std::vector<FeatureMatchingPipelineStage*>  pipeline;

	std::vector<bool>                           runEnable;
	std::vector<std::pair<bool, std::string> >  saveParams;
	std::vector<std::pair<bool, std::string> >  loadParams;
	FeatureMatchingPipeline(const FeatureMatchingPipeline&);

    corecvs::StatusTracker*         processState = nullptr;
};

//---------------------------------------------------------------------------

void addDetectExtractAndMatchStage(FeatureMatchingPipeline& pipeline,
    DetectorType detectorType,
    DescriptorType descriptorType,
    MatcherType matcherType,
    int maxFeatureCount = 4000,
    int downsampleFactor = 1,
    const std::string &params = "",
    size_t responsesPerPoint = 2);

void addDetectAndExtractStage(FeatureMatchingPipeline& pipeline,
    DetectorType detectorType,
    DescriptorType descriptorType,
    int maxFeatureCount = 4000,
    int downsampleFactor = 1,
    const std::string &params = "",
    bool keypointsColor = true);

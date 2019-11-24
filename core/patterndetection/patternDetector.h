#ifndef PATTERNDETECTOR
#define PATTERNDETECTOR

#include "core/stats/calculationStats.h"
#include "core/utils/global.h"

#include "core/alignment/selectableGeometryFeatures.h"
#include "core/buffers/g8Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"


#include "core/filters/newstyle/newStyleBlock.h"
#include "core/utils/debuggableBlock.h"
#include "core/reflection/dynamicObject.h"

#include "core/xml/generated/patternDetectorResultBase.h"


namespace corecvs {

/**
 *   Generic interface for pattern detecting
 **/
class PatternGeometryDetector
{
public:
    /**
     * Calls pattern detector on grayscale buffer
     **/
    virtual bool detectPattern(corecvs::G8Buffer    &buffer) = 0;
    /**
     * (by default) calls detectPattern(G8Buffer) with transformed image
     **/
    virtual bool detectPattern(corecvs::RGB24Buffer &buffer);
    /**
     * Returns previously detected points as ObservationList
     **/
    virtual void getPointData(corecvs::ObservationList &observations) = 0;
    /**
     *  Returns previously detected points as SGF (by default - brutforces ObservationList
     **/
    virtual void getPointData(corecvs::SelectableGeometryFeatures &features);
    virtual ~PatternGeometryDetector() {}

    virtual void setStatistics(corecvs::Statistics * /*stats*/ = NULL) {}

    virtual size_t detectPatterns(corecvs::RGB24Buffer &buffer, std::vector<corecvs::ObservationList> &patterns);
};

class PatternDetectorResult : public PatternDetectorResultBase
{
public:
    PatternDetectorResult() :
        PatternDetectorResultBase(
            Vector2dParameters(Vector2dd(0, 0)),
            Vector2dParameters(Vector2dd(1, 0)),
            Vector2dParameters(Vector2dd(0, 1)),
            Vector2dParameters(Vector2dd(1, 1)),
            0
        )
    {}



};

class PatternDetector : public NewStyleBlock, public DebuggableBlock
{
public:

    virtual std::map<std::string, DynamicObject> getParameters() = 0;
    virtual bool setParameters(std::string name, const DynamicObject &param) = 0;

    virtual void setInputImage(RGB24Buffer * /*input*/) {}
    virtual int operator()() override { return 0; }

    virtual void getOutput(vector<PatternDetectorResult> &/*patterns*/){}

    virtual ~PatternDetector() {}
    virtual void setStatistics(corecvs::Statistics * /*stats*/ = NULL) {}
};

class PatternDetectorProducerBase {
public:
    virtual std::string getName() const {return "base";}
    virtual PatternDetector *produce() { return NULL; }
    virtual ~PatternDetectorProducerBase() {}
};

template<class Wrappee>
class PatternDetectorProducer : public PatternDetectorProducerBase
{
public:
    std::string name;

    PatternDetectorProducer(const std::string &name) :
        name(name)
    {}

    virtual std::string getName() const override {return name;}
    virtual Wrappee *produce() override
    {
        return new Wrappee();
    }
};

class PatternDetectorFabric
{
public:
    /** Public function to get the only one instance of this object.
     *  It will be automatically destroyed on shut down.
     */
    static PatternDetectorFabric* getInstance();


    std::vector<PatternDetectorProducerBase *> producers;

    void add(PatternDetectorProducerBase *producer)
    {
        producers.push_back(producer);
    }

    PatternDetector *fabricate(const std::string &name)
    {
        for (PatternDetectorProducerBase *p : producers)
        {
            if (p->getName() == name)
            {
                return p->produce();
            }
        }
        return NULL;
    }

    virtual ~PatternDetectorFabric()
    {
        for (PatternDetectorProducerBase *p : producers)
        {
            delete_safe(p);
        }
        producers.clear();
    }

    std::vector<std::string> getCaps() const
    {
        std::vector<std::string> result;

        for (const PatternDetectorProducerBase *p : producers)
        {
            result.push_back(p->getName());
        }
        return result;
    }

    void print()
    {
        std::cout << "PatternDetectorFabric knows how to:" << std::endl;
        for (PatternDetectorProducerBase *p : producers)
        {
            std::cout << "  " << p->getName() << std::endl;
        }
    }


};

} // namespace corecvs

#endif

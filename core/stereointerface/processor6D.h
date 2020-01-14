#ifndef PROCESSOR6D_H_
#define PROCESSOR6D_H_

/**
 * \file processor6D.h
 * \brief Add Comment Here
 *
 * \date Apr 2, 2011
 * \author alexander
 */


#include "core/utils/global.h"

#include "core/reflection/dynamicObject.h"

#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/flow/flowBuffer.h"
#include "core/buffers/flow/sixDBuffer.h"
#include "core/stats/calculationStats.h"

#include "core/patterndetection/patternDetector.h"

namespace corecvs {

/**
 *  Type to store point tracks:
 *  vector - is per-point
 *     map - maps frame number to PatternDetectorResult
 **/
typedef std::vector<std::map<int, PatternDetectorResult> > FlowTracks;

class ProcessorFlow {
public:
    enum ResultNames {
       RESULT_FLOW = 0x01,
       RESULT_FLOAT_FLOW   = 0x08,
       RESULT_FLOAT_FLOW_LIST = 0x20,
       RESULT_FLOW_TRACKS = 0x100,
    };

    enum FrameNames {
       FRAME_DEFAULT = 0
    };


    virtual int beginFrame() = 0;
    virtual int setFrameG12  (G12Buffer   *frame, int frameType = FRAME_DEFAULT) = 0;
    virtual int setFrameRGB24(RGB24Buffer *frame, int frameType = FRAME_DEFAULT) = 0;
    virtual int endFrame() = 0;

    /** Completly reset internal data structures. parameters are left intact **/
    virtual int reset() = 0;

    virtual std::map<std::string, DynamicObject> getParameters() = 0;
    virtual bool setParameters(std::string name, const DynamicObject &param) = 0;

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(Statistics *stats) = 0;

    /**
     * Setting this value to the combination of ResultNames flags requests output buffers.
     * This is a request only, if other combination parameters doesn't allow to produce particular outputs,
     * they may be returned as NULL
     **/
    virtual int requestResultsi(int /*parameterName*/)                         { return 0; }


    virtual int getResultCaps  () {return 0;}

    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/

    /** This method computes flow form current frame to previous **/
    virtual FlowBuffer         *getFlow() = 0;
    virtual CorrespondenceList *getFlowList() = 0;
    virtual FloatFlowBuffer    *getFloatFlow() = 0;
    virtual FlowTracks         *getFlowTracks() = 0;



    virtual int getError(std::string *errorString) = 0;


    virtual ~ProcessorFlow() {}
};

class Processor6D : public ProcessorFlow
{
protected:
    Processor6D() {}

public:
    enum StructureNames {
       FEATURES = 0x1
    };

    enum ResultNames {
       RESULT_FLOW = ProcessorFlow::RESULT_FLOW,
       RESULT_STEREO = 0x02,
       RESULT_6D = 0x04,
       RESULT_FLOAT_FLOW   = ProcessorFlow::RESULT_FLOAT_FLOW,
       RESULT_FLOAT_STEREO = 0x10,
       RESULT_FLOAT_FLOW_LIST = ProcessorFlow::RESULT_FLOAT_FLOW_LIST
    };

    enum FrameNames {
       FRAME_LEFT_ID = ProcessorFlow::FRAME_DEFAULT,
       FRAME_RIGHT_ID,
       FRAME_NAME_LAST = FRAME_RIGHT_ID
    };

    /** Implemetation may allow you to reset some internal structures at will **/
    virtual int clean(int mask) = 0;

    /**
     * You are responisble to delete the frame sometime after endFrame() is called
     * Implemetation may (and will) store additinal views (pointers) to actual image data
     * for as long as implementation need them.
     *
     * see AbstractBuffer::createViewPtr<> for details on this mechanism
     *
     * To get consistent result don't modify frame data after the call to setFrame*
     **/

    /** ProcessorFlow  unshadow **/
    using ProcessorFlow::setFrameG12  ;
    using ProcessorFlow::setFrameRGB24;

    /** Depricated use calls from ProcessorFlow  **/
    virtual int setFrameG12  (int frameType, G12Buffer   *frame);
    virtual int setFrameRGB24(int frameType, RGB24Buffer *frame);


    virtual int setDisparityBufferS16(FlowBuffer *frame, int frameType) = 0;


    /**
     * Oldstyle calls
     *
     * \attention Depricated
     **/
    virtual int setParameteri  (int /*parameterName*/, int /*parameterValue*/) { return 0; }
    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/
    virtual FlowBuffer *getStereo() = 0;


    virtual ~Processor6D() {}


};


/**  Factory. Note. this is similar to AlgoNaming **/



template<class Product>
class AlgoFactoryBase
{
public:
    typedef Product ProductType;
    virtual std::string getName() { return "unknown"; }
    virtual ProductType *getProcessor() = 0;
    virtual ~AlgoFactoryBase() {}
};

template<class Product, class ProductBase>
class AlgoFactory : public AlgoFactoryBase<ProductBase>
{    
public:
    AlgoFactory(const std::string &name) :
        name(name)
    {}

    std::string name;

    typedef Product ProductType;
    virtual std::string getName() { return name; }
    virtual ProductType *getProcessor() {return new Product; }
    virtual ~AlgoFactory() {}
};

typedef AlgoFactoryBase<Processor6D>   Processor6DFactory;
typedef AlgoFactoryBase<ProcessorFlow> ProcessorFlowFactory;



/* Singleton that holds factories */
template<class FactoryBaseType>
class AlgoFactoryHolder
{
private:
    AlgoFactoryHolder() {}
    static std::unique_ptr<AlgoFactoryHolder> sThis;

public:
    typedef typename FactoryBaseType::ProductType ProductType;

    ~AlgoFactoryHolder()
    {
        for (FactoryBaseType *provider : mProviders)
        {
           delete_safe(provider);
        }
        mProviders.clear();
    }


    static AlgoFactoryHolder* getInstance()
    {
        CORE_ASSERT_TRUE(sThis.get() != NULL, "Out of memory!");
        return sThis.get();
    }

    static ProductType *getProcessor(const std::string &name)
    {
        AlgoFactoryHolder *holder = getInstance();
        for (FactoryBaseType *provider : holder->mProviders)
        {
            if (provider->getName() == name) {
                return provider->getProcessor();
            }
        }
        return NULL;
    }


    static vector<std::string> getHints()
    {
        vector<std::string> hints;

        AlgoFactoryHolder *holder = getInstance();
        for (FactoryBaseType *provider : holder->mProviders)
        {
            hints.push_back(provider->getName());
        }
        return hints;
    }



    static void printCaps(std::ostream &out = std::cout)
    {
        out << "Processor6DFactoryHolder has providers:" << endl;
        AlgoFactoryHolder *holder = getInstance();
        for (FactoryBaseType *provider : holder->mProviders)
        {
            out << "   " << provider->getName() << endl;
        }
    }

    bool registerProcessor(FactoryBaseType * factory)
    {
        mProviders.push_back(factory);
        return true;
    }

    bool hasProvider(const std::string &name);


    vector<FactoryBaseType *> mProviders;

};

template<typename FactoryBaseType>
std::unique_ptr<AlgoFactoryHolder<FactoryBaseType> > AlgoFactoryHolder<FactoryBaseType>::sThis(new AlgoFactoryHolder<FactoryBaseType>);


typedef AlgoFactoryHolder<Processor6DFactory  > Processor6DFactoryHolder;
typedef AlgoFactoryHolder<ProcessorFlowFactory> ProcessorFlowFactoryHolder;


} // namespace

#endif /* PROCESSOR6D_H_ */


#include <iostream>

#include <core/buffers/bufferFactory.h>

#include "core/stereointerface/processor6D.h"

#include "core/utils/global.h"

#ifdef WITH_OPENCV
#include "KLTFlow.h"
#endif



#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif


using namespace std;
using namespace corecvs;

class DummyFlowProcessor : public Processor6D
{
    Statistics *stats = NULL;

    FlowBuffer *opticalFlow = NULL;

    RGB24Buffer *inPrev  = NULL;
    RGB24Buffer *inCurr  = NULL;




    virtual int beginFrame() {return 0;}

    /** Completly reset internal data structures. parameters are left intact **/
    virtual int reset()
    {
        inPrev = NULL;
        inCurr = NULL;
        delete_safe(opticalFlow);
        return 0;
    }


    /** Implemetation may allow you to reset some internal structures at will **/
    virtual int clean(int mask) {
        return 0;
    }

    /**
     * You are responisble to delete the frame sometime after endFrame() is called
     * Implemetation may (and will) store additinal views (pointers) to actual image data
     * for as long as implementation need them.
     *
     * see AbstractBuffer::createViewPtr<> for details on this mechanism
     *
     * To get consistent result don't modify frame data after the call to setFrame*
     **/
    virtual int setFrameG12  (FrameNames frameType, G12Buffer   *frame)
    {
        return 1;
    }

    virtual int setFrameRGB24(FrameNames frameType, RGB24Buffer *frame)
    {
        inCurr = frame;
    }

    virtual int setDisparityBufferS16(FrameNames frameType, FlowBuffer *frame)
    {
        return 1;

    }

    /** sets statistics data. Implementation should support stats == NULL **/
    virtual int setStats(Statistics *stats)
    {
        this->stats = stats;
    }



    virtual int endFrame()
    {

        if (inCurr != NULL && inPrev != NULL)
        {
            delete_safe(opticalFlow);
            opticalFlow = new FlowBuffer(inCurr->h, inCurr->w);
            for (int i = 0; i < inCurr->h; i+=10)
                for (int j = 0; j < inCurr->w; j+=10)
                {
                    opticalFlow->element(i,j) = FlowElement(6,6);
                }


        }
        inPrev = inCurr;
    }


    virtual std::map<std::string, DynamicObject> getParameters() { return std::map<std::string, DynamicObject>();}
    virtual bool setParameters(std::string name, const DynamicObject &param) {return true;}


    /** Oldstyle calls **/
    virtual int setParameteri(int parameterName, int parameterValue) {return 0;}
    virtual int requestResultsi(int parameterName) {return 0;}

    /**
     * Methods below return the pointers to the internal data structures that are only valid
     * untill next beginFrame() is called. If you want them to persist - make a copy
     **/

    /* This method computes flow form current frame to previous */
    virtual FlowBuffer *getFlow(){
       return opticalFlow;
    }


    virtual FlowBuffer *getStereo()
    {
        return NULL;
    }

    virtual CorrespondenceList *getFlowList()
    {
        return NULL;
    }


    virtual int getError(std::string *errorString) {return 0;}
};

class DummyProcessor6DFactory : public Processor6DFactory
{
public:
   virtual Processor6D *getProcessor() { return new DummyFlowProcessor(); }
   virtual std::string getName() {return "DummyFlowProcessor"; }
   virtual ~DummyProcessor6DFactory() {}
};


int main(int argc, char *argv[])
{


#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    LibjpegFileSaver::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    BufferFactory::printCaps();

#ifdef WITH_OPENCV
    Processor6DFactoryHolder::getInstance()->addFactory(new OpenCVProcessor6DFactory());
#endif
    Processor6DFactoryHolder::getInstance()->addFactory(new DummyProcessor6DFactory());
    Processor6DFactoryHolder::printCaps();

    if (argc != 5)
    {
        printf("You passed %d parameters\n", argc);
        printf("Usage    test_flowtest <provider> <in1> <in2> <out>\n");
        return 1;
      }


    std::string provider = argv[1];
    std::string in1name = argv[2];
    std::string in2name = argv[3];
    std::string outname = argv[4];

    SYNC_PRINT(("Will compute flow for <%s> <%s> with <%s> and put result to <%s>\n", in1name.c_str(), in2name.c_str(), provider.c_str(), outname.c_str() ));


    RGB24Buffer *in1 = BufferFactory::getInstance()->loadRGB24Bitmap(in1name);
    RGB24Buffer *in2 = BufferFactory::getInstance()->loadRGB24Bitmap(in2name);

    if (in1 == NULL || in2 == NULL)
    {
        printf("Unable to load\n");
        return 2;
    }


    Processor6D *proc = Processor6DFactoryHolder::getInstance()->getProcessor(provider); //new DummyFlowProcessor;
    if (proc == NULL) {
        printf("No such processor\n");
        return 3;
    }

    std::map<std::string, DynamicObject> params = proc->getParameters();
    cout << "Provider parameters" << endl;
    for (auto &it: params)
    {
        cout << it.first << endl;
        cout << it.second << endl;
    }

    proc->requestResultsi(Processor6D::RESULT_FLOW);
    proc->beginFrame();
    proc->setFrameRGB24(Processor6D::FRAME_LEFT_ID, in1);
    proc->endFrame();
    proc->beginFrame();
    proc->setFrameRGB24(Processor6D::FRAME_LEFT_ID, in2);
    proc->endFrame();
    FlowBuffer *flow = proc->getFlow();

    //RGB24Buffer result(flow->h, flow->w);
    RGB24Buffer result(in1);

    result.drawFlowBuffer3(flow);

    BufferFactory::getInstance()->saveRGB24Bitmap(&result, outname);

#if 0
    for (int i = 0; i < flow->h; i ++) {
        for (int j = 0; j < flow->w; j ++)
        {
            if (!flow->isElementKnown(i,j))
                continue;
            cout << j << "," << i << " => " << (j + flow->element(i,j).x()) << ", " << (i + flow->element(i,j).y()) << endl;
        }
    }
#endif


    delete_safe(proc);


    return 0;
}

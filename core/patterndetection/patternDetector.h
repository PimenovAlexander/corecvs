#ifndef PATTERNDETECTOR
#define PATTERNDETECTOR

#include "stats/calculationStats.h"
#include "utils/global.h"

#include "alignment/selectableGeometryFeatures.h"
#include "buffers/g8Buffer.h"
#include "buffers/rgb24/rgb24Buffer.h"


#include "filters/newstyle/newStyleBlock.h"
#include "utils/debuggableBlock.h"
#include "reflection/dynamicObject.h"

#include "xml/generated/patternDetectorResultBase.h"


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

class PatternDetectorResult : public ObservationList
{
public:
    /**
     * \brief id
     * pattern id
     */
    int mId;

#if 0

    Vector2dd position() const
    {
        return mPosition;
    }

    Vector2dd ortX() const
    {
        return mOrtX;
    }

    Vector2dd ortY() const
    {
        return mOrtY;
    }

    Vector2dd unityPoint() const
    {
        return mUnityPoint;
    }
#endif

    int id() const
    {
        return mId;
    }

    /** Section with setters */
    void setPosition(const Vector2dd& position)
    {
        add(Vector3dd(0,0,0) , position);
    }

    void setOrtX(const Vector2dd &ortX)
    {
        add(Vector3dd(1,0,0) , ortX);
    }

    void setOrtY(const Vector2dd &ortY)
    {
        add(Vector3dd(0,1,0) , ortY);
    }

    void setUnityPoint(const Vector2dd &unityPoint)
    {
        add(Vector3dd(1,1,0) , unityPoint);
    }

    void setId(int id)
    {
        mId = id;
    }

    friend std::ostream& operator << (std::ostream &out, PatternDetectorResult &toSave)
    {
        corecvs::PrinterVisitor printer(out);
        toSave.accept<corecvs::PrinterVisitor>(printer);
        return out;
    }

#if 0
    int getProjectionCornerId(int corner)
    {
        if (corner > 4) return 0;
        Vector2dd directions[4] = {
            Vector2dd(-1, -1),
            Vector2dd(-1,  1),
            Vector2dd( 1,  1),
            Vector2dd( 1, -1)
        };
        double v = -numeric_limits<double>::max();
        int best = 0;
        for (size_t i = 0; i < size(); i++)
        {
            double dp = operator[](i).projection & directions[corner];
            if (dp > v) {
                v = dp;
                best = i;
            }
        }
        return  best;
    }
#endif
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

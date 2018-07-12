/**
 * \file copterThread.cpp
 * \brief Implements a frame recording calculation thread based on BaseCalculationThread
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "copterThread.h"

#include <stdio.h>
#include <QMetaType>
#include <QMessageBox>

#include "g12Image.h"
#include "imageResultLayer.h"

#include <core/segmentation/segmentator.h>
// TEST
// #include "viFlowStatisticsDescriptor.h"

CopterThread::CopterThread() :
   BaseCalculationThread()
  , mFrameCount(0)
  , mCopterParameters(NULL)
{
    qRegisterMetaType<CopterThread::RecordingState>("CopterThread::RecordingState");
    mIdleTimer = PreciseTimer::currentTime();
}

class TileSegment : public BaseSegment<TileSegment>
{
public:
    /*TODO: These members should be moved to a sort of payload*/
    int size;

    TileSegment() :
          size(0)
    {}

    void dadify()
    {
        BaseSegment<TileSegment>::dadify();
        if (father != NULL && father != this)
        {
            father->size += this->size;
//            father->pointNum += this->pointNum;
        }
    }

    void addPoint(int /*i*/, int /*j*/, uint16_t &value)
    {
        this->size++;
    }


    ~TileSegment()
    {
    }

    static bool sortPredicate(TileSegment *a1, TileSegment *a2)
    {
        return a1->size >  a2->size;
    }

};

class BrightSegmentator : public Segmentator<BrightSegmentator, TileSegment>
{
public:
    uint16_t thres = 200*16;

    typedef Segmentator<BrightSegmentator, TileSegment>::SegmentationResult SegmentationResult;

    static int xZoneSize()
    {
        return 1;
    }

    static int yZoneSize()
    {
        return 1;
    }

    bool canStartSegment(int /*i*/, int /*j*/, const uint16_t &value)
    {
        return value > thres;
    }

};


AbstractOutputData* CopterThread::processNewData()
{
    Statistics stats;

//    qDebug("CopterThread::processNewData(): called");

#if 0
    stats.setTime(ViFlowStatisticsDescriptor::IDLE_TIME, mIdleTimer.usecsToNow());
#endif

    PreciseTimer start = PreciseTimer::currentTime();
//    PreciseTimer startEl = PreciseTimer::currentTime();

    bool have_params = !(mCopterParameters.isNull());
    bool two_frames = have_params && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber); // FIXME: additional params needed here

    // We are missing data, so pause calculation
    if ((!mFrames.getCurrentFrame(Frames::LEFT_FRAME) ) ||
       ((!mFrames.getCurrentFrame(Frames::RIGHT_FRAME)) && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber)))
    {
        emit errorMessage("Capture error.");
        pauseCalculation();
    }

    recalculateCache();

    G12Buffer *result[Frames::MAX_INPUTS_NUMBER] = {NULL, NULL};

    Vector2dd centerCl = Vector2dd::Zero();

    /*TODO: Logic here should be changed according to the host base change*/
    for (int id = 0; id < mActiveInputsNumber; id++)
    {
        G12Buffer   *buf    = mFrames.getCurrentFrame   ((Frames::FrameSourceId)id);
        RGB24Buffer *bufrgb = mFrames.getCurrentRgbFrame((Frames::FrameSourceId)id);
        if (bufrgb != NULL) {
            buf = bufrgb->toG12Buffer();
        }


        //result[id] = mTransformationCache[id] ? mTransformationCache[id]->doDeformation(mBaseParams->interpolationType(), buf) : buf;
        result[id] = buf;

        if (id == Frames::LEFT_FRAME)
        {
            /*
            for (int i = 0; i < buf->h; i++)
                for (int j = 0; j < buf->w; j++)
                {
                    if (buf->element(i,j) < (200 * 16))
                    {
                        buf->element(i,j) = 0;
                    }
                }*/
            BrightSegmentator seg;

            BrightSegmentator::SegmentationResult *result =
                    seg.segment(buf);

            for (int i = 0; i < buf->h; i++)
                for (int j = 0; j < buf->w; j++)
                {
                    if (result->markup->element(i,j) != 0)
                    {
                        buf->element(i,j) = 0;
                    }
                }

            int maxSize = 0;
            TileSegment *best = NULL;
            for (TileSegment *segment: *result->segments)
            {
                if (segment->size > maxSize)
                {
                     maxSize = segment->size;
                     best = segment;
                }
            }

            if (best != NULL)
            {
                for (int i = 0; i < buf->h; i++)
                {
                    for (int j = 0; j < buf->w; j++)
                    {
                        if (result->markup->element(i,j) == best)
                        {
                            centerCl += Vector2dd(j,i);
                        }
                    }
                }

                centerCl /= best->size;
                AbstractPainter<G12Buffer> p(buf);
                p.drawCircle(centerCl.x(), centerCl.y(), 5, 250 * 16);

            }

            delete_safe(result);

        }

    }

    CopterOutputData* outputData = new CopterOutputData();

    outputData->mMainImage.addLayer(
            new ImageResultLayer(
                    mPresentationParams->output(),
                    result
            )
    );

    outputData->mMainImage.setHeight(mBaseParams->h());
    outputData->mMainImage.setWidth (mBaseParams->w());

#if 1
    stats.setTime("Total time", start.usecsToNow());
#endif
    mIdleTimer = PreciseTimer::currentTime();

    for (int id = 0; id < mActiveInputsNumber; id++)
    {
        if (result[id] != mFrames.getCurrentFrame((Frames::FrameSourceId)id)) {
             delete_safe(result[id]);
        }
    }

    outputData->frameCount = this->mFrameCount;
    outputData->stats = stats;

    return outputData;
}

void CopterThread::copterControlParametersChanged(QSharedPointer<Copter> copterParameters)
{
    if (!copterParameters)
        return;

    mCopterParameters = copterParameters;
}

void CopterThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void CopterThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}

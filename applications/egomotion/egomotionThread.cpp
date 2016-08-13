/**
 * \file egomotionThread.cpp
 * \brief Implements a frame recording calculation thread based on BaseCalculationThread
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "egomotionThread.h"

#include <stdio.h>
#include <QMetaType>
#include <QMessageBox>

#include "g12Image.h"
#include "imageResultLayer.h"
// TEST
// #include "viFlowStatisticsDescriptor.h"
static G12Buffer* oldFrame;
static bool mark = false;
EgomotionThread::EgomotionThread() :
   BaseCalculationThread()
  , mFrameCount(0)
  , mRecorderParameters(NULL)
{
    qRegisterMetaType<EgomotionThread::RecordingState>("EgomotionThread::RecordingState");
    mIdleTimer = PreciseTimer::currentTime();
}


AbstractOutputData* EgomotionThread::processNewData()
{
    Statistics stats;

//    qDebug("EgomotionThread::processNewData(): called");

#if 0
    stats.setTime(ViFlowStatisticsDescriptor::IDLE_TIME, mIdleTimer.usecsToNow());
#endif

    PreciseTimer start = PreciseTimer::currentTime();
//    PreciseTimer startEl = PreciseTimer::currentTime();

    bool have_params = !(mRecorderParameters.isNull());
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

    /*TODO: Logic here should be changed according to the host base change*/
    for (int id = 0; id < mActiveInputsNumber; id++)
    {
        G12Buffer   *buf    = mFrames.getCurrentFrame   ((Frames::FrameSourceId)id);
        RGB24Buffer *bufrgb = mFrames.getCurrentRgbFrame((Frames::FrameSourceId)id);
        if (bufrgb != NULL) {
            buf = bufrgb->toG12Buffer();
        }
        if (mark){
            SpatialGradient *sg = new SpatialGradient(buf);

            KLTCalculationContext context;
            context.first  = buf;
            context.second = oldFrame;
            context.gradient = new SpatialGradientIntegralBuffer(sg);


            KLTGenerator<BilinearInterpolator> generator(Vector2d32(2,2), 5);
            Vector2dd guess(0,0);
            generator.kltIteration(context, Vector2d32(3,3), &guess, 2.0);

            Vector2dd guessSubpixel(0,0);
            generator.kltIterationSubpixel(context, Vector2dd(3,3), &guessSubpixel, 2.0);

            cout << "Result shift is " << guess.x() << ":" << guess.y() << "\n";
            cout << "Result shift is " << guessSubpixel.x() << ":" << guessSubpixel.y() << "\n";
            delete_safe(oldFrame);
        }
        oldFrame = new G12Buffer(buf);
        mark = true;
        //result[id] = mTransformationCache[id] ? mTransformationCache[id]->doDeformation(mBaseParams->interpolationType(), buf) : buf;
        result[id] = buf;


    }
#if 0
    stats.setTime(ViFlowStatisticsDescriptor::CORRECTON_TIME, startEl.usecsToNow());
#endif
    mFrameCount++;

    EgomotionOutputData* outputData = new EgomotionOutputData();

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


void EgomotionThread::egomotionControlParametersChanged(QSharedPointer<EgomotionParameters> egomotionParameters)
{
    if (!egomotionParameters)
        return;

    mRecorderParameters = egomotionParameters;
}

void EgomotionThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void EgomotionThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}

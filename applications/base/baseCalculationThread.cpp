/**
 * \file baseCalculationThread.cpp
 * \brief Implements a basic calculation thread that performs a simple transform on the captured
          frames and produces a QImage.
          Reimplement or extend this when designing specific host applications
 *
 * \date Aug 27, 2010
 */

#include <QtCore/QDebug>
#include <QtGui/QImage>

#include "global.h"

#include "g12Image.h"
#include "rotationPresets.h"
#include "baseCalculationThread.h"
#include "layers/imageResultLayer.h"
#include "inputFilter.h"
#include "outputFilter.h"

#ifdef WITH_HARDWARE
#include "../../hardware/platform/xparameters.h"
#endif

BaseCalculationThread::BaseCalculationThread()
    : AbstractCalculationThread()
    , mDistortionTransform(NULL)
    , mActiveInputsNumber(CamerasConfigParameters::TwoCapDev)
    , mBaseParams(NULL)
    , mCacheUpdateNeeded(true)
{
    qDebug() <<  "BaseCalculationThread::BaseCalculationThread() : hardware initialized";

    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++)
    {
        mFrameTransforms    [i] = Matrix33(1.0);
        mTransformationCache[i] = NULL;

        mTransformedBuffers[i]  = NULL;
        mFilterExecuter    [i]  = NULL;
#ifdef WITH_HARDWARE
        if (i >= 2) {
            qDebug() << "We have only 2 hardware correctors!" << endl;
            exit(1);
        }
        mHardwareCorrectors.push_back(new FpgaCamCorrector(XPAR_AXI_CAM_CORRECTOR_0_BASEADDR + i * 0x1000));
#endif
    }
    mProcessorGraph = new FilterGraph(&mFiltersCollection, NULL);
}


AbstractOutputData* BaseCalculationThread::processNewData()
{
    qDebug("BaseCalculationThread::processNewData(): called");
    BaseOutputData *resultData = new BaseOutputData();

    initData(resultData);

    return resultData;
}

void BaseCalculationThread::initData(BaseOutputData *calculationOutputData, Statistics *stats)
{
    recalculateCache();

    if (mBaseParams.isNull())
        return;

    // We are missing data, so pause calculation
    if ((!mFrames.getCurrentFrame(Frames::LEFT_FRAME) ) ||
       ((!mFrames.getCurrentFrame(Frames::RIGHT_FRAME)) && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber)))
    {
        pauseCalculation();
    }

    if (mBaseParams->enableFilterGraph()) {
        executeFilterGraph(stats);
    } else {
        transformInputFrames();
    }

    calculationOutputData->mMainImage.addLayer(
            new ImageResultLayer(
                    mPresentationParams->output(),
                    mTransformedBuffers
            )
    );

    calculationOutputData->mMainImage.setHeight(mBaseParams->h());
    calculationOutputData->mMainImage.setWidth (mBaseParams->w());
}

void BaseCalculationThread::executeFilterGraph(Statistics *stats)
{
    for (unsigned int i = 0; i < mProcessorGraph->inputs.size(); i++ )
    {
        InputFilter* input = dynamic_cast<InputFilter*>(mProcessorGraph->inputs[i]->parent);
        if (input->mInputParameters.inputType() == InputType::LEFT_FRAME) //FIXME change to Frames::LEFT_FRAME
            mProcessorGraph->inputs[i]->initPin(mFrames.getCurrentFrame(Frames::LEFT_FRAME));

        if (input->mInputParameters.inputType() == InputType::RIGHT_FRAME) //FIXME change to Frames::RIGHT_FRAME
            mProcessorGraph->inputs[i]->initPin(mFrames.getCurrentFrame(Frames::RIGHT_FRAME));
    }

    string oldPrefix;
    PreciseTimer timer = PreciseTimer::currentTime();
    if (stats != NULL)
    {
        oldPrefix = stats->prefix;
        stats->prefix += "Input Graph> ";
    }
    mProcessorGraph->stats = stats;
    mProcessorGraph->execute();
    if (stats != NULL)
    {
        stats->setTime("Total", timer.usecsToNow());
        stats->prefix = oldPrefix;
    }

    int currentOutputBlocks = Frames::DEFAULT_FRAME;
    for (unsigned int i = 0; i < mProcessorGraph->outputs.size(); i++ )
    {
        G12Buffer *outputFrame = NULL;

        OutputFilter* result = dynamic_cast<OutputFilter*>(mProcessorGraph->outputs[i]->parent);
        if (result->mOutputParameters.outputType() == OutputType::LEFT_FRAME)
        {   mProcessorGraph->outputs[i]->getPin(outputFrame);
            currentOutputBlocks = Frames::LEFT_FRAME;
        }
        else if (result->mOutputParameters.outputType() == OutputType::RIGHT_FRAME)
        {   mProcessorGraph->outputs[i]->getPin(outputFrame);
            currentOutputBlocks = Frames::RIGHT_FRAME;
        }

        if (outputFrame == NULL) {
            continue;
        }

        if (mDistortionTransform != NULL) {
            outputFrame = outputFrame->doReverseDeformationBl<G12Buffer, DisplacementBuffer>(
                mDistortionTransform.data(),
                outputFrame->h, outputFrame->w
            );
        }

        delete mTransformedBuffers[currentOutputBlocks];
        mTransformedBuffers[currentOutputBlocks] = mTransformationCache[currentOutputBlocks]->doDeformation(mBaseParams->interpolationType(), outputFrame);
    }

    mProcessorGraph->clearAllData();

    if (mTransformedBuffers[0] == NULL)
    {
//        cout<<"mTransformedBuffers[0] == NULL"<<endl;
        Frames::FrameSourceId frameId = (Frames::FrameSourceId)0;
        G12Buffer *inputFrame  = mFrames.getCurrentFrame(frameId);
        mTransformedBuffers[0] = mTransformationCache[0]->doDeformation(mBaseParams->interpolationType(), inputFrame);
    }
//    else
//        cout<<"mTransformedBuffers[0] != NULL"<<endl;

    if (mTransformedBuffers[1] == NULL)
    {
//        cout<<"mTransformedBuffers[1] == NULL"<<endl;
        Frames::FrameSourceId frameId = (Frames::FrameSourceId)1;
        G12Buffer *inputFrame  = mFrames.getCurrentFrame(frameId);
        mTransformedBuffers[1] = mTransformationCache[1]->doDeformation(mBaseParams->interpolationType(), inputFrame);
    }
//    else
//        cout<<"mTransformedBuffers[1] != NULL"<<endl;

}

void BaseCalculationThread::transformInputFrames()
{
    for (int i = 0; i < mActiveInputsNumber; i++)
    {
        Frames::FrameSourceId frameId = (Frames::FrameSourceId)i;
        G12Buffer *inputFrame     = mFrames.getCurrentFrame(frameId);
        G12Buffer *inputFrameOrig = inputFrame;

        if (mFilterExecuter[i] != NULL && !mFilterExecuter[i]->mFilters.empty())
        {
            G12Buffer *newInputFrame = mFilterExecuter[i]->filter(inputFrame);
            inputFrame = newInputFrame;
        }

        if (mDistortionTransform != NULL)
        {
            G12Buffer *newInputFrame = inputFrame->doReverseDeformationBl<G12Buffer, DisplacementBuffer>(
                mDistortionTransform.data(),
                inputFrame->h, inputFrame->w
            );
            if (inputFrame != inputFrameOrig) {
                delete inputFrame;
            }
            inputFrame = newInputFrame;
        }

        delete_safe (mTransformedBuffers[i]);
        mTransformedBuffers[i] = mTransformationCache[i]->doDeformation(mBaseParams->interpolationType(), inputFrame);


        if (inputFrame != inputFrameOrig)
        {
            delete_safe(inputFrame);
        }
    } // for (int i = 0; i < mInputsN; i++)
}

BaseCalculationThread::~BaseCalculationThread()
{
    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++)
    {
        delete_safe (mTransformationCache[i]);
        delete_safe (mTransformedBuffers[i]);
        delete_safe (mFilterExecuter[i]);
#ifdef WITH_HARDWARE
        delete_safe (mHardwareCorrectors[i]);
#endif
    }
    delete_safe(mProcessorGraph);
    mDistortionTransform.clear();
}

//---------------------------------------------------------------------------

void BaseCalculationThread::graphChanged(tinyxml2::XMLDocument* doc)
{
//    cout << "graphChanged slot here!" << endl;
    delete_safe(mProcessorGraph);
    mProcessorGraph = new FilterGraph(&mFiltersCollection, NULL);
    mProcessorGraph->deserialize(doc);
}

void BaseCalculationThread::filterControlParametersChanged(
        Frames::FrameSourceId id,
        QSharedPointer<FilterSelectorParameters> params)
{
    qDebug("BaseCalculationThread::filterControlParametersChanged(%d, _): Filter params received in BaseCalculationThread", id);
    /*if (id == Frames::LEFT_FRAME) {
        return;
    }*/

    params->printParams();
    delete mFilterExecuter[id];
    mFilterExecuter[id] = new FilterExecuter(*params);
}

void BaseCalculationThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    if (!params.isNull())
        mBaseParams = params;

    mCacheUpdateNeeded = true;
}

void BaseCalculationThread::presentationControlParametersChanged(QSharedPointer<PresentationParameters> params)
{
    if (!params.isNull())
        mPresentationParams = params;
}

void BaseCalculationThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parametersShPtr)
{
    mActiveInputsNumber = parametersShPtr->inputsN();
    mRectificationData  = parametersShPtr->rectifierData();
    mCacheUpdateNeeded  = true;
    if (parametersShPtr->distortionTransform() != NULL)
    {
        mDistortionTransform = parametersShPtr->distortionTransform();
    }
}


/**
 *  As the transformation procedure uses the inverse of the transformation matrix
 *  We first apply the rectification matrix, and then the scaling and shift.
 *  With inverses it is vice versa
 *
 *  We have a description of the math here - corecvs::StereoAligner::getAlignmentTransformation()
 *
 *
 *
 *  Let's have a look starting form camera input to rectification input image
 *
 *  1. CS = First we shift image by half to put it's center to [0,0]
 *  2. MI = Second we make mirror and rotation operation
 *  3. BH = Then we shift it back to where it was
 *  4. SH = We make a shift of the mirrored image
 *  5. SL = Afterwards we scale
 *
 *  Total transformation
 *     T1 = SL * SH * (BH * MI * CS)
 *
 *  as BH = CS^(-1)
 *     T1^(-1) = BH * MI * CS * SH * SL
 *
 *  T1       transforms from camera image to rectification input image
 *  T1^(-1)  transforms from rectification input image to camera image
 *
 *  following the operation from corecvs::StereoAligner we have a rectification transformation for
 *  camera images - H_left and H_right.
 *
 *  H_left       - from camera input to rectified image
 *  H_left^(-1)
 *
 *
 *
 *
 *
 **/
void BaseCalculationThread::recalculateCache()
{
    G12Buffer *firstInput = mFrames.getCurrentFrame(Frames::LEFT_FRAME);
    if (!mCacheUpdateNeeded || firstInput == NULL)
        return;

    if (mBaseParams->downsample() == 0)
        mBaseParams->setDownsample(1);

    if (mBaseParams->autoH())
        mBaseParams->setH(firstInput->h / mBaseParams->downsample());

    if (mBaseParams->autoW())
        mBaseParams->setW(firstInput->w / mBaseParams->downsample());

    /* Now we know the corrected params */
    double mDownsample = mBaseParams->downsample();
    int const x = mBaseParams->x();
    int const y = mBaseParams->y();
    int const h = mBaseParams->h();
    int const w = mBaseParams->w();


    Matrix33 centerShift     = Matrix33::ShiftProj(-firstInput->w / 2.0, -firstInput->h / 2.0);
    Matrix33 centerBackShift = Matrix33::ShiftProj( firstInput->w / 2.0,  firstInput->h / 2.0);

    Matrix33 centerAction(1.0);
    switch (mBaseParams->rotation())
    {
        /* Note that clockwise rotation is against the direction common to math notation */
        case RotationPresets::CLOCKWISE_90DEG :  centerAction = Matrix33::RotationZ270(); break;
        case RotationPresets::CLOCKWISE_180DEG:  centerAction = Matrix33::RotationZ180(); break;
        case RotationPresets::CLOCKWISE_270DEG:  centerAction = Matrix33::RotationZ90();  break;
        default:                                 centerAction = Matrix33(1.0);            break;
    }

    if (mBaseParams->mirror()) {
        centerAction = Matrix33::MirrorYZ() * centerAction;
    }


    mInputPretransform    = Matrix33::Scale2(1.0 / mDownsample) * Matrix33::ShiftProj(-x, -y) * centerBackShift * centerAction * centerShift;
    mInputPretransformInv = mInputPretransform.inv();

    mPretransformedRectificationData = mRectificationData.addPretransform(mInputPretransform);

    mFrameTransformsInv[Frames::RIGHT_FRAME] = mPretransformedRectificationData.rightTransform.inv();
    mFrameTransformsInv[Frames::LEFT_FRAME]  = mPretransformedRectificationData.leftTransform .inv();

    /*mFrameTransformsInv[Frames::RIGHT_FRAME] = Matrix33(1.0);
    mFrameTransformsInv[Frames::LEFT_FRAME] = Matrix33(1.0);*/

    for (int i = 0; i < mActiveInputsNumber; i++)
    {
        G12Buffer *currentBuffer = mFrames.getCurrentFrame((Frames::FrameSourceId)i);

        Q_ASSERT(currentBuffer != NULL);
        Q_ASSERT(currentBuffer->hasSameSize(firstInput));

        delete_safe(mTransformationCache[i]);
        mTransformationCache[i] = new TransformationCache(mFrameTransformsInv[i], w, h, currentBuffer->getSize());
#ifdef WITH_HARDWARE
        //Matrix33 mat = Matrix33::Scale2(1.08) * Matrix33::ShiftProj(-39.5, -39.5) * Matrix33::RotateProj(6.0 / 128.0);
        try {
            mHardwareCorrectors[i] -> configure(w, h,  mFrameTransformsInv[i]);//  mat); // mFrameTransformsInv[i]);
            mHardwareCorrectors[i] -> allocReadBuffers();
        } catch (Failure fail ) {
            qDebug() << fail.getReason().c_str();
        }
#endif

    }

    mCacheUpdateNeeded = false;
}

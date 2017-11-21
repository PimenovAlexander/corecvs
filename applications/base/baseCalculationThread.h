#pragma once
/**
 * \file baseCalculationThread.h
 * \see baseCalculationThread.cpp for function implementations
 *
 * \date Aug 27, 2010
 */

#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtGui/QImage>

#include "core/utils/global.h"

#include "core/math/matrix/matrix33.h"
#include "core/buffers/displacementBuffer.h"

#include "abstractCalculationThread.h"
#include "imageCaptureInterface.h"
#include "frames.h"
#include "generatedParameters/baseParameters.h"
#include "generatedParameters/presentationParameters.h"
#include "camerasConfigParameters.h"
#include "filters/filterSelector.h"
#include "filters/filterExecuter.h"
#include "core/filters/blocks/filterGraph.h"
#include "baseOutputData.h"
#include "core/buffers/transformationCache.h"

#ifdef WITH_HARDWARE
#include <vector>
#include "../../../restricted/wrappers/hardware/blocks/corrector/fpgaCamCorrector.h"
#endif


/**
 * A base implementation of calculation thread that grabs the
 * frames from a capture interface, applies a transform and passes
 * them to the host application
 *
 */

class BaseCalculationThread : public AbstractCalculationThread
{
    Q_OBJECT
public:
    BaseCalculationThread(int inputNumbers = CamerasConfigParameters::TwoCapDev);

    virtual ~BaseCalculationThread();

    void setLeftTransform(Matrix33 &leftTransform);

public slots:
    virtual void camerasParametersChanged            (QSharedPointer<CamerasConfigParameters>  params);
    virtual void baseControlParametersChanged        (QSharedPointer<BaseParameters>           params);
    virtual void presentationControlParametersChanged(QSharedPointer<PresentationParameters>   params);

    virtual void filterControlParametersChanged      (Frames::FrameSourceId id, QSharedPointer<FilterSelectorParameters> params);

    virtual void graphChanged(tinyxml2::XMLDocument*);

protected:
    /**
     * The cache of the transformation applied to the left and right image
     */
    TransformationCache *mTransformationCache[Frames::MAX_INPUTS_NUMBER];
#ifdef WITH_HARDWARE
    std::vector<FpgaCamCorrector*> mHardwareCorrectors;
#endif


    /**
     *   Transformed buffers
     **/
    G12Buffer *mTransformedBuffers[Frames::MAX_INPUTS_NUMBER];

    virtual AbstractOutputData *processNewData();


    void initData(BaseOutputData *data, Statistics *stats = NULL);
    void executeFilterGraph(Statistics *stats = NULL);
    void transformInputFrames();
    void recalculateCache();


    FilterExecuter      *mFilterExecuter [Frames::MAX_INPUTS_NUMBER];

    /* Original rectification data for camera images*/
    RectificationResult  mRectificationData;

    /* Rectification data from scaled image */
    RectificationResult  mPretransformedRectificationData;

    /* Scale and shift transformation */
    Matrix33             mInputPretransform;
    /* Scale and shift inverse transformation */
    Matrix33             mInputPretransformInv;

    /**
     * The transforms from original input image camera image to rectified image
     **/
    Matrix33             mFrameTransforms   [Frames::MAX_INPUTS_NUMBER];
    /**
     * Inverse of previous to use in interpolation of the buffer
     **/
    Matrix33             mFrameTransformsInv[Frames::MAX_INPUTS_NUMBER];

    /**
     * This is a lens precorrection
     **/
    QSharedPointer<DisplacementBuffer> mDistortionTransform;


    int mActiveInputsNumber;

    QSharedPointer<BaseParameters>         mBaseParams;
    QSharedPointer<PresentationParameters> mPresentationParams;

    /**
     *  For a thread to start calculating it needs first frame and the input parameters.
     *  When we get new params, we should recalculate cached data.
     **/

    bool mCacheUpdateNeeded;
    FilterGraph* mProcessorGraph;
};

/* EOF */

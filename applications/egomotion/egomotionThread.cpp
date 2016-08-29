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
#include <essentialEstimator.h>
#include <ransacEstimator.h>
#include <eulerAngles.h>

#include "g12Image.h"
#include "imageResultLayer.h"

#ifdef WITH_OPENCV
#include "KLTFlow.h"
#endif

// #include "viFlowStatisticsDescriptor.h"

EgomotionThread::EgomotionThread() :
   BaseCalculationThread()
  , mFrameCount(0)
  , mEgomotionParameters(NULL)
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

    bool have_params = !(mEgomotionParameters.isNull());
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


    EgomotionOutputData* outputData = new EgomotionOutputData();


    /*TODO: Logic here should be changed according to the host base change*/
    for (int id = 0; id < mActiveInputsNumber; id++)
    {
        G12Buffer   *buf    = mFrames.getCurrentFrame   ((Frames::FrameSourceId)id);
        RGB24Buffer *bufrgb = mFrames.getCurrentRgbFrame((Frames::FrameSourceId)id);
        if (bufrgb != NULL) {
            buf = bufrgb->toG12Buffer();
        }
        if (mark && oldFrame != NULL && buf != NULL){

#if 0
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
#endif
            /* Calculating flow*/
            PreciseTimer startEl = PreciseTimer::currentTime();

            FlowBuffer *flow = NULL;

            if (!mEgomotionParameters->useOpenCV())
            {
                KLTGenerator<BilinearInterpolator> generator;
                stats.startInterval();
                flow  = generator.calculateHierarchicalKLTFlow(oldFrame, buf);
                stats.endInterval("Without OpenCV Flow");
            } else {
#ifdef WITH_OPENCV
                stats.startInterval();

                outputData->debugOutput = new RGB24Buffer(buf->getSize());

                for (int i = 0; i < buf->h; i ++)
                {
                    for (int j = 0; j < buf->w; j ++)
                    {
                        //int mean = (buf->element(i,j) + oldFrame->element(i,j)) / 2.0;
                        //outputData->debugOutput->element(i,j) = RGBColor::gray(mean / 16);
                        outputData->debugOutput->element(i,j) =
                                RGBColor(
                                    buf->element(i,j) / 16,
                                    oldFrame->element(i,j) / 16,
                                    buf->element(i,j) / 16
                                    );

                    }
                }

                vector<FloatFlowVector> *flowVectors = KLTFlow::getOpenCVKLT(oldFrame, buf,
                                             mEgomotionParameters->selectorQuality(),
                                             mEgomotionParameters->selectorDistance(),
                                             mEgomotionParameters->selectorSize(),
                                             mEgomotionParameters->useHarris(),
                                             mEgomotionParameters->harrisK(),
                                             mEgomotionParameters->kltSize()
                                           );

                stats.resetInterval("Opencv Flow");
                flow = new FlowBuffer(buf->getSize());
                for (FloatFlowVector &v : *flowVectors)
                {
                    if (flow->isValidCoord(v.start.y(), v.start.x()) &&  flow->isValidCoord(v.end.y(), v.end.x()))
                    {
                        Vector2dd delta = v.end - v.start;
                        flow->element(v.start.y(), v.start.x()) = FlowElement(delta.x(), delta.y());

                        outputData->debugOutput->element(v.start.y(), v.start.x()) = RGBColor::Green();
                        outputData->debugOutput->element(v.end.y(), v.end.x()) = RGBColor::Magenta();


                    }

                }

                stats.resetInterval("Forming a buffer");

                double focal = mEgomotionParameters->cameraFocal();
                Vector2dd center(mEgomotionParameters->cameraPrincipalX(), mEgomotionParameters->cameraPrincipalX());

                /* Egomotion magic */
                vector<Correspondence> cv;
                for (FloatFlowVector &v : *flowVectors)
                {
                    Correspondence c;
                    c.start = (v.start - center) / focal;
                    c.end   = (v.end   - center) / focal;
                    cv.push_back(c);
                }


                std::vector<Correspondence*> cl;
                for (auto& cc: cv)
                    cl.push_back(&cc);

                EssentialMatrix E;
                RansacEstimator estimator;
                estimator.trySize = 9;
                estimator.maxIterations = 1000;
                estimator.treshold = 0.01;

                E = estimator.getEssentialRansac(&cl);
                stats.resetInterval("Essential estimation");
#if 0
                E.decompose( outputData->rot, outputData->trans);
                for (int var = 0; var < 4; var++)
                {
                    qDebug() << "Version " << var;
                    Quaternion q = Quaternion::FromMatrix( outputData->rot[var]);
                    q.printAxisAndAngle();
                }
#endif

                ////////////////////////////////////////////////////////////
                EssentialDecomposition decomposition[4];
                for (int i = 0; i < 4; i++)
                {
                    decomposition[i] = EssentialDecomposition(outputData->rot[i], outputData->trans[i]);
                }
                double cost[4];
                RectificationResult result;
                cout << "=============================================================" << endl;
                E.decompose(decomposition);
                int selected = 0;
                for (selected = 0; selected < 4; selected++)
                {
                     cout << "Decomposition " << selected << endl << decomposition[selected];

                    double d1;
                    double d2;
                    double err;
                    cost[selected] = 0.0;
                    EssentialDecomposition *dec = &(decomposition[selected]);
                    for (unsigned i = 0; i < cl.size(); i++)
                    {
                        dec->getScaler(*cl.at(i), d1, d2, err);
                        if (d1 > 0.0 && d2 > 0.0)
                            cost[selected]++;
                    }
                    cout << "decomposition cost:" << cost[selected] << endl << endl;
                }

                double maxCost = 0.0;
                int finalSelection = 0;
                for (selected = 0; selected < 4; selected++)
                {
                    if (cost[selected] > maxCost)
                    {
                        maxCost = cost[selected];
                        finalSelection = selected;
                    }
                }
                cout << endl << endl;

                result.decomposition = decomposition[finalSelection];
                Quaternion q = Quaternion::FromMatrix( result.decomposition.rotation);
                CameraAnglesLegacy angles = CameraAnglesLegacy::FromQuaternion(q);
                q.printAxisAndAngle();
                cout << "Chosen decomposition" << endl << result.decomposition << endl;
                cout << "pitch: " << angles.pitch() << " yaw: " << angles.yaw() << " roll: " << angles.roll() << endl;
                //////////////////////////////////////////////////////////////////////////
                ///
                outputData->result = angles;

                delete_safe(flowVectors);
                stats.endInterval("Essential decomposition");
#endif

            }

            stats.startInterval();
            if (flow != NULL && outputData->debugOutput == NULL)
            {
                outputData->debugOutput = new RGB24Buffer(flow->getSize());
                outputData->debugOutput->drawFlowBuffer3(flow);
            }
            stats.endInterval("Drawing the result on a debug canvas");

            delete_safe(flow);
            delete_safe(oldFrame);
        }

        delete_safe(oldFrame);
        oldFrame = new G12Buffer(buf);
        mark = true;
        //result[id] = mTransformationCache[id] ? mTransformationCache[id]->doDeformation(mBaseParams->interpolationType(), buf) : buf;
        result[id] = buf;


    }
#if 0
    stats.setTime(ViFlowStatisticsDescriptor::CORRECTON_TIME, startEl.usecsToNow());
#endif
    mFrameCount++;


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

    mEgomotionParameters = egomotionParameters;
}

void EgomotionThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void EgomotionThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}

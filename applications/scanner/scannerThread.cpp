/**
 * \file scannerThread.cpp
 * \brief Implements a frame recording calculation thread based on BaseCalculationThread
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "scannerThread.h"

#include <stdio.h>
#include <QMetaType>
#include <QMessageBox>

#include "g12Image.h"
#include "imageResultLayer.h"

#include "gentryState.h"
#include "calibrationHelpers.h"
#include "abstractPainter.h"

// TEST
// #include "viFlowStatisticsDescriptor.h"

ScannerThread::ScannerThread() :
   BaseCalculationThread()
  , mRecordingStarted(false)
  , mIsRecording(false)
  , mFrameCount(0)
  , mPath("")
  , mScannerParameters(NULL)
{
    qRegisterMetaType<ScannerThread::RecordingState>("ScannerThread::RecordingState");
    mIdleTimer = PreciseTimer::currentTime();
}

void ScannerThread::toggleRecording()
{
#if 0
    if (!mIsRecording)
    {
        if (mScannerParameters.isNull())
        {
            cout << "ScannerThread: Internal error. Recording toggled but no parameters provided." << endl;
        }

        if (mScannerParameters->path().empty())
        {
            cout << "ScannerThread: Path is empty" << endl;
        }

        if (mScannerParameters->fileTemplate().empty())
        {
            cout << "ScannerThread: File template is empty\n";
        }

        if (!mRecordingStarted)
        {
            mRecordingStarted = true;
            printf("Recording started.\n");
        }
        else
        {
            printf("Recording resumed.\n");
        }
        mIsRecording = true;
        emit recordingStateChanged(StateRecordingActive);
    }
    else
    {
        mIsRecording = false;
        printf("Recording paused.\n");
        emit recordingStateChanged(StateRecordingPaused);
    }
#endif

}

class ParallelConvolve
{
public:

    RGB24Buffer *in;
    G8Buffer   *out;
    bool useSSE = false;

    ParallelConvolve(RGB24Buffer *in, G8Buffer *out) :
        in(in), out(out)
    {}

    void operator ()(const BlockedRange<int>& range) const
    {
        int32_t koef[5] = {-5, -5, 0, 5, 5};
        int r = CORE_COUNT_OF(koef) / 2;


        for (int i = range.begin(); i != range.end(); i++)
        {
            RGBColor* lineStart = &in->element(i,0);
            uint8_t * outStart  = &out->element(i,0);

            int j = r;
#ifdef WITH_SSE
            for (; (j < in->w - r - 16) && useSSE; j+= 16)
            {
                Int16x8 sum0 = Int16x8::Zero();
                Int16x8 sum1 = Int16x8::Zero();

                RGBColor* pos = &(lineStart[-r]);

                for (int c = 0; c < CORE_COUNT_OF(koef); c++, pos++)
                {
                    FixedVector<Int16x8, 4> pixel0 = SSEReader8BBBB_DDDD::read((uint32_t *)pos);
                    FixedVector<Int16x8, 4> pixel1 = SSEReader8BBBB_DDDD::read((uint32_t *)(pos + 8));

                    Int16x8 br0 = pixel0[0] + pixel0[1] + pixel0[2] + Int16x8(1);
                    br0 = br0 >> 2;
                    Int16x8 br1 = pixel1[0] + pixel1[1] + pixel1[2] + Int16x8(1);
                    br1 = br1 >> 2;


                    sum0 += (br0 * Int16x8(koef[c]));
                    sum1 += (br1 * Int16x8(koef[c]));
                }

                sum0 += Int16x8(128*5);
                sum1 += Int16x8(128*5);

                UInt16x8 su0(sum0);
                UInt16x8 su1(sum1);

                su0 = SSEMath::div<5>(su0);
                su1 = SSEMath::div<5>(su1);

                UInt8x16 result = UInt8x16::packUnsigned(su0, su1);
                result.save((uint8_t *)outStart);
                outStart += 16;
                lineStart += 16;
            }
#endif
            for (; j < in->w - r; j++)
            {
                int32_t sum = 0;
                RGBColor* pos = &(lineStart[-r]);

                for (int c = 0; c < CORE_COUNT_OF(koef); c++, pos++)
                {
                     RGBColor &pixel = *pos;
                     uint16_t r = pixel.r();
                     uint16_t g = pixel.g();
                     uint16_t b = pixel.b();

                     uint16_t br = (r + g + b + 1) / 3;
                     sum += br * koef[c];
                }
                sum /= 5;
                sum += 128;
                *outStart = sum;
                outStart++;
                lineStart++;
            }
        }
    }

};


AbstractOutputData* ScannerThread::processNewData()
{
    Statistics stats;

//    qDebug("ScannerThread::processNewData(): called");

#if 0
    stats.setTime(ViFlowStatisticsDescriptor::IDLE_TIME, mIdleTimer.usecsToNow());
#endif

    PreciseTimer start = PreciseTimer::currentTime();
//    PreciseTimer startEl = PreciseTimer::currentTime();

    bool have_params = !(mScannerParameters.isNull());
    bool two_frames = have_params && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber); // FIXME: additional params needed here

    // We are missing data, so pause calculation
    if ((!mFrames.getCurrentFrame(Frames::LEFT_FRAME) ) ||
       ((!mFrames.getCurrentFrame(Frames::RIGHT_FRAME)) && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber)))
    {
        emit errorMessage("Capture error.");
        pauseCalculation();
    }

    recalculateCache();

    RGB24Buffer *bufrgb = mFrames.getCurrentRgbFrame(Frames::LEFT_FRAME);

    ScannerOutputData* outputData = new ScannerOutputData();

    if (bufrgb != NULL && !mScannerParameters.isNull())
    {
        RGB24Buffer red(bufrgb);
        outputData->brightness = new G8Buffer(red.getSize());
        AbstractPainter<G8Buffer> painter(outputData->brightness);
        painter.drawCircle(100,100, 70, 50);


        vector<double> tops;
        tops.reserve(red.w);

        if (mScannerParameters->algo() == RedRemovalType::BRIGHTNESS)
        {
            stats.startInterval();

            ParallelConvolve par(&red, outputData->brightness);
            par.useSSE = mScannerParameters->useSSE();
            parallelable_for(0, red.h, par, mScannerParameters->useSSE());



            stats.endInterval("Removing red");
        } else {
            /*for (int i = 0; i < red.h; i++)
            {
                for (int j = 0; j < red.w; j++)
                {
                    RGBColor &pixel = red.element(i,j);
                    if (pixel.hue() <= mScannerParameters->redThreshold())
                        pixel = RGBColor::Black();
                }
            }*/

        }


        if (mScannerParameters->algo() != RedRemovalType::DUMMY)
        {
            for (int j = 0; j < red.w; j++)
            {
                for (int i = 0; i < red.h; i++)
                {
                    RGBColor &pixel = red.element(i,j);
                    if (pixel.r() > mScannerParameters->redThreshold())
                    {
                        tops.push_back(i);
                        pixel = RGBColor::Cyan();
                        break;
                    }
                }
            }
        } else {
            for (int j = 0; j < red.w; j++)
            {
                tops.push_back( sin(j / 100.0) * 500 + red.h / 2 );
            }
        }

        outputData->cut.reserve(red.h);
        for (int i = 0; i < red.h; i++)
        {
            RGBColor &pixel = red.element(i,red.w/2);
            outputData->cut.push_back(pixel.r());
        }



        GentryState state;
        Vector2dd resolution = Vector2dd(bufrgb->w, bufrgb->h);

        state.camera.intrinsics = PinholeCameraIntrinsics(resolution,  degToRad(60));
        state.camera.setLocation(
                    Affine3DQ::Shift(0,0,0) *
                    Affine3DQ::RotationX(degToRad(90.0))
                    );
        state.laserPlane = Plane3d::FormNormalAndPoint(
                    Vector3dd(0,0,1),
                    Vector3dd(0,0,-30)
                    );

        outputData->outputMesh.switchColor();

        static int framecount=0;
        static Mesh3D model;

        framecount++;

        if (framecount > 200) {
            framecount = 0;
            model.clear();
        }


        for (size_t i = 0; i < tops.size(); i++)
        {
            Vector2dd pixel(i, tops[i]);
            Ray3d ray = state.camera.rayFromPixel(pixel);

            outputData->outputMesh.setColor(RGBColor::Green());
            //outputData->outputMesh.addLine(ray.p, ray.getPoint(60.0));

            bool hasIntersection = false;
            Vector3dd point = state.laserPlane.intersectWith(ray, &hasIntersection);

            if (!hasIntersection || !state.camera.isInFront(point))
            {
                continue;
            }

            outputData->outputMesh.setColor(RGBColor::Yellow());
            outputData->outputMesh.addPoint(point);

            model.addPoint(point + Vector3dd(0, 0, framecount * 2.0));

        }
        outputData->outputMesh.setColor(RGBColor::Magenta());
        outputData->outputMesh.add(model);

        CalibrationHelpers drawer;
        drawer.drawCamera(outputData->outputMesh, state.camera, 1.0);

        Circle3d pd;
        pd.c = Vector3dd::Zero();
        pd.normal = state.laserPlane.normal();
        pd.r = 70;
        outputData->outputMesh.addCircle(pd);

        outputData->mMainImage.addLayer(
                new ImageResultLayer(
                        &red
                )
        );
    }

    outputData->mMainImage.setHeight(mBaseParams->h());
    outputData->mMainImage.setWidth (mBaseParams->w());

#if 1
    stats.setTime("Total time", start.usecsToNow());
#endif
    mIdleTimer = PreciseTimer::currentTime();

    for (int id = 0; id < mActiveInputsNumber; id++)
    {

    }

    outputData->frameCount = this->mFrameCount;
    outputData->stats = stats;

    return outputData;
}

void ScannerThread::resetRecording()
{
    mIsRecording = false;
    mRecordingStarted = false;
    mPath = "";
    mFrameCount = 0;
    emit recordingStateChanged(StateRecordingReset);
}

void ScannerThread::scannerControlParametersChanged(QSharedPointer<ScannerParameters> scannerParameters)
{
    if (!scannerParameters)
        return;

    mScannerParameters = scannerParameters;
   // mPath = QString(scannerParameters->path().c_str()) + "/" + QString(scannerParameters->fileTemplate().c_str());
}

void ScannerThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void ScannerThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}

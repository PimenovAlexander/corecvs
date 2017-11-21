/**
 * \file main_test_snooker.cpp
 * \brief This is the main file for the test snooker 
 *
 * \date Apr 08, 2011
 * \author alexander
 *
 * \ingroup
 */

#include <iostream>
#include <fstream>

#include "core/utils/global.h"

//#include "ballDescriptor.h"
//#include "detector.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/math/mathUtils.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/projectiveTransform.h"
#include "core/geometry/polygons.h"
#include "core/segmentation/segmentator.h"
#include "core/buffers/g8Buffer.h"
#include "core/geometry/ellipticalApproximation.h"
#include "commonTypes.h"
//#include "OpenCVTools.h"


#include "snookerSegmentator.h"
#include "reflectionSegmentator.h"

using namespace std;

extern int color_rgb[][3];


class Ball
{
public:
    int state;
    Vector2dd measureCenter;
    Vector2dd measureReflection;
    Vector2dd predictedCenter;
};



int getColorFromHueAndSat(int hue, int sat)
{
    if (sat < 30 && (hue < 45 || hue > 320)) return BALL_WHITE;
    if (hue < 45 || hue >  320)   return BALL_RED;

    if (hue > 180 && hue < 187)  return BALL_BLUE;
    if (hue > 170 && hue < 180)  return BALL_GREEN;
    if (hue > 45  && hue < 55)   return BALL_BROWN;
    if (hue > 55  && hue < 65)   return BALL_YELLOW;
    if (hue < 5 )                return BALL_WHITE;
    if (hue > 145  && hue < 155) return BALL_BLACK;

    return BALL_UNKNOWN;
}


Vector2dd hueToVector (int hue)
{
    double rads = (double)hue / 180.0 * M_PI;
    return Vector2dd(cos(rads), sin(rads));
}

int vectorToHue (Vector2dd input)
{
    double rads = atan2(input.y(), input.x());
    int deg = (int)(rads / M_PI * 180.0);
    if (deg < 0) deg += 360;
    return deg;
}

Vector2dd vectorFromPos(const Position &input)
{
  return Vector2dd(input.x, input.y);
}

int detectBalls (RGB24Buffer *work, const DetectionParameters &params, BallsDescriptor *result)
{

  if (work == NULL || result == NULL) {
    return ERR_NULL_INPUT_PARAMETER;
  }


  Vector2dd tableSize(params.settings.snookerTable.length, params.settings.snookerTable.width);


  const CalibrationInfo * calibration = &params.calibration;
  Vector2dd points[4] = {
    vectorFromPos(calibration->VertexA),
    vectorFromPos(calibration->VertexB),
    vectorFromPos(calibration->VertexC),
    vectorFromPos(calibration->VertexD)
  };


    Vector2dd images[4] = {
            Vector2dd(0,0),
            Vector2dd(tableSize.x(),0),
            Vector2dd(tableSize.x(),tableSize.y()),
            Vector2dd(0,tableSize.y())
    };


    Poligon tablePoligon(points, 4);
    Matrix33 imageToTableMatrix = Matrix33::ProjectiveByPoints(points, images);
    ProjectiveTransform tableToImage(imageToTableMatrix.inv());

    G8Buffer    *ballsMask    = new G8Buffer(work->getSize());

    RGB24Buffer *debugSegmentation = NULL;
    if (params.produceDebug)
      debugSegmentation = new RGB24Buffer(work->getSize());

    for (int i = 0; i < work->h; i++)
    {
        for (int j = 0; j < work->w; j++)
        {
            RGBColor pixel = work->element(i,j);
            uint16_t hue        = pixel.hue();
            uint16_t luma       = pixel.y();

            int isTableColor = (
              hue  > params.settings.parameters.tableHue - params.settings.parameters.tableHueInterval &&
              hue  < params.settings.parameters.tableHue + params.settings.parameters.tableHueInterval &&
              luma > params.settings.parameters.tableLumaMin &&
              luma < params.settings.parameters.tableLumaMax
            );
            int isTablePos   = tablePoligon.isInside(Vector2dd(j,i));

            if (isTablePos && !isTableColor)
            {
                ballsMask->element(i,j) = 0xFF;
                if (params.produceDebug)
                  debugSegmentation->element(i,j) = RGBColor::gray(0xFF);
            }
        }
    }

    SnookerSegmentator segmentator(0x7F);
    SnookerSegmentator::SegmentationResult *balls;

    balls = segmentator.segment<G8Buffer>(ballsMask);


    /* Segments are now finalized. And won't be merged. We can create payloads now */
    for (unsigned k = 0; k < balls->segments->size(); k++)
    {
        SnookerSegment *segment = balls->segments->at(k);
        segment->payload = new SnookerPayload();
    }

   /* One more path to fill payloads with information */
    AbstractBuffer<SnookerSegment *> *markup = balls->markup;
    for (int i = 0; i < markup->h; i++)
    {
       for (int j = 0; j < markup->w; j++)
       {
           SnookerSegment *seg = markup->element(i,j);
           if (seg == NULL)
               continue;

           SnookerPayload *payload = seg->payload;
           payload->approx.addPoint(j,i);
           payload->hyst->inc(work->element(i,j).brightness());

           if (i < payload->yMin) payload->yMin = i;
           if (j < payload->xMin) payload->xMin = j;
           if (j > payload->xMax) payload->xMax = j;
       }
    }

    /**
     *  Once again - per segment operation
     *  Compute the threshold for the reflection
     *
     * */
    for (unsigned k = 0; k < balls->segments->size(); k++)
    {
       SnookerPayload *payload = balls->segments->at(k)->payload;

       payload->highPercentile = payload->hyst->getHigherPersentile(params.settings.parameters.reflectionPercentile / 100.0);
       if (payload->highPercentile < params.settings.parameters.reflectionLumaMin )  
           payload->highPercentile = params.settings.parameters.reflectionLumaMin;
    }

    /* One more path highlight the reflections */

    G8Buffer *reflectMask = new G8Buffer(work->getSize());
    for (int i = 0; i < markup->h; i++)
    {
       for (int j = 0; j < markup->w; j++)
       {
           SnookerSegment *seg = markup->element(i,j);
           if (seg == NULL)
               continue;

           if (seg->payload->approx.getSize() < params.settings.parameters.minimumBallClusterSize)
               continue;

           if (work->element(i,j).brightness() >= seg->payload->highPercentile)
           {
//                       work->element(i, j) = RGBColor(255,0,0);
                reflectMask->element(i, j) = 0xFF;
                if (params.produceDebug)
                  debugSegmentation->element(i,j) = RGBColor(0xFF,0,0);
           }

       }
    }


    /*======================================= Stage 2 ========================================  */

    ReflectionSegmentator::SegmentationResult *reflections;
    ReflectionSegmentator reflectionSegmentator(0x7F);
    reflections = reflectionSegmentator.segment<G8Buffer>(reflectMask);


    /* Segments are now finalized. And won't be merged. We can create payloads now */
    for (unsigned k = 0; k < reflections->segments->size(); k++)
    {
        ReflectionSegment *segment = reflections->segments->at(k);
        segment->payload = new ReflectionPayload();
    }

    AbstractBuffer<ReflectionSegment *> *markupStage2 = reflections->markup;
    for (int i = 0; i < markupStage2->h; i++)
    {
       for (int j = 0; j < markupStage2->w; j++)
       {
           ReflectionSegment *seg = markupStage2->element(i,j);
           if (seg == NULL)
               continue;

           int hue = work->element(i,j).hue();
           int sat = work->element(i,j).saturation();
           ReflectionPayload *payload = seg->payload;
           payload->approx.addPoint(j,i);
           payload->hue += hueToVector(hue);
           payload->saturation += sat;

           SnookerSegment *ballSegment = markup->element(i,j);
           if (payload->owner == NULL && ballSegment != NULL)
           {
             payload->owner = ballSegment->payload;
           }
       }
    }
    /**
     *   Now invert reference links
     **/
    for (unsigned k = 0; k < reflections->segments->size(); k++)
    {
        ReflectionSegment *seg = reflections->segments->at(k);
        ReflectionPayload *payload = seg->payload;
        SnookerPayload *owner = payload->owner;
        owner->reflections.push_back(payload);
    }


    /**
     *  Compute actual ball center
     *
     **/
    for (unsigned k = 0; k < balls->segments->size(); k++)
    {
        SnookerPayload *payload = balls->segments->at(k)->payload;
        if (payload->approx.getSize() < params.settings.parameters.minimumBallClusterSize)
            continue;

       Vector2dd mean = payload->approx.getMean();

       int radius = (payload->xMax - payload->xMin) / 2;
       Vector2dd axis = payload->approx.getEllipseAxis();

       cout << "Segment " << k << " rad" << radius << " axis " << axis << endl;
       if (radius < 30 &&
           axis.x() > 7 && axis.x() < 40 &&
           axis.y() > 7 && axis.y() < 40)
       {
           Vector2dd center(payload->xMin + radius, payload->yMin + radius);
           payload->hasSingleCenter = true;
           payload->ballCenter = center;
       }

    }


    RGB24Buffer *cleanDetection = new RGB24Buffer(work->getSize());
    cleanDetection->fillWith(RGBColor(64,64,64));


     
    FILE* logtrace = NULL;    

    if (params.produceDebug)
      logtrace = fopen("balls.txt", "wt");

    result->number = 0;

    for (unsigned k = 0; k < reflections->segments->size(); k++)
    {
        ReflectionPayload *payload = reflections->segments->at(k)->payload;
        Vector2dd mean = payload->approx.getMean();
        Vector2dd hueVec = payload->hue / payload->approx.count;
        int hue = vectorToHue(hueVec);
        int sat = payload->saturation / payload->approx.count;
        int color = getColorFromHueAndSat(hue, sat);

        RGBColor rgbColor(color_rgb[color][0], color_rgb[color][1], color_rgb[color][2]);
        cleanDetection->drawCrosshare2(fround(mean.x()), fround(mean.y()), rgbColor);
        Vector2dd projected = imageToTableMatrix * mean;


        Vector2dd ballCenter;

        int siblingsNumber = payload->owner->reflections.size();
    
        int hasMeasuredCenter = (siblingsNumber == 1 && payload->owner->hasSingleCenter);

        /* Pity, I'm an orphan */
        if (siblingsNumber == 1 && payload->owner->hasSingleCenter)
        {
          ballCenter = payload->owner->ballCenter;
        } else {
          /* Do Correction Here*/
          ballCenter = mean;
        }
        
        unsigned maxAllowed = params.maxBalls;
        if (result->number < maxAllowed)
        {
          BallDescriptor ball;

          ball.speckPosition.x  = mean.x();
          ball.speckPosition.y  = mean.y();

          ball.ballPosition.x   = ballCenter.x();
          ball.ballPosition.y   = ballCenter.y();

          ball.isCenterMesuared = hasMeasuredCenter;

          ball.tableBallPosition.x = projected.x();
          ball.tableBallPosition.y = projected.y();
          ball.color = color;

          result->ballDescriptor[result->number] = ball;

          result->number++;        
        }               

        if (params.produceDebug)
        {
          fprintf(logtrace, "Reflection %2d at [%4d x %4d] % 7s (h=%3d sat=%3d) sibs=%d", 
            k, (int)(mean.x()), (int)(mean.y()), color_names[color], hue, sat, siblingsNumber);

          if (hasMeasuredCenter)
              fprintf(logtrace, "%15s [%2.5lf x %2.5lf]", "measured", ballCenter.x(), ballCenter.y());
          else
              fprintf(logtrace, "%15s", "no center");

          fprintf(logtrace, "\n");
        }
    }

    if (params.produceDebug)
    {
      fclose(logtrace);
  
      BMPLoader().save("segmentation.bmp", debugSegmentation);
      BMPLoader().save("table-mark.bmp", work);
      BMPLoader().save("detection.bmp",cleanDetection);
    }

    cout << "Done" << endl;

    delete ballsMask;
    delete reflectMask;
    delete balls;
    delete reflections;
    delete cleanDetection;
    delete debugSegmentation;

        return 0;
}

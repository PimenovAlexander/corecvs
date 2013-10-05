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
#include "global.h"
#include "rgb24Buffer.h"
#include "bmpLoader.h"
#include "mathUtils.h"
#include "matrix33.h"
#include "projectiveTransform.h"
#include "polygons.h"
#include "segmentator.h"
#include "g8Buffer.h"
#include "ellipticalApproximation.h"


#include "snookerSegmentator.h"
#include "reflectionSegmentator.h"

#include "commonTypes.h"

using namespace std;

Vector2dd tableSize(3556.0, 1778.0);


class Ball
{
public:
    int state;
    Vector2dd measureCenter;
    Vector2dd measureReflection;
    Vector2dd predictedCenter;
};




void drawTable (ProjectiveTransform &transform, RGB24Buffer *surface)
{
    enum {
        CORNER_1,
        CORNER_2,
        CORNER_3,
        CORNER_4,
        MAX
    };

    Vector2dd images[MAX] = {
            Vector2dd(0,0),
            Vector2dd(tableSize.x(),0),
            Vector2dd(tableSize.x(),tableSize.y()),
            Vector2dd(0,tableSize.y())
      };

    Vector2dd points[MAX];

    for (int i = 0; i < MAX; i++)
        points[i] = transform * images[i];

    RGBColor color = RGBColor(0xFF,0xFF,0xFF);

    surface->drawLine(
            points[CORNER_1].x(), points[CORNER_1].y(), points[CORNER_2].x(), points[CORNER_2].y(), color);
    surface->drawLine(
            points[CORNER_2].x(), points[CORNER_2].y(), points[CORNER_3].x(), points[CORNER_3].y(), color);
    surface->drawLine(
            points[CORNER_3].x(), points[CORNER_3].y(), points[CORNER_4].x(), points[CORNER_4].y(), color);
    surface->drawLine(
            points[CORNER_4].x(), points[CORNER_4].y(), points[CORNER_1].x(), points[CORNER_1].y(), color);

    /* Draw Pockets*/
    Vector2dd pocketImages[6] = {
                images[CORNER_1],
                images[CORNER_2],
                images[CORNER_3],
                images[CORNER_4],
                (images[CORNER_1] + images[CORNER_2]) / 2.0,
                (images[CORNER_3] + images[CORNER_4]) / 2.0,
          };

    for (int i = 0; i < 6; i++)
    {
        RGBColor color = RGBColor(0xFF,0x00,0xFF);

        double poketSize = 60.0;
    //      Vector2d center = transform * pocketImages[i];
          Vector2dd up     = transform * Vector2dd(pocketImages[i] + Vector2dd(       0.0, poketSize));
          Vector2dd down   = transform * Vector2dd(pocketImages[i] + Vector2dd(       0.0,-poketSize));
          Vector2dd left   = transform * Vector2dd(pocketImages[i] + Vector2dd(-poketSize,  0.0));
          Vector2dd right  = transform * Vector2dd(pocketImages[i] + Vector2dd( poketSize,  0.0));
          surface->drawLine(   up.x(),    up.y(), right.x(), right.y(), color);
          surface->drawLine(right.x(), right.y(),  down.x(),  down.y(), color);
          surface->drawLine( down.x(),  down.y(),  left.x(),  left.y(), color);
          surface->drawLine( left.x(),  left.y(),    up.x(),    up.y(), color);
    }


}


static const int MINIMUM_BALL_CLUSTER_SIZE = 25;
static const int REFLECTION_PERCENTILE = 12;
static const int REFLECTION_LUMA_MIN = 201;

static const int TABLE_HUE          = 120;
static const int TABLE_HUE_INTERVAL =  25;
static const int TABLE_LUMA_MIN     =  20;
static const int TABLE_LUMA_MAX     = 200;

const char *color_names[] =
{
    "WHITE",
    "RED",
    "YELLOW",
    "GREEN",
    "BROWN",
    "BLUE",
    "PINK",
    "BLACK"
    "UNKNOWN"
};

const RGBColor cleanColor[] =
{
   RGBColor(255,255,255),
   RGBColor(255,0,0),
   RGBColor(255,255,0),
   RGBColor(0,255,0),
   RGBColor(255,110,0),
   RGBColor(0,0,255),
   RGBColor(255,128,128),
   RGBColor(0,0,0),
   RGBColor(128,128,128)
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
    int deg = rads / M_PI * 180.0;
    if (deg < 0) deg += 360;
    return deg;
}

int main (int argC, char **argV)
{

    Vector2dd points[4] = {
            Vector2dd( 418.0, 274.0),
            Vector2dd(1194.0, 402.0),
            Vector2dd( 959.0, 634.0),
            Vector2dd(  70.0, 349.0)
    };


    Vector2dd images[4] = {
            Vector2dd(0,0),
            Vector2dd(tableSize.x(),0),
            Vector2dd(tableSize.x(),tableSize.y()),
            Vector2dd(0,tableSize.y())
    };

   printf("Arguments %d\n", argC);
   if (argC != 2)
   {
     printf("Please provide the image to process\n");
     return -1;
   }

        RGB24Buffer *input  = BMPLoader().loadRGB(argV[1]);
//        BMPLoader().save("input.bmp", input);

    Poligon tablePoligon(points, 4);

    //input = input->


    RGB24Buffer *work  = new RGB24Buffer(input);

    Matrix33 imageToTableMatrix = Matrix33::ProjectiveByPoints(points, images);
    ProjectiveTransform tableToImage(imageToTableMatrix.inv());
//    drawTable (tableToImage, work);

//    RGB24Buffer *table = work->doReverseDeformation<RGB24Buffer, ProjectiveTransform>(tableToImage, tableSize.y(), tableSize.x());

//    RGB24Buffer *threshold = new RGB24Buffer(work->getSize());
//    RGB24Buffer *huebuf       = new RGB24Buffer(work->getSize());
//    RGB24Buffer *satbuf       = new RGB24Buffer(work->getSize());
//    RGB24Buffer *valbuf       = new RGB24Buffer(work->getSize());

//    RGB24Buffer *isTableMask = new RGB24Buffer(work->getSize());
//    RGB24Buffer *noTableMask = new RGB24Buffer(work->getSize());


    RGB24Buffer *balls        = new RGB24Buffer(work->getSize());
    G8Buffer    *ballsMask    = new G8Buffer(work->getSize());

    for (int i = 0; i < work->h; i++)
    {
        for (int j = 0; j < work->w; j++)
        {
            RGBColor pixel = input->element(i,j);
//            if (pixel.brightness() > 180)
//                threshold->element(i,j) = pixel;

            uint16_t hue        = pixel.hue();
//            uint16_t saturation = pixel.saturation();
//            uint16_t value      = pixel.value();
            uint16_t luma       = pixel.y();

            int isTableColor = (
                    hue  > TABLE_HUE - TABLE_HUE_INTERVAL &&
                    hue  < TABLE_HUE + TABLE_HUE_INTERVAL &&
                    luma > TABLE_LUMA_MIN &&
                    luma < TABLE_LUMA_MAX
                );
            int isTablePos   = tablePoligon.isInside(Vector2dd(j,i));

/*            if (isTableColor)
            {
                balls->element(i,j) = RGBColor(128,128,128);
            } else
            {
                balls->element(i,j) = pixel;
            }


            if (isTablePos)
            {
                isTableMask->element(i,j) = pixel;
            } else {
                noTableMask->element(i,j) = pixel;
            }*/

            if (isTablePos && !isTableColor)
            {
                ballsMask->element(i,j) = 0xFF;
            }

//            uint8_t scaledHue = hue * 255 / 360;
//            huebuf->element(i,j) = RGBColor::gray(scaledHue);
//            satbuf->element(i,j) = RGBColor::gray(saturation);
//            valbuf->element(i,j) = RGBColor::gray(value);
        }
    }

    SnookerSegmentator segmentator(0x7F);
    SnookerSegmentator::SegmentationResult *result;
    result = segmentator.segment<G8Buffer>(ballsMask);


    /* Segments are now finalized. And won't be merged. We can create payloads now */
    for (unsigned k = 0; k < result->segments->size(); k++)
    {
        SnookerSegment *segment = result->segments->at(k);
        segment->payload = new SnookerPayload();
    }

//    RGB24Buffer *segmentDebug = new RGB24Buffer(work->getSize());

    /* One more path to fill payloads with information */
    AbstractBuffer<SnookerSegment *> *markup = result->markup;
    for (int i = 0; i < markup->h; i++)
    {
       for (int j = 0; j < markup->w; j++)
       {
           SnookerSegment *seg = markup->element(i,j);
           if (seg == NULL)
               continue;

           SnookerPayload *payload = seg->payload;
//           segmentDebug->element(i, j) = payload->color;
           payload->approx.addPoint(j,i);
           payload->hyst->inc(input->element(i,j).brightness());

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
    for (unsigned k = 0; k < result->segments->size(); k++)
    {
       SnookerSegment *seg = result->segments->at(k);
       seg->payload->highPercentile = seg->payload->hyst->getHigherPersentile(REFLECTION_PERCENTILE / 100.0);
       if (seg->payload->highPercentile < REFLECTION_LUMA_MIN )  seg->payload->highPercentile  = REFLECTION_LUMA_MIN;
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

           if (seg->payload->approx.getSize() < MINIMUM_BALL_CLUSTER_SIZE)
               continue;

           if (input->element(i,j).brightness() >= seg->payload->highPercentile)
           {
//               segmentDebug->element(i, j) = RGBColor(255,0,0);
                       work->element(i, j) = RGBColor(255,0,0);
                reflectMask->element(i, j) = 0xFF;
           }

       }
    }


    /*======================================= Stage 2 ========================================  */

    ReflectionSegmentator::SegmentationResult *resultStage2;
    ReflectionSegmentator reflectionSegmentator(0x7F);
    resultStage2 = reflectionSegmentator.segment<G8Buffer>(reflectMask);


    /* Segments are now finalized. And won't be merged. We can create payloads now */
    for (unsigned k = 0; k < resultStage2->segments->size(); k++)
    {
        ReflectionSegment *segment = resultStage2->segments->at(k);
        segment->payload = new ReflectionPayload();
    }

    AbstractBuffer<ReflectionSegment *> *markupStage2 = resultStage2->markup;
    for (int i = 0; i < markupStage2->h; i++)
    {
       for (int j = 0; j < markupStage2->w; j++)
       {
           ReflectionSegment *seg = markupStage2->element(i,j);
           if (seg == NULL)
               continue;

           ReflectionPayload *payload = seg->payload;
           payload->approx.addPoint(j,i);
           payload->hue += hueToVector(input->element(i,j).hue());
           payload->saturation += input->element(i,j).saturation();
       }
    }

    /**
     *  Draw ball center and other info
     *
     **/
    for (unsigned k = 0; k < result->segments->size(); k++)
    {
        SnookerPayload *payload = result->segments->at(k)->payload;
        if (payload->approx.getSize() < MINIMUM_BALL_CLUSTER_SIZE)
            continue;

       Vector2dd mean = payload->approx.getMean();
       work        ->drawCrosshare2(fround(mean.x()), fround(mean.y()), RGBColor(0xFFFFFF));
//       segmentDebug->drawCrosshare2(fround(mean.x()), fround(mean.y()), RGBColor(0xFFFFFF));

       int radius = (payload->xMax - payload->xMin) / 2;
       Vector2dd axis = payload->approx.getEllipseAxis();

       cout << "Segment " << k << " rad" << radius << " axis " << axis << endl;
       if (radius < 30 &&
           axis.x() > 7 && axis.x() < 40 &&
           axis.y() > 7 && axis.y() < 40)
       {
           Vector2dd center(payload->xMin + radius, payload->yMin + radius);
           work        ->drawCrosshare3(fround(center.x()), fround(center.y()), RGBColor(0x00FF00));
//           segmentDebug->drawCrosshare3(fround(center.x()), fround(center.y()), RGBColor(0x00FF00));
       }

//       segmentDebug->drawFormat(mean.x() + 10, mean.y() + 10, RGBColor(0x00FF00), "%d", k);
    }

    RGB24Buffer *cleanDetection = new RGB24Buffer(work->getSize());
    cleanDetection->fillWith(RGBColor(64,64,64));

    for (unsigned k = 0; k < resultStage2->segments->size(); k++)
    {
        ReflectionPayload *payload = resultStage2->segments->at(k)->payload;
        Vector2dd mean = payload->approx.getMean();
        cleanDetection->drawCrosshare2(fround(mean.x()), fround(mean.y()), RGBColor(0xFF0000));
        Vector2dd hueVec = payload->hue / (double)(payload->approx.count);
        int hue = vectorToHue(hueVec);
        int sat = payload->saturation / payload->approx.count;
        cout << "Reflection " << k << " at "<< Vector2d32(mean) << " hue " << hue << " sat " << sat << endl;
//        segmentDebug->drawFormat(mean.x() + 10, mean.y() + 10, RGBColor(0xFFFF00), "%d %d", hue, sat);

        int color = getColorFromHueAndSat(hue, sat);

        cleanDetection->drawCrosshare2(fround(mean.x()), fround(mean.y()), cleanColor[color]);
        Vector2dd projected = imageToTableMatrix * mean;
//        table->drawCircle(fround(projected.x()), fround(projected.y()), 30, cleanColor[color]);
//        table->drawCircle(fround(projected.x()) + 5, fround(projected.y()) + 5, 7, RGBColor(255,255,255));
//        table->drawCrosshare1(fround(projected.x()), fround(projected.y()), cleanColor[color]);

    }

//    BMPLoader().save("segment.bmp", segmentDebug);

//    BMPLoader().save("threshold.bmp", threshold);
//    BMPLoader().save("hue.bmp", huebuf);
//    BMPLoader().save("sat.bmp", satbuf);
//    BMPLoader().save("val.bmp", valbuf);

//    BMPLoader().save("is-table-mask.bmp", isTableMask);
//    BMPLoader().save("no-table-mask.bmp", noTableMask);

    BMPLoader().save("balls.bmp", balls);
    BMPLoader().save("table-mark.bmp", work);
    BMPLoader().save("detection.bmp",cleanDetection);
//    BMPLoader().save("table.bmp", table);
    cout << "Done" << endl;


//    delete segmentDebug;
//    delete threshold;
//    delete huebuf;
//    delete satbuf;
//    delete valbuf;
//    delete isTableMask;
//    delete noTableMask;
    delete balls;
    delete work;
    delete cleanDetection;
//    delete table;

        return 0;
}

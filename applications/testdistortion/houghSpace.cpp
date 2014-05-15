#include "houghSpace.h"
#include <QDebug>

HoughSpace::HoughSpace(G12Buffer *image = 0)
{
    originalImage = new G12Buffer(image);

    int w = originalImage->getW();
    int h = originalImage->getH();
    double minDist = - sqrt(w * w + h * h);
    maxDist =   sqrt(w * w + h * h);

    numberOfDistances = (int)((maxDist - minDist) / scale);
    double distanceBias = -minDist;

    accumulator = new AbstractBuffer<double>(numberOfAngles,numberOfDistances);

    for (int angleCounter = 0; angleCounter < numberOfAngles; angleCounter++)
    {
        double angle = (M_PI / numberOfAngles)*angleCounter;
        Vector2dd normal(cos(angle), sin(angle));
        for (int x = 0; x < h; x++)
        {
            for (int y = 0; y < w; y++)
            {
                double distance = Vector2dd(x, y) & normal;
                distance += distanceBias;
                distance /= scale;

                if (image->element(x,y)>=G12Buffer::BUFFER_MAX_VALUE / 2)
                {
                    accumulator->element(angleCounter,(int)distance) += 1;
                }
            }
        }
    }
}

G12Buffer* HoughSpace::getHoughImage(bool withCorrection = false)
{
    G12Buffer* renderOfHoughTransform = new G12Buffer(numberOfAngles, numberOfDistances);

    AbstractBuffer<double> tempAccumulator(numberOfAngles, numberOfDistances);

    double maxValue = 0;
    double couf = 1;
    for (int i = 0; i < numberOfAngles; i++)
    {
        for (int j = 0; j < numberOfDistances; j++)
        {
            if (i==250 && j==615)
            {
                qDebug()<<i<<" "<<j;
            }
            if (withCorrection)
            {
                QLine* line = getLineOfPoint(new QPoint(j, i));
                couf = sqrt(line->dx()*line->dx()+line->dy()*line->dy()) / (double)numberOfDistances;
                if (couf == 0)
                {
                    continue;
                }
            }

            tempAccumulator.element(i,j) = accumulator->element(i,j) / couf;
            maxValue = max(maxValue, tempAccumulator.element(i,j));
        }
    }

    for (int i = 0; i < numberOfAngles; i++)
    {
        for (int j = 0; j < numberOfDistances; j++)
        {
            renderOfHoughTransform->element(i,j) = tempAccumulator.element(i,j) / maxValue * G12Buffer::BUFFER_MAX_VALUE ;
        }
    }
    return renderOfHoughTransform;
}

QLine* HoughSpace::getLineOfPoint(QPoint *point)
{
    int w = originalImage->getW();
    int h = originalImage->getH();
    int distance = point->x() * scale - maxDist;
    double angle = (M_PI / numberOfAngles)*point->y();

    if (sin(angle) == 0)
    {
        return new QLine(0, distance, w, distance);
    }
    double topX = distance / cos(angle);
    double topY;
    if (topX > h)
    {
        topY = max((double)0, min((double)w, (topX - h) / tan(angle)));
        topX = h;
    }
    else if (topX < 0)
    {
        topY = max((double)0, min(topX / tan(angle), (double)w));
        topX = 0;
    }
    else
    {
        topY = 0;
    }

    double lowerX = distance * cos(angle) - (w-distance*sin(angle)) * tan(angle);
    double lowerY;
    if (lowerX < 0)
    {
        lowerY = max((double)0, min((double)w, w + lowerX / tan(angle)));
        lowerX = 0;
    }
    else if (lowerX > h)
    {
        lowerY = max((double)0, min(w + (lowerX - h) / tan(angle), (double)w));
        lowerX = h;
    }
    else
    {
        lowerY = w;
    }
    return new QLine(topY,topX,lowerY,lowerX);
}

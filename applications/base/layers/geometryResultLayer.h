#ifndef GEOMETRYRESULTLAYER_H
#define GEOMETRYRESULTLAYER_H

#include "resultImage.h"

class GeometryResultLayer : public ResultLayerBase
{

public:
    static const ResultLayerType LAYER_CLASS_ID = ResultLayerBase::LAYER_GEOMETRY;

public:
    QList<QRect> rects;

    GeometryResultLayer() : ResultLayerBase(LAYER_CLASS_ID)
    {
    }

    GeometryResultLayer(QRect rect) : ResultLayerBase(LAYER_CLASS_ID)
    {
        rects.append(rect);
    }


    virtual void drawImage(QImage * /*image*/);
    virtual void print() const;

};

#endif // GEOMETRYRESULTLAYER_H

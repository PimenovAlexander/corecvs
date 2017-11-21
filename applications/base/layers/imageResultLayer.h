#pragma once
/**
 * \file imageResultLayer.h
 *
 * \date Oct 17, 2012
 **/
#include <QtGui/QImage>
#include <QPainter>
#include <QDebug>

#include "core/utils/global.h"

#include "resultImage.h"
#include "outputStyle.h"
#include "frames.h"
#include "g12Image.h"
#include "painterHelpers.h"



class ImageResultLayer : public ResultLayerBase
{

    OutputStyle::OutputStyle mStyle;
    QImage* mImages[Frames::MAX_INPUTS_NUMBER];

public:
    static const ResultLayerType LAYER_CLASS_ID = ResultLayerBase::LAYER_IMAGE;


template<class BufferType>
    ImageResultLayer(
        OutputStyle::OutputStyle style,
        BufferType* images[Frames::MAX_INPUTS_NUMBER]
    );

/*    ImageResultLayer(
        OutputStyle::OutputStyle style,
        RGB24Buffer* images[Frames::MAX_INPUTS_NUMBER],
        bool showLeftFrame = false
    );*/

    ImageResultLayer(G8Buffer* image);

    ImageResultLayer(G12Buffer* image);

    ImageResultLayer(RGB24Buffer* image);


    /** Polymorphic type so virtual destructor needed */
    virtual ~ImageResultLayer();

    virtual void drawImage (QImage *image);

    virtual int  modifyHeight (int height)
    {
    	return height;
    }

    virtual int  modifyWidth (int width)
    {
        if (mStyle == OutputStyle::ALL) {
            return Frames::MAX_INPUTS_NUMBER * width;
        } else if (mStyle == OutputStyle::SIDEBYSIDE_STEREO) {
            return 2 * width;
        } else {
            return width;
        }
    }

    QImage* getImage(const int id) const {
        return mImages[id];
    }

    virtual void print() const {
        for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++)
        {
            printf("Image %d: ", i);
            if (mImages[i] == NULL) {
                printf(" NULL\n");
                continue;
            }
            printf(" [%dx%d]\n", mImages[i]->height(), mImages[i]->width());
        }
    }
};


template<class BufferType>
ImageResultLayer::ImageResultLayer(
        OutputStyle::OutputStyle style,
        BufferType* images[Frames::MAX_INPUTS_NUMBER]
)
    : ResultLayerBase(LAYER_CLASS_ID)
    , mStyle(style)
{
    //qDebug() << "ImageResultLayer::ImageResultLayer():"
    //<< (images[0] ? "non null" : "null")   << ", " << (images[1] ? "non null" : "null");

    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        mImages[id] = NULL;
    }

    switch (mStyle)
    {
        case OutputStyle::OUTPUT_STYLE_LAST:
        case OutputStyle::NONE:
            break;

        case OutputStyle::ANAGLYPH_RC:
        case OutputStyle::ANAGLYPH_RG:
        case OutputStyle::SIDEBYSIDE_STEREO:        
        case OutputStyle::BLEND:
            for (int id = 0; id <= Frames::RIGHT_FRAME; id++ )
            {
                if (images[id] == NULL) {
                    continue;
                }
                mImages[id] = toQImage(images[id]);
            }
            break;
        case OutputStyle::ALL:
            for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
            {
                if (images[id] == NULL) {
                    continue;
                }
                mImages[id] = toQImage(images[id]);
            }
            break;

        case OutputStyle::LEFT_FRAME:
            if (images[Frames::LEFT_FRAME] != NULL)
            {
                mImages[Frames::LEFT_FRAME] = toQImage(images[Frames::LEFT_FRAME]);
            }
            break;
        case OutputStyle::RIGHT_FRAME:
            if (images[Frames::RIGHT_FRAME] != NULL)
            {
                mImages[Frames::RIGHT_FRAME] = toQImage(images[Frames::RIGHT_FRAME]);
            }
            break;

    }
}

#if 0
/* Instaniate two specifications of the template*/
template
ImageResultLayer::ImageResultLayer<corecvs::G12Buffer>(
        OutputStyle::OutputStyle style,
        corecvs::G12Buffer* images[Frames::MAX_INPUTS_NUMBER],
        bool showLeftFrame);

template
ImageResultLayer::ImageResultLayer<corecvs::RGB24Buffer>(
        OutputStyle::OutputStyle style,
        corecvs::RGB24Buffer* images[Frames::MAX_INPUTS_NUMBER],
        bool showLeftFrame);
#endif

/* EOF */

#pragma once
/**
 * \file imageResultLayer.h
 *
 * \date Oct 17, 2012
 **/
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include "global.h"

#include "resultImage.h"
#include "outputStyle.h"
#include "frames.h"
#include "g12Image.h"
#include "painterHelpers.h"



class ImageResultLayer : public ResultLayerBase
{

    OutputStyle::OutputStyle mStyle;
    QImage* mImages[Frames::MAX_INPUTS_NUMBER];
    bool mShowLeftFrame;

public:
    ImageResultLayer(
        OutputStyle::OutputStyle style,
        G12Buffer* images[Frames::MAX_INPUTS_NUMBER],
        bool showLeftFrame = false
    );

    ImageResultLayer(
        G12Buffer* image
    );

    ImageResultLayer(
        RGB24Buffer* image
    );


    /** Polymorphic type so virtual destructor needed */
    virtual ~ImageResultLayer();

    virtual void drawImage (QImage *image);

    virtual int  modifyHeight (int height)
    {
    	return height;
    }

    virtual int  modifyWidth (int width)
    {
        if (mStyle == OutputStyle::SIDEBYSIDE_STEREO) {
            return 2 * width;
        } else {
            return width;
        }
    }

    QImage* getImage(const int id) const {
        return mImages[id];
    }
};


/* EOF */

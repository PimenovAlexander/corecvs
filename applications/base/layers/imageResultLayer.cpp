/*
 * imageResultLayer.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: alexander
 */

#include "imageResultLayer.h"



ImageResultLayer::ImageResultLayer(G8Buffer* image) :
    ResultLayerBase(LAYER_CLASS_ID)
  , mStyle(OutputStyle::RIGHT_FRAME)
{
    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        mImages[id] = NULL;
    }

    if (image == NULL) {
        qDebug("ImageResultLayer::ImageResultLayer( G12Buffer* NULL) : Called with null input");
        return;
    }

    mImages[Frames::RIGHT_FRAME] = new G8Image(image);
}

ImageResultLayer::ImageResultLayer(
    G12Buffer* image
) : ResultLayerBase(LAYER_CLASS_ID)
, mStyle(OutputStyle::RIGHT_FRAME)
{
    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        mImages[id] = NULL;
    }

    if (image == NULL) {
        qDebug("ImageResultLayer::ImageResultLayer( G12Buffer* NULL) : Called with null input");
        return;
    }

    mImages[Frames::RIGHT_FRAME] = new G12Image(image);
}

ImageResultLayer::ImageResultLayer(
    RGB24Buffer* image
) : ResultLayerBase(LAYER_CLASS_ID)
, mStyle(OutputStyle::RIGHT_FRAME)
{
    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        mImages[id] = NULL;
    }

    if (image == NULL) {
        qDebug("ImageResultLayer::ImageResultLayer( RGB24Buffer* NULL) : Called with null input");
        return;
    }

    mImages[Frames::RIGHT_FRAME] = new RGB24Image(image);
}


ImageResultLayer::~ImageResultLayer()
{
    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        delete_safe(mImages[id]);
    }
}

void ImageResultLayer::drawImage (QImage *image)
{
//    qDebug("ImageResultLayer::drawImage (QImage *): called");

    if (image == NULL) {
        return;
    }

    QPainter painter(image);
    if (mImages[Frames::LEFT_FRAME] == NULL && mImages[Frames::RIGHT_FRAME] == NULL)
    {
        image->fill(Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(image->rect(), "No image to draw");
    }

//    qDebug("ImageResultLayer::drawImage (): style: %s", OutputStyle::getName(mStyle));
    switch (mStyle)
    {
        case OutputStyle::OUTPUT_STYLE_LAST:
        case OutputStyle::NONE:
            return;
            break;

        case OutputStyle::ANAGLYPH_RC:
            if (mImages[Frames::LEFT_FRAME] != NULL && mImages[Frames::RIGHT_FRAME] != NULL)
                uniteAnaglyph(mImages[Frames::LEFT_FRAME], mImages[Frames::RIGHT_FRAME], image, 0xFFFF0000, 0xFF00FFFF);
            break;
        case OutputStyle::ANAGLYPH_RG:
            if (mImages[Frames::LEFT_FRAME] != NULL && mImages[Frames::RIGHT_FRAME] != NULL)
                uniteAnaglyph(mImages[Frames::LEFT_FRAME], mImages[Frames::RIGHT_FRAME], image);
            break;
        case OutputStyle::SIDEBYSIDE_STEREO:
            {
                //int lFrameId = mShowLeftFrame ? Frames::LEFT_FRAME  : Frames::RIGHT_FRAME;
                //int rFrameId = mShowLeftFrame ? Frames::RIGHT_FRAME : Frames::LEFT_FRAME;
                int lFrameId =  Frames::RIGHT_FRAME;
                int rFrameId =  Frames::LEFT_FRAME;

                if (mImages[lFrameId] != NULL)
                    painter.drawImage(QPoint(mImages[lFrameId]->width(),0), *(mImages[lFrameId]));
                if (mImages[rFrameId] != NULL)
                    painter.drawImage(QPoint(                         0,0), *(mImages[rFrameId]));
            }
            break;
        case OutputStyle::BLEND:
            if (mImages[Frames::LEFT_FRAME] != NULL && mImages[Frames::RIGHT_FRAME] != NULL)
            {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(QPoint(0,0), *(mImages[Frames::RIGHT_FRAME]));
                painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                painter.fillRect(image->rect(), QColor(0, 0, 0, 128));
                painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
                painter.drawImage(QPoint(0,0), *(mImages[Frames::LEFT_FRAME]));
            }
            break;

        case OutputStyle::LEFT_FRAME:
        case OutputStyle::RIGHT_FRAME:
            {
                Frames::FrameSourceId id = (mStyle == OutputStyle::LEFT_FRAME) ? Frames::LEFT_FRAME : Frames::RIGHT_FRAME;

                if (mImages[id] != NULL) {
                    painter.drawImage(QPoint(0,0), *(mImages[id]));
                } else {
                    qDebug("ImageResultLayer::drawImage (): %d (%s) image is NULL", id, Frames::getEnumName(id));
                }
            }
            break;
        case OutputStyle::ALL:
            {
                for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++)
                {
                    if (mImages[i] != NULL)
                        painter.drawImage(QPoint(mImages[i]->width() * i, 0), *(mImages[i]));
                }
            }
            break;
    }

}

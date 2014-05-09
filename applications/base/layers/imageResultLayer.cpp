/*
 * imageResultLayer.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: alexander
 */

#include "imageResultLayer.h"


ImageResultLayer::ImageResultLayer(
    G12Buffer* image
) : ResultLayerBase(LAYER_CLASS_ID)
, mStyle(OutputStyle::STANDART_OUTPUT)
, mShowLeftFrame(false)
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
, mStyle(OutputStyle::STANDART_OUTPUT)
, mShowLeftFrame(false)
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
                int lFrameId = mShowLeftFrame ? Frames::LEFT_FRAME  : Frames::RIGHT_FRAME;
                int rFrameId = mShowLeftFrame ? Frames::RIGHT_FRAME : Frames::LEFT_FRAME;

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

        case OutputStyle::STANDART_OUTPUT:
            int selectedFrameId = mShowLeftFrame ? Frames::LEFT_FRAME : Frames::RIGHT_FRAME;
            if (mImages[selectedFrameId] != NULL) {
                painter.drawImage(QPoint(0,0), *(mImages[selectedFrameId]));
            } else {
                qDebug("ImageResultLayer::drawImage (): %d image is NULL", selectedFrameId);
            }
            break;
    }

}

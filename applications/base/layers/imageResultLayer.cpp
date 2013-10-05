/*
 * imageResultLayer.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: alexander
 */

#include "imageResultLayer.h"

ImageResultLayer::ImageResultLayer(
        OutputStyle::OutputStyle style,
        G12Buffer* images[Frames::MAX_INPUTS_NUMBER],
        bool showLeftFrame
)
: ResultLayerBase(ResultLayerBase::LAYER_IMAGE)
, mStyle(style)
, mShowLeftFrame(showLeftFrame)
{
    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        mImages[id] = NULL;
    }

    int selectedFrameId = showLeftFrame ? Frames::LEFT_FRAME : Frames::RIGHT_FRAME;

    switch (mStyle)
    {
        case OutputStyle::OUTPUT_STYLE_LAST:
        case OutputStyle::NONE:
            break;

        case OutputStyle::ANAGLYPH_RC:
        case OutputStyle::ANAGLYPH_RG:
        case OutputStyle::SIDEBYSIDE_STEREO:
        case OutputStyle::BLEND:
            for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
            {
                if (images[id] == NULL) {
                    continue;
                }
                mImages[id] = new G12Image(images[id]);
            }
            break;

        case OutputStyle::STANDART_OUTPUT:
            if (images[selectedFrameId] != NULL)
            {
                mImages[selectedFrameId] = new G12Image(images[selectedFrameId]);
            }
            break;
    }
}

ImageResultLayer::ImageResultLayer(
    G12Buffer* image
) : ResultLayerBase(ResultLayerBase::LAYER_IMAGE)
, mStyle(OutputStyle::STANDART_OUTPUT)
, mShowLeftFrame(false)
{
    for (int id = 0; id < Frames::MAX_INPUTS_NUMBER; id++ )
    {
        mImages[id] = NULL;
    }

    mImages[Frames::RIGHT_FRAME] = new G12Image(image);
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
    if (image == NULL)
        return;

    QPainter painter(image);

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
            if (mImages[selectedFrameId] != NULL)
                painter.drawImage(QPoint(0,0), *(mImages[selectedFrameId]));
            break;
    }

}

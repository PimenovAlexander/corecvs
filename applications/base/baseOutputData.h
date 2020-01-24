#pragma once
/*
 * \file baseOutputData.h
 *
 *  Created on: Sep 25, 2012
 */
#include <QtGui/QImage>

#include "core/framesources/frames.h"
#include "abstractCalculationThread.h"
#include "layers/resultImage.h"


class BaseOutputData : public AbstractOutputData
{
public:
    ResultImage mMainImage;
};




/* EOF */

/*
 * resultImage.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: alexander
 */

#include "global.h"
#include "resultImage.h"

const char* ResultLayerBase::LAYER_TYPE_NAMES[] =
{
    "LAYER_IMAGE",     // LAYER_IMAGE,
    "LAYER_FLOW",      // LAYER_FLOW,
    "LAYER_STEREO",    // LAYER_STEREO,
    "LAYER_TILES",     // LAYER_TILES,
    "LAYER_CLUSTERS",  // LAYER_CLUSTERS,
    "LAYER_OBJECT",    // LAYER_OBJECT,
    "LAYER_PARAMS",    // LAYER_PARAMS
    "LAYER_GEOMETRY",  // LAYER_GEOMETRY,
    "LAYER_LAST"       // LAYER_LAST
};


STATIC_ASSERT (CORE_COUNT_OF(ResultLayerBase::LAYER_TYPE_NAMES) ==
               ResultLayerBase::LAYER_LAST + 1, wrong_state_number);

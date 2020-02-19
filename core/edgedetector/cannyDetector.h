#pragma once

/**
 * \file cannyDetector.h
 *
 * \date Oct 19, 2013
 **/

#if 0
#include "buffers/g12Buffer.h"
#include "xml/generated/cannyParameters.h"

using corecvs::G12Buffer;


class CannyDetector
{
    static void recursiveEdgeProver(G12Buffer *buffer, int h, int w);
    static G12Buffer *doFilter(G12Buffer *buffer, const CannyParameters &mCannyParameters);
};

#endif
/* EOF */

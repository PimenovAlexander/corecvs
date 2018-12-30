/**
 * \file main_test_draw.cpp
 * \brief This is the main file for the test draw
 *
 * \date Apr 19, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#ifndef BSPRENDERTEST_H
#define BSPRENDERTEST_H

#include <iostream>
#include <random>
#include <fstream>

#include "core/geometry/renderer/attributedTriangleSpanIterator.h"

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/cameraModel.h"

#include "core/geometry/polygonPointIterator.h"
#include "core/buffers/bufferFactory.h"

using namespace corecvs;

#endif // BSPRENDERTEST_H

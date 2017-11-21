/**
 * \file main_test_deform.cpp
 * \brief This is the main file for the test deform
 *
 * \date Mar 01, 2012
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/fixeddisp/fixedPointDisplace.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/projectiveTransform.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/alignment/radialCorrection.h"
#include "core/buffers/displacementBuffer.h"

#include "core/utils/preciseTimer.h"

using corecvs::Matrix33;
using corecvs::ProjectiveTransform;
using corecvs::G12Buffer;
using corecvs::BufferFactory;
using corecvs::FixedPointDisplace;
using corecvs::BMPLoader;
using corecvs::RGB24Buffer;

void testRadialApplication(int scale);
void testRadialInversion(int scale);

TEST(Deform, profileRadialApplication)  // TODO: move to perf-tests!
{
    testRadialApplication(10);
}

TEST(Deform, profileRadialInversion)  // TODO: move to perf-tests!
{
    testRadialInversion(10);
}

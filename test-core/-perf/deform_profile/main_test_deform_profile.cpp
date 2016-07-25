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

#include "global.h"

#include "fixedPointDisplace.h"
#include "matrix33.h"
#include "projectiveTransform.h"
#include "g12Buffer.h"
#include "bufferFactory.h"
#include "bmpLoader.h"
#include "rgb24Buffer.h"
#include "radialCorrection.h"
#include "displacementBuffer.h"

#include "preciseTimer.h"

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

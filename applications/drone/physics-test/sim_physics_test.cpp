#include "gtest/gtest.h"

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/buffers/bufferFactory.h"
#include "core/filesystem/folderScanner.h"


#ifdef WITH_AVCODEC
#include "avEncoder.h"

#include <core/cameracalibration/projection/pinholeCameraIntrinsics.h>

#include <core/buffers/bufferFactory.h>
#endif

#include "radio/frSkyMultimodule.h"

#include <bitset>
#include "radio/multimoduleController.h"

#include "core/fileformats/meshLoader.h"
#include "copter/quad.h"


using namespace corecvs;


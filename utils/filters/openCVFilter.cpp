#include <QtGlobal>

#include "openCVFilter.h"
#include "core/buffers/g12Buffer.h"

#ifdef WITH_OPENCV
# include "opencv2/imgproc/imgproc.hpp"
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/core/core_c.h"
# include "openCVTools.h"
#endif

OpenCVFilter::OpenCVFilter()
{
}


//G12Buffer *OpenCVFilter::filter(G12Buffer *input)
int OpenCVFilter::operator()()
{
#ifdef WITH_OPENCV
    IplImage *inputIpl = OpenCVTools::getCVImageFromG12Buffer(input);

    CVMAT_FROM_IPLIMAGE( detected_edges, inputIpl, false );

    Canny( detected_edges, detected_edges, mOpenCVParameters.param1() , mOpenCVParameters.param2() , 3 );

    *inputIpl = detected_edges;
    result = OpenCVTools::getG12BufferFromCVImage(inputIpl);

    cvReleaseImage(&inputIpl);
    return 0;
#else
    Q_UNUSED(input);
    return 0;
#endif
}

OpenCVFilter::~OpenCVFilter()
{
    // TODO Auto-generated destructor stub
}

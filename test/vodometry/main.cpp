#include <iostream>

#include <core/buffers/bufferFactory.h>

#include "core/utils/global.h"

#include "KLTFlow.h"

#include <stdio.h>


#include "libpngFileReader.h"

#include "findessentialmatrix.h"

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include "openCvFileReader.h"
#include "openCvKeyPointsWrapper.h"
#include "openCVTools.h"

#include <exception>
#include <sstream>

#include <opencv2/core/core.hpp>        // Mat
#include <opencv2/highgui/highgui.hpp>  // imread

using namespace std;
using namespace corecvs;
using namespace cv;

int main()
{
    Processor6D *proc = new OpenCVFlowProcessor;

    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();

    char filename[40];
    sprintf(filename,  "votest/%d.png", 1);
    RGB24Buffer *fstFrame = BufferFactory::getInstance()->loadRGB24Bitmap(filename);
    proc->beginFrame();
    proc->setFrameRGB24(Processor6D::FRAME_LEFT_ID, fstFrame);
    proc->endFrame();

    Matrix33 R_f = Matrix33::Identity();
    Vector3dd t_f(0, 0, 0);

    int i = 2;

    RGB24Buffer traj(700, 1000);

    while (true) {
        RGB24Buffer *in = BufferFactory::getInstance()->loadRGB24Bitmap(filename);
        if (in == NULL) {
            break;
        }


        proc->beginFrame();
        proc->setFrameRGB24(Processor6D::FRAME_LEFT_ID, in);
        proc->endFrame();
        FlowBuffer *flow = proc->getFlow();

        Matrix33 E, R;
        Vector3dd t;

        findEssentialMatrix(&E, flow);
        findRt(&R, &t, E, flow);

        R_f = R * R_f;
        t_f += 3 * (R_f * t);

//        cout << "\nFrame " << i << "has read\n" << "E matrix: \n" << E << "\nR matrix: \n" << R << "\nt vector: \n" << t;


//        delete_safe(in1);
        i++;
        sprintf(filename,  "votest/%d.png", i);

        int x = int(t_f.x()) + 600;
        int y = int(t_f.y()) + 600;
        traj.drawPixel(x, y, RGBColor::Red());

    }
    BufferFactory::getInstance()->saveRGB24Bitmap(&traj, "result.png");

//    cout << "\nR_f: \n" << R_f << "\n t_f: \n" << t_f;
    return 0;
}

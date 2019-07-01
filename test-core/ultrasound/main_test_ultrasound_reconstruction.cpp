/**
 * \file main_test_draw.cpp
 * \brief This is the main file for the test draw
 *
 * \date Apr 19, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#ifdef WITH_EIGEN
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "imgreader.h"
#include "model.h"


#include <iostream>
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


typedef AttributedTriangleSpanIterator AttributedTriangleSpanIteratorFix;

TEST(Ultrasound, testUltrasoundReconstruction)
{

    RGB24Buffer *buffer = new RGB24Buffer(20, 20, RGBColor::Gray());

    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawCircle(10, 10, 10, RGBColor::White());

    painter.drawCircle(7, 7, 3, RGBColor::Black());

    BMPLoader().save("ultrasound_reconstruction_input_1.bmp", buffer);

    delete_safe(buffer);

    Eigen::VectorXd a = pixel_matrix("ultrasound_reconstruction_input_1.bmp");

    Eigen::VectorXd x1 = count(a);

    std::ofstream outfile;

    outfile.open("i.txt");

    for(int i = 0;i<400;i++) {
        if (x1(i) >= 0.1 && x1(i)<2) {
            outfile << 1;
            cout<<i<<"\n";
        } else {
            outfile<<0;
        }
        if (i%20==19) outfile<<"\n";
    }
    outfile.close();

    RGB24Buffer *image = new RGB24Buffer(20, 20, RGBColor::Gray());

    AbstractPainter<RGB24Buffer> paint(image);
    paint.drawCircle(10, 10, 10, RGBColor::White());

    for(int i = 0;i<400;i++) {

        if (x1(i) >= 0.1 && x1(i)<2)
        {
            image->element(i / 20, i % 20) = RGBColor::Black();
        }

    }

    BMPLoader().save("reconstructed_image.bmp", image);

    delete_safe(image);

}

TEST(Ultrasound, testUltrasoundReconstruction2)
{

    RGB24Buffer *buffer = new RGB24Buffer(20, 20, RGBColor::Gray());

    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawCircle(10, 10, 10, RGBColor::White());

    painter.drawCircle(6, 6, 2, RGBColor::Black());
    painter.drawCircle(11, 11, 3, RGBColor::Black());

    BMPLoader().save("ultrasound_reconstruction_input_2.bmp", buffer);

    delete_safe(buffer);

    Eigen::VectorXd a = pixel_matrix("ultrasound_reconstruction_input_2.bmp");

    Eigen::VectorXd x1 = count(a);

    RGB24Buffer *image = new RGB24Buffer(20, 20, RGBColor::Gray());

    AbstractPainter<RGB24Buffer> paint(image);
    paint.drawCircle(10, 10, 10, RGBColor::White());

    for(int i = 0;i<400;i++) {

        if (x1(i) >= 0.7)
        {
            image->element(i / 20, i % 20) = RGBColor::Black();
        }

    }

    BMPLoader().save("reconstructed_image_2.bmp", image);

    delete_safe(image);

}

#endif

/**
 * \file test_convolve.c
 * \brief Add Comment Here
 *
 * \date Feb 25, 2010
 * \author alexander
 */

#include <stdint.h>
#include <stdio.h>
#include <core/buffers/kernels/laplace.h>
#include <core/buffers/kernels/fastkernel/fastKernel.h>
#include <core/buffers/convolver/convolver.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
//#include "core/math/vector/vector2d.h"

#include "core/buffers/g12Buffer.h"

#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/mipmapPyramid.h"
#include "core/kltflow/kltGenerator.h"
#include "core/buffers/bufferFactory.h"

using namespace corecvs;

TEST(Convolve, GaussDraw)
{
    double pixSpan = 500;
    RGB24Buffer output(2 * pixSpan, 2 * pixSpan);
    double levels[3] = {0.3, 1.0, 2.0};

    for (int s = 0; s < 3; s++)
    {
        double sigma = levels[s];
        for (int j = -pixSpan; j < pixSpan; j++)
        {
            double x = (j / pixSpan) * 5.0;
            double i = output.h - (output.h * normalPDF(x, sigma));            
            output.setElement(i, j + pixSpan, RGBColor::parula(s / 3.0));
        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(&output, "gauss.bmp");
}

TEST(Convolve, LoGDraw)
{
    double pixSpan = 500;
    RGB24Buffer output(2 * pixSpan, 2 * pixSpan);
    double levels[3] = {0.3, 1.0, 2.0};

    for (int s = 0; s < 3; s++)
    {
        double sigma = levels[s];
        for (int j = -pixSpan; j < pixSpan; j++)
        {
            double x = (j / pixSpan) * 5.0;
            double i = output.h / 2 - (output.h * laplacianOfGaussian(x, sigma));
            output.setElement(i, j + pixSpan, RGBColor::parula(s / 3.0));
        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(&output, "log.bmp");
}

TEST(Convolve, GaussDraw2d)
{
    double pixSpan = 500;
    RGB24Buffer output(2 * pixSpan, 2 * pixSpan);
    GaussianKernel kernel(output.h, output.w, output.w / 2, output.h / 2, output.h / 5.0);
    output.drawDoubleBuffer(kernel);
    BufferFactory::getInstance()->saveRGB24Bitmap(&output, "gauss2d.bmp");
}

TEST(Convolve, LoGDraw2d)
{
    double pixSpan = 500;
    RGB24Buffer output(2 * pixSpan, 2 * pixSpan);
    LaplacianKernel kernel(output.h, output.w, output.w / 2, output.h / 2, output.h * 60.0);
    output.drawDoubleBuffer(kernel);
    BufferFactory::getInstance()->saveRGB24Bitmap(&output, "log2d.bmp");
}

TEST(Convolve, LaplaceAndGauss)
{
    RGB24Buffer *input24 = BufferFactory::getInstance()->loadRGB24Bitmap("lenna.bmp");
    if(input24 == NULL)
    {
        SYNC_PRINT(("To load real image you can run:\n"));
        SYNC_PRINT(("   wget http://www.hlevkin.com/TestImages/lenna.bmp\n"));
        input24 = new RGB24Buffer(10,10);
        input24->checkerBoard(4, RGBColor::White(), RGBColor::Black());
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(input24, "conv_input.bmp");

    G8Buffer  *input8  = input24->getChannelG8(ImageChannel::GRAY);
    //G12Buffer *input12 = input24->toG12Buffer();                  /*< this uses luma  and 12 bits that leaves you with 4 upper bits for operation */

    G16Buffer *input16 = input24->getChannelG16(ImageChannel::GRAY); /*< this uses brightness and uses low 8 bits */
    DpImage   *inputD  = input24->getChannelDp (ImageChannel::GRAY);
    FpImage   *inputF  = input24->getChannelFp (ImageChannel::GRAY);


    /** Just to check */
    Gaussian3x3<DpKernel> gaussian3x3(true);
    Gaussian5x5<DpKernel> gaussian5x5(true);
    Laplace3x3 <DpKernel> laplace3x3(128.0);

    cout << "Gaussian33:\n" << gaussian3x3 << endl;
    cout << "Gaussian55:\n" << gaussian5x5 << endl;

    Gaussian3x3<FpKernel> gaussian3x3f(true);
    Gaussian5x5<FpKernel> gaussian5x5f(true);
    Laplace3x3 <FpKernel> laplace3x3f(128.0f);

    /** Variant one. Double version */
        DpImage *g33OutputD = new DpImage(inputD->getSize());
        Convolver().convolve(*inputD, gaussian3x3, *g33OutputD);
        BufferFactory::getInstance()->saveRGB24Bitmap(g33OutputD, "conv_gauss33_double.bmp", RGB24Buffer::STYLE_GRAY255);

        DpImage *g55OutputD = new DpImage(inputD->getSize());
        Convolver().convolve(*inputD, gaussian5x5, *g55OutputD);
        BufferFactory::getInstance()->saveRGB24Bitmap(g55OutputD, "conv_gauss55_double.bmp", RGB24Buffer::STYLE_GRAY255);

        DpImage *lOutputD = new DpImage(inputD->getSize());
        Convolver().convolveIB(*inputD, laplace3x3, *lOutputD);
        BufferFactory::getInstance()->saveRGB24Bitmap(lOutputD, "conv_log_double.bmp", RGB24Buffer::STYLE_GRAY255);

    /** Variant two. Float version */
        FpImage *g33OutputF = new FpImage(inputD->getSize());
        Convolver().convolve(*inputF, gaussian3x3f, *g33OutputF);
        BufferFactory::getInstance()->saveRGB24Bitmap(g33OutputF, "conv_gauss33_float.bmp", RGB24Buffer::STYLE_GRAY255);

        FpImage *g55OutputF = new FpImage(inputD->getSize());
        Convolver().convolve(*inputF, gaussian5x5f, *g55OutputF);
        BufferFactory::getInstance()->saveRGB24Bitmap(g55OutputF, "conv_gauss55_float.bmp", RGB24Buffer::STYLE_GRAY255);

        FpImage *lOutputF = new FpImage(inputD->getSize());
        Convolver().convolveIB(*inputF, laplace3x3f, *lOutputF);
        BufferFactory::getInstance()->saveRGB24Bitmap(lOutputF, "conv_log_float.bmp", RGB24Buffer::STYLE_GRAY255);

    /** Variant three. Fastkernel 16 bit unsigned version **/
        BufferProcessor<G16Buffer, G16Buffer, Gaussian3x3Kernel, G12BufferAlgebra> procVectorGauss33;
        G16Buffer *g33Output16 = new G16Buffer(input16->getSize());
        Gaussian3x3Kernel<DummyAlgebra> kernelG33;
        procVectorGauss33.processSaveAligned(&input16, &g33Output16, kernelG33);
        BufferFactory::getInstance()->saveRGB24Bitmap(g33Output16, "conv_gauss33_g16.bmp");

        BufferProcessor<G16Buffer, G16Buffer, Laplacian3x3Kernel, G12BufferAlgebra> procVectorLog;
        G16Buffer *lOutput16 = new G16Buffer(input16->getSize());
        Laplacian3x3Kernel<DummyAlgebra> kernelLog(127);
        procVectorLog.processSaveAligned(&input16, &lOutput16, kernelLog);
        BufferFactory::getInstance()->saveRGB24Bitmap(lOutput16, "conv_log_g16.bmp");

    /** Signed version */
#if 0
        BufferProcessor<G16Buffer, G16Buffer, Laplacian3x3KernelS, G12BufferAlgebra> procVectorSLog;
        G16Buffer *lOutput16s = new G16Buffer(input16->getSize());
        Laplacian3x3KernelS<DummyAlgebra> kernelSLog(127);
        procVectorSLog.processSaveAligned(&input16, &lOutput16s, kernelSLog);
        BufferFactory::getInstance()->saveRGB24Bitmap(lOutput16, "conv_log_s_g16.bmp");
#endif

    /** Variant four. Fastkernel 8 bit unsigned version **/



    /** Variant five. 3-channel double buffer **/



    if (g33OutputD->h < 20 && g33OutputD->w < 20)
    {
        cout << "g33OutputD" << endl << *g33OutputD << endl;
    }

    if (g33OutputF->h < 20 && g33OutputF->w < 20)
    {
        cout << "g33OutputF" << endl << *g33OutputF << endl;
    }

    if (g33Output16->h < 20 && g33Output16->w < 20)
    {
        //input16->touchOperationElementwize([](int, int, uint16_t &value){value = (value >> 4);});
        cout << "input16" << endl << *input16 << endl;

        //g33Output16->touchOperationElementwize([](int, int, uint16_t &value){value = (value >> 4);});
        cout << "g33Output16" << endl << *g33Output16 << endl;
    }

    delete_safe(input8);
    delete_safe(input16);
    delete_safe(input24);
    delete_safe(inputD);
    delete_safe(inputF);
}


TEST(Convolve, ConvolveEdgesDouble)
{
    Gaussian5x5<DpKernel> gaussian5x5(true);
    DpImage *input = new DpImage(8, 16);

    input->touchOperationElementwize([](int i, int  j, double &target){ target  = i + j;} );
    cout << "Input" << endl;
    cout << *input  << endl;

    DpImage *out[Convolver::ALGORITHM_LAST] = {0};

    for (int i = 0; i < Convolver::ALGORITHM_LAST; i++ )
    {
        Convolver::ConvolverImplementation c = (Convolver::ConvolverImplementation)i;
        //Convolver::ConvolverImplementation c = Convolver::ALGORITHM_SSE_UNROLL_1;
        out[i] = new DpImage(input->h, input->w, -1.0);
        Convolver(true, false).convolve(*input, gaussian5x5, *out[i], c);
        cout << "Output: " << Convolver::getName(c) << endl;
        cout << *out[i] << endl;
    }

    for (int i = 1; i < Convolver::ALGORITHM_LAST; i++ )
    {


    }

    for (int i = 0; i < Convolver::ALGORITHM_LAST; i++ )
    {
        delete_safe(out[i]);
    }
}

TEST(Convolve, ConvolveEdgesFloat)
{
    Gaussian5x5<FpKernel> gaussian5x5(true);
    FpImage *input = new FpImage(50, 50);

    input->touchOperationElementwize([](int i, int  j, float &target){ target  = (i + j) % 10;} );
    cout << "Input" << endl;
    cout << *input  << endl;

    FpImage *out[Convolver::ALGORITHM_LAST] = {0};

    for (int i = 0; i < /*Convolver::ALGORITHM_LAST*/ 2; i++ )
    {
        Convolver::ConvolverImplementation c = (Convolver::ConvolverImplementation)i;
        //Convolver::ConvolverImplementation c = Convolver::ALGORITHM_SSE_UNROLL_1;
        out[i] = new FpImage(input->h, input->w, -1.0f);
        Convolver(true, false).convolve(*input, gaussian5x5, *out[i], c);
        cout << "Output: " << Convolver::getName(c) << endl;
        cout << *out[i] << endl;
    }

    for (int i = 1; i < Convolver::ALGORITHM_LAST; i++ )
    {


    }

    for (int i = 0; i < Convolver::ALGORITHM_LAST; i++ )
    {
        delete_safe(out[i]);
    }

}



//int main ( int /*argV*/, char * /*argC*/[])
TEST(Convolve, DISABLED_main)
{
    G12Buffer *buffer = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    if (buffer == nullptr)
    {
        cout << "Could not open test image" << endl;
        return;
    }

/*    int numLevels = 4;
    AbstractMidmapPyramid<G12Buffer> *pyr = new AbstractMidmapPyramid<G12Buffer>(buffer, numLevels);
    for (int i = 0; i < numLevels; i++)
    {
        char outName[256];
        snprintf2buf(outName, "pyr%d.bmp", i);

        G12Buffer *level = pyr->levels[i];

        G12Buffer *toSave = g12BufferCreate(level->h, level->w);
        memcpy(toSave->data, level->data, sizeof (uint16_t) * level->h * level->w);

        SDL_Surface *tmp;
        tmp = SDL_CreateRGBSurface(0,toSave->w,toSave->h, 32, 0,0,0,0);
        drawG12BufferIntoSDL(toSave, tmp, 0, 0);
        printf("Writing to %s\n", outName);
        SDL_SaveBMP(tmp, outName);
        SDL_FreeSurface(tmp);
    }*/

    SpatialGradient *sg = new SpatialGradient(buffer);
    SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);

    for (int i = 1; i < gradient->h; i++)
    {
        for (int j = 1; j < gradient->w; j++)
        {
            double *grad1 = (double *)(&gradient->element(i,j));
            double *grad2 = (double *)(&gradient->element(i - 1,j));
            double *grad3 = (double *)(&gradient->element(i,j - 1));
            double *grad4 = (double *)(&gradient->element(i - 1,j - 1));

            double val1  = *grad1 + *grad4 - (*grad2 + *grad3);
            double *val = (double *)(&sg->element(i,j));
            if (*val != val1)
                printf("Problem");
        }
    }

//    return 1;
}

/*
int main ( int argV, char * argC[])
{
    uint16_t data1[36] = {
                        1,3,7,8,1,3,
                        1,1,7,8,1,1,
                        1,3,7,8,1,3,
                        1,1,7,8,1,1,
                        1,3,7,8,1,3,
                        4,4,4,4,4,4
                     };
    G12Buffer *base1 = new G12Buffer(6,6,data1);
    base1->print();

    G12Buffer *filtered = base1->doConvolve1<double, uint32_t>(Gaussian3x3::instance);
    filtered->print();

    AbstractMidmapPyramid<G12Buffer> *pyr = new AbstractMidmapPyramid<G12Buffer>(base1, 2);

    printf("Levels:\n");
    pyr->levels[0]->print();
    pyr->levels[1]->print();
    return 0;
}*/

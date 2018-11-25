/**
 * \file main_test_bezierRasterizer.cpp
 * \brief This is the main file for the test bezierRasterizer
 *
 * \date Nov 25, 2018
 * \author mishinnikita
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/abstractBuffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/bufferFactory.h"
#include "core/buffers/rgb24/bezierrasterizer.h"
#include <ctime>

using namespace corecvs;

template<typename T>
class MockBuffer {
public:
    typedef T InternalElementType;

    T dummy;
    T & element(int y, int x) {
        return dummy;
    }
    int getH() {return 800;}
    int getW() {return 800;}

};

TEST(bezierRasterizer, compareSpeedOfDifferentImplementation) {
    auto mock_buffer =MockBuffer<float>();
    auto rasterizer = WuRasterizer();
    BezierRasterizer< MockBuffer<float>,WuRasterizer> bezier(mock_buffer,rasterizer,2.);
    std::vector<Curve> randCurves;
    SYNC_PRINT(("Time for 10000 curves to be rendered:\n"));

    for(int i = 0;i < 10000; i++) {
        Curve randomCurve = {{rand()%700,rand()%800},{rand()%800,rand()%800},{rand()%800,rand()%800},{rand()%800,rand()%800}};
        randCurves.push_back(randomCurve);
    }

    clock_t begin = clock();
    for(int i = 0;i<10000;i++){
        bezier.cubicBezierDummy(randCurves.at(i));
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    SYNC_PRINT(("DUMMY:%f\n",elapsed_secs));


    begin = clock();
    for(int i = 0;i<10000;i++){
        bezier.cubicBezierCasteljauArticle(randCurves.at(i));
    }
     end = clock();
    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    SYNC_PRINT(("ArticleAlgorithm:%f\n",elapsed_secs));


    begin = clock();
    for(int i = 0;i<10000;i++){
        bezier.cubicBezierCasteljauApproximationByFlatness(randCurves.at(i));
    }
    end = clock();
    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    SYNC_PRINT(("Castlejau:%f\n",elapsed_secs));

}


TEST(bezierRasterizer,bezierRasterizerDog ) {
    const static int H = 500;
    const static int W = 500;

    RGB24Buffer buffer1 = RGB24Buffer(H, W );
    RGB24Buffer buffer2 = RGB24Buffer(H, W );
    RGB24Buffer buffer3 = RGB24Buffer(H, W );
    buffer1.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    buffer2.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    buffer3.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    auto rasterizer = WuRasterizer();
    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier(buffer1,rasterizer,RGBColor::Black());
    //DOG PICASSO
    std::vector<Curve> curves;
    curves.push_back({{180,280},{183,268},{186,256},{189,244}});//front leg
    curves.push_back({{191,244},{290,244},{300,230},{339,245}});//tummy
    curves.push_back({{340,246},{350,290},{360,300},{355,210}});//back leg
    curves.push_back({{340,246},{350,290},{360,300},{355,210}});//tail
    curves.push_back({{375,193},{310,220},{190,220},{164,205}});//back
    curves.push_back({{164,205},{135,194},{135,265},{153,275}});//ear start
    curves.push_back({{153,275},{168,275},{170,180},{150,190}});//back leg
    curves.push_back({{149,190},{122,214},{142,204},{85,240}});//node bridge
    curves.push_back( {{86,240},{100,247},{125,233},{140,238}});//mouth

    for(int i = 0;i < 9;i++){
        bezier.cubicBezierDummy(curves.at(i));

    }
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer1, "CubicBezierDummyDog.bmp");


    bezier.buffer = buffer2;
    for(int i = 0;i < 9;i++){
        bezier.cubicBezierCasteljauApproximationByFlatness(curves.at(i));
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer2, "CubicBezierCasteljauDivideDog.bmp");


    bezier.buffer = buffer3;
    for(int i = 0;i < 9;i++){
        bezier.cubicBezierCasteljauArticle(curves.at(i));
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer3, "CubicBezierMathArticleDog.bmp");
}

TEST(bezierRasterizer,bezierRasterizerDifferentCases ) {
    const static int H = 480;
    const static int W = 640;
    RGB24Buffer buffer = RGB24Buffer(H, W );

    auto rasterizer = WuRasterizer();
    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier(buffer,rasterizer,RGBColor::Black());
    //--
    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierDummy({{126,88},{515,360},{109,348},{126,88}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierDummyClosed.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauArticle({{126,88},{515,360},{109,348},{126,88}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierArticleAlgorithmClosed.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauApproximationByFlatness({{126,88},{515,360},{109,348},{126,88}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierCasteljauAlgorithmClosed.bmp");

    //--- loop
    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierDummy({{133,383},{496,300},{221,128},{279,422}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierDummyLoop.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauArticle({{133,383},{496,300},{221,128},{279,422}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierArticleAlgorithmLoop.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauApproximationByFlatness({{133,383},{496,300},{221,128},{279,422}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierCasteljauAlgorithmLoop.bmp");

    // --- twirl

    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierDummy({{79,373},{452,120},{310,280},{429,278}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierDummyTwirl.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauArticle({{79,373},{452,120},{310,280},{429,278}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierArticleAlgorithmTwirl.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauApproximationByFlatness({{79,373},{452,120},{310,280},{429,278}});
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierCasteljauAlgorithmTwirl.bmp");
}

TEST(bezierRasterizer,bezierRasterizerRandom ) {
    const static int H = 700;
    const static int W = 700;
    RGB24Buffer buffer = RGB24Buffer(H, W );
    auto rasterizer = WuRasterizer();
    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier(buffer,rasterizer,RGBColor::Black());
    //--
    Curve randomCurve = {{rand()%700,rand()%700},{rand()%700,rand()%700},{rand()%700,rand()%700},{rand()%700,rand()%700}};

    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierDummy(randomCurve); //closed
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierDummyRandom.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauArticle(randomCurve); //closed
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierArticleAlgorithmRandom.bmp");


    buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
    bezier.cubicBezierCasteljauApproximationByFlatness(randomCurve); //closed
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "CubicBezierCasteljauAlgorithmRandom.bmp");

}


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
#include "core/buffers/rgb24/bezierRasterizer.h"
#include <ctime>
#include <core/utils/preciseTimer.h>

using namespace corecvs;

template<typename T>
class MockBuffer {
public:
    typedef T InternalElementType;

    T dummy;
    T & element(int /*y*/, int /*x*/) {
        return dummy;
    }
    int getH() {return 800;}
    int getW() {return 800;}

};

TEST(bezierRasterizer, profileDifferentImplementations) {
    auto mock_buffer =MockBuffer<float>();
    auto rasterizer = WuRasterizer();
    BezierRasterizer< MockBuffer<float>,WuRasterizer> bezier(mock_buffer,rasterizer,2.);
    std::vector<Curve> randCurves;

    const int LIMIT = 100000;
    SYNC_PRINT(("Time for %d curves to be rendered:\n", LIMIT));

    for(int i = 0; i < LIMIT; i++) {
        std::mt19937 rng;
        std::uniform_real_distribution<double> runifX(0, mock_buffer.getW() - 1);
        std::uniform_real_distribution<double> runifY(0, mock_buffer.getH() - 1);

        Curve randomCurve = {{runifX(rng), runifY(rng)},
                             {runifX(rng), runifY(rng)},
                             {runifX(rng), runifY(rng)},
                             {runifX(rng), runifY(rng)}};
        randCurves.push_back(randomCurve);
    }

    PreciseTimer timer = PreciseTimer::currentTime();
    for(int i = 0; i < LIMIT; i++){
        bezier.cubicBezierDummy(randCurves.at(i));
    }
    SYNC_PRINT(("DUMMY:           %9" PRIu64 "us  (%lf s)\n", timer.usecsToNow(), timer.usecsToNow() / 1000000.0));


    timer = PreciseTimer::currentTime();
    for(int i = 0; i < LIMIT; i++){
        bezier.cubicBezierCasteljauArticle(randCurves.at(i));
    }
    SYNC_PRINT(("ArticleAlgorithm:%9" PRIu64 "us  (%lf s)\n", timer.usecsToNow(), timer.usecsToNow() / 1000000.0));


    timer = PreciseTimer::currentTime();
    for(int i = 0; i < 10000; i++){
        bezier.cubicBezierCasteljauApproximationByFlatness(randCurves.at(i));
    }
    SYNC_PRINT(("Castlejau:       %9" PRIu64 "us  (%lf s)\n", timer.usecsToNow(), timer.usecsToNow() / 1000000.0));

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
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer1, "BezierDogDummy.bmp");


    bezier.buffer = buffer2;
    for(int i = 0;i < 9;i++){
        bezier.cubicBezierCasteljauApproximationByFlatness(curves.at(i));
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer2, "BezierDogCasteljauDivide.bmp");


    bezier.buffer = buffer3;
    for(int i = 0;i < 9;i++){
        bezier.cubicBezierCasteljauArticle(curves.at(i));
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer3, "BezierDogMathArticle.bmp");
}

TEST(bezierRasterizer,bezierRasterizerDifferentCases ) {
    const static int H = 480;
    const static int W = 640;
    RGB24Buffer buffer = RGB24Buffer(H, W);

    struct TestCase
    {
        Curve curve;
        std::string name;
    };

    TestCase cases[] =
    { { {{126, 88},{515,360},{109,348},{126, 88}}, "Closed" },
      { {{133,383},{496,300},{221,128},{279,422}}, "Loop"   },
      { {{ 79,373},{452,120},{310,280},{429,278}}, "Twirl" }
    };

    std::string prefix = "Bezier";
    auto rasterizer = WuRasterizer();

    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier(buffer,rasterizer,RGBColor::Black());

    for (size_t i = 0; i < CORE_COUNT_OF(cases); i++)
    {
        buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
        bezier.cubicBezierDummy(cases[i].curve);
        BufferFactory::getInstance()->saveRGB24Bitmap(buffer, prefix + cases[i].name + "Dummy"  + ".bmp");

        buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
        bezier.cubicBezierCasteljauArticle(cases[i].curve);
        BufferFactory::getInstance()->saveRGB24Bitmap(buffer, prefix + cases[i].name+ "ArticleAlgorithm" + ".bmp");

        buffer.checkerBoard(10, RGBColor::White(),  RGBColor::White());
        bezier.cubicBezierCasteljauApproximationByFlatness(cases[i].curve);
        BufferFactory::getInstance()->saveRGB24Bitmap(buffer, prefix + cases[i].name + "CasteljauAlgorithm" + ".bmp");

    }
}

TEST(bezierRasterizer,bezierRasterizerRandom ) {
    const static int H = 500;
    const static int W = 500;
    RGB24Buffer buffer1 = RGB24Buffer(H, W );
    RGB24Buffer buffer2 = RGB24Buffer(H, W );
    RGB24Buffer buffer3 = RGB24Buffer(H, W );

    auto rasterizer = WuRasterizer();
    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier1(buffer1,rasterizer,RGBColor::Black());
    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier2(buffer2,rasterizer,RGBColor::Black());
    BezierRasterizer< RGB24Buffer,WuRasterizer> bezier3(buffer3,rasterizer,RGBColor::Black());

    //--
    std::mt19937 rng;
    std::uniform_real_distribution<double> runifX(0, W - 1);
    std::uniform_real_distribution<double> runifY(0, H - 1);

    buffer1.fillWith(RGBColor::White());
    buffer2.fillWith(RGBColor::White());
    buffer3.fillWith(RGBColor::White());


    for (int i = 0; i < 10; i++)
    {
        Curve randomCurve = {{runifX(rng), runifY(rng)},
                             {runifX(rng), runifY(rng)},
                             {runifX(rng), runifY(rng)},
                             {runifX(rng), runifY(rng)}};

        cout << "Draw curve" << randomCurve << endl;
        bezier1.color = RGBColor::getPalleteColor(i);
        bezier2.color = RGBColor::getPalleteColor(i);
        bezier3.color = RGBColor::getPalleteColor(i);


        /* */
        bezier1.cubicBezierDummy(randomCurve);
        bezier2.cubicBezierCasteljauArticle(randomCurve);
        bezier3.cubicBezierCasteljauApproximationByFlatness(randomCurve);
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer1, "BezierRandomDummy.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer2, "BezierRandomArticleAlgorithm.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(buffer3, "BezierRandomCasteljauAlgorithm.bmp");
}



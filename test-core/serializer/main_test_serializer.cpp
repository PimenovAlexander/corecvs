/**
 * \file main_test_serializer.cpp
 * \brief This is the main file for the test serializer 
 *
 * \date Nov 26, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <unordered_map>
#include <iostream>
#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/xml/generated/checkerboardDetectionParameters.h"
#include "core/reflection/jsonPrinter.h"
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
#include "core/utils/propertyList.h"
#include "core/utils/visitors/propertyListVisitor.h"
#include "core/rectification/triangulator.h"
#include "core/reflection/printerVisitor.h"
#include "core/reflection/dynamicObject.h"
#include "core/reflection/binaryReader.h"
#include "core/reflection/binaryWriter.h"

#include "core/reflection/advanced/advancedBinaryReader.h"
#include "core/reflection/advanced/advancedBinaryWriter.h"

using namespace corecvs;

TEST(Serializer, testReflectionDEATH)
{
    Vector2dd vec(5.0, 0.4);
    PrinterVisitor visitor;

    // unfortunately such an error is differently handling on linux and win!
#ifdef WIN32
    ASSERT_EXIT(visitor.visit(vec, static_cast<const CompositeField *>(NULL)), ::testing::ExitedWithCode(1), "");
#else
    ASSERT_EXIT(visitor.visit(vec, static_cast<const CompositeField *>(NULL)), ::testing::KilledBySignal(11), "");
#endif
}



TEST(Serializer, testReflection1)
{
    Vector3dd vec3a(5.0, 0.4, -1.0);
    Vector3dd vec3b(6.0, 0.5, -1.1);
    int dataInt[7] = {1, 2, 3, 4, 5, 6, 7};
    FixedVector<int, 7> vecc(dataInt);
    double dataDouble[2] = {14.7, 5.33333333333};
    FixedVector<double, 2> vecd(dataDouble);
    bool dataBool[4] = {true, false, true, false};
    FixedVector<bool, 4> vece(dataBool);


    PropertyList list;
    PropertyListWriterVisitor writerVisitor(&list);

    Vector3dd defaultValue(0);
    FixedVector<int, 7>    veccd(0);
    FixedVector<double, 2> vecdd((double)0);
    FixedVector<bool, 4>   veced((bool)0);

    writerVisitor.visit( vec3a, defaultValue, "vector1" );
    writerVisitor.visit( vec3b, defaultValue, "vector2" );
    writerVisitor.visit( vecc,  veccd, "vector3" );
    writerVisitor.visit( vecd,  vecdd, "vector4" );
    writerVisitor.visit( vece,  veced, "vector5" );

    list.save(cout);

    /* now try to read saved data */
    Vector3dd rvec3a;
    Vector3dd rvec3b;
    FixedVector<int, 7>    rvecc;
    FixedVector<double, 2> rvecd;
    FixedVector<bool, 4>   rvece;

    PropertyListReaderVisitor readerVisitor(&list);
    readerVisitor.visit( rvec3a, defaultValue, "vector1" );
    readerVisitor.visit( rvec3b, defaultValue, "vector2" );
    readerVisitor.visit( rvecc,  veccd, "vector3" );
    readerVisitor.visit( rvecd,  vecdd, "vector4" );
    readerVisitor.visit( rvece,  veced, "vector5" );

    CORE_ASSERT_TRUE(rvec3a == vec3a, "serializer failed 1");
    CORE_ASSERT_TRUE(rvec3b == vec3b, "serializer failed 2");
    CORE_ASSERT_TRUE(rvecc == vecc, "serializer failed 3");
    CORE_ASSERT_TRUE(rvecd == vecd, "serializer failed 4");
    CORE_ASSERT_TRUE(rvece == vece, "serializer failed 5");

    cout << "Output" << rvecc << endl;
}

TEST(Serializer, testPropertyListLoader)
{
    const char *example =
    "# Test \n"
    "v.x = 1.1\n"
    "v.y = 2.2\n"
    "v.z = 3.3\n";

    std::istringstream stream(example);

    PropertyList list;
    list.load(stream);
    PropertyListReaderVisitor readerVisitor(&list);
    Vector3dd test;
    readerVisitor.visit(test, Vector3dd(0.0), "v");
    cout << test << endl;

    CORE_ASSERT_TRUE(test.notTooFar(Vector3dd(1.1, 2.2, 3.3)), "Fail parsing vector");
}

TEST(Serializer, testSerializer1)
{
    RectificationResult result;
    PropertyList list;
    PropertyListWriterVisitor writerVisitor(&list);
    writerVisitor.visit(result, result, "RectificationResult");

    list.save(cout);
}

TEST(Serializer, printIterable)
{
    PrinterVisitor visitor;

    std::vector<int>       vectorOfInt    = {1,2,3,4};
    std::vector<Vector2dd> vectorOfVector = {Vector2dd::OrtX(), Vector2dd::OrtY()};

    std::unordered_map<int, Vector2dd> mapIntToVec = {{1, Vector2dd::OrtX()}, {1, Vector2dd::OrtY()}};

    visitor.visit(vectorOfInt, "vector of integers");
    visitor.visit(vectorOfVector, "vector of vector2dd");

    //visitor.visit(mapIntToVec, "map of int to vector2dd");
}

TEST(Serializer, printReflection)
{
    PrinterVisitor visitor;
    CheckerboardDetectionParameters params;


    DynamicObject obj(&params);
    BoolField testField(0,0,false, "me");
    bool me;
    visitor.visit(me, &testField);
    visitor.visit(obj, "example");
}

TEST(Serializer, jsonEscape)
{
    std::ostringstream os;
    {
        JSONPrinter printer(&os);
        std::string in("ABC\\ \n \t \r \b \"");
        std::string def;
        printer.visit<std::string>(in, def, "test");
    }
    std::string out(
    "{\n"
    "\"test\":\"ABC\\\\ \\n \\t \\r \\b \\\"\"\n"
    "}");
    CORE_ASSERT_TRUE_P(os.str() == out, ("<%s> and <%s>", os.str().c_str(), out.c_str() ));
    std::cout << os.str() << endl;
}

TEST(Serializer, jsonDoubleArray)
{
    std::vector<std::vector<RgbColorParameters>> rgbArray;
    for (int i = 0; i < 3; i++) {
        rgbArray.push_back(std::vector<RgbColorParameters>());
    }
    rgbArray[0].push_back(RGBColor::Amber().toRGBParameters());
    rgbArray[0].push_back(RGBColor::Indigo().toRGBParameters());
    rgbArray[1].push_back(RGBColor::Yellow().toRGBParameters());
    rgbArray[2].push_back(RGBColor::Black().toRGBParameters());
    rgbArray[2].push_back(RGBColor::Blue().toRGBParameters());

    std::ostringstream os;
    {
        JSONPrinter printer(&os);
        printer.visit(rgbArray, "test");
    }
    std::cout << os.str() << endl;
}



TEST(Serializer, binarySerializer)
{

    Vector3dd   test  = Vector3dd(1.0, 2.0, 3.0);
    RGBColor    testc = RGBColor::Indigo();
    std::string teststr   = "Example";

    CheckerboardDetectionParameters testcb;

    {
        BinaryWriter writer("out.txt");
        writer.visit(test ,   "out");
        writer.visit(testc,   "out1");
        writer.visit(teststr, teststr, "out2");
        writer.visit(testcb,  testcb, "out3");

    }

    Vector3dd   result;
    RGBColor    resultc;
    std::string resultstr   = "Example";
    CheckerboardDetectionParameters resultcb;

    {
        BinaryReader reader("out.txt");
        reader.visit(result   , "out");
        reader.visit(resultc  , "out1");
        reader.visit(resultstr, resultstr, "out2");
        reader.visit(resultcb,  "out3");
    }

    cout << "Loaded result : " << result << std::endl;
    cout << "Loaded result : " << resultc << std::endl;
    cout << "Loaded result : " << resultstr << std::endl;
    cout << "Loaded result : " << resultcb << std::endl;

    CORE_ASSERT_TRUE(result.notTooFar(test)  , "Double vector not loaded");
    CORE_ASSERT_TRUE(resultc.notTooFar(testc), "Int vector not loaded");
    CORE_ASSERT_TRUE(resultstr == teststr    , "String not loaded");

}

TEST(Serializer, advancedBinarySerializer)
{
    Vector3dd   test  = Vector3dd(1.0, 2.0, 3.0);
    RGBColor    testc = RGBColor::Indigo();
    std::string teststr   = "Example";

    CheckerboardDetectionParameters testcb;

    {
        AdvancedBinaryWriter writer("out.txt");
        writer.visit(test ,   "out");
        writer.visit(testc,   "out1");
        writer.visit(teststr, teststr, "out2");
        writer.visit(testcb,  testcb, "out3");
    }
    {
        AdvancedBinaryReader reader("out.txt");
//        reader.readDictionary();
    }

    Vector3dd   result;
    RGBColor    resultc;
    std::string resultstr   = "Example";
    CheckerboardDetectionParameters resultcb;
    memset(&resultcb, 0xA5, sizeof(resultcb));

    {
        AdvancedBinaryReader reader("out.txt");
        reader.visit(result   , "out");
        reader.visit(resultc  , "out1");
        reader.visit(resultstr, resultstr, "out2");
        reader.visit(resultcb,  "out3");
    }

    cout << "Loaded result : " << result << std::endl;
    cout << "Loaded result : " << resultc << std::endl;
    cout << "Loaded result : " << resultstr << std::endl;
    cout << "Initial data  : " << testcb << std::endl;
    cout << "Loaded result : " << resultcb << std::endl;

    CORE_ASSERT_TRUE(result.notTooFar(test)  , "Double vector not loaded");
    CORE_ASSERT_TRUE(resultc.notTooFar(testc), "Int vector not loaded");
    CORE_ASSERT_TRUE(resultstr == teststr    , "String not loaded");

    CORE_ASSERT_TRUE(resultcb == testcb    , "Large structure not loaded");


}


TEST(Serializer, binarySerializerScene)
{
    FixtureScene scene;

    {
        AdvancedBinaryWriter writer("scene.bin");
        writer.visit(scene,   "scene");
    }

    {
        BinaryReader reader("scene.bin");
        reader.visit(scene, "scene");
    }
}

TEST(Serializer, testCameraModels)
{
    OmnidirectionalBaseParameters baseParams;
    JSONPrinter printer;
    printer.visit(baseParams, "test1");

    CameraModel catadioptric;
    catadioptric.intrinsics.reset(new OmnidirectionalProjection(Vector2dd(100,100), 100, Vector2dd(200,200)));
    catadioptric.setLocation(Affine3DQ::Shift(40,0,0));

    FixtureCamera *cameraCat = new FixtureCamera;
    cameraCat->nameId = "catadioptric";
    cameraCat->copyModelFrom(catadioptric);

    printer.visit(*cameraCat, "test2");



}

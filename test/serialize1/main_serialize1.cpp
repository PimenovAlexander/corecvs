#include <stdio.h>
#include <vector>

#include "gtest/gtest.h"

#include <QtXml/QDomDocument>

#ifndef WIN32
#include <unistd.h>
#endif
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/fileformats/bmpLoader.h"
#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/math/vector/vector3d.h"
#include "xmlSetter.h"
#include "xmlGetter.h"
#include "core/reflection/jsonPrinter.h"
#include "jsonSetter.h"
#include "jsonGetter.h"
#ifdef WITH_RAPIDJSON
    #include "rapidJSONReader.h"
#endif
#ifdef WITH_JSONMODERN
    #include "jsonModernReader.h"
#endif

/*******************************
 *    XML
 ********************************/

void testXML1()
{
    QDomDocument document("document");
    //QDomElement mDocumentElement = document.createElement("xml");
    //QDomElement mDocumentElement = document; //document.documentElement();

    //QDomElement mDocumentElement1 = mDocumentElement;
    QDomElement newElement = document.createElement("tag");
    newElement.setAttribute("value", "test");
    QDomNode node = document.appendChild(newElement);

    qDebug() << document.toString();
}

void testXML2()
{

    printf("Serializing some data\n");

    Vector3dd someData(1.0,2.0,5.0);
    {
        XmlSetter setter("output.xml");
        setter.visit(someData, "This_is_some_data");
    }

    printf("====== Loading back some data ======\n");

    Vector3dd result(1.5,2.5,5.5);
    {
        XmlGetter getter("output.xml");
        getter.visit(result, "This_is_some_data");
    }

    cout << "Result:" << result;
}


/*******************************
 *    Now with QT json
 ********************************/

void testJSON1()
{
    const char input[] =
    "{\n"
    "  \"vector\":{\n"
    "     \"x\": 1,\n"
    "     \"y\": 2,\n"
    "     \"z\": 3\n"
    "  }\n"
    "}\n";



   /* printf("Serializing some data\n");

    Vector3dd someData(1.0,2.0,5.0);
    {
        XmlSetter setter("output.xml");
        setter.visit(someData, "This_is_some_data");
    }

    printf("====== Loading back some data ======\n");
*/
    SYNC_PRINT(("Parsing %" PRISIZE_T " input:\n%s\n", CORE_COUNT_OF(input), input));


    QByteArray array(input, CORE_COUNT_OF(input));
    SYNC_PRINT(("Array:\n%s\n", array.constData()));


    QJsonDocument document = QJsonDocument::fromJson(array);
    if (document.isNull())
    {
         SYNC_PRINT(("Fail parsing the data"));
    }

    QJsonObject  object = document.object();

    Vector3dd result(1.5,2.5,5.5);
    {
        JSONGetter getter(object);
        getter.visit(result, "vector");
    }

    /*======== Now saving ===========*/
    {
        JSONSetter setter("out.txt");
        setter.visit(result, "vector");
    }

    cout << "Result:" << result;
}


class TestComplexStructure : public BaseReflection<TestComplexStructure>{
public:
    std::vector<double> testField;

    static int staticInit();

    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            visitor.visit(testField,  static_cast<const DoubleVectorField *>(fields()[0]));
        }

};

template<>
Reflection BaseReflection<TestComplexStructure>::reflection = Reflection();
template<>
int BaseReflection<TestComplexStructure>::dummy = TestComplexStructure::staticInit();

int TestComplexStructure::staticInit()
{

    ReflectionNaming &nameing = naming();
    nameing = ReflectionNaming("TestComplexStructure");

    fields().push_back(
        new DoubleVectorField
        (
          0,
          offsetof(TestComplexStructure, testField),
          0,
          0,
          "testField",
          "testField",
          "testField"
        )
    );
   return 0;
}

void testJSON2()
{
    TestComplexStructure testObject;
    for (int i = 0; i < 7; i++) {
        testObject.testField.push_back(i / 7.8);
    }

    /*======== Now saving ===========*/
    {
        JSONSetter setter("out.txt");
        setter.visit(testObject, "vector");
    }

    /*======== And loading back =======*/
    TestComplexStructure testObject1;
    {
        JSONGetter getter("out.txt");
        getter.visit(testObject1, "vector");
    }


    CORE_ASSERT_TRUE(testObject.testField.size() == testObject1.testField.size(), "Wrong loaded array size");
    for (size_t i = 0; i < testObject.testField.size(); i++)
    {
        CORE_ASSERT_TRUE_P(testObject1.testField[i] == testObject.testField[i], ("Error at pos %" PRISIZE_T "", i));
        cout << testObject1.testField[i] << " == " << testObject.testField[i] << std::endl;
    }
    cout << std::endl;

}

/*======================================*/
class Uint64Structure : public BaseReflection<TestComplexStructure>{
public:
    uint64_t testField1;
    uint64_t testField2;

    static int staticInit();

    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            visitor.visit(testField1,  static_cast<const Int64Field *>(fields()[0]));
            visitor.visit(testField2,  (uint64_t)0xDEADBEEF, "test1" );
        }

};

template<>
Reflection BaseReflection<Uint64Structure>::reflection = Reflection();
template<>
int BaseReflection<Uint64Structure>::dummy = TestComplexStructure::staticInit();

int Uint64Structure::staticInit()
{

    ReflectionNaming &nameing = naming();
    nameing = ReflectionNaming("TestComplexStructure");

    fields().push_back(
        new Int64Field
        (
          0,
          offsetof(TestComplexStructure, testField),
          0,
          0,
          "testField",
          "testField",
          "testField"
        )
    );
   return 0;
}

void testJSON_UInt64()
{
    Uint64Structure testObject;
    testObject.testField1 = 0xF000F000E000E000lu;
    testObject.testField2 = 700;

    cout << "In   1" << std::hex << testObject.testField1 << std::endl;
    cout << "In   2" << std::hex << testObject.testField2 << std::endl;


    /*======== Now saving ===========*/
    {
        JSONSetter setter("out.txt");
        setter.visit(testObject, "test64");
    }

    /*======== And loading back =======*/
    Uint64Structure testObject1;
    {
        JSONGetter getter("out.txt");
        getter.visit(testObject1, "test64");
    }


    cout << "Orig 1" << std::hex << testObject.testField1 << std::endl;
    cout << "Orig 2" << std::hex << testObject.testField2 << std::endl;

    cout << "Load 1" << std::hex << testObject1.testField1 << std::endl;
    cout << "Load 2" << std::hex << testObject1.testField2 << std::endl;


}


void testJSON_saveDistortion()
{
    LensDistortionModelParameters params;

    params.setKoeff({1.0, std::numeric_limits<double>::min()});

    cout << "testJSON_saveDistortion() test called" << endl;
    cout << "Initial state" << endl;
    cout << params << endl;

    {
        JSONSetter saver("out1.json");
        saver.visit(params, "stage1");
    }
    {
        JSONGetter loader("out1.json");
        loader.visit(params, "stage1");
    }
    {
        JSONSetter saver("out2.json");
        saver.visit(params, "stage2");
    }

    LensDistortionModelParameters loaded;
    {
        JSONGetter loader("out2.json");
        loader.visit(loaded, "stage2");
    }
    cout << "After loading" << endl;
    cout << loaded << endl;

    cout << "Exiting" << endl;


}

#ifdef WITH_RAPIDJSON
/*******************************
 *    Now with Rapid json
 ********************************/


void testRapidJSON1()
{
    cout << std::endl;
    cout << "testRapidJSON1()" << std::endl;
    cout << std::endl;

    const char input[] =
    "{\n"
    "  \"vector\":{\n"
    "     \"x\": 1,\n"
    "     \"y\": 2,\n"
    "     \"z\": 3\n"
    "  }\n"
    "}\n";

    SYNC_PRINT(("Parsing %" PRISIZE_T "bytes input:\n%s\n", CORE_COUNT_OF(input), input));

    rapidjson::Document document;
    document.Parse(input);
    rapidjson::Value object = document.GetObject();

    cout << "Before parsing" << endl;
    Vector3dd result(1.5,2.5,5.5);
    cout << result << endl;
    {
        RapidJSONReader getter(object);
        getter.visit(result, "vector");
    }
    cout << "After parsing" << endl;
    cout << result << endl;

}

void testRapidJSON_saveDistortion()
{
    cout << std::endl;
    cout << "testRapidJSON_saveDistortion()" << std::endl;
    cout << std::endl;

    LensDistortionModelParameters params;
    LensDistortionModelParameters loaded;

    params.setPrincipalX(400);
    params.setPrincipalY(400);
    params.setTangentialX(0.3);
    params.setTangentialY(0.4);

    params.setShiftX(200);
    params.setShiftY(200);

    params.setAspect(10.0);
    params.setScale(1.1);
    params.setKoeff({1.0, 2.0, std::numeric_limits<double>::min()});

    cout << "testJSON_saveDistortion() test called" << endl;
    cout << "Initial state" << endl;
    cout << params << endl;

    {
        JSONSetter saver("out1.json");
        saver.visit(params, "stage1");
    }
    {
        RapidJSONReader loader("out1.json");
        loader.visit(loaded, "stage1");
    }

    cout << "After parsing" << endl;
    cout << loaded << endl;

}
#endif

#ifdef WITH_JSONMODERN
/*******************************
 *    Now with json modern
 ********************************/

TEST(Serialise, testJSONModern1)
{
    cout << std::endl;
    cout << "testRapidJSON1()" << std::endl;
    cout << std::endl;

    const char input[] =
    "{\n"
    "  \"vector\":{\n"
    "     \"x\": 1,\n"
    "     \"y\": 2,\n"
    "     \"z\": 3\n"
    "  }\n"
    "}\n";

    SYNC_PRINT(("Parsing %" PRISIZE_T "bytes input:\n%s\n", CORE_COUNT_OF(input), input));

    nlohmann::json document = nlohmann::json::parse(input);
    cout << "Before parsing" << endl;
    Vector3dd result(1.5,2.5,5.5);
    cout << result << endl;
    {
        JSONModernReader getter(document);
        getter.visit(result, "vector");
    }
    cout << "After parsing" << endl;
    cout << result << endl;

    CORE_ASSERT_TRUE(result.notTooFar(Vector3dd(1,2,3)), "Fail to parse trivial data structure");
}

void testJSONModern_saveDistortion()
{
    cout << std::endl;
    cout << "testRapidJSON_saveDistortion()" << std::endl;
    cout << std::endl;

    LensDistortionModelParameters params;
    LensDistortionModelParameters loaded;

    params.setPrincipalX(400);
    params.setPrincipalY(400);
    params.setTangentialX(0.3);
    params.setTangentialY(0.4);

    params.setShiftX(200);
    params.setShiftY(200);

    params.setAspect(10.0);
    params.setScale(1.1);
    params.setKoeff({1.0, 2.0, std::numeric_limits<double>::min()});

    cout << "testJSON_saveDistortion() test called" << endl;
    cout << "Initial state" << endl;
    cout << params << endl;

    {
        JSONSetter saver("out1.json");
        saver.visit(params, "stage1");
    }
    {
        JSONModernReader loader("out1.json");
        loader.visit(loaded, "stage1");
    }

    cout << "After parsing" << endl;
    cout << loaded << endl;

}

void testJSONModernScene()
{
    FixtureScene scene;
    std::string calibration = "/media/workarea/work/data/Measure_31_prototype-3_martishkino-bridge/bridge//bridge_25ph/scene_M31p2_bridge_25ph_v32d.json";
    {
        cout << "Loading :" << calibration << endl;
        JSONModernReader getter(calibration);
       // getter.trace = true;
        if (!getter.hasError())
        {
            getter.visit(scene, "scene");
        } else {
            SYNC_PRINT(("Unable to parse json: Unable to parse json. See log for details"));
        }
    }
    scene.dumpInfo();

}


void testJSONPrinterArrays()
{
    std::vector<std::vector<RgbColorParameters>> rgbArray;
    std::vector<std::vector<RgbColorParameters>> rgbArray1;

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

    std::istringstream is(os.str());
    {
        JSONModernReader reader(is);
        reader.visit(rgbArray1, "test");
    }

    cout << rgbArray1.size();
    std::ostringstream os1;
    {
        JSONPrinter printer(&os1);
        printer.visit(rgbArray1, "test");
    }
    std::cout << os1.str() << endl;


}
#endif

#ifdef WITH_JSONMODERN
TEST(Serialise, testCalstructs)
{
    const char input[] =
            "{\n"
            "  \"extrinsics\": {\n"
            "      \"orientation\": {\n"
            "          \"t\": 0.9998452042040369,\n"
            "          \"x\": 0.00665909472413579,\n"
            "          \"y\": 0.013121745045589037,\n"
            "          \"z\": 0.00964592632158969\n"
            "      },\n"
            "      \"position\": {\n"
            "          \"x\": -0.13825168557091438,\n"
            "          \"y\": 0.003052130148347319,\n"
            "          \"z\": -0.003139061540835704\n"
            "      }\n"
            "  }\n"
            "}\n";


    CameraLocationData result;
    nlohmann::json document = nlohmann::json::parse(input);

    SYNC_PRINT(("Parsing %" PRISIZE_T "bytes input:\n%s\n", CORE_COUNT_OF(input), input));
    {
        JSONModernReader getter(document);
        getter.visit(result, "extrinsics");
    }

    CORE_ASSERT_TRUE(result.orientation.notTooFar(Quaternion(0.00665909472413579, 0.013121745045589037,  0.00964592632158969, 0.9998452042040369)), "Orientation parse error");
    CORE_ASSERT_TRUE(result.position   .notTooFar(Vector3dd(-0.13825168557091438, 0.003052130148347319, -0.003139061540835704)), "Vector parse error");
}

TEST(Serialise, testCalstructs1)
{
    const char input[] =
            "{\n"
            "  \"extrinsics\": {\n"
            "      \"orientation\": {\n"
            "          \"t\": 0.9998452042040369,\n"
            "          \"x\": 0.00665909472413579,\n"
            "          \"y\": 0.013121745045589037,\n"
            "          \"z\": 0.00964592632158969\n"
            "      },\n"
            "      \"rotation\": {\n"
            "          \"t\": 1,\n"
            "          \"x\": 0.0,\n"
            "          \"y\": 0.0,\n"
            "          \"z\": 0.0\n"
            "      },\n"
            "      \"position\": {\n"
            "          \"x\": -0.13825168557091438,\n"
            "          \"y\": 0.003052130148347319,\n"
            "          \"z\": -0.003139061540835704\n"
            "      }\n"
            "  }\n"
            "}\n";


    CameraLocationData result;
    nlohmann::json document = nlohmann::json::parse(input);

    SYNC_PRINT(("Parsing %" PRISIZE_T "bytes input:\n%s\n", CORE_COUNT_OF(input), input));
    {
        JSONModernReader getter(document);
        getter.visit(result, "extrinsics");
    }

    CORE_ASSERT_TRUE(result.orientation.notTooFar(Quaternion(0, 0, 0, 1)), "Orientation parse error");
    CORE_ASSERT_TRUE(result.position   .notTooFar(Vector3dd(-0.13825168557091438, 0.003052130148347319, -0.003139061540835704)), "Vector parse error");
}


TEST(Serialise, testSerialiseFeaturePoint)
{
    const char input[] =
            "{\n"
            "\"point\":\n"
            "  {\n"
            "   \"id\":\"35752448u64\",\n"
            "   \"name\":\"\",\n"
            "   \"position\":{\n"
            "       \"x\":3.200534200836,\n"
            "       \"y\":-9.8502314200793215,\n"
            "       \"z\":17.109340191841447\n"
            "    },\n"
            "   \"hasKnownPosition\":false,\n"
            "   \"reprojectedPosition\":{\n"
            "       \"x\":3.2005342008359579,\n"
            "       \"y\":-9.8502314200792558,\n"
            "       \"z\":17.109340191841284\n"
            "    },\n"
            "    \"hasKnownReprojectedPosition\":true,\n"
            "    \"type\":1,\n"
            "    \"color\":{\n"
            "        \"b\":255,\n"
            "        \"g\":255,\n"
            "        \"r\":0,\n"
            "        \"a\":0\n"
            "     }\n"
            "  }\n"
            "}\n"            ;


    cout << "input:" << endl << input << endl;

    SceneFeaturePoint result;
    nlohmann::json document = nlohmann::json::parse(input);

    SYNC_PRINT(("Parsing %" PRISIZE_T "bytes input:\n%s\n", CORE_COUNT_OF(input), input));
    {
        JSONModernReader getter(document);
        getter.visit(result, "point");
    }

    CORE_ASSERT_TRUE(result.hasKnownPosition            == false, "Has known position read wrongly");
    CORE_ASSERT_TRUE(result.hasKnownReprojectedPosition == true, "Has known reproj position read wrongly");


}

#endif

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


#if 0

int main (int /*argc*/, char ** /*argv*/)
{
    printf("Quick test\n");

//    testXML1();
//    testXML2();

//    testJSON1();
//    testJSON2();
//    testJSON_UInt64();
//    testJSON_saveDistortion();

    /*testJSON_FixtureScene();*/
    // testJSON_StereoScene();

#ifdef WITH_RAPIDJSON
     testRapidJSON1();
     testRapidJSON_saveDistortion();
#endif

#ifdef WITH_JSONMODERN
     testJSONPrinterArrays();
     return 0;
     testJSONModernScene();


     return 0;
     testJSONModern1();
     testJSONModern_saveDistortion();
#endif

	return 0;
}
#endif

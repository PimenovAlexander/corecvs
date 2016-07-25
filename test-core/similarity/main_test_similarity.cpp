/**
 * \file main_test_similarity.cpp
 * \brief This is the main file for the test similarity 
 *
 * \date февр. 13, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <fstream>
#include <vector>
#include "gtest/gtest.h"

#include "global.h"

#include "vector3d.h"
#include "similarityReconstructor.h"
#include "mesh3d.h"

using namespace corecvs;

TEST(Similarity, testSimilarity)
{
    Matrix44 transform = Quaternion::Rotation(Vector3dd(0.345,-0.2,-123), 0.12).toMatrix() * Matrix44::Scale(2.0);

    vector<Vector3dd> data;
    data.push_back(Vector3dd(0,0,0));
    data.push_back(Vector3dd(1,0,0));
    data.push_back(Vector3dd(1,1,0));
    data.push_back(Vector3dd(0,1,0));

    vector<Vector3dd> out;
    for (unsigned i = 0; i < data.size(); i++)
    {
        Vector3dd noise(randRanged<double>(1.0,-1.0), randRanged<double>(1.0,-1.0), randRanged<double>(1.0,-1.0));
        out.push_back(transform * data[i] + noise * 0.00);
    }

    SimilarityReconstructor reconstructor;


    for (unsigned i = 0; i < data.size(); i++)
    {
        reconstructor.addPoint2PointConstraint(data[i], out[i]);
    }

    Similarity sim = reconstructor.getBestSimilarity();

    cout << sim << endl;

    Matrix44 m = sim.toMatrix();

    Mesh3D mesh;
    for (unsigned i = 0; i < data.size(); i++)
    {

        cout << "(" << ((out[i].notTooFar(m * data[i], 1e-7)) ? "V" : "X") << ")  "
             << data[i] << " => " << m * data[i] << " =? " << out[i] <<  endl;

        mesh.addPoint(data[i]);
        mesh.addPoint(out[i]);
        mesh.addPoint(m * data[i]);
    }

    double cost = reconstructor.getCostFunction(sim);
    cout << "Result:" << cost * cost << endl;

    Similarity sim1 = reconstructor.getBestSimilarityLM(sim);
    cost = reconstructor.getCostFunction(sim1);
    cout << "Result LM:" << cost * cost  << endl;

    Similarity sim2 = reconstructor.getBestSimilarityLM(sim);
    cost = reconstructor.getCostFunction(sim2);
    cout << "Result LM(N):" << cost * cost  << endl;


    std::fstream file;
    file.open("out.ply", std::ios::out);
    mesh.dumpPLY(file);
    file.close();
}

TEST(Similarity, testSimilarity1)
{
    vector<Vector3dd> data;
    vector<Vector3dd> out;

    data.push_back(Vector3dd(-0.00186905,   -0.0003186, -0.000661256)); out.push_back(Vector3dd(1639, -1887, 0));
    data.push_back(Vector3dd(   0.353449,   -0.0343923, -0.932715   )); out.push_back(Vector3dd(-2465, 3868, 0));
    data.push_back(Vector3dd(   0.145224,   -0.0125637, -0.321719   )); out.push_back(Vector3dd(0, 0, 0));
    data.push_back(Vector3dd(   0.277315,   -0.0243076, -0.632757   )); out.push_back(Vector3dd(-1391, 1968, 0));
    data.push_back(Vector3dd(   0.408238, -4.44682e-05, -0.109351   )); out.push_back(Vector3dd(-1365, -1841, 0));



    SimilarityReconstructor reconstructor;


    for (unsigned i = 0; i < data.size(); i++)
    {
        reconstructor.addPoint2PointConstraint(data[i], out[i]);
    }

    Similarity sim = reconstructor.getBestSimilarity();

    cout << sim << endl;

    double cost = reconstructor.getCostFunction(sim);
    cout << "Result:" << cost * cost << endl;
/*
    Similarity sim1 = reconstructor.getBestSimilarityLM(sim);
    cost = reconstructor.getCostFunction(sim1);
    cout << "Result LM:" << cost * cost  << endl;

    Similarity sim2 = reconstructor.getBestSimilarityLM(sim);
    cost = reconstructor.getCostFunction(sim2);
    cout << sim2 << endl;
    cout << "Result LM(N):" << cost * cost  << endl;
*/

    Matrix44 m = sim.toMatrix();
//    Matrix44 m1 = sim2.toMatrix();

    Mesh3D mesh;
    mesh.switchColor();
    RGBColor color[] = {RGBColor::Red(), RGBColor::Green(), RGBColor::Blue(), RGBColor::Yellow(), RGBColor::Cyan()};

    Vector3dd sumIn (0.0);
    Vector3dd sumOut(0.0);
    Vector3dd sumTraget(0.0);

    Vector3dd sumSqIn (0.0);
    Vector3dd sumSqOut(0.0);
    Vector3dd sumSqTraget(0.0);

    for (unsigned i = 0; i < data.size(); i++)
    {

        /*cout << "(" << ((out[i].notTooFar(m * data[i], 1e-7)) ? "V" : "X") << ")  "
             << data[i] << " => " << m * data[i] << " =? " << out[i] <<  endl;*/

        sumIn  += data[i];
        sumOut += m * data[i];
        sumTraget += out[i];

        sumSqIn     += data[i] * data[i];
        sumSqOut    += (m * data[i]) * (m * data[i]);
        sumSqTraget += out[i] * out[i];

        //mesh.addPoint(data[i]);
        mesh.currentColor = color[0];
        mesh.addPoint(out[i]);
        mesh.currentColor = color[1];
        mesh.addPoint(m * data[i]);
        //mesh.currentColor = color[2];
        //mesh.addPoint(m1 * data[i]);

        //mesh.addSphere(m * data[i], 0.01, 12);
    }

    cout << "================================" << endl;

    sumIn /= data.size();
    sumOut /= data.size();
    sumTraget /= data.size();

    cout << "sumIn "     << sumIn << endl;
    cout << "sumOut "    << sumOut << endl;
    cout << "sumTraget " << sumTraget  << endl;

    cout << "================================" << endl;

    sumSqIn     = (sumSqIn     / data.size()) - (sumIn * sumIn);
    sumSqOut    = (sumSqOut    / data.size()) - (sumOut * sumOut);
    sumSqTraget = (sumSqTraget / data.size()) - (sumTraget * sumTraget);

    cout << "sumSqIn "     << sumSqIn.sumAllElements() << endl;
    cout << "sumSqOut "    << sumSqOut.sumAllElements()    << endl;
    cout << "sumSqTraget " << sumSqTraget.sumAllElements() << endl;


    std::fstream file;
    file.open("out.ply", std::ios::out);
    mesh.dumpPLY(file);
    file.close();
}

TEST(Similarity, testCostFunction)
{
    Similarity s;
    s.rotation = Quaternion::Rotation(Vector3dd(1.0,2.0,3.0), 4.0);
    s.scaleL = 5.0;
    s.scaleR = 6.0;

    s.shiftL = Vector3dd( 7.0,  8.0,  9.0);
    s.shiftR = Vector3dd(10.0, 11.0, 12.0);

    Matrix44 matrix = s.toMatrix();

    double in[Similarity::PARAM_NUMBER];
    s.fillFunctionInput(in);

    Similarity s1(in);
    Matrix44 matrix1 = s1.toMatrix();

    for(int i = 0; i < Similarity::PARAM_NUMBER; i++)
    {
        cout << in[i] << " ";
    }
    cout << endl;
    cout << "=======" << endl;
    cout << s << endl;
    cout << "=======" << endl;
    cout << s1 << endl;


    cout << "=======" << endl;
    cout << matrix << endl;
    cout << "=======" << endl;
    cout << matrix1 << endl;
}


TEST(Similarity, testConverstions)
{
    Similarity sim;
    sim.shiftL = Vector3dd(0.236471, -0.0143253, -0.399441);
    sim.scaleL = 0.148925;
    sim.shiftR = Vector3dd(-716.4, 421.6, 0);
    sim.scaleR = 2641.6;
    Quaternion Q(0.0914267, -0.686214, 0.716433, -0.0864643);

    cout << ((sim.toMatrix() * sim.shiftL) - sim.shiftR) << endl;

    Vector3dd test = sim.shiftL + Vector3dd(sim.scaleL);

    cout << ((sim.toMatrix() * test) - sim.shiftR - Vector3dd(sim.scaleR)) << endl;
}

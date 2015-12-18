/**
 * \file main_test_rectify.cpp
 * \brief Add Comment Here
 *
 * \date Jul 29, 2010
 * \author alexander
 */

#include <random>
#include <vector>
#include <algorithm>
#include <fstream>
#include "gtest/gtest.h"

#include "global.h"

#include "mathUtils.h"
#include "correspondenceList.h"
#include "matrix.h"
#include "essentialMatrix.h"
#include "ransacEstimator.h"
#include "essentialEstimator.h"

using namespace std;
using namespace corecvs;


const unsigned GRID_STEP = 10;
const unsigned GRID_SIZE = GRID_STEP * GRID_STEP;
const int DEFAULT_SEED = 777;
const int RNG_RETRIES = 16384;

void printMatrixInfo (const Matrix33 &matrix)
{
    printf("Guessed fundamental matrix:\n");
    matrix.print();
    printf("\n");

    Vector3dd E1;
    Vector3dd E2;
    EssentialMatrix(matrix).nullspaces(&E1, &E2);
    printf("Guessed Epipoles:\n");
    E1.print();
    printf("\n\n");
    E2.print();
    printf("\n\n");


    Matrix33  X = matrix;
    Vector3dd W;
    Matrix33  V;
    Matrix::svd(&X, &W, &V);
    printf("Fundamental Singular values:\n");
    W.println();



/*    Matrix33 leftTranstorm;
    Matrix33 rightTranstorm;

    rectifier.getRectificationTransformationSimple1(
        matrix,
        &leftTranstorm,
        &rightTranstorm, Vector3dd(1.0,0,0));

    printf("Guessed left transform:\n");
    leftTranstorm.print();
    printf("\n");
    printf("Guessed right transform:\n");
    rightTranstorm.print();
    printf("\n");*/
}

TEST(Rectification, testFundamentalEstimator)
{
    cout << "=================Fundamental Estimator test===============" << endl;

    CorrespondenceList points;
    for (unsigned i = 0; i < GRID_STEP; i++)
    {
        for (unsigned j = 0; j < GRID_STEP; j++)
        {

            Correspondence corr;
            double step = 1.0 / (GRID_STEP - 1);
            double x = (step * j) - 0.5;
            double y = (step * i) - 0.5;
            double alpha = 0.05;

            Matrix33 rotation = Matrix33::RotationZ(alpha);

            /*TODO: Shift only causes the memory error */
            corr.start = Vector2dd(x, y);
            corr.end = rotation * corr.start + Vector2dd (+ 0.01, - 0.1);

            corr.flags = 0;
            corr.value = 1;

            points.push_back(corr);
        }
    }

    RansacEstimator ransacEstimator(14, 1, 0.000001);
    srand(1);
    Matrix33 F = ransacEstimator.getFundamentalRansac1(&points);
    srand(1);
    Matrix33 F1 = ransacEstimator.getFundamentalRansac1(&points);
    srand(1);
    Matrix33 E = ransacEstimator.getEssentialRansac1(&points);
    srand(1);
    Matrix33 E1 = ransacEstimator.getEssentialRansac1(&points);

    printMatrixInfo(F);
    printf("====================================\n");
    printMatrixInfo(F1);
    printf("==========Essential ================\n");
    printMatrixInfo(E);
    printf("====================================\n");
    printMatrixInfo(E1);

}

TEST(Rectification, testSVDDesc)
{
    Matrix33  R = Matrix33::RotationZ(M_PI/6);
    Vector3dd t = Vector3dd(2.0, 1.0, 0.0);
    Matrix33  T = Matrix33::CrossProductLeft(t);
    Matrix33  E = R * T;

    Matrix33  U = E;
    Vector3dd S;
    Matrix33  V;
    Matrix::svdDesc(&U, &S, &V);

    cout << E;
    cout << (U * Matrix33::Scale3(S) * V.transposed());

}

TEST(Rectification, DISABLED_testComposeDecompose)
{
    Matrix33  R = Matrix33::RotationZ(M_PI/6);
    Vector3dd t = Vector3dd(2.0, 1.0, 0.0);
    //Matrix33  T = Matrix33::CrossProductLeft(t);
    EssentialMatrix E;
    E = E.compose(R,t);

    Matrix33 rot[4];
    Vector3dd trans[4];
    E.decompose(rot,trans);

    for(int i = 0; i < 4; i++ )
    {
        cout << "rot: " << endl;
        cout << rot[i] << endl;
        cout << "trans: " << endl;
        cout << trans[i] << endl;
    }
    cout << "R = " << endl;
    cout << R << endl;
    cout << "t = " << endl;
    cout << t << endl;


}


#if NONFREE
TEST(Rectification, test7point)
{
    corecvs::Matrix33 P1(4000.0, 0.0, 2000.0, 0.0, 4000.0, 2000.0, 0.0, 0.0, 1.0);
    corecvs::Matrix33 P2 = P1;
    corecvs::Matrix33 R2(cos(.5), sin(.5), 0, -sin(.5), cos(.5), 0.0, 0.0, 0.0, 1.0);
    corecvs::Vector3dd T(1.0, 2.0, 3.0);

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);
    std::normal_distribution<double> rnorm(0.0, 0.5);

    for (int rngi = 0; rngi < RNG_RETRIES; ++rngi)
    {
        std::vector<Correspondence> cv;
        for (int i = 0; i < 7; ++i)
        {
            corecvs::Vector3dd p(runif(rng), runif(rng), runif(rng));
            auto ppl = P1 * p;
            auto ppr = P2 * (R2 * p + T);
            auto projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
            auto projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            corecvs::Correspondence corr;
            corr.start = projL;
            corr.end   = projR;
            cv.push_back(corr);
        }
        std::vector<Correspondence*> cl;
        for (auto& cc: cv)
            cl.push_back(&cc);
        auto Fv = corecvs::EssentialEstimator().getEssential7point(cl);
        
        for (uint i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            for (auto& F: Fv)
            {
                auto line = F * R;
                double diff = L & line;
                double lineNorm = std::sqrt(line[0] * line[0] + line[1] * line[1]);
                ASSERT_NEAR(diff / lineNorm, 0.0, 1);
            }
        }
    }
}

TEST(Rectification, test5point)
{
    corecvs::Matrix33 P1(4000.0,    0.0, 2000.0,
                            0.0, 4000.0, 2000.0,
                            0.0,    0.0,    1.0);
    corecvs::Matrix33 P2(3000.0,    0.0, 1500.0,
                            0.0, 3000.0, 1500.0,
                            0.0,    0.0,    1.0);
    corecvs::Matrix33  R(cos(.5), sin(.5), 0.0,
                        -sin(.5), cos(.5), 0.0,
                            0.0,     0.0,  1.0);
    corecvs::Vector3dd T(1.0, 2.0, 3.0);

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);
    std::normal_distribution<double> rnorm(0.0, 0.5);

    for (int rngi = 0; rngi < RNG_RETRIES; ++rngi)
    {
        std::vector<Correspondence> cv;
        for (int i = 0; i < 5; ++i)
        {
            corecvs::Vector3dd p(runif(rng), runif(rng), runif(rng));
            auto ppl = P1 * p;
            auto ppr = P2 * (R * p + T);
            auto projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
            auto projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            ppl = P1.inv() * corecvs::Vector3dd(projL[0], projL[1], 1.0);
            ppr = P2.inv() * corecvs::Vector3dd(projR[0], projR[1], 1.0);
            projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
            projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            corecvs::Correspondence corr;
            
            corr.start = projL;
            corr.end   = projR;
            cv.push_back(corr);
        }
        std::vector<Correspondence*> cl;
        for (auto& cc: cv)
            cl.push_back(&cc);
        auto Fv = corecvs::EssentialEstimator().getEssential5point(cl);
        
        for (int i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            for (auto& F: Fv)
            {
                auto line = F * R;
                double diff = L & line;
                double lineNorm = std::sqrt(line[0] * line[0] + line[1] * line[1]);
                ASSERT_NEAR(diff / lineNorm, 0.0, 1);
            }
        }
    }

}
#endif

TEST(Rectification, testEssentialEstimator)
{
    corecvs::Matrix33 P1(4000.0, 0.0, 2000.0, 0.0, 4000.0, 2000.0, 0.0, 0.0, 1.0);
    corecvs::Matrix33 P2 = P1;
    corecvs::Matrix33 R2(cos(.5), sin(.5), 0, -sin(.5), cos(.5), 0.0, 0.0, 0.0, 1.0);
    corecvs::Vector3dd T(1.0, 2.0, 3.0);

    std::mt19937 rng(DEFAULT_SEED);
    std::uniform_real_distribution<double> runif(-1e3, 1e3);
    std::normal_distribution<double> rnorm(0.0, 0.5);

    std::vector<Correspondence> cv;
    for (int i = 0; i < 8; ++i)
    {
        corecvs::Vector3dd p(runif(rng), runif(rng), runif(rng));
        auto ppl = P1 * p;
        auto ppr = P2 * (R2 * p + T);
        auto projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
        auto projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
        corecvs::Correspondence corr;
        corr.start = projL;
        corr.end   = projR;
        cv.push_back(corr);

    }
    std::vector<Correspondence*> cl;
    for (auto& cc: cv)
        cl.push_back(&cc);
    corecvs::Matrix33 F = corecvs::EssentialEstimator().getEssentialLSE(cl);
    
    for (uint i = 0; i < cv.size(); ++i)
    {
        corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
        corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
        double diff = L & (F * R);
        ASSERT_NEAR(diff, 0.0, 1);
    }

    std::vector<double> rmses;
    for (int iii = 0; iii < RNG_RETRIES; ++iii)
    {
        int N = (rng() % 1024) + 8;
        std::vector<Correspondence> cv;
        for (int i = 0; i < N; ++i)
        {
            corecvs::Vector2dd projL, projR;
            do
            {
                corecvs::Vector3dd p(runif(rng), runif(rng), runif(rng));
                auto ppl = P1 * p;
                auto ppr = P2 * (R2 * p + T);
                projL = corecvs::Vector2dd(ppl[0], ppl[1]) / ppl[2];
                projR = corecvs::Vector2dd(ppr[0], ppr[1]) / ppr[2];
            } while(projL[0] < 0 || projR[0] < 0 || projL[1] < 0 || projR[1] < 0 ||
                    projL[0] > 4000 || projR[0] > 4000 || projL[1] > 4000 || projR[1] > 4000);
            for (int ii = 0; ii < 2; ++ii)
            {
                assert(projL[ii] >= 0.0 && projL[ii] <= 4000);
                assert(projR[ii] >= 0.0 && projR[ii] <= 4000);
            }

            corecvs::Correspondence corr;
            corr.start = projL;
            corr.end   = projR;
            cv.push_back(corr);
        }
        std::vector<Correspondence*> cl;
        for (auto& cc: cv)
            cl.push_back(&cc);
        auto F1 = corecvs::EssentialEstimator().getEssentialLSE(cl);
        F1.assertRank2();
        for (uint i = 0; i < cl.size(); ++i)
        {
            cl[i]->start += corecvs::Vector2dd(
                    std::min(1.0, std::max(-1.0, rnorm(rng))), 
                    std::min(1.0, std::max(-1.0, rnorm(rng))));
            cl[i]->end += corecvs::Vector2dd(
                    std::min(1.0, std::max(-1.0, rnorm(rng))), 
                    std::min(1.0, std::max(-1.0, rnorm(rng))));
        }
        auto F = corecvs::EssentialEstimator().getEssentialLSE(cl);
        F.assertRank2();
        double rmse1 = 0.0;
        for (uint i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            double diff = L & (F1 * R);
            auto line = F1 * R;
            double ln = std::sqrt(line[0] * line[0] + line[1] * line[1]);
            diff = diff / ln;
            rmse1 += diff * diff;
        }
        rmse1 = std::sqrt(rmse1 / cv.size());
        ASSERT_NEAR(rmse1, 0.0, 5);
       
        double rmse = 0.0;
        for (uint i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            double diff = L & (F1 * R);
            auto line = F1 * R;
            double ln = std::sqrt(line[0] * line[0] + line[1] * line[1]);
            diff = diff / ln;
            rmse += diff * diff;
        }
        rmse = std::sqrt(rmse / cv.size());
        //double rmseValid = rmse;
        ASSERT_NEAR(rmse, 0.0, (15.0));
//      std::cout << "ORIG:OK" << std::endl; 
        rmse = 0.0;
        for (uint i = 0; i < cv.size(); ++i)
        {
            corecvs::Vector3dd L(cv[i].start[0], cv[i].start[1], 1.0);
            corecvs::Vector3dd R(cv[i].end[0], cv[i].end[1], 1.0);
            double diff = L & (F * R);
            auto line = F * R;
            double ln = std::sqrt(line[0] * line[0] + line[1] * line[1]);
            diff = diff / ln;
            rmse += diff * diff;
        }
        rmse = std::sqrt(rmse / cv.size());
//      ASSERT_TRUE(rmseValid >= rmse);
        rmses.push_back(rmse);   
    }
    std::ofstream of;
    of.open("errors_fest_n.m", std::ios_base::app);
    of << "EN=[";
    for (auto&r : rmses)
        of << r << ",";
    of << "];";
}

//int main (int /*argC*/, char ** /*argV*/)
//{
//    //vector<int> a;
//    //vector<int>::iterator b = find (a.begin(), a.end(), 1);
//    //printf("%s\n", b == a.end() ? "true" : "flase");

//    //testSVDDesc();
//    //testFundamentalEstimator ();
//    testComposeDecompose();


//}


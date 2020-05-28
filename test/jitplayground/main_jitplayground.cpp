#include <cmath>
#include <stdio.h>
#include <fstream>

#include <iostream>
#include <random>

#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/geometry/mesh/mesh3d.h"

#include "reprojectionCostFunction.h"
#include "core/meta/astNode.h"

#include "dllFunction.h"


using namespace std;
using namespace corecvs;

FixtureScene *genTest()
{
    std::mt19937 rng;
    std::uniform_real_distribution<double> camPos(-100,100);
    std::normal_distribution<double> pointPos(0, 100);

    double radius = 0.1;

#if 1
    const int FIXTURE_NUM = 16;
    const int CAM_NUM = 6;
    const int POINT_NUM = 100;
#else
    const int FIXTURE_NUM = 1;
    const int CAM_NUM = 3;
    const int POINT_NUM = 5;
#endif

    FixtureScene *scene = new FixtureScene;

    CameraModel model;
    model.getPinhole()->setCx(320);
    model.getPinhole()->setCy(240);
    model.getPinhole()->setFx(589);
    model.getPinhole()->setFy(589);
    model.getPinhole()->setSize(Vector2dd(640, 480));


    for (int idf = 0; idf < FIXTURE_NUM; idf++)
    {
        CameraFixture *fixture = scene->createCameraFixture();
        fixture->setLocation(Affine3DQ::Shift(camPos(rng), camPos(rng), camPos(rng) / 10.0));

        for (int ic = 0; ic < CAM_NUM; ic++)
        {
            FixtureCamera *camera = scene->createCamera();
            char buffer[100];
            snprintf2buf(buffer, "camera %d", ic);
            camera->nameId = buffer;

            camera->copyModelFrom(model);

            Affine3DQ position =
                    Affine3DQ::RotationX(degToRad(180)) *
                    Affine3DQ::RotationZ(degToRad(360.0 / CAM_NUM) * ic) * Affine3DQ::Shift(radius, 0, 0);

            scene->positionCameraInFixture(fixture, camera, position);
            scene->addCameraToFixture(camera, fixture);

        }

        SYNC_PRINT(("Creating station: %d\n", idf));

    }

    for (int ip = 0; ip < POINT_NUM; ip++)
    {
        SceneFeaturePoint *point = scene->createFeaturePoint();
        point->setPosition(Vector3dd(pointPos(rng), pointPos(rng), pointPos(rng) / 10.0));
    }

    SYNC_PRINT(("Projecting points..."));
    PreciseTimer timer = PreciseTimer::currentTime();
    scene->projectForward(SceneFeaturePoint::POINT_ALL, true);
    printf("Projection elapsed %.2lf ms\n", timer.usecsToNow() / 1000.0);
    SYNC_PRINT(("...Done\n"));

    return scene;
}

void generateFileHeader(std::ofstream &generated, int inputs, int outputs)
{
    generated << "struct SparseEntry {" << endl <<
                 "   int i;" << endl <<
                 "   int j;" << endl <<
                 "   double val; " << endl <<
                 "};" << endl << endl;


    generated << "extern \"C\" int  test(int in) {return in+1;}" << endl;
    generated << "extern \"C\" int  input()  {return "<< inputs  << ";}" << endl;
    generated << "extern \"C\" int  output() {return "<< outputs << ";}" << endl;


}

void codeGenerator(
          std::ofstream &generated
        , vector <ASTNode> &elements
        , const vector <std::string> &names = vector <std::string>()
        )
{
    bool extractConstpool = false;

    /* Extact constpool */
    std::unordered_map<double, string> constpool;
    if (extractConstpool)
    {
        SYNC_PRINT(("Constpool extracting...\n"));

        for (size_t i = 0; i < elements.size(); i++)
        {
            elements[i].p->extractConstPool("c", constpool);
        }
        SYNC_PRINT(("finished - size = %" PRISIZE_T "\n", constpool.size()));
    }

    /*Compute hashes*/
    SYNC_PRINT(("Hashing...\n"));
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements[i].p->rehash();
    }
    /* Form subtree dictionary to check for common subexpresstions */
    SYNC_PRINT(("Making cse dictionary...\n"));

    std::unordered_map<uint64_t, ASTNodeInt *> subexpDictionary;
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements[i].p->cseR(subexpDictionary);
    }

    vector<ASTNodeInt *> commonSubexpressions;
    for (auto it : subexpDictionary)
    {
        if (it.second->cseCount == 0)
            continue;
        commonSubexpressions.push_back(it.second);
        it.second->cseName = commonSubexpressions.size();
    }
    std::sort(commonSubexpressions.begin(), commonSubexpressions.end(),
              [](ASTNodeInt *a, ASTNodeInt *b){return a->height < b->height;});

    SYNC_PRINT(("finished - size = %" PRISIZE_T "\n", commonSubexpressions.size() ));

    /** Generate **/

    ASTRenderDec params("", "", false, generated);
    params.cse = &subexpDictionary;

    if (extractConstpool)
    {
        SYNC_PRINT(("Dumping constpool...\n"));
        for (auto it : constpool)
        {
            generated << std::setprecision(17);
            generated << "   double " << it.second << " = " << it.first << ";\n";
        }
    }


    SYNC_PRINT(("Dumping cse trees...\n"));
    for (auto it : commonSubexpressions)
    {
        int cseName = it->cseName;
        generated << "   const double " << "cse" << std::hex << cseName << std::dec << " = ";

        /* Hack node not to be selference. Restore it back afterwards */
        uint64_t hash = it->hash;
        it->hash = 0;
        it->codeGenCpp(0, params);
        it->hash = hash;
        generated << ";\n";
    }


    SYNC_PRINT(("Dumping main code...\n"));
    for (size_t i = 0; i < elements.size(); i++)
    {
        if (i < names.size()) {
            generated << names[i];
        } else {
            char buffer[100];
            snprintf2buf(buffer, "out[%d]", i);
            generated << buffer;
        }
        generated << " = ";

        elements[i].p->codeGenCpp(0, params);
        generated << ";\n";
    }
    SYNC_PRINT(("Finished codegen\n"));
}



int main (void)
{
    Statistics stats;

    ASTContext *mainContext = new ASTContext();
    ASTContext::MAIN_CONTEXT = mainContext;
    PreciseTimer timer;

    stats.startInterval();
    FixtureScene *scene = genTest();
    stats.endInterval("Creating a scene");

    scene->dumpInfo(cout, true);

#ifdef DUMP_SCENE
    Mesh3D dump;
    dump.switchColor();
    CalibrationHelpers().drawScene(dump, *scene, 0.2);
    dump.dumpPLY("large.ply");
#endif


    ReprojectionCostFunction cost(scene);
    vector<double> inD (cost.getInputs());
    vector<double> outD(cost.getOutputs());

    uint64_t doubleNanoDelay = 0;
    /** Double check **/
    {
        cout << "ReprojectionCostFunction will act " << cost.getInputs() << " - " << cost.getOutputs() << endl;

        cost.fillModel(inD.data());

        PreciseTimer timer = PreciseTimer::currentTime();
        for (int count = 0; count < 1000; count++) {
            cost.costFunction<double>(inD.data(), outD.data());
        }
        doubleNanoDelay = timer.nsecsToNow() / 1000;
        SYNC_PRINT(("Function execution time %" PRIu64 " ns \n", doubleNanoDelay));
        stats.setValue("Double single execution (ns)", doubleNanoDelay);

        cout << "Result:" << endl;
        cout << std::setprecision(17);
        double meansq = 0;
        for (size_t i = 0; i < outD.size(); i++)
        {
            meansq += (outD[i] * outD[i]);
            //cout << out[i] << " ";
        }
        cout << " Mean sqdev = " << sqrt(meansq / outD.size());
        cout << endl;
    }
    /** AST **/
    vector<double> outA(cost.getOutputs());
    {
        stats.startInterval();
        vector<ASTNode> ain (cost.getInputs());
        for (size_t i = 0; i < ain.size(); i++)
        {
            char buffer[100];
            snprintf2buf(buffer, "M[%d]", i);
            ain[i] = ASTNode(buffer);
        }

        vector<ASTNode> aout(cost.getOutputs());

        cost.costFunction<ASTNode>(ain.data(), aout.data());
        stats.resetInterval("Creating AST");


        SYNC_PRINT(("Simplifying...\n"));
        cout << std::setprecision(17);
        for (size_t i = 0; i < aout.size(); i++)
        {
            ASTNodeInt *toSimplify = aout[i].p;
            aout[i].p = toSimplify->compute();
        }
        cout << endl;
        stats.resetInterval("Simplifing AST");

#ifndef GC_AFTER_SIMPLIFY
        /**
         * Garbage collection
         **/
        size_t before = ASTContext::MAIN_CONTEXT->nodes.size();
        ASTContext::MAIN_CONTEXT->clear();
        for (size_t i = 0; i < aout.size(); i++)
        {
            ASTContext::MAIN_CONTEXT->mark(aout[i].p);
        }
        ASTContext::MAIN_CONTEXT->sweep();
        size_t after = ASTContext::MAIN_CONTEXT->nodes.size();
        cout << "before: "<< before << " after:" << after << endl;
#endif
        stats.endInterval("Garbage collection");

        ofstream generated("jit.cpp");
        generateFileHeader(generated, cost.getInputs(), cost.getOutputs());

        /**
         * Jacobian computation
         **/
#if 1
        /**
         * Seems like Jacobian is too large to constuct in a single pass *
         **/
        SYNC_PRINT(("Symbolic Jacobian  computation \n"));
        stats.enterContext("Jacobian computation ->");

        int reflushMem = 600;

        timer = PreciseTimer::currentTime();
        int nonTrivial = 0;
        int partNum = 0;

        ASTContext::MAIN_CONTEXT = new ASTContext();
        for (size_t i = 0; i < aout.size();)
        {            
            vector<ASTNode> part;
            vector<string> names;

            SYNC_PRINT(("Computing derivative...\n"));
            stats.startInterval();
            size_t j;
            for (j = i; j < i + reflushMem && j < aout.size(); j++)
            {
                ASTNodeInt *toProcess = aout[j].p;
                vector<string> vars;
                toProcess->getVars(vars);
                nonTrivial += vars.size();

                for (string var : vars)
                {
                    ASTNodeInt *der = toProcess->derivative(var);
                    der = der->compute();
                    int varId = 0;
                    sscanf(var.c_str(), "M[%d]", &varId);

                    ASTNode node;
                    node.p = der;
                    part.push_back(node);

                    char name[100];
                    snprintf2buf(name, "out[%d].i = %d; out[%d].j = %d; out[%d].val",
                                 names.size(), j, names.size(), varId, names.size());
                    names.push_back(name);
                }
            }
            i = j;
            cout << "Part:" << partNum << endl;

            stats.resetInterval("Computig derivatives");

            generated << "extern \"C\" void j"<< partNum << "(const double *M, SparseEntry *out, int *size) {" << endl;
            generated << "   if (out == (void *)0) { *size = " << nonTrivial << "; return;}";
            codeGenerator(generated, part, names);
            generated << "}\n\n";

            partNum++;

            stats.resetInterval("Generating code");
            SYNC_PRINT(("Deleting objects\n"));
            delete_safe(ASTContext::MAIN_CONTEXT);
            ASTContext::MAIN_CONTEXT = new ASTContext();
            SYNC_PRINT(("Cycle finished\n"));
            stats.endInterval("Cleanup");
        }
        cout << "Part Number: " << partNum << endl;

        delete_safe(ASTContext::MAIN_CONTEXT);
        ASTContext::MAIN_CONTEXT = mainContext;

        SYNC_PRINT(("Symbolic Jacobian  %" PRIu64 " ms (%d non trivial elements %.2lf%%  %d per line) \n"
                    , timer.msecsToNow(), nonTrivial
                    , nonTrivial * 100.0 / (cost.getInputs() * cost.getOutputs())
                    , nonTrivial / (cost.getOutputs())
                    ));

        stats.leaveContext();
#endif

        generated << "extern \"C\" void function(const double *M, double *out) {" << endl;
        codeGenerator(generated, aout);
        generated << "}\n\n";

        generated.close();


        BaseTimeStatisticsCollector collector;
        collector.addStatistics(stats);
        collector.printAdvanced();

#if 1

        SYNC_PRINT(("Running GCC compiler...\n"));
        PreciseTimer timer = PreciseTimer::currentTime();
        int result = system("gcc -march=native -shared -fPIC jit.cpp -o jit.so");
        printf("GCC (%d) elapsed %.2lf ms\n", result, timer.usecsToNow() / 1000.0);


        /* Loading DLL*/
        DllFunction dllF("jit.so");

        timer = PreciseTimer::currentTime();
        for (int count = 0; count < 1000; count++) {
            dllF(inD.data(), outA.data());
        }
        SYNC_PRINT(("Function execution time %" PRIu64 " ns \n", timer.nsecsToNow() / 1000));

        cout << "Result:" << endl;
        cout << std::setprecision(17);
        double meansq = 0;
        for (size_t i = 0; i < outA.size(); i++)
        {
            meansq += (outA[i] * outA[i]);
            //cout << out[i] << " ";
        }
        cout << " Mean sqdev = " << sqrt(meansq / outA.size());
        cout << endl;

        timer = PreciseTimer::currentTime();
        corecvs::SparseMatrix J1;
        for (int count = 0; count < 100; count++) {
            J1 = dllF.getJacobian(inD.data());
        }

        //cout << matrix << endl;

        SYNC_PRINT(("Jacobian execution        time %" PRIu64 " ns \n", timer.nsecsToNow() / 100));
        SYNC_PRINT(("Jacobian classic estimate time %" PRIu64 " ns \n", doubleNanoDelay * (nonTrivial + 1)));

        Matrix J = cost.getJacobian(inD.data());

        if (J.numElements() < 1000)
        {
            cout << "H" << J.h << " - " << J1.h << endl;
            cout << "W" << J.w << " - " << J1.w << endl;

            for (int i = 0; i < J.h; i++)
            {
                for (int j = 0; j < J.w; j++)
                {
                    cout << J.element(i, j) << " - " << J1.a(i,j) << endl;
                }
            }
        }
#endif



    }


    return 0;
}

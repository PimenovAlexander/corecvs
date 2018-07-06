/**
 * \file main_test_jit.cpp
 * \brief This is the main file for the test jit 
 *
 * \date сен 02, 2016
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <fstream>
#include <iostream>
#include "gtest/gtest.h"
#include "core/math/matrix/homographyReconstructor.h"
#include "core/meta/astNode.h"
#include "core/meta/packedDerivative.h"

#if defined (__GNUC__)
#include <dlfcn.h>
#endif

#include "core/utils/global.h"

using namespace corecvs;
using namespace std;

void asmF(double * /*in*/, double * /*out*/)
{
#if defined (__GNUC__) && __x86_64

#endif
}

TEST(jit, testasm)
{

#if defined (__GNUC__) && __x86_64

    double sin_a, cos_a, a = 0.5;
    asm ("fldl %2;"
         "fsincos;"
         "fstpl %1;"
         "fstpl %0;" : "=m"(sin_a), "=m"(cos_a) : "m"(a));
    printf("sin(29°) = %f, cos(29°) = %f\n", sin_a, cos_a);
#endif

    double in [5] = {1,2,3,4,5};
    double out[5] = {0};

    asmF(in, out);
    for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
    {
        cout << out[i] << " " << endl;
    }
}

TEST(jit, DISABLED_testjit)
{
    cout << "Starting test <jit>" << endl;
    cout << "This test is x64 and GCC only" << endl;


    HomographyReconstructor example;
    Matrix33 transform = Matrix33::RotateProj(degToRad(10));

    for (int i = 0; i < 3; i++ )
    {
        for (int j = 0; j < 3; j++ )
        {
            Vector2dd start(i,j);
            Vector2dd end = transform * start;
            example.addPoint2PointConstraint(start, end);
        }
    }

    Matrix33 exampleM = Matrix33::RotateProj(degToRad(-5.0)) * Matrix33::ShiftProj(0.4, 0.6);
    double din[8] = {
        exampleM[0], exampleM[1], exampleM[2],
        exampleM[3], exampleM[4], exampleM[5],
        exampleM[6], exampleM[7]
    };


    cout << "Example Matrix " << endl << exampleM << endl;

    /* Double run */
    {
        cout << "Double run.." << endl;

        double out[9*2];

        example.genericCostFunction<double>(din, out);
        for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
        {
            cout << out[i] << endl;
        }
    }

    ASTContext::MAIN_CONTEXT = new ASTContext();

    /* AST run*/
    {
        cout << "AST run.." << endl;

        ASTNode in[8] = {
            ASTNode("M[0]"), ASTNode("M[1]"), ASTNode("M[2]"),
            ASTNode("M[3]"), ASTNode("M[4]"), ASTNode("M[5]"),
            ASTNode("M[6]"), ASTNode("M[7]")
        };
        ASTNode out[9*2];

        example.genericCostFunction<ASTNode>(in, out);

        std::map<std::string, double> binds = {
            {"M[0]", exampleM[0]}, {"M[1]", exampleM[1]}, {"M[2]", exampleM[2]},
            {"M[3]", exampleM[3]}, {"M[4]", exampleM[4]}, {"M[5]", exampleM[5]},
            {"M[6]", exampleM[6]}, {"M[7]", exampleM[7]}};

        for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
        {
            cout << out[i].p->compute(binds)->val << endl;
        }

#if defined (__GNUC__)

        ofstream generated("jit.cpp");
        ASTRenderDec params("", "", false, generated);
        //params.output = generated;

        generated << "extern \"C\" int  test(int in) {return in+1;}" << endl;
        generated << "extern \"C\" void function(double *M, double *out) {" << endl;
        for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
        {
            char buffer[100];
            snprintf2buf(buffer, "out[%d]", i);

            generated << buffer << " = ";
            out[i].p->compute()->codeGenCpp(buffer, params);
            generated << ";\n";
        }
        generated << "}\n" << endl;

        generated.close();

        SYNC_PRINT(("Running GCC compiler...\n"));
        system("gcc -shared -fPIC jit.cpp -o jit.so");

        /*
         *  DLL Load Stuff
         */

        SYNC_PRINT(("Loading DLL...\n"));
        void *handle;
        typedef int  (*TestFunctor)(int);
        typedef void (*FFunctor)(double *, double *);

        handle = dlopen ("jit.so", RTLD_LAZY);
        if (!handle) {
            fprintf (stderr, "%s\n", dlerror());
            exit(1);
        }
        /*char *error =*/ dlerror();    /* Clear any existing error */

        const char *testName = "test";
        void *testF = dlsym(handle, testName);
        if (testF != NULL) {
            TestFunctor testCall = (TestFunctor)testF;
            cout << "Dll call result" << testCall(100) << endl;
        }

        const char *fName = "function";
        void *fF = dlsym(handle, fName);
        if (fF != NULL) {
            FFunctor fCall = (FFunctor)fF;
            double out[9*2];
            fCall(din, out);

            for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
            {
                cout << out[i] << endl;
            }
        }

        /*Constpool test*/
        SYNC_PRINT(("Constpool extracting...\n"));
        std::unordered_map<double, string> constpool;

        for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
        {
            out[i].p->extractConstPool("c", constpool);
        }
        SYNC_PRINT(("finished - size = %d\n", (int)constpool.size()));


#endif

    }

#if 1
    /* Packed derivative run*/
    {
        cout << "PackedDerivative run.." << endl;
        PackedDerivative<8> in[8];
        for (unsigned i = 0; i < CORE_COUNT_OF(in); i++)
        {
            in[i] = PackedDerivative<8>::ID(exampleM[i], i);
        }
        PackedDerivative<8> out[9*2];

        example.genericCostFunction<PackedDerivative<8> >(in, out);

        for (size_t i = 0; i < CORE_COUNT_OF(out); i++)
        {
            cout << out[i] << endl;
        }
    }
#endif

    cout << "Test <jit> PASSED" << endl;
}

class ASTNodeDotProduct : public ASTNodeFunctionPayload {
public:

    // ASTNodeFunctionPayload interface

    virtual int inputNumber() override {
        return 6;
    }

    virtual int outputNumber() override {
        return 1;
    }

    virtual void f(double in[], double out[]) override {
        out[0] = in[0] * in[3] + in[1] * in[4] + in[2] * in[5];
    }

    virtual string getCCode() override {
        return "out[0] = in[0] * in[3] + in[1] * in[4] + in[2] * in[5]";
    }

    virtual ASTNodeFunctionPayload *derivative(int /*input*/) override
    {
        return NULL;
    }
};

TEST(jit, testjitfunction)
{
    ASTContext::MAIN_CONTEXT = new ASTContext();

    cout << "AST functions" << endl;

    ASTNode in[6] = {
         ASTNode("M[0]"), ASTNode("M[1]"), ASTNode("M[2]"),
         ASTNode("M[3]"), ASTNode("M[4]"), ASTNode("M[5]"),
    };
    ASTNodeInt out(ASTNodeInt::OPERATOR_FUNCTION);
    out.payload = new ASTNodeDotProduct();

    for (auto& elem : in)
    {
        elem.p->print();
    }
}

/**
  * \file main_test_meta.cpp
 * \brief This is the main file for the test meta
 *
 * \date дек. 15, 2015
 * \author alexander
 *
 * \ingroup autotest
 */
#include <iostream>
#include <fstream>

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/fixedVector.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/quaternion.h"
#include "core/buffers/abstractBuffer.h"
#include "core/math/matrix/matrixOperations.h"

#include "core/meta/packedDerivative.h"
#include "core/rectification/essentialEstimator.h"
#include "core/stats/calculationStats.h"

#include "core/meta/astNode.h"
#include "core/function/function.h"

using namespace std;
using namespace corecvs;

#if !defined(_WIN32) && !defined(_MSC_VER)

ASTNode operator "" _x(const char * name, size_t /*len*/)
{
    return ASTNode(name);
}

typedef GenericQuaternion<ASTNode> ASTQuaternion;



#if 0
template<class Element>
class AbsMatrix33 : public AbstractBuffer<Element, int>, public MatrixOperationsBase<AbsMatrix33<Element>, Element>
{
public:
    typedef Element ElementType;

    AbsMatrix33() : AbstractBuffer<Element, int>(3,3) {}

    /* Matrix Operations interface */
    Element &atm(int i, int j) {
        return this->element(i, j);
    }
    const Element &atm(int i, int j) const {
        return this->element(i, j);
    }

    static AbsMatrix33 createMatrix(int /*h*/, int /*w*/) {return AbsMatrix33(); }

    /* Additional helper function */
    void fillWithArgs(
             Element _a00, Element _a01, Element _a02,
             Element _a10, Element _a11, Element _a12,
             Element _a20, Element _a21, Element _a22
         )
    {
        this->element(0,0) = _a00;  this->element(0,1) = _a01;   this->element(0,2) = _a02;
        this->element(1,0) = _a10;  this->element(1,1) = _a11;   this->element(1,2) = _a12;
        this->element(2,0) = _a20;  this->element(2,1) = _a21;   this->element(2,2) = _a22;
    }
};
#endif

typedef AbsMatrixFixed<ASTNode, 3, 3> ASTMatrix33;
typedef AbsMatrixFixed<PackedDerivative<1>, 3, 3> PDMatrix33;

TEST(meta, testmeta)
{
    ASTContext::MAIN_CONTEXT = new ASTContext();

    cout << "Starting test <meta>" << endl;

    ASTNode e = (ASTNode("X") * (ASTNode(5.0)  + ASTNode(4.0)));
    e.p->codeGenCpp("test", e.p->identSymDefault);

    e.p->codeGenCpp("test", ASTNodeInt::identSymLine);

    vector<std::string> params;
    e.p->getVars(params);
    for (std::string p : params) {
        cout << p << ", ";
    }

    cout << "Some more advanced stuff" << endl;

    FixedVector<ASTNode, 5> test1;
    FixedVector<ASTNode, 5> test2;

    for (int i = 0; i < test1.LENGTH; i++)
    {
        test1[i] = ASTNode((double)i);
        test2[i] = ASTNode(1);
    }

    ASTNode dotProduct = (test1 & test2);
    ASTRenderDec dec = {"", "", true};
    dotProduct.p->codeGenCpp("dot_product", dec);

    ASTNodeInt *dotRes = dotProduct.p->compute();
    dotRes->codeGenCpp("dot_product_res", dec);


    cout << "Some more stuff" << endl;

    ASTQuaternion Q(  "Qx"_x     , ASTNode("Qy"), ASTNode("Qz"), ASTNode("Qt"));
    ASTQuaternion P(ASTNode("Px"), ASTNode("Py"), ASTNode("Pz"), ASTNode("Pt"));
    ASTQuaternion R(ASTNode("Rx"), ASTNode("Ry"), ASTNode("Rz"), ASTNode("Rt"));

    ASTNode Z = ((Q+(P^R)) & Q);
    Z.p->codeGenCpp("quaternion1", dec);

    ASTNodeInt *Zder = Z.p->derivative(std::string("Qx"));
    Zder->codeGenCpp("dQx", dec);

    ASTNodeInt *Zsimp = Zder->compute();
    Zsimp->codeGenCpp("dQx1", dec);

    ASTNodeInt *ZsimpAt7 = Zsimp->compute({{"Qx", 7.0}});
    ZsimpAt7->codeGenCpp("dQx7", dec);


    delete_safe(ASTContext::MAIN_CONTEXT);

    cout << "Test <meta> PASSED" << endl;
}


class Q2M : public FunctionArgs {
public:
    Q2M() : FunctionArgs(4, 9) {}

    virtual void operator()(const double in[], double out[])
    {
       Quaternion Q(in[0], in[1], in[2], in[3]);
       Matrix33 M = Q.toMatrix();
       for (int i = 0; i < M.H * M.W; i++)
           out[i] = M.element[i];

        /*for (int i = 0; i < 9; i++)
        {
            out[i] = (i < 4) ? in[i] : 1.0;
        }
        out[0] = in[0] * in[0];
        out[1] = sqrt(in[1]);*/
    }
};


TEST(meta, pdExample)
{
    typedef PackedDerivative<2> Derivative2D;

    {
        Derivative2D F = Derivative2D::X(5.0);
        std::cout << F << endl;
    }

    {
        Derivative2D B = Derivative2D::X(5.0);
        Derivative2D F = B * B;
        std::cout << F << endl;
    }

    {
        Derivative2D B = Derivative2D::X(5.0);
        Derivative2D F = sqrt(B);
        std::cout << F << endl;
    }
}

TEST(meta, matrixExample)
{
    ASTContext::MAIN_CONTEXT = new ASTContext();

    /* Symbol presentation */
    ASTQuaternion Q("x"_x, "y"_x, "z"_x, "t"_x);
    ASTMatrix33 M = Q.toMatrixGeneric<ASTMatrix33>();

    /*ASTMatrix33 M;
    M.atm(0, 0) = "x"_x * "x"_x;
    M.atm(0, 1) = sqrt("y"_x);
    M.atm(0, 2) = "z"_x;
    M.atm(1, 0) = "t"_x;
    M.atm(1, 1) = ASTNode(0.0);
    M.atm(1, 2) = ASTNode(0.0);
    M.atm(2, 0) = ASTNode(0.0);
    M.atm(2, 1) = ASTNode(0.0);
    M.atm(2, 2) = ASTNode(0.0);*/

    ASTRenderDec dec = {"", "", true};

    for (int i = 0; i < 1; i++)
    {
        for (int j = 1; j < 2; j++)
        {
            M.atm(i,j).p->codeGenCpp("matrixFromQ_00", dec);
            M.atm(i,j).p->derivative("y")->compute()->codeGenCpp("matrixFromQ_00", dec);
        }
    }
    /* Compiled presentation */

    Q2M func;


    /* Let's compare  */
    vector<std::string> names = {"x", "y", "z", "t"};
    vector<double> in = {0.2, 0.5, 0.1, 0.5};
    std::map<std::string, double> binds = {{"x", in[0]}, {"y", in[1]}, {"z", in[2]}, {"t", in[3]},};

    vector<double> out(9);
    vector<double> outs(9, numeric_limits<double>::max());

    func(&in[0], &out[0]);
    Matrix J = func.getJacobian(&in[0]);
    Matrix J1(J);

    cout << J << endl;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            /* Value itself */
            ASTNodeInt *r = M.atm(i,j).p->compute(binds);
            if (r->op == ASTNodeInt::OPREATOR_NUM) {
                outs[i * 3 + j] = r->val;
            }

            cout <<  "value: " << out[i * 3 + j] << " "  << outs[i * 3 + j] << endl;

        }
    }

    //double maxdiff = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (size_t n = 0; n < names.size(); n++)
            {
                ASTNodeInt *rd = M.atm(i,j).p->derivative(names[n])->compute(binds);
                cout << rd->val << " ";

                J1.a(i*3+j, n) = fabs(rd->val - J.a(i*3+j, n));
            }
            cout << endl;
        }
    }

    cout << J1;


    /*Packed derivative*/
    GenericQuaternion<PackedDerivative<> > QP(PackedDerivative<>::X(in[0]), PackedDerivative<>(in[1]), PackedDerivative<>(in[2]), PackedDerivative<>(in[3]));
    PDMatrix33 MP = QP.toMatrixGeneric<PDMatrix33>();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << MP.atm(i,j) << endl;
        }
    }



    /*First compare the output */
}

TEST(meta, testmeta1)
{
    ASTContext::MAIN_CONTEXT = new ASTContext();

    std::cout << "Starting test <meta>" << std::endl;

    ASTNode e = (ASTNode("X") * (ASTNode(5.0)  + ASTNode(4.0)));
    ASTRenderDec dec = { "", "", true };

    e.p->codeGenCpp("test");
    e.p->codeGenCpp("test", dec);

    std::cout << "Some more advanced stuff" << std::endl;

    FixedVector<ASTNode, 5> test1;
    FixedVector<ASTNode, 5> test2;

    for (int i = 0; i < test1.LENGTH; i++)
    {
        test1[i] = ASTNode((double)i);
        test2[i] = ASTNode(1);
    }

    (test1 & test2).p->codeGenCpp("dot_product", dec);

    std::cout << "Some more stuff" << std::endl;

    GenericQuaternion<ASTNode> Q(ASTNode("Qx"), ASTNode("Qy"), ASTNode("Qz"), ASTNode("Qt"));
    GenericQuaternion<ASTNode> P(ASTNode("Px"), ASTNode("Py"), ASTNode("Pz"), ASTNode("Pt"));
    GenericQuaternion<ASTNode> R(ASTNode("Rx"), ASTNode("Ry"), ASTNode("Rz"), ASTNode("Rt"));
    ((Q+(P^R)) & Q).p->codeGenCpp("quaternion1", dec);

    delete_safe(ASTContext::MAIN_CONTEXT);

    std::cout << "Test <meta> PASSED" << std::endl;
}

#endif

#if !defined(_WIN32) && !defined(_MSC_VER)
TEST(meta, testMetaNodeFunction)
{
    cout << "Starting meta.testMetaNodeFunction" << endl;
    ASTContext::MAIN_CONTEXT = new ASTContext();
    ASTQuaternion Q(ASTNode("Qx"), ASTNode("Qy"), ASTNode("Qz"), ASTNode("Qt"));

    ASTMatrix33 M = Q.toMatrixGeneric<ASTMatrix33>();

    ASTNodeFunctionWrapper F;
    for (int i = 0; i < M.h; i++)
    {
        for (int j = 0; j < M.w; j++)
        {
            F.components.push_back(M.element(i,j).p);
        }
    }

    cout << F.getCCode();
}
#endif


TEST(meta, testShortcutDerivative)
{
    ASTNodeShortcutFunction function;
    function.name = "F(x,y)";
    function.params.push_back("x");
    function.params.push_back("y");
    function.derivatives.push_back("dF/dx");
    function.derivatives.push_back("dF/dy");

    ASTNode example = ASTNode(5) * ASTNode("x") +  ASTNode("y") * ASTNode("F", &function);
    example.p->derivative("x")->codeGenCpp(0, ASTNodeInt::identSymLine);

}


TEST(meta, testEssentialCostFunction)
{

    //double in[7] = {1/2, 1/2, -1/2, 1/2, 0.4, 0.2, 0.3};

    double in[7] = {-0.00913099, 0.0054594, -0.00347481, 0.999937, -0.999999, -0.00125867, -0.00011209};

    vector<Correspondence> data;
    for (int i = 0; i < 1000; i+=10) {
        for (int j = 0; j < 200; j+=10) {
            Vector2dd input(i - 500, j - 10);
            input /= 500.0;

            Vector2dd out = input + Vector2dd(((i / 10 + j) % 10) / 100.0, ((i/ 10 + j) / 2000.0));
            data.push_back(Correspondence(input, out));
        }
    }

    vector<Correspondence *> dataPtr;
    for (size_t i = 0; i < data.size(); i++) {
        dataPtr.push_back(&data[i]);
    }

    EssentialEstimator::CostFunction7toN             old(&dataPtr);
    EssentialEstimator::CostFunction7toNPacked    modern(&dataPtr);
    //EssentialEstimator::CostFunction7toNGenerated    gen(&dataPtr);
    EssentialEstimator::CostFunction7toNGenerated1  gen1(&dataPtr);

#if 0
    cout << "Gen1\n" << gen1  .getJacobian(in) << endl;
    cout << "Old \n" << old   .getJacobian(in) << endl;
    cout << "New \n" << modern.getJacobian(in) << endl;
    //cout << "Gen \n" << gen   .getJacobian(in) << endl;
#endif


    static int LIMIT = 50;

    Statistics stats;
    double noopt = 0;

    for (int i=0; i < LIMIT; i++) {
        Matrix test = gen1.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    for (int i=0; i < LIMIT; i++) {
        Matrix test = modern.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    for (int i=0; i < LIMIT; i++) {
        Matrix test = old.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    /*for (int i=0; i < LIMIT; i++) {
        Matrix test = gen.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }*/

    /*============================*/
    stats.startInterval();
    for (int i=0; i < LIMIT; i++) {
        Matrix test = gen1.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    stats.endInterval("Gen1");

    stats.startInterval();
    for (int i=0; i < LIMIT; i++) {
        Matrix test = modern.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    stats.endInterval("New");

    stats.startInterval();
    for (int i=0; i < LIMIT; i++) {
        Matrix test = old.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    stats.endInterval("Old");

/*    stats.startInterval();
    for (int i=0; i < LIMIT; i++) {
        Matrix test = gen.getJacobian(in);
        noopt += test.a(test.h / 2, test.w / 2);
    }
    stats.endInterval("Gen");*/

    cout << stats << endl;
    BaseTimeStatisticsCollector(stats).printAdvanced();
    cout << noopt << endl;
}

#if 1
TEST(meta, genEssentialCostFunction)
{
    ASTNodeInt *nodeRaw = EssentialEstimator::CostFunction7toNPacked::derivative(EssentialEstimator::CostFunction7toNPacked::essentialAST());
    ASTNodeInt *node = nodeRaw->compute();

    ASTNodeInt *dqx = node->derivative("Qx")->compute();
    ASTNodeInt *dqy = node->derivative("Qy")->compute();
    ASTNodeInt *dqz = node->derivative("Qz")->compute();
    ASTNodeInt *dqt = node->derivative("Qt")->compute();


    ASTNodeInt *dtx = node->derivative("Tx")->compute();
    ASTNodeInt *dty = node->derivative("Ty")->compute();
    ASTNodeInt *dtz = node->derivative("Tz")->compute();

    std::ofstream file("src/open/core/xml/generated/essentialDerivative.cpp");

    ASTRenderDec style("", "", false, file);


    file <<
    "#include \"core/rectification/correspondenceList.h\"\n"
    "#include \"core/rectification/essentialEstimator.h\"\n"
    "\n"
    "namespace corecvs {\n"
    "\n"
    "    void derivative(const double in[], double out[], Correspondence *c) {\n"
    "    double Qx = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_X]; \n"
    "    double Qy = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Y]; \n"
    "    double Qz = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Z]; \n"
    "    double Qt = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_T]; \n"

    "    double Tx = in[EssentialEstimator::CostFunctionBase::TRANSLATION_X]; \n"
    "    double Ty = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Y]; \n"
    "    double Tz = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Z]; \n"

    "    Vector2dd start = c->start;\n"
    "    Vector2dd end   = c->end;\n"
    "    double startx = start.x();\n"
    "    double starty = start.y();\n"
    "    double endx = end.x();\n"
    "    double endy = end.y();\n";

    file << "   double value = ";
    node->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::ROTATION_Q_X]=";
    dqx->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::ROTATION_Q_X]=";
    dqx->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::ROTATION_Q_Y]=";
    dqy->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::ROTATION_Q_Z]=";
    dqz->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::ROTATION_Q_T]=";
    dqt->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::TRANSLATION_X]=";
    dtx->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::TRANSLATION_Y]=";
    dty->codeGenCpp(2, style);
    file << ";\n";

    file << "   out[EssentialEstimator::CostFunctionBase::TRANSLATION_Z]=";
    dtz->codeGenCpp(2, style);
    file << ";\n";

    file <<
    "if (value < 0)\n"
    " for (int i = 0; i < EssentialEstimator::CostFunctionBase::VECTOR_SIZE; i++) out[i] = -out[i];\n";
    file << "} }";
}


TEST(meta, genEssentialCostFunction1)
{
    typedef EssentialEstimator::CostFunction7toNPacked::Matrix33Diff Matrix33Diff;

    //std::ofstream file("src/open/core/xml/generated/essentialDerivative3.cpp");

    std::ofstream file("essentialDerivative3.cpp");
    ASTRenderDec style("", "", false, file);

    Matrix33Diff matrixRaw = EssentialEstimator::CostFunction7toNPacked::essentialAST();

    const char *names[] = {"Qx", "Qy", "Qz", "Qt", "Tx", "Ty", "Tz"};

    file <<
    "#include <vector>\n"
    "#include \"correspondenceList.h\"\n"
    "#include \"essentialEstimator.h\"\n"
    "\n"
    "using namespace std;\n"
    "namespace corecvs {\n"
    "\n"
    "Matrix derivative2(const double in[], const vector<Correspondence *> *samples) {\n"
    "    Matrix result(samples->size(), EssentialEstimator::CostFunctionBase::VECTOR_SIZE);\n"
    "    double Qx = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_X]; \n"
    "    double Qy = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Y]; \n"
    "    double Qz = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Z]; \n"
    "    double Qt = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_T]; \n"

    "    double Tx = in[EssentialEstimator::CostFunctionBase::TRANSLATION_X]; \n"
    "    double Ty = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Y]; \n"
    "    double Tz = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Z]; \n";


    ASTNodeShortcutFunction function[9];
    /*const char *tnames[9] = {
        "a00", "a01", "a02",
        "a10", "a11", "a12",
        "a20", "a21", "a22",
    };*/

    file << "   double m[9];\n";
    file << "   double md[7 * 9];\n";


    vector <ASTNodeInt *> elements;
    vector <std::string>  elementNames;


    int c = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            ostringstream namer;
            namer << "m[" << c << "]";

            ASTNodeInt *node = matrixRaw.atm(i,j).p->compute();
            elements    .push_back(node);
            elementNames.push_back(namer.str());

            function[c].name = namer.str();
            function[c].params     .resize(EssentialEstimator::CostFunctionBase::VECTOR_SIZE);
            function[c].derivatives.resize(EssentialEstimator::CostFunctionBase::VECTOR_SIZE);

            for (int k = 0; k < EssentialEstimator::CostFunctionBase::VECTOR_SIZE; k++)
            {
                ostringstream namer;
                namer << "md[" << c * 7 + k  << "]";

                /*file << "     " << namer.str() << "= ";
                node->derivative(names[k])->compute()->codeGenCpp(2, style);
                file << ";\n";*/
                elements.push_back(node->derivative(names[k])->compute());
                elementNames.push_back(namer.str());

                function[c].params     [k] = names[k];
                function[c].derivatives[k] = namer.str();
            }
            c++;
        }
    }

    /* Ok. CSE. */
    /*Compute hashes*/
    SYNC_PRINT(("Hashing...\n"));
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements[i]->rehash();
    }
    /* Form subtree dictionary to check for common subexpresstions */
    SYNC_PRINT(("Making cse dictionary...\n"));

    std::unordered_map<uint64_t, ASTNodeInt *> subexpDictionary;
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements[i]->cseR(subexpDictionary);
    }

    /* Ok let's sort cse by height */
    vector<ASTNodeInt *> commonSubexpressions;
    for (auto it : subexpDictionary)
    {
        if (it.second->cseCount == 0)
            continue;
        commonSubexpressions.push_back(it.second);
        it.second->cseName = (int)commonSubexpressions.size();
    }
    std::sort(commonSubexpressions.begin(), commonSubexpressions.end(),
              [](ASTNodeInt *a, ASTNodeInt *b){return a->height < b->height;});

    SYNC_PRINT(("finished - size = %" PRISIZE_T "\n", commonSubexpressions.size() ));

    style.cse = &subexpDictionary;

    SYNC_PRINT(("Dumping cse trees...\n"));
    for (auto it : commonSubexpressions)
    {
        int cseName = it->cseName;
        file << "   const double " << "cse" << std::hex << cseName << std::dec << " = ";

        /* Hack node not to be selference. Restore it back afterwards */
        uint64_t hash = it->hash;
        it->hash = 0;
        it->codeGenCpp(0, style);
        it->hash = hash;
        file << ";\n";
    }


    SYNC_PRINT(("Dumping main code...\n"));
    for (size_t i = 0; i < elements.size(); i++)
    {
        if (i < elementNames.size()) {
            file << elementNames[i];
        } else {
            char buffer[100];
            snprintf2buf(buffer, "out[%d]", i);
            file << buffer;
        }
        file << " = ";

        elements[i]->codeGenCpp(0, style);
        file << ";\n";
    }



    /* Ok... All done with fixed part of function */
    file <<
    "\n"
    "   for (size_t i = 0; i < samples->size(); i++)\n"
    "   {\n"
    "        double startx = (*samples)[i]->start.x();\n"
    "        double starty = (*samples)[i]->start.y();\n"
    "        double endx   = (*samples)[i]->end.x();\n"
    "        double endy   = (*samples)[i]->end.y();\n"
    "   \n";

    Matrix33Diff matrix;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int c = i * 3 + j;
            matrix.atm(i,j) = ASTNode(function[c].name.c_str(), &function[c]);
        }
    }
    ASTNodeInt *nodeFinal = EssentialEstimator::CostFunction7toNPacked::derivative(matrix);

    file << "         double v = ";
    nodeFinal->compute()->codeGenCpp(2, style);
    file << ";\n";

    for (int k = 0; k < EssentialEstimator::CostFunctionBase::VECTOR_SIZE; k++)
    {
        file << "         result.element(i," << k << ") = ";
        nodeFinal->derivative(names[k])->compute()->codeGenCpp(2, style);
        file << ";\n";
        file << "         if (v < 0) result.element(i," << k << ") = -result.element(i," << k << ");\n";
    };


    file <<
    "   }\n"
    "   return result;\n"
    "}\n"
    "} // namespace\n";

}

/**

 **/

TEST(meta, genProjectionDerivative)
{
    typedef EssentialEstimator::CostFunction7toNPacked::Matrix33Diff Matrix33Diff;
    std::ofstream file("src/open/core/xml/generated/projectionDerivative.cpp");

    //std::ofstream file("essentialDerivative3.cpp");
    ASTRenderDec style("", "", false, file);

    Matrix33Diff matrixRaw = EssentialEstimator::CostFunction7toNPacked::essentialAST();

    const char *names[] = {"Qx", "Qy", "Qz", "Qt", "Tx", "Ty", "Tz"};

    file <<
    "#include <vector>\n"
    "#include \"correspondenceList.h\"\n"
    "#include \"essentialEstimator.h\"\n"
    "\n"
    "using namespace std;\n"
    "namespace corecvs {\n"
    "\n"
    "Matrix derivative2(const double in[], const vector<Correspondence *> *samples) {\n"
    "    Matrix result(samples->size(), EssentialEstimator::CostFunctionBase::VECTOR_SIZE);\n"
    "    double Qx = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_X]; \n"
    "    double Qy = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Y]; \n"
    "    double Qz = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Z]; \n"
    "    double Qt = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_T]; \n"

    "    double Tx = in[EssentialEstimator::CostFunctionBase::TRANSLATION_X]; \n"
    "    double Ty = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Y]; \n"
    "    double Tz = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Z]; \n";


    ASTNodeShortcutFunction function[9];
    /*const char *tnames[9] = {
        "a00", "a01", "a02",
        "a10", "a11", "a12",
        "a20", "a21", "a22",
    };*/

    file << "   double m[9];\n";
    file << "   double md[7 * 9];\n";


    vector <ASTNodeInt *> elements;
    vector <std::string>  elementNames;


    int c = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            ostringstream namer;
            namer << "m[" << c << "]";

            ASTNodeInt *node = matrixRaw.atm(i,j).p->compute();
            elements    .push_back(node);
            elementNames.push_back(namer.str());

            function[c].name = namer.str();
            function[c].params     .resize(EssentialEstimator::CostFunctionBase::VECTOR_SIZE);
            function[c].derivatives.resize(EssentialEstimator::CostFunctionBase::VECTOR_SIZE);

            for (int k = 0; k < EssentialEstimator::CostFunctionBase::VECTOR_SIZE; k++)
            {
                ostringstream namer;
                namer << "md[" << c * 7 + k  << "]";

                /*file << "     " << namer.str() << "= ";
                node->derivative(names[k])->compute()->codeGenCpp(2, style);
                file << ";\n";*/
                elements.push_back(node->derivative(names[k])->compute());
                elementNames.push_back(namer.str());

                function[c].params     [k] = names[k];
                function[c].derivatives[k] = namer.str();
            }
            c++;
        }
    }

    /* Ok. CSE. */
    /*Compute hashes*/
    SYNC_PRINT(("Hashing...\n"));
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements[i]->rehash();
    }
    /* Form subtree dictionary to check for common subexpresstions */
    SYNC_PRINT(("Making cse dictionary...\n"));

    std::unordered_map<uint64_t, ASTNodeInt *> subexpDictionary;
    for (size_t i = 0; i < elements.size(); i++)
    {
        elements[i]->cseR(subexpDictionary);
    }

    /* Ok let's sort cse by height */
    vector<ASTNodeInt *> commonSubexpressions;
    for (auto it : subexpDictionary)
    {
        if (it.second->cseCount == 0)
            continue;
        commonSubexpressions.push_back(it.second);
        it.second->cseName = (int)commonSubexpressions.size();
    }
    std::sort(commonSubexpressions.begin(), commonSubexpressions.end(),
              [](ASTNodeInt *a, ASTNodeInt *b){return a->height < b->height;});

    SYNC_PRINT(("finished - size = %" PRISIZE_T "\n", commonSubexpressions.size() ));

    style.cse = &subexpDictionary;

    SYNC_PRINT(("Dumping cse trees...\n"));
    for (auto it : commonSubexpressions)
    {
        int cseName = it->cseName;
        file << "   const double " << "cse" << std::hex << cseName << std::dec << " = ";

        /* Hack node not to be selference. Restore it back afterwards */
        uint64_t hash = it->hash;
        it->hash = 0;
        it->codeGenCpp(0, style);
        it->hash = hash;
        file << ";\n";
    }


    SYNC_PRINT(("Dumping main code...\n"));
    for (size_t i = 0; i < elements.size(); i++)
    {
        if (i < elementNames.size()) {
            file << elementNames[i];
        } else {
            char buffer[100];
            snprintf2buf(buffer, "out[%d]", i);
            file << buffer;
        }
        file << " = ";

        elements[i]->codeGenCpp(0, style);
        file << ";\n";
    }



    /* Ok... All done with fixed part of function */
    file <<
    "\n"
    "   for (size_t i = 0; i < samples->size(); i++)\n"
    "   {\n"
    "        double startx = (*samples)[i]->start.x();\n"
    "        double starty = (*samples)[i]->start.y();\n"
    "        double endx   = (*samples)[i]->end.x();\n"
    "        double endy   = (*samples)[i]->end.y();\n"
    "   \n";

    Matrix33Diff matrix;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int c = i * 3 + j;
            matrix.atm(i,j) = ASTNode(function[c].name.c_str(), &function[c]);
        }
    }
    ASTNodeInt *nodeFinal = EssentialEstimator::CostFunction7toNPacked::derivative(matrix);

    file << "         double v = ";
    nodeFinal->compute()->codeGenCpp(2, style);
    file << ";\n";

    for (int k = 0; k < EssentialEstimator::CostFunctionBase::VECTOR_SIZE; k++)
    {
        file << "         result.element(i," << k << ") = ";
        nodeFinal->derivative(names[k])->compute()->codeGenCpp(2, style);
        file << ";\n";
        file << "         if (v < 0) result.element(i," << k << ") = -result.element(i," << k << ");\n";
    };


    file <<
    "   }\n"
    "   return result;\n"
    "}\n"
    "} // namespace\n";

}



#endif

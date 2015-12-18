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
#include "gtest/gtest.h"

#include "global.h"
#include "astNode.h"

#include "fixedVector.h"
#include "matrix33.h"
#include "quaternion.h"

using namespace std;
using namespace corecvs;

TEST(meta, testmeta)
{
    cout << "Starting test <meta>" << endl;

    ASTNode e = (ASTNode("X") * (ASTNode(5.0)  + ASTNode(4.0)));
    e.p->codeGenCpp("test");
    e.p->codeGenCpp("test", {"", ""});

    cout << "Some more advanced stuff" << endl;

    FixedVector<ASTNode, 5> test1;
    FixedVector<ASTNode, 5> test2;

    for (int i = 0; i < test1.LENGTH; i++)
    {
        test1[i] = ASTNode((double)i);
        test2[i] = ASTNode(1);
    }

    (test1 & test2).p->codeGenCpp("dot_product", {"", ""});

    cout << "Some more stuff" << endl;

    GenericQuaternion<ASTNode> Q(ASTNode("Qx"), ASTNode("Qy"), ASTNode("Qz"), ASTNode("Qt"));
    GenericQuaternion<ASTNode> P(ASTNode("Px"), ASTNode("Py"), ASTNode("Pz"), ASTNode("Pt"));
    GenericQuaternion<ASTNode> R(ASTNode("Rx"), ASTNode("Ry"), ASTNode("Rz"), ASTNode("Rt"));
    ((Q+(P^R)) & Q).p->codeGenCpp("quaternion1", {"", ""});


    cout << "Test <meta> PASSED" << endl;
}

/**
 * \file main_test_serializer.cpp
 * \brief This is the main file for the test serializer 
 *
 * \date Nov 26, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>

#ifndef ASSERTS
#define ASSERTS
#endif

#include "global.h"

#include "vector3d.h"
#include "propertyList.h"
#include "propertyListVisitor.h"
#include "triangulator.h"
#include "printerVisitor.h"


using namespace std;
using namespace corecvs;

void testReflection( void )
{
    Vector2dd vec(5.0, 0.4);
    PrinterVisitor visitor;

    visitor.visit(vec, static_cast<const CompositeField *>(NULL));

}



void testSerializer( void )
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

    ASSERT_TRUE(rvec3a == vec3a, "serializer failed 1");
    ASSERT_TRUE(rvec3b == vec3b, "serializer failed 2");
    ASSERT_TRUE(rvecc == vecc,   "serializer failed 3");
    ASSERT_TRUE(rvecd == vecd,   "serializer failed 4");
    ASSERT_TRUE(rvece == vece,   "serializer failed 5");

    cout << "Output" << rvecc << endl;

}

void testSerializer1( void )
{
    RectificationResult result;
    PropertyList list;
    PropertyListWriterVisitor writerVisitor(&list);
    writerVisitor.visit(result, result, "RectificationResult");

    list.save(cout);
}

int main (int /*argC*/, char ** /*argV*/)
{
    testReflection();
    testSerializer();
    testSerializer1();

    cout << "PASSED" << endl;
        return 0;
}

/**
 * \file example.cpp
 * \brief Add Comment Here
 *
 * \date Dec 27, 2011
 * \author alexander
 */

#include <vector>
#include <stddef.h>
#include "example.h"

/**
 *  Looks extreamly unsafe because it depends on the order of static initialisation.
 *  Should check standard if this is ok
 *
 *  Also it's not clear why removing "= Reflection()" breaks the code;
 **/
template<>
Reflection BaseReflection<Example1>::reflection = Reflection();

template<>
int BaseReflection<Example1>::dummy = Example1::staticInit();

//template<>
//Reflection Example1::reflection;

//template<>
//int Example1::dummy = Example1::staticInit();


int Example1::staticInit()
{
    reflection.name = ReflectionNaming("Example", "This is the example class", "We use this class as an example");

    reflection.fields.push_back(
        new IntField
        (
          Example1::INT_FIELD_ID,
          offsetof(ExampleParamsContainer1, mIntField),
          42,
          "intField",
          "Intrger Field",
          "This is the integer field"
        )
    );
    reflection.fields.push_back(
        new DoubleField
        (
          Example1::DOUBLE_FIELD_ID,
          offsetof(ExampleParamsContainer1, mDoubleField),
           3.14159265,
          "doubleField",
          "Double Field",
          "This is the double field"
        )
    );
    reflection.fields.push_back(
        new BoolField
        (
          Example1::BOOL_FIELD_ID,
          offsetof(ExampleParamsContainer1, mBoolField),
          true,
          "boolField",
          "Boolean Field",
          "This is the bool field"
        )
    );

    return 0;
}








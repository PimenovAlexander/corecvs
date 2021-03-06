/**
 * \file sobelParameters.cpp
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from bufferFilters.xml
 */

#include <vector>
#include <stddef.h>
#include "sobelParameters.h"

/**
 *  Looks extremely unsafe because it depends on the order of static initialization.
 *  Should check standard if this is ok
 *
 *  Also it's not clear why removing "= Reflection()" breaks the code;
 **/

namespace corecvs {
#if 0
template<>
Reflection BaseReflection<SobelParameters>::reflection = Reflection();
template<>
int BaseReflection<SobelParameters>::dummy = SobelParameters::staticInit();
#endif
} // namespace corecvs 

SUPPRESS_OFFSET_WARNING_BEGIN


using namespace corecvs;

int SobelParameters::staticInit(corecvs::Reflection *toFill)
{
    if (toFill == NULL || toFill->objectSize != 0) {
        SYNC_PRINT(("staticInit(): Contract Violation in <SobelParameters>\n"));
         return -1;
    }

    toFill->name = ReflectionNaming(
        "Sobel Parameters",
        "Sobel Parameters",
        ""
    );

     toFill->objectSize = sizeof(SobelParameters);
     

    EnumField* field0 = new EnumField
        (
          SobelParameters::MIXING_TYPE_ID,
          offsetof(SobelParameters, mMixingType),
          0,
          "Mixing Type",
          "Mixing Type",
          "Mixing Type",
          new EnumReflection(2
          , new EnumOption(0,"Sum of Absolute")
          , new EnumOption(1,"L2")
          )
        );
    field0->widgetHint=BaseField::COMBO_BOX;
    toFill->fields.push_back(field0);
    /*  */ 
    BoolField* field1 = new BoolField
        (
          SobelParameters::HORIZONTAL_ID,
          offsetof(SobelParameters, mHorizontal),
          false,
          "Horizontal",
          "Horizontal",
          "Horizontal"
        );
    field1->widgetHint=BaseField::CHECK_BOX;
    toFill->fields.push_back(field1);
    /*  */ 
    BoolField* field2 = new BoolField
        (
          SobelParameters::VERTICAL_ID,
          offsetof(SobelParameters, mVertical),
          false,
          "Vertical",
          "Vertical",
          "Vertical"
        );
    field2->widgetHint=BaseField::CHECK_BOX;
    toFill->fields.push_back(field2);
    /*  */ 
    ReflectionDirectory &directory = *ReflectionDirectoryHolder::getReflectionDirectory();
    directory[std::string("Sobel Parameters")]= toFill;
   return 0;
}
int SobelParameters::relinkCompositeFields()
{
   return 0;
}

SUPPRESS_OFFSET_WARNING_END



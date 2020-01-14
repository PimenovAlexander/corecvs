#ifndef SOBEL_PARAMETERS_H_
#define SOBEL_PARAMETERS_H_
/**
 * \file sobelParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from bufferFilters.xml
 */

#include "core/reflection/reflection.h"
#include "core/reflection/defaultSetter.h"
#include "core/reflection/printerVisitor.h"

/*
 *  Embed includes.
 */
/*
 *  Additional includes for Composite Types.
 */

// using namespace corecvs;

/*
 *  Additional includes for Pointer Types.
 */

// namespace corecvs {
// }
/*
 *  Additional includes for enum section.
 */
#include "core/xml/generated/sobelMixingType.h"

/**
 * \brief Sobel Parameters 
 * Sobel Parameters 
 **/
class SobelParameters : public corecvs::BaseReflection<SobelParameters>
{
public:
    enum FieldId {
        MIXING_TYPE_ID,
        HORIZONTAL_ID,
        VERTICAL_ID,
        SOBEL_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief Mixing Type 
     * Mixing Type 
     */
    int mMixingType;

    /** 
     * \brief Horizontal 
     * Horizontal 
     */
    bool mHorizontal;

    /** 
     * \brief Vertical 
     * Vertical 
     */
    bool mVertical;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit(corecvs::Reflection *toFill);

    static int relinkCompositeFields();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    SobelMixingType::SobelMixingType mixingType() const
    {
        return static_cast<SobelMixingType::SobelMixingType>(mMixingType);
    }

    bool horizontal() const
    {
        return mHorizontal;
    }

    bool vertical() const
    {
        return mVertical;
    }

    /** Section with setters */
    void setMixingType(SobelMixingType::SobelMixingType mixingType)
    {
        mMixingType = mixingType;
    }

    void setHorizontal(bool horizontal)
    {
        mHorizontal = horizontal;
    }

    void setVertical(bool vertical)
    {
        mVertical = vertical;
    }

    /** Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit((int &)mMixingType,         static_cast<const corecvs::EnumField *>(fields()[MIXING_TYPE_ID]));
        visitor.visit(mHorizontal,                static_cast<const corecvs::BoolField *>(fields()[HORIZONTAL_ID]));
        visitor.visit(mVertical,                  static_cast<const corecvs::BoolField *>(fields()[VERTICAL_ID]));
    }

    SobelParameters()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    SobelParameters(
          SobelMixingType::SobelMixingType mixingType
        , bool horizontal
        , bool vertical
    )
    {
        mMixingType = mixingType;
        mHorizontal = horizontal;
        mVertical = vertical;
    }

    /** Exact match comparator **/ 
    bool operator ==(const SobelParameters &other) const 
    {
        if ( !(this->mMixingType == other.mMixingType)) return false;
        if ( !(this->mHorizontal == other.mHorizontal)) return false;
        if ( !(this->mVertical == other.mVertical)) return false;
        return true;
    }
    friend std::ostream& operator << (std::ostream &out, SobelParameters &toSave)
    {
        corecvs::PrinterVisitor printer(out);
        toSave.accept<corecvs::PrinterVisitor>(printer);
        return out;
    }

    void print ()
    {
        std::cout << *this;
    }
};
#endif  //SOBEL_PARAMETERS_H_

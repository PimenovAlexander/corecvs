#ifndef MAKE_PRECISE_PARAMETERS_H_
#define MAKE_PRECISE_PARAMETERS_H_
/**
 * \file makePreciseParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from precise.xml
 */

#include "reflection/reflection.h"
#include "reflection/defaultSetter.h"
#include "reflection/printerVisitor.h"

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
#include "xml/generated/makePreciseAlgorithm.h"
#include "xml/generated/preciseInterpolationType.h"

/**
 * \brief Make Precise Parameters 
 * Make Precise Parameters 
 **/
class MakePreciseParameters : public corecvs::BaseReflection<MakePreciseParameters>
{
public:
    enum FieldId {
        SHOULD_MAKE_PRECISE_ID,
        ALGORITHM_ID,
        INTERPOLATION_ID,
        KLT_ITERATIONS_ID,
        KLT_RADIUS_H_ID,
        KLT_RADIUS_W_ID,
        KLT_THRESHOLD_ID,
        MAKE_PRECISE_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief Should Make Precise 
     * Should Make Precise 
     */
    bool mShouldMakePrecise;

    /** 
     * \brief Algorithm 
     * Algorithm 
     */
    int mAlgorithm;

    /** 
     * \brief Interpolation 
     * Interpolation 
     */
    int mInterpolation;

    /** 
     * \brief KLT iterations 
     * KLT iterations 
     */
    int mKLTIterations;

    /** 
     * \brief KLT Radius H 
     * KLT Radius H 
     */
    int mKLTRadiusH;

    /** 
     * \brief KLT Radius W 
     * KLT Radius W 
     */
    int mKLTRadiusW;

    /** 
     * \brief KLT Threshold 
     * KLT Threshold 
     */
    double mKLTThreshold;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit(corecvs::Reflection *toFill);

    static int relinkCompositeFields();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    bool shouldMakePrecise() const
    {
        return mShouldMakePrecise;
    }

    MakePreciseAlgorithm::MakePreciseAlgorithm algorithm() const
    {
        return static_cast<MakePreciseAlgorithm::MakePreciseAlgorithm>(mAlgorithm);
    }

    PreciseInterpolationType::PreciseInterpolationType interpolation() const
    {
        return static_cast<PreciseInterpolationType::PreciseInterpolationType>(mInterpolation);
    }

    int kLTIterations() const
    {
        return mKLTIterations;
    }

    int kLTRadiusH() const
    {
        return mKLTRadiusH;
    }

    int kLTRadiusW() const
    {
        return mKLTRadiusW;
    }

    double kLTThreshold() const
    {
        return mKLTThreshold;
    }

    /** Section with setters */
    void setShouldMakePrecise(bool shouldMakePrecise)
    {
        mShouldMakePrecise = shouldMakePrecise;
    }

    void setAlgorithm(MakePreciseAlgorithm::MakePreciseAlgorithm algorithm)
    {
        mAlgorithm = algorithm;
    }

    void setInterpolation(PreciseInterpolationType::PreciseInterpolationType interpolation)
    {
        mInterpolation = interpolation;
    }

    void setKLTIterations(int kLTIterations)
    {
        mKLTIterations = kLTIterations;
    }

    void setKLTRadiusH(int kLTRadiusH)
    {
        mKLTRadiusH = kLTRadiusH;
    }

    void setKLTRadiusW(int kLTRadiusW)
    {
        mKLTRadiusW = kLTRadiusW;
    }

    void setKLTThreshold(double kLTThreshold)
    {
        mKLTThreshold = kLTThreshold;
    }

    /** Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mShouldMakePrecise,         static_cast<const corecvs::BoolField *>(fields()[SHOULD_MAKE_PRECISE_ID]));
        visitor.visit((int &)mAlgorithm,          static_cast<const corecvs::EnumField *>(fields()[ALGORITHM_ID]));
        visitor.visit((int &)mInterpolation,      static_cast<const corecvs::EnumField *>(fields()[INTERPOLATION_ID]));
        visitor.visit(mKLTIterations,             static_cast<const corecvs::IntField *>(fields()[KLT_ITERATIONS_ID]));
        visitor.visit(mKLTRadiusH,                static_cast<const corecvs::IntField *>(fields()[KLT_RADIUS_H_ID]));
        visitor.visit(mKLTRadiusW,                static_cast<const corecvs::IntField *>(fields()[KLT_RADIUS_W_ID]));
        visitor.visit(mKLTThreshold,              static_cast<const corecvs::DoubleField *>(fields()[KLT_THRESHOLD_ID]));
    }

    MakePreciseParameters()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    MakePreciseParameters(
          bool shouldMakePrecise
        , MakePreciseAlgorithm::MakePreciseAlgorithm algorithm
        , PreciseInterpolationType::PreciseInterpolationType interpolation
        , int kLTIterations
        , int kLTRadiusH
        , int kLTRadiusW
        , double kLTThreshold
    )
    {
        mShouldMakePrecise = shouldMakePrecise;
        mAlgorithm = algorithm;
        mInterpolation = interpolation;
        mKLTIterations = kLTIterations;
        mKLTRadiusH = kLTRadiusH;
        mKLTRadiusW = kLTRadiusW;
        mKLTThreshold = kLTThreshold;
    }

    /** Exact match comparator **/ 
    bool operator ==(const MakePreciseParameters &other) const 
    {
        if ( !(this->mShouldMakePrecise == other.mShouldMakePrecise)) return false;
        if ( !(this->mAlgorithm == other.mAlgorithm)) return false;
        if ( !(this->mInterpolation == other.mInterpolation)) return false;
        if ( !(this->mKLTIterations == other.mKLTIterations)) return false;
        if ( !(this->mKLTRadiusH == other.mKLTRadiusH)) return false;
        if ( !(this->mKLTRadiusW == other.mKLTRadiusW)) return false;
        if ( !(this->mKLTThreshold == other.mKLTThreshold)) return false;
        return true;
    }
    friend std::ostream& operator << (std::ostream &out, MakePreciseParameters &toSave)
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
#endif  //MAKE_PRECISE_PARAMETERS_H_

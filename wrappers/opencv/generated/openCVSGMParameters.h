#ifndef OPENCV_SGM_PARAMETERS_H_
#define OPENCV_SGM_PARAMETERS_H_
/**
 * \file openCVSGMParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from opencvsgm.xml
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

/**
 * \brief OpenCV SGM Parameters Class 
 * OpenCV SGM Parameters Class 
 **/
class OpenCVSGMParameters : public corecvs::BaseReflection<OpenCVSGMParameters>
{
public:
    enum FieldId {
        PRE_FILTER_CAP_ID,
        SAD_WINDOW_SIZE_ID,
        P1_MULTIPLIER_ID,
        P2_MULTIPLIER_ID,
        MIN_DISPARITY_ID,
        UNIQUENESS_RATIO_ID,
        SPECKLE_WINDOW_SIZE_ID,
        SPECKLE_RANGE_ID,
        DISP_12_MAX_DIFF_ID,
        FULL_DP_ID,
        OPENCV_SGM_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief Pre Filter Cap 
     * Pre Filter Cap 
     */
    int mPreFilterCap;

    /** 
     * \brief SAD Window Size 
     * SAD Window Size 
     */
    int mSADWindowSize;

    /** 
     * \brief P1 multiplier 
     * P1 multiplier 
     */
    int mP1Multiplier;

    /** 
     * \brief P2 multiplier 
     * P2 multiplier 
     */
    int mP2Multiplier;

    /** 
     * \brief Min Disparity 
     * Min Disparity 
     */
    int mMinDisparity;

    /** 
     * \brief Uniqueness Ratio 
     * Uniqueness Ratio 
     */
    int mUniquenessRatio;

    /** 
     * \brief Speckle Window Size 
     * Speckle Window Size 
     */
    int mSpeckleWindowSize;

    /** 
     * \brief Speckle Range 
     * Speckle Range 
     */
    int mSpeckleRange;

    /** 
     * \brief Disp 12 Max Diff 
     * Disp 12 Max Diff 
     */
    int mDisp12MaxDiff;

    /** 
     * \brief Full DP 
     * Full DP 
     */
    bool mFullDP;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit(corecvs::Reflection *toFill);

    static int relinkCompositeFields();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    int preFilterCap() const
    {
        return mPreFilterCap;
    }

    int sADWindowSize() const
    {
        return mSADWindowSize;
    }

    int p1Multiplier() const
    {
        return mP1Multiplier;
    }

    int p2Multiplier() const
    {
        return mP2Multiplier;
    }

    int minDisparity() const
    {
        return mMinDisparity;
    }

    int uniquenessRatio() const
    {
        return mUniquenessRatio;
    }

    int speckleWindowSize() const
    {
        return mSpeckleWindowSize;
    }

    int speckleRange() const
    {
        return mSpeckleRange;
    }

    int disp12MaxDiff() const
    {
        return mDisp12MaxDiff;
    }

    bool fullDP() const
    {
        return mFullDP;
    }

    /* Section with setters */
    void setPreFilterCap(int preFilterCap)
    {
        mPreFilterCap = preFilterCap;
    }

    void setSADWindowSize(int sADWindowSize)
    {
        mSADWindowSize = sADWindowSize;
    }

    void setP1Multiplier(int p1Multiplier)
    {
        mP1Multiplier = p1Multiplier;
    }

    void setP2Multiplier(int p2Multiplier)
    {
        mP2Multiplier = p2Multiplier;
    }

    void setMinDisparity(int minDisparity)
    {
        mMinDisparity = minDisparity;
    }

    void setUniquenessRatio(int uniquenessRatio)
    {
        mUniquenessRatio = uniquenessRatio;
    }

    void setSpeckleWindowSize(int speckleWindowSize)
    {
        mSpeckleWindowSize = speckleWindowSize;
    }

    void setSpeckleRange(int speckleRange)
    {
        mSpeckleRange = speckleRange;
    }

    void setDisp12MaxDiff(int disp12MaxDiff)
    {
        mDisp12MaxDiff = disp12MaxDiff;
    }

    void setFullDP(bool fullDP)
    {
        mFullDP = fullDP;
    }

    /* Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mPreFilterCap,              static_cast<const corecvs::IntField *>(fields()[PRE_FILTER_CAP_ID]));
        visitor.visit(mSADWindowSize,             static_cast<const corecvs::IntField *>(fields()[SAD_WINDOW_SIZE_ID]));
        visitor.visit(mP1Multiplier,              static_cast<const corecvs::IntField *>(fields()[P1_MULTIPLIER_ID]));
        visitor.visit(mP2Multiplier,              static_cast<const corecvs::IntField *>(fields()[P2_MULTIPLIER_ID]));
        visitor.visit(mMinDisparity,              static_cast<const corecvs::IntField *>(fields()[MIN_DISPARITY_ID]));
        visitor.visit(mUniquenessRatio,           static_cast<const corecvs::IntField *>(fields()[UNIQUENESS_RATIO_ID]));
        visitor.visit(mSpeckleWindowSize,         static_cast<const corecvs::IntField *>(fields()[SPECKLE_WINDOW_SIZE_ID]));
        visitor.visit(mSpeckleRange,              static_cast<const corecvs::IntField *>(fields()[SPECKLE_RANGE_ID]));
        visitor.visit(mDisp12MaxDiff,             static_cast<const corecvs::IntField *>(fields()[DISP_12_MAX_DIFF_ID]));
        visitor.visit(mFullDP,                    static_cast<const corecvs::BoolField *>(fields()[FULL_DP_ID]));
    }

    OpenCVSGMParameters()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    OpenCVSGMParameters(
          int preFilterCap
        , int sADWindowSize
        , int p1Multiplier
        , int p2Multiplier
        , int minDisparity
        , int uniquenessRatio
        , int speckleWindowSize
        , int speckleRange
        , int disp12MaxDiff
        , bool fullDP
    )
    {
        mPreFilterCap = preFilterCap;
        mSADWindowSize = sADWindowSize;
        mP1Multiplier = p1Multiplier;
        mP2Multiplier = p2Multiplier;
        mMinDisparity = minDisparity;
        mUniquenessRatio = uniquenessRatio;
        mSpeckleWindowSize = speckleWindowSize;
        mSpeckleRange = speckleRange;
        mDisp12MaxDiff = disp12MaxDiff;
        mFullDP = fullDP;
    }

    bool operator ==(const OpenCVSGMParameters &other) const 
    {
        if ( !(this->mPreFilterCap == other.mPreFilterCap)) return false;
        if ( !(this->mSADWindowSize == other.mSADWindowSize)) return false;
        if ( !(this->mP1Multiplier == other.mP1Multiplier)) return false;
        if ( !(this->mP2Multiplier == other.mP2Multiplier)) return false;
        if ( !(this->mMinDisparity == other.mMinDisparity)) return false;
        if ( !(this->mUniquenessRatio == other.mUniquenessRatio)) return false;
        if ( !(this->mSpeckleWindowSize == other.mSpeckleWindowSize)) return false;
        if ( !(this->mSpeckleRange == other.mSpeckleRange)) return false;
        if ( !(this->mDisp12MaxDiff == other.mDisp12MaxDiff)) return false;
        if ( !(this->mFullDP == other.mFullDP)) return false;
        return true;
    }
    friend std::ostream& operator << (std::ostream &out, OpenCVSGMParameters &toSave)
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
#endif  //OPENCV_SGM_PARAMETERS_H_

#ifndef HEAD_SEARCH_PARAMETERS_H_
#define HEAD_SEARCH_PARAMETERS_H_
/**
 * \file headSearchParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from clustering1.xml
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

/**
 * \brief Head Search Parameters 
 * Head Search Parameters 
 **/
class HeadSearchParameters : public corecvs::BaseReflection<HeadSearchParameters>
{
public:
    enum FieldId {
        THRESHOLD_DISTANCE_ID,
        CLUSTER_DEPTH_ID,
        CLUSTER_MIN_SIZE_ID,
        HEAD_AREA_RADIUS_ID,
        HEAD_NUMBER_ID,
        HEAD_SEARCH_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief Threshold Distance 
     * Threshold Distance in mm 
     */
    double mThresholdDistance;

    /** 
     * \brief Cluster depth 
     * Cluster depth in mm 
     */
    double mClusterDepth;

    /** 
     * \brief Cluster min size 
     * Cluster min size 
     */
    int mClusterMinSize;

    /** 
     * \brief Head area radius 
     * Head area in mm 
     */
    double mHeadAreaRadius;

    /** 
     * \brief Head Number 
     * Expected number of heads 
     */
    int mHeadNumber;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit(corecvs::Reflection *toFill);

    static int relinkCompositeFields();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    double thresholdDistance() const
    {
        return mThresholdDistance;
    }

    double clusterDepth() const
    {
        return mClusterDepth;
    }

    int clusterMinSize() const
    {
        return mClusterMinSize;
    }

    double headAreaRadius() const
    {
        return mHeadAreaRadius;
    }

    int headNumber() const
    {
        return mHeadNumber;
    }

    /* Section with setters */
    void setThresholdDistance(double thresholdDistance)
    {
        mThresholdDistance = thresholdDistance;
    }

    void setClusterDepth(double clusterDepth)
    {
        mClusterDepth = clusterDepth;
    }

    void setClusterMinSize(int clusterMinSize)
    {
        mClusterMinSize = clusterMinSize;
    }

    void setHeadAreaRadius(double headAreaRadius)
    {
        mHeadAreaRadius = headAreaRadius;
    }

    void setHeadNumber(int headNumber)
    {
        mHeadNumber = headNumber;
    }

    /* Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mThresholdDistance,         static_cast<const corecvs::DoubleField *>(fields()[THRESHOLD_DISTANCE_ID]));
        visitor.visit(mClusterDepth,              static_cast<const corecvs::DoubleField *>(fields()[CLUSTER_DEPTH_ID]));
        visitor.visit(mClusterMinSize,            static_cast<const corecvs::IntField *>(fields()[CLUSTER_MIN_SIZE_ID]));
        visitor.visit(mHeadAreaRadius,            static_cast<const corecvs::DoubleField *>(fields()[HEAD_AREA_RADIUS_ID]));
        visitor.visit(mHeadNumber,                static_cast<const corecvs::IntField *>(fields()[HEAD_NUMBER_ID]));
    }

    HeadSearchParameters()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    HeadSearchParameters(
          double thresholdDistance
        , double clusterDepth
        , int clusterMinSize
        , double headAreaRadius
        , int headNumber
    )
    {
        mThresholdDistance = thresholdDistance;
        mClusterDepth = clusterDepth;
        mClusterMinSize = clusterMinSize;
        mHeadAreaRadius = headAreaRadius;
        mHeadNumber = headNumber;
    }

    bool operator ==(const HeadSearchParameters &other) const 
    {
        if ( !(this->mThresholdDistance == other.mThresholdDistance)) return false;
        if ( !(this->mClusterDepth == other.mClusterDepth)) return false;
        if ( !(this->mClusterMinSize == other.mClusterMinSize)) return false;
        if ( !(this->mHeadAreaRadius == other.mHeadAreaRadius)) return false;
        if ( !(this->mHeadNumber == other.mHeadNumber)) return false;
        return true;
    }
    friend std::ostream& operator << (std::ostream &out, HeadSearchParameters &toSave)
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
#endif  //HEAD_SEARCH_PARAMETERS_H_

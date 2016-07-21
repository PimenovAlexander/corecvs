#ifndef SCANNER_PARAMETERS_H_
#define SCANNER_PARAMETERS_H_
/**
 * \file scannerParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */

#include "reflection.h"
#include "defaultSetter.h"
#include "printerVisitor.h"

/*
 *  Embed includes.
 */
/*
 *  Additional includes for Composite Types.
 */

using namespace corecvs;

/*
 *  Additional includes for Pointer Types.
 */

namespace corecvs {
}
/*
 *  Additional includes for enum section.
 */
#include "imageChannel.h"
#include "redRemovalType.h"

/**
 * \brief Scanner parameters 
 * Scanner parameters 
 **/
class ScannerParameters : public BaseReflection<ScannerParameters>
{
public:
    enum FieldId {
        CHANNEL_ID,
        ALGO_ID,
        RED_THRESHOLD_ID,
        HEIGHT_ID,
        GRAPH_LINE_ID,
        USE_SSE_ID,
        CALCULATE_CONVOLUTION_ID,
        CORNER_SCORE_ID,
        SCANNER_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief channel 
     * channel 
     */
    int mChannel;

    /** 
     * \brief algo 
     * algo 
     */
    int mAlgo;

    /** 
     * \brief red threshold 
     * red threshold 
     */
    int mRedThreshold;

    /** 
     * \brief height 
     * height 
     */
    double mHeight;

    /** 
     * \brief Graph line 
     * Graph line 
     */
    int mGraphLine;

    /** 
     * \brief Use SSE 
     * Use SSE 
     */
    bool mUseSSE;

    /** 
     * \brief Calculate convolution 
     * Calculate convolution 
     */
    bool mCalculateConvolution;

    /** 
     * \brief corner Score 
     * corner Score 
     */
    double mCornerScore;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    ImageChannel::ImageChannel channel() const
    {
        return static_cast<ImageChannel::ImageChannel>(mChannel);
    }

    RedRemovalType::RedRemovalType algo() const
    {
        return static_cast<RedRemovalType::RedRemovalType>(mAlgo);
    }

    int redThreshold() const
    {
        return mRedThreshold;
    }

    double height() const
    {
        return mHeight;
    }

    int graphLine() const
    {
        return mGraphLine;
    }

    bool useSSE() const
    {
        return mUseSSE;
    }

    bool calculateConvolution() const
    {
        return mCalculateConvolution;
    }

    double cornerScore() const
    {
        return mCornerScore;
    }

    /* Section with setters */
    void setChannel(ImageChannel::ImageChannel channel)
    {
        mChannel = channel;
    }

    void setAlgo(RedRemovalType::RedRemovalType algo)
    {
        mAlgo = algo;
    }

    void setRedThreshold(int redThreshold)
    {
        mRedThreshold = redThreshold;
    }

    void setHeight(double height)
    {
        mHeight = height;
    }

    void setGraphLine(int graphLine)
    {
        mGraphLine = graphLine;
    }

    void setUseSSE(bool useSSE)
    {
        mUseSSE = useSSE;
    }

    void setCalculateConvolution(bool calculateConvolution)
    {
        mCalculateConvolution = calculateConvolution;
    }

    void setCornerScore(double cornerScore)
    {
        mCornerScore = cornerScore;
    }

    /* Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit((int &)mChannel,            static_cast<const EnumField *>    (fields()[CHANNEL_ID]));
        visitor.visit((int &)mAlgo,               static_cast<const EnumField *>    (fields()[ALGO_ID]));
        visitor.visit(mRedThreshold,              static_cast<const IntField *>     (fields()[RED_THRESHOLD_ID]));
        visitor.visit(mHeight,                    static_cast<const DoubleField *>  (fields()[HEIGHT_ID]));
        visitor.visit(mGraphLine,                 static_cast<const IntField *>     (fields()[GRAPH_LINE_ID]));
        visitor.visit(mUseSSE,                    static_cast<const BoolField *>    (fields()[USE_SSE_ID]));
        visitor.visit(mCalculateConvolution,      static_cast<const BoolField *>    (fields()[CALCULATE_CONVOLUTION_ID]));
        visitor.visit(mCornerScore,               static_cast<const DoubleField *>  (fields()[CORNER_SCORE_ID]));
    }

    ScannerParameters()
    {
        DefaultSetter setter;
        accept(setter);
    }

    ScannerParameters(
          ImageChannel::ImageChannel channel
        , RedRemovalType::RedRemovalType algo
        , int redThreshold
        , double height
        , int graphLine
        , bool useSSE
        , bool calculateConvolution
        , double cornerScore
    )
    {
        mChannel = channel;
        mAlgo = algo;
        mRedThreshold = redThreshold;
        mHeight = height;
        mGraphLine = graphLine;
        mUseSSE = useSSE;
        mCalculateConvolution = calculateConvolution;
        mCornerScore = cornerScore;
    }

    friend ostream& operator << (ostream &out, ScannerParameters &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void print ()
    {
        cout << *this;
    }
};
#endif  //SCANNER_PARAMETERS_H_
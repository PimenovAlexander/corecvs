#ifndef EUCLIDIAN_MOVE_PARAMETERS_H_
#define EUCLIDIAN_MOVE_PARAMETERS_H_
/**
 * \file euclidianMoveParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from parameters.xml
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
 * \brief Euclidian Move Parameters 
 * Euclidian Move Parameters 
 **/
class EuclidianMoveParameters : public corecvs::BaseReflection<EuclidianMoveParameters>
{
public:
    enum FieldId {
        X_ID,
        Y_ID,
        Z_ID,
        ALPHA_ID,
        BETA_ID,
        GAMMA_ID,
        EUCLIDIAN_MOVE_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief X 
     * X 
     */
    double mX;

    /** 
     * \brief Y 
     * Y 
     */
    double mY;

    /** 
     * \brief Z 
     * Z 
     */
    double mZ;

    /** 
     * \brief alpha 
     * alpha 
     */
    double mAlpha;

    /** 
     * \brief beta 
     * beta 
     */
    double mBeta;

    /** 
     * \brief gamma 
     * gamma 
     */
    double mGamma;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit(corecvs::Reflection *toFill);

    static int relinkCompositeFields();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    double x() const
    {
        return mX;
    }

    double y() const
    {
        return mY;
    }

    double z() const
    {
        return mZ;
    }

    double alpha() const
    {
        return mAlpha;
    }

    double beta() const
    {
        return mBeta;
    }

    double gamma() const
    {
        return mGamma;
    }

    /* Section with setters */
    void setX(double x)
    {
        mX = x;
    }

    void setY(double y)
    {
        mY = y;
    }

    void setZ(double z)
    {
        mZ = z;
    }

    void setAlpha(double alpha)
    {
        mAlpha = alpha;
    }

    void setBeta(double beta)
    {
        mBeta = beta;
    }

    void setGamma(double gamma)
    {
        mGamma = gamma;
    }

    /* Section with embedded classes */
    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mX,                         static_cast<const corecvs::DoubleField *>(fields()[X_ID]));
        visitor.visit(mY,                         static_cast<const corecvs::DoubleField *>(fields()[Y_ID]));
        visitor.visit(mZ,                         static_cast<const corecvs::DoubleField *>(fields()[Z_ID]));
        visitor.visit(mAlpha,                     static_cast<const corecvs::DoubleField *>(fields()[ALPHA_ID]));
        visitor.visit(mBeta,                      static_cast<const corecvs::DoubleField *>(fields()[BETA_ID]));
        visitor.visit(mGamma,                     static_cast<const corecvs::DoubleField *>(fields()[GAMMA_ID]));
    }

    EuclidianMoveParameters()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    EuclidianMoveParameters(
          double x
        , double y
        , double z
        , double alpha
        , double beta
        , double gamma
    )
    {
        mX = x;
        mY = y;
        mZ = z;
        mAlpha = alpha;
        mBeta = beta;
        mGamma = gamma;
    }

    bool operator ==(const EuclidianMoveParameters &other) const 
    {
        if ( !(this->mX == other.mX)) return false;
        if ( !(this->mY == other.mY)) return false;
        if ( !(this->mZ == other.mZ)) return false;
        if ( !(this->mAlpha == other.mAlpha)) return false;
        if ( !(this->mBeta == other.mBeta)) return false;
        if ( !(this->mGamma == other.mGamma)) return false;
        return true;
    }
    friend std::ostream& operator << (std::ostream &out, EuclidianMoveParameters &toSave)
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
#endif  //EUCLIDIAN_MOVE_PARAMETERS_H_

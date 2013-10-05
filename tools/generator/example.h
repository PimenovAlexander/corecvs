#ifndef EXAMPLE1_H_
#define EXAMPLE1_H_

#include "reflection.h"
#include "defaultSetter.h"
/**
 * \file example.h
 * \brief a header for example.cpp
 *
 * \date Dec 27, 2011
 * \author alexander
 */


/**
 *  This class is a holder of the fields.
 *  Unfortunately safe getting of the offset is allowed only for the PDO types
 *
 **/
struct ExampleParamsContainer1 {
    /** Section with variables @{ */

    /** This is the integer field */
    int        mIntField;

    /** This is the double field */
    double  mDoubleField;

    /** This is the bool field */
    bool      mBoolField;
    /**<@}*/
};

/**
 *  Main class with getters, setters and stuff
 **/
class Example1 : public BaseReflection<Example1>, public ExampleParamsContainer1
{
public:

    enum FieldId {
        INT_FIELD_ID,
        DOUBLE_FIELD_ID,
        BOOL_FIELD_ID
    };

    static int staticInit();

    /* Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)static_cast<const ExampleParamsContainer1 *>(this) + fields()[fieldId]->offset;
    }

    double getDoubleField() const
    {
        return mDoubleField;
    }

    int getIntField() const
    {
        return mIntField;
    }

    bool isBoolField() const
    {
        return mBoolField;
    }

    /* Section with setters */
    void setBoolField(bool mBoolField)
    {
        this->mBoolField = mBoolField;
    }

    void setDoubleField(double mDoubleField)
    {
        this->mDoubleField = mDoubleField;
    }

    void setIntField(int mIntField)
    {
        this->mIntField = mIntField;
    }

    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mIntField,    static_cast<const IntField *>   (fields()[INT_FIELD_ID]));
        visitor.visit(mDoubleField, static_cast<const DoubleField *>(fields()[DOUBLE_FIELD_ID]));
        visitor.visit(mBoolField,   static_cast<const BoolField *>  (fields()[BOOL_FIELD_ID]));
    }

    Example1()
    {
        DefaultSetter setter;
        accept(setter);
    }

    friend ostream& operator << (ostream &out, Example1 &toSave)
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





#endif  //EXAMPLE1_H_



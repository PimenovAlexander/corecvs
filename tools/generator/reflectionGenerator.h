#ifndef REFLECTION_GENERATOR_H_
#define REFLECTION_GENERATOR_H_
/**
 * \file reflectionGenerator.h
 * \brief A header for reflectionGenerator.c
 *
 *
 *  Seems like this class is becoming obsolete
 *
 * \date Apr 19, 2012
 * \author alexander
 * \ingroup cppcorefiles
 */

#include <QString>
#include "reflection.h"

using corecvs::BaseField;
using corecvs::SimpleScalarField;
using corecvs::SimpleVectorField;
using corecvs::BoolField;
using corecvs::IntField;
using corecvs::DoubleField;
using corecvs::StringField;
using corecvs::WStringField;
using corecvs::CompositeField;
using corecvs::CompositeArrayField;
using corecvs::EnumField;
using corecvs::PointerField;
using corecvs::ReflectionNaming;
using corecvs::Reflection;
using corecvs::EnumReflection;
using corecvs::EnumOption;


class ReflectionGen : public Reflection
{
public:
    const char *uiBaseClass;

    ReflectionGen () : uiBaseClass(NULL) {}
};

template<typename Type>
class SimpleScalarFieldGen : public SimpleScalarField<Type>
{
public:
    SimpleScalarFieldGen (
            const Type _defaultValue,
            const ReflectionNaming &_nameing,
            bool _hasAdditionalValues = false,
            Type _min = Type(0),
            Type _max = Type(0),
            Type _step = Type(0)
    ) : SimpleScalarField<Type>(
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _nameing,
            _hasAdditionalValues,
            _min,
            _max,
            _step
    ) {}

    virtual BaseField* clone() const
    {
        return new SimpleScalarFieldGen(*this);
    }
};

//typedef SimpleScalarFieldGen<int>         IntFieldGen;
typedef SimpleScalarFieldGen<int64_t>     TimestampFieldGen;
//typedef SimpleScalarFieldGen<double>      DoubleFieldGen;
//typedef SimpleScalarFieldGen<std::string> StringFieldGen;


template<typename Type>
class SimpleVectorFieldGen : public SimpleVectorField<Type>
{
public:
    typedef typename SimpleVectorField<Type>::CPPType CPPType;

    SimpleVectorFieldGen (
            const CPPType _defaultValue,
            int _defaultSize,
            const ReflectionNaming &_nameing,
            bool _hasAdditionalValues = false,
            Type _min = Type(0),
            Type _max = Type(0),
            Type _step = Type(0)
    ) : SimpleVectorField<Type>(
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _defaultSize,
            _nameing,
            _hasAdditionalValues,
            _min,
            _max,
            _step
    ) {}

    virtual BaseField* clone() const
    {
        return new SimpleVectorFieldGen(*this);
    }
};

typedef SimpleVectorFieldGen<int>         IntVectorFieldGen;
typedef SimpleVectorFieldGen<double>      DoubleVectorFieldGen;


class StringFieldGen : public StringField
{
public:
    StringFieldGen (
            const std::string _defaultValue,
            const ReflectionNaming &_nameing
    ) : StringField (
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _nameing
    ) {}

    virtual BaseField* clone() const
    {
        return new StringFieldGen(*this);
    }
};

class WStringFieldGen : public WStringField
{
public:
    WStringFieldGen (
            const std::wstring _defaultValue,
            const ReflectionNaming &_nameing
    ) : WStringField (
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _nameing
    ) {}

    virtual BaseField* clone() const
    {
        return new WStringFieldGen(*this);
    }
};

enum BoolWidgetType {
    checkBox,
    radioButton
};

class BoolFieldGen : public BoolField
{
public:
    BoolFieldGen(
            bool _defaultValue,
            const ReflectionNaming &_nameing
    ) : BoolField (
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _nameing
        )
    {}

    virtual BaseField* clone() const
    {
        return new BoolFieldGen(*this);
    }
};

enum DoubleWidgetType {
    doubleSpinBox,
    exponentialSlider
};

class DoubleFieldGen : public DoubleField
{
public:
    DoubleFieldGen(
        double _defaultValue,
        const ReflectionNaming &_nameing,
        bool _hasAdditionalValues = false,
        double _min = 0.0,
        double _max = 0.0,
        double _step = 0.1
        ) : DoubleField (
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _nameing,
            _hasAdditionalValues,
            _min,
            _max,
            _step
        ){}

    virtual BaseField* clone() const
    {
        return new DoubleFieldGen(*this);
    }
};

class IntFieldGen : public IntField
{
public:
    IntFieldGen(
        double _defaultValue,
        const ReflectionNaming &_nameing,
        bool _hasAdditionalValues = false,
        double _min = 0.0,
        double _max = 0.0,
        double _step = 0.1
        ) : IntField (
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
                _defaultValue,
            _nameing,
            _hasAdditionalValues,
            _min,
            _max,
            _step
        )
    {}

    virtual BaseField* clone() const
    {
        return new IntFieldGen(*this);
    }
};

class CompositeFieldGen : public CompositeField
{
public:
    CompositeFieldGen(
            const ReflectionNaming &_nameing,
            const char *_typeName,
            const Reflection *_reflection = NULL
      ) : CompositeField (
              BaseField::UNKNOWN_ID,
              BaseField::UNKNOWN_OFFSET,
              _nameing,
             _typeName,
             _reflection
      ) {}

    virtual BaseField* clone() const
    {
        return new CompositeFieldGen(*this);
    }
};

class CompositeArrayFieldGen : public CompositeArrayField
{
public:
    CompositeArrayFieldGen(
            const ReflectionNaming &_nameing,
            const char *_typeName,
            int _size,
            const Reflection *_reflection = NULL
      ) : CompositeArrayField (
              BaseField::UNKNOWN_ID,
              BaseField::UNKNOWN_OFFSET,
              _nameing,
              _typeName,
              _size,
              _reflection
      ) {}

    virtual BaseField* clone() const
    {
        return new CompositeArrayFieldGen(*this);
    }
};


class EnumOptionGen : public EnumOption
{
public:
    EnumOptionGen(
        int _id,
        const ReflectionNaming &_nameing
    ) : EnumOption(_id,_nameing)
    {
    }
};

class EnumFieldGen : public EnumField
{
public:

    EnumFieldGen(
            int _defaultValue,
            const ReflectionNaming &_nameing,
            const EnumReflection *_enumReflection
    ) : EnumField (
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            _defaultValue,
            _nameing,
            _enumReflection
        )
    {}

    virtual BaseField* clone() const
    {
        return new EnumFieldGen(*this);
    }
};

class PointerFieldGen : public PointerField
{
public:
    PointerFieldGen(
        const ReflectionNaming &_nameing,
        const char *_typeName
    ) :
        PointerField(
            BaseField::UNKNOWN_ID,
            BaseField::UNKNOWN_OFFSET,
            NULL,
            _nameing,
            _typeName
        )
    {}


    virtual BaseField* clone() const
    {
        return new PointerFieldGen(*this);
    }
};


#endif  /* #ifndef REFLECTION_GENERATOR_H_ */



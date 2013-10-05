#ifndef REFLECTION_H_
#define REFLECTION_H_
/**
 * \file reflection.h
 * \brief a header for Reflection.c
 *
 * \date Dec 27, 2011
 * \author alexander
 */

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>


#include "global.h"

//#include "vector3d.h"
//#include "vector2d.h"

#undef min
#undef max


using std::vector;

#if defined(__GNUC__)
#define SUPPRESS_OFFSET_WARNING_BEGIN _Pragma("GCC diagnostic ignored \"-Winvalid-offsetof\"")
#define SUPPRESS_OFFSET_WARNING_END   _Pragma("GCC diagnostic warning \"-Winvalid-offsetof\"")
#elif defined( __clang__ )
#define SUPPRESS_OFFSET_WARNING_BEGIN _Pragma("clang diagnostic ignored \"-Winvalid-offsetof\"")
#define SUPPRESS_OFFSET_WARNING_END   _Pragma("clang diagnostic warning \"-Winvalid-offsetof\"")
#else
#define SUPPRESS_OFFSET_WARNING_BEGIN
#define SUPPRESS_OFFSET_WARNING_END
#endif


namespace corecvs {

/**
 *  We have several styles of nameing
 *
 *  basic - the format in which element is named in XML and reflection
 *  My Favorite Variable
 *
 *  C++  format that is used in the C++ getters/setters
 *  myFavoriteVariable
 *  setMyFavoriteVariable
 *
 *  C++  format that is used in the enums and defines
 *  MY_FAVORITE_VARIABLE
 *
 *
 *
 *
 **/
class ReflectionNaming
{
public:
    const char *name;       /**< The name of the object */
    const char *decription; /**< The description of the object */
    const char *comment;    /**< The comment for the describable object */

    ReflectionNaming() :
        name(NULL),
        decription(NULL),
        comment(NULL)
    {}

    ReflectionNaming(
        const char *_name,
        const char *_decription = NULL,
        const char *_comment = NULL
    ) :
        name(_name),
        decription(_decription == NULL ? _name      : _decription),
        comment   (_comment    == NULL ? decription : _comment)
    {}

};



/**
 *  This class is a core class for the reflection.
 *  It describes a field of the class
 *
 *  TODO: It's better to rename this to property, because in can not only
 *  decribe a field of the class but also an atom of the marshaling interface
 *
 **/
class BaseField /*: public FieldCloner<BaseField>*/
{
protected:
    BaseField(){}

public:
    static const int UNKNOWN_OFFSET = -1;
    static const int UNKNOWN_ID = -1;

    enum FieldType {
        /* ScalarTypes */
        TYPE_INT,
        TYPE_TIMESTAMP,
        TYPE_DOUBLE,
        TYPE_FLOAT,
        TYPE_BOOL,
        TYPE_STRING,
        TYPE_ENUM,

        /* CoreTypes */
        TYPE_VECTOR2DD,
        TYPE_VECTOR3DD,

        /* POINTER_TYPE */
        TYPE_POINTER,

        TYPE_COMPOSITE
    };

    /**
     * This helper typedef will be defined in all the kids of the BaseField
     * and serve as a simple way to get C++ type compatible with the type
     * that is described by the child class
     ***/
    typedef void CPPType;

    /**
     *  This function is called when we need to get type in statically
     *  polymorphic way i.e. get ID of the FieldType by known type
     **/
template<typename Type>
    static FieldType getType() {
        return TYPE_COMPOSITE;
    }

    BaseField(
        int _id,
        FieldType _type,
        const char *_name,
        const char *_decription = NULL,
        const char *_comment = NULL,
        int _offset = UNKNOWN_OFFSET
    ) :
         id(_id)
       , type(_type)
       , name(_name, _decription, _comment)
       , offset(_offset)
       , isAdvanced(false)
    {}

    BaseField(
        int _id,
        FieldType _type,
        const ReflectionNaming &_naming,
        int _offset = UNKNOWN_OFFSET
    ) :
        id(_id)
      , type(_type)
      , name(_naming)
      , offset(_offset)
      , isAdvanced(false)
    {}


    /** Id of the field */
    int id;

    /** Type of the field described by enum */
    FieldType type;
    /** Naming of the reflected class */
    ReflectionNaming name;
    /** Offsets of the field in the class */
    int offset;

    /** flags */
    bool isAdvanced;

    const char *getSimpleName() const
    {
        return name.name;
    }

#define REFLECTION_WITH_VIRTUAL_SUPPORT

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     * This is used when embedding objects in the other objects
     **/
    virtual BaseField* clone() const
    {
        return new BaseField(*this);
    }
#endif

};


/** Template specializations for type trails */
template<> inline BaseField::FieldType BaseField::getType<int>()         { return TYPE_INT;       }
template<> inline BaseField::FieldType BaseField::getType<int64_t>()     { return TYPE_TIMESTAMP; }
template<> inline BaseField::FieldType BaseField::getType<double>()      { return TYPE_DOUBLE;    }
template<> inline BaseField::FieldType BaseField::getType<float>()       { return TYPE_FLOAT;     }
template<> inline BaseField::FieldType BaseField::getType<bool>()        { return TYPE_BOOL;      }
template<> inline BaseField::FieldType BaseField::getType<std::string>() { return TYPE_STRING;    }
//template<> inline BaseField::FieldType BaseField::getType<corecvs::Vector2dd>() { return TYPE_VECTOR2DD; }
//template<> inline BaseField::FieldType BaseField::getType<corecvs::Vector3dd>() { return TYPE_VECTOR3DD; }


/**
 *  This is a simple scalar field
 *
 **/
template<typename Type>
class SimpleScalarField : public BaseField
{
public:
    typedef Type CPPType;

    Type defaultValue;
    bool hasAdditionalValues;
    Type min;
    Type max;
    Type step;

    SimpleScalarField (
            int _id,
            int _offset,
            const Type _defaultValue,
            const char *_name,
            const char *_decription   = NULL,
            const char *_comment      = NULL,
            bool _hasAdditionalValues = false,
            Type _min = 0,
            Type _max = 0,
            Type _step = 0
    ) :
        BaseField(_id, BaseField::getType<Type>(), ReflectionNaming(_name, _decription, _comment) , _offset),
        defaultValue (_defaultValue),
        hasAdditionalValues(_hasAdditionalValues),
        min (_min),
        max (_max),
        step(_step)
    {}

    SimpleScalarField (
            int _id,
            int _offset,
            Type _defaultValue,
            const ReflectionNaming &_naming,
            bool _hasAdditionalValues = false,
            Type _min = 0, // is bad for StringField
            Type _max = 0,
            Type _step = 0
    ) :
        BaseField(_id, BaseField::getType<Type>(), _naming, _offset),
        defaultValue (_defaultValue),
        hasAdditionalValues(_hasAdditionalValues),
        min (_min),
        max (_max),
        step(_step)
    {}

    SimpleScalarField (int _id, Type _defaultValue, const char *_name) :
        BaseField(_id, BaseField::getType<Type>(), _name, NULL, NULL, BaseField::UNKNOWN_OFFSET),
        defaultValue (_defaultValue),
        hasAdditionalValues(false),
        min (0),
        max (0),
        step(0)
    {}

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new SimpleScalarField(*this);
    }
#endif

};

typedef SimpleScalarField<int>         IntField;
typedef SimpleScalarField<int64_t>     TimestampField;
typedef SimpleScalarField<double>      DoubleField;
typedef SimpleScalarField<float>       FloatField;
typedef SimpleScalarField<bool>        BoolField;


class StringField : public BaseField
{
public:
    typedef std::string CPPType;
    std::string defaultValue;

    StringField (
            int _id,
            int _offset,
            std::string _defaultValue,
            const char *_name,
            const char *_decription,
            const char *_comment
    ) :
        BaseField(_id, getType<std::string>(), _name, _decription, _comment, _offset),
        defaultValue (_defaultValue)
    {}

    StringField (
            int _id,
            int _offset,
            std::string _defaultValue,
            const ReflectionNaming &_naming
    ) :
        BaseField(_id, getType<std::string>(), _naming, _offset),
        defaultValue (_defaultValue)
    {}

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new StringField(*this);
    }
#endif

};


/** Small helper struct for fast and convenient initialization of Reflection*/
/*struct StaticReflection {
    BaseField::FieldType type;
    int offset;
    const char *name;
    union {
        int    intVal;
        double doubleVal;
        void * ptrVal;
    } defaultValue;
};
*/

class Reflection;

class EmbedSubclass
{
public:
    Reflection *subclass;
    ReflectionNaming name;

    /* Map that renames fields of the embedded class to fields of current class */
    struct EmbedMap{
        const char *originalName;
        const char *embeddedName;
    };

    vector<EmbedMap> renameing;

    const char *getOrignalName(const char *embeddedName) const
    {
        vector<EmbedMap>::const_iterator it;
        for (it = renameing.begin(); it != renameing.end(); ++it)
        {
            if (strcmp(embeddedName, (*it).embeddedName) == 0)
            {
                return (*it).originalName;
            }
        }
        return embeddedName;
    }

    const char *getEmbeddedName(const char *originalName) const
    {
        vector<EmbedMap>::const_iterator it;
        for (it = renameing.begin(); it != renameing.end(); ++it)
        {
            if (strcmp(originalName, (*it).originalName) == 0)
            {
                return (*it).embeddedName;
            }
        }
        return originalName;
    }
};

/* Reflection for a typical PDO class*/
class Reflection
{
public:
    ReflectionNaming name;
    vector<const BaseField *>     fields;
    /* Seems like used only in generator */
    vector<const EmbedSubclass *> embeds;

    Reflection(){}

    Reflection(int number, ...)
    {
        va_list marker;
        va_start( marker, number );
        for (int i = 0; i < number; i++)
        {
            const BaseField *ref = va_arg( marker, const BaseField *);
            fields.push_back(ref);
        }
        va_end( marker );
    }


    int fieldNumber()
    {
        return (int)fields.size();
    }

    const char *nameById(int fieldId)
    {
        return fields[fieldId]->name.name;
    }

    const char *commentById(int fieldId)
    {
        return fields[fieldId]->name.comment;
    }

/*    ~Reflection()
    {
#ifndef REFLECTION_STATIC_ALLOCATION
        for (int i = 0; i < fieldNumber(); i++) {
            delete fields[i];
        }
        for (int i = 0; i < embeds.size(); i++) {
            delete embeds[i];
        }
#endif
    }*/
};




class CompositeField : public BaseField
{
public:
    const Reflection *reflection;
    const char *typeName;

    CompositeField (
            int _id,
            int _offset,
            const char *_name,
            const char *_typeName,
            const char *_decription = NULL,
            const char *_comment = NULL,
            const Reflection *_reflection = NULL
    ) :
        BaseField(_id, TYPE_COMPOSITE, _name, _decription, _comment, _offset),
        reflection(_reflection),
        typeName(_typeName)
    {}


    CompositeField (
            int _id,
            int _offset,
            const ReflectionNaming &_nameing,
            const char *_typeName,
            const Reflection *_reflection = NULL
    ) :
        BaseField(_id, TYPE_COMPOSITE, _nameing, _offset),
        reflection(_reflection),
        typeName(_typeName)
    {}

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new CompositeField(*this);
    }
#endif

};

/*TODO: This is bad - redo this*/
//typedef SimpleScalarField<corecvs::Vector3dd> Vector3ddField;
//typedef SimpleScalarField<corecvs::Vector2dd> Vector2ddField;

class EnumOption
{
public:
    int id;
    ReflectionNaming name;

    EnumOption() {}
    EnumOption(
        int _id,
        const char *_name,
        const char *_decription,
        const char *_comment
    ) :
        id(_id),
        name(_name, _decription, _comment)
    {}

    EnumOption(
        int _id,
        const ReflectionNaming &_nameing
    ) :
        id(_id),
        name(_nameing)
    {}
};

class EnumReflection {
public:
    ReflectionNaming name;
    vector<const EnumOption *> options;

    int optionsNumber() const
    {
        return (int)options.size();
    }

};

class EnumField : public BaseField
{
public:
    typedef int CPPType;

    int defaultValue;
    const EnumReflection *enumReflection;

    EnumField (
            int _id,
            int _offset,
            int _defaultValue,
            const char *_name,
            const char *_decription,
            const char *_comment,
            const EnumReflection *_enumReflection
    ) :
        BaseField(_id, TYPE_ENUM, _name, _decription, _comment, _offset),
        defaultValue (_defaultValue),
        enumReflection(_enumReflection)
    {}

    EnumField (
            int _id,
            int _offset,
            int _defaultValue,
            const ReflectionNaming &_nameing,
            const EnumReflection *_enumReflection
    ) :
        BaseField(_id, TYPE_ENUM, _nameing, _offset),
        defaultValue (_defaultValue),
        enumReflection(_enumReflection)
    {
        if (enumReflection == NULL)
        {
            printf("Problem with data\n");
        }
    }

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new EnumField(*this);
    }
#endif

};

class PointerField : public BaseField
{
public:
    typedef void * CPPType;
    const char *targetClass;


    PointerField (
            int _id,
            int _offset,
            void * /*_dummy*/,
            const char *_name,
            const char *_decription,
            const char *_comment,
            const char *_targetClass
    ) :
        BaseField(_id, TYPE_POINTER, _name, _decription, _comment, _offset),
        targetClass (_targetClass)
    {}

    PointerField (
            int _id,
            int _offset,
            void * /*_dummy*/,
            const ReflectionNaming &_nameing,
            const char *_targetClass
    ) :
        BaseField(_id, TYPE_POINTER, _nameing, _offset),
        targetClass (_targetClass)
    {}

};

/**
 * Now traits that allow getting reflection from basic type
 **/
template<typename InputType>
struct ReflectionHelper {  typedef CompositeField Type; };

template<> struct ReflectionHelper<int>         { typedef IntField       Type; };
template<> struct ReflectionHelper<double>      { typedef DoubleField    Type; };
template<> struct ReflectionHelper<float>       { typedef FloatField     Type; };
template<> struct ReflectionHelper<int64_t>     { typedef TimestampField Type; };
template<> struct ReflectionHelper<bool>        { typedef BoolField      Type; };
template<> struct ReflectionHelper<std::string> { typedef StringField    Type; };
template<> struct ReflectionHelper<void *>      { typedef PointerField   Type; };


class BaseReflectionStatic
{

};

/**
 *   This is a common parent for a reflection enabled class basically it just transforms
 *   dynamic Reflection class into static field
 *
 **/
template<typename RealThis>
class BaseReflection : public BaseReflectionStatic
{
public:
    static Reflection reflection;
    static int dummy;

public:

    static Reflection *getReflection()
    {
        return &(RealThis::reflection);
    }

    static vector<const BaseField *>& fields()
    {
        return getReflection()->fields;
    }

    static ReflectionNaming & naming()
    {
        return getReflection()->name;
    }

    static int fieldNumber()
    {
        return fields().size();
    }

    static const char *nameById(int fieldId)
    {
        return fields()[fieldId]->name.name;
    }

    static const char *commentById(int fieldId)
    {
        return fields()[fieldId]->name.comment;
    }
};

} //namespace corecvs

#endif  //REFLECTION_H_

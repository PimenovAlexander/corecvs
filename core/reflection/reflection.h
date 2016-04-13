#ifndef REFLECTION_H_
#define REFLECTION_H_
/**
 * \file reflection.h
 * \brief a header for Reflection.c
 *
 * \date Dec 27, 2011
 * \author alexander
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include <vector>
#include <string>

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
    BaseField() {}

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

        TYPE_COMPOSITE,

        /* Array types */
       // TYPE_DOUBLE_ARRAY,    /*< This should be made more generic*/
        TYPE_COMPOSITE_ARRAY,
        TYPE_LAST,

        TYPE_VECTOR_BIT = 0x80
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
    int                 id;

    /** Type of the field described by enum */
    FieldType           type;
    /** Naming of the reflected class */
    ReflectionNaming    name;
    /** Offsets of the field in the class */
    int                 offset;

    /** flags */
    bool                isAdvanced;

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

    virtual ~BaseField() {}
#endif

};


/** Template specializations for type trails */
template<> inline BaseField::FieldType BaseField::getType<int>()                  { return TYPE_INT;       }
template<> inline BaseField::FieldType BaseField::getType<int64_t>()              { return TYPE_TIMESTAMP; }
template<> inline BaseField::FieldType BaseField::getType<double>()               { return TYPE_DOUBLE;    }
template<> inline BaseField::FieldType BaseField::getType<float>()                { return TYPE_FLOAT;     }
template<> inline BaseField::FieldType BaseField::getType<bool>()                 { return TYPE_BOOL;      }
template<> inline BaseField::FieldType BaseField::getType<std::string>()          { return TYPE_STRING;    }

template<> inline BaseField::FieldType BaseField::getType<vector<int> >()         { return (FieldType)(TYPE_VECTOR_BIT | TYPE_INT);       }
template<> inline BaseField::FieldType BaseField::getType<vector<int64_t> >()     { return (FieldType)(TYPE_VECTOR_BIT | TYPE_TIMESTAMP); }
template<> inline BaseField::FieldType BaseField::getType<vector<double> >()      { return (FieldType)(TYPE_VECTOR_BIT | TYPE_DOUBLE);    }
template<> inline BaseField::FieldType BaseField::getType<vector<float> >()       { return (FieldType)(TYPE_VECTOR_BIT | TYPE_FLOAT);     }
template<> inline BaseField::FieldType BaseField::getType<vector<bool> >()        { return (FieldType)(TYPE_VECTOR_BIT | TYPE_BOOL);      }
template<> inline BaseField::FieldType BaseField::getType<vector<std::string> >() { return (FieldType)(TYPE_VECTOR_BIT | TYPE_STRING);    }

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

    Type    defaultValue;
    bool    hasAdditionalValues;
    Type    min;
    Type    max;
    Type    step;

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

    virtual ~SimpleScalarField() {}
#endif

};

typedef SimpleScalarField<uint8_t>     ByteField;
typedef SimpleScalarField<int>         IntField;
typedef SimpleScalarField<int64_t>     TimestampField;
typedef SimpleScalarField<int64_t>     Int64Field;
typedef SimpleScalarField<int64_t>     UInt64Field;
typedef SimpleScalarField<double>      DoubleField;
typedef SimpleScalarField<float>       FloatField;
typedef SimpleScalarField<bool>        BoolField;

/***************************************************************************************
 *
 * simple type vectors
 *
 ***************************************************************************************/
template<typename Type>
class SimpleVectorField : public BaseField
{
public:
    /* Ok... lets try it...*/
    typedef vector<Type> CPPType;
    typedef Type CPPBaseType;

    const CPPType defaultValue;
    unsigned    defaultSize;
    bool        hasAdditionalValues;
    Type        min;
    Type        max;
    Type        step;

    SimpleVectorField (
            int _id,
            int _offset,
            const CPPType _defaultValue,
            int _defaultSize,
            const char *_name,
            const char *_decription   = NULL,
            const char *_comment      = NULL,
            bool _hasAdditionalValues = false,
            Type _min = 0,
            Type _max = 0,
            Type _step = 0
    ) :
        BaseField(_id, BaseField::getType<vector<Type> >(), ReflectionNaming(_name, _decription, _comment) , _offset),
        defaultValue (_defaultValue),
        defaultSize(_defaultSize),
        hasAdditionalValues(_hasAdditionalValues),
        min (_min),
        max (_max),
        step(_step)
    {}

    SimpleVectorField (
            int _id,
            int _offset,
            const Type & /*_defaultValue*/,
            int _defaultSize,
            const char *_name,
            const char *_decription   = NULL,
            const char *_comment      = NULL,
            bool _hasAdditionalValues = false,
            Type _min = 0,
            Type _max = 0,
            Type _step = 0
    ) :
        BaseField(_id, BaseField::getType<vector<Type> >(), ReflectionNaming(_name, _decription, _comment) , _offset),
        /*defaultValue(),*/
        defaultSize(_defaultSize),
        hasAdditionalValues(_hasAdditionalValues),
        min (_min),
        max (_max),
        step(_step)
    {}

    SimpleVectorField (
            int _id,
            int _offset,
            CPPType _defaultValue,
            int _defaultSize,
            const ReflectionNaming &_naming,
            bool _hasAdditionalValues = false,
            Type _min = 0, // is bad for StringField
            Type _max = 0,
            Type _step = 0
    ) :
        BaseField(_id, BaseField::getType<vector<Type> >(), _naming, _offset),
        defaultValue (_defaultValue),
        defaultSize (_defaultSize),
        hasAdditionalValues(_hasAdditionalValues),
        min (_min),
        max (_max),
        step(_step)
    {}

    SimpleVectorField (int _id, int _defaultSize, const char *_name) :
        BaseField(_id, BaseField::getType<CPPType >(), _name, NULL, NULL, BaseField::UNKNOWN_OFFSET),
        defaultSize (_defaultSize),
        hasAdditionalValues(false),
        min (0),
        max (0),
        step(0)
    {}

    SimpleVectorField (int _id, CPPType _defaultValue, int _defaultSize, const char *_name) :
        BaseField(_id, BaseField::getType<CPPType >(), _name, NULL, NULL, BaseField::UNKNOWN_OFFSET),
        defaultValue (_defaultValue),
        defaultSize (_defaultSize),
        hasAdditionalValues(false),
        min (0),
        max (0),
        step(0)
    {}

    Type getDefaultElement(size_t index) const
    {
        if (index < defaultValue.size())
            return defaultValue[index];
        if (!defaultValue.empty())
            return defaultValue.back();
        return Type(0);
    }

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new SimpleVectorField(*this);
    }

    virtual ~SimpleVectorField() {}
#endif

};

typedef SimpleVectorField<int>         IntVectorField;
typedef SimpleVectorField<int64_t>     TimestampVectorField;
typedef SimpleVectorField<double>      DoubleVectorField;
typedef SimpleVectorField<float>       FloatVectorField;
typedef SimpleVectorField<bool>        BoolVectorField;


/* Specific types */

class StringField : public BaseField
{
public:
    typedef std::string CPPType;
    std::string     defaultValue;

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

    virtual ~StringField() {}
#endif

};

class Reflection;

class EmbedSubclass
{
public:
    Reflection       *subclass;
    ReflectionNaming name;

    /* Map that renames fields of the embedded class to fields of current class */
    struct EmbedMap {
        const char *originalName;
        const char *embeddedName;
    };

    vector<EmbedMap> renameing;

    const char *getOrignalName(const char *embeddedName) const
    {
        FOREACH(const EmbedMap& el, renameing)
        {
            if (strcmp(embeddedName, el.embeddedName) == 0)
                return el.originalName;
        }
        return embeddedName;
    }

    const char *getEmbeddedName(const char *originalName) const
    {
        FOREACH(const EmbedMap& el, renameing)
        {
            if (strcmp(originalName, el.originalName) == 0)
                return el.embeddedName;
        }
        return originalName;
    }
};

/* Reflection for a typical PDO class */
class Reflection
{
public:
    ReflectionNaming                name;
    vector<const BaseField *>       fields;
    /* Seems like used only in generator */
    vector<const EmbedSubclass *>   embeds;

    Reflection() {}

    Reflection(int number, ...)
    {
        va_list marker;
        va_start(marker, number);
        for (int i = 0; i < number; i++)
        {
            const BaseField *ref = va_arg(marker, const BaseField *);
            fields.push_back(ref);
        }
        va_end(marker);
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

    int idByName(const char *name)
    {
        for (int i = 0; i < fieldNumber(); i++)
        {
            if (strcmp(fields[i]->name.name, name) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    /*virtual*/ ~Reflection()   // it may be non virtual
    {
//#ifndef REFLECTION_STATIC_ALLOCATION
        FOREACH(const BaseField * el, fields) {
            // crash silly workaround // TODO: review this and fix the problem!
            if (el->id < 0) {
                SYNC_PRINT(("~Reflection: bad id in the reflection object: id:%d size:%d\n", el->id, (int)fields.size()));
            }
            else {
                delete el;
            }
        }
        fields.clear();
        FOREACH (const EmbedSubclass * el, embeds) {
            delete el;
        }
        embeds.clear();
//#endif
    }
};


class CompositeField : public BaseField
{
public:
    const Reflection *reflection;
    const char       *typeName;

    CompositeField (
            int _id,
            int _offset,
            const char *_name,
            const char *_typeName,
            const char *_decription = NULL,
            const char *_comment = NULL,
            const Reflection *_reflection = NULL)
        : BaseField(_id, TYPE_COMPOSITE, _name, _decription, _comment, _offset)
        , reflection(_reflection)
        , typeName(_typeName)
    {}

    CompositeField (
            int _id,
            int _offset,
            const ReflectionNaming &_nameing,
            const char *_typeName,
            const Reflection *_reflection = NULL)
        : BaseField(_id, TYPE_COMPOSITE, _nameing, _offset)
        , reflection(_reflection)
        , typeName(_typeName)
    {}

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new CompositeField(*this);
    }

    virtual ~CompositeField() {}
#endif

};


class CompositeArrayField : public BaseField
{
public:
    const Reflection *reflection;
    const char       *typeName;
    int               size;

    CompositeArrayField(
            int _id,
            int _offset,
            const char *_name,
            const char *_typeName,
            int _size,
            const char *_decription = NULL,
            const char *_comment = NULL,
            const Reflection *_reflection = NULL)
        : BaseField(_id, TYPE_COMPOSITE_ARRAY, _name, _decription, _comment, _offset)
        , reflection(_reflection)
        , typeName(_typeName)
        , size(_size)
    {}

    CompositeArrayField(
            int _id,
            int _offset,
            const ReflectionNaming &_nameing,
            const char *_typeName,
            int _size,
            const Reflection *_reflection = NULL)
        : BaseField(_id, TYPE_COMPOSITE_ARRAY, _nameing, _offset)
        , reflection(_reflection)
        , typeName(_typeName)
        , size(_size)
    {}

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    /**
     * Make a bit-by-bit clone
     **/
    virtual BaseField* clone() const
    {
        return new CompositeArrayField(*this);
    }

    virtual ~CompositeArrayField() {}
#endif

};


/*TODO: This is bad - redo this*/
//typedef SimpleScalarField<corecvs::Vector3dd> Vector3ddField;
//typedef SimpleScalarField<corecvs::Vector2dd> Vector2ddField;

class EnumOption
{
public:
    int              id;
    ReflectionNaming name;

    EnumOption() {}
    EnumOption(int _id,
        const char *_name,
        const char *_decription = NULL,
        const char *_comment = NULL)
        : id(_id)
        , name(_name, _decription, _comment)
    {}

    EnumOption(int _id, const ReflectionNaming &_nameing)
        : id(_id)
        , name(_nameing)
    {}
};

class EnumReflection
{
public:
    ReflectionNaming           name;
    vector<const EnumOption *> options;

    int optionsNumber() const
    {
        return (int)options.size();
    }

    EnumReflection() {}

    EnumReflection(int number, ...)
    {
        va_list marker;
        va_start(marker, number);
        for (int i = 0; i < number; i++)
        {
            const EnumOption *ref = va_arg(marker, const EnumOption *);
            options.push_back(ref);
        }
        va_end(marker);
    }

    /*virtual*/ ~EnumReflection()   // it may be non virtual
    {
        FOREACH (const EnumOption * el, options) {
            delete el;
        }
        options.clear();
    }
};


class EnumField : public BaseField
{
public:
    typedef int           CPPType;

    int                   defaultValue;
    const EnumReflection *enumReflection;

    EnumField(
            int _id,
            int _offset,
            int _defaultValue,
            const char *_name,
            const char *_decription,
            const char *_comment,
            const EnumReflection *_enumReflection)
        : BaseField(_id, TYPE_ENUM, _name, _decription, _comment, _offset)
        , defaultValue (_defaultValue)
        , enumReflection(_enumReflection)
    {}

    EnumField(
            int _id,
            int _offset,
            int _defaultValue,
            const ReflectionNaming &_nameing,
            const EnumReflection *_enumReflection)
        : BaseField(_id, TYPE_ENUM, _nameing, _offset)
        , defaultValue (_defaultValue)
        , enumReflection(_enumReflection)
    {
        if (enumReflection == NULL) {
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

    virtual ~EnumField()    // it must be virtual with virtual ~BaseField()
    {
        delete_safe(enumReflection);
    }
#endif

};


class PointerField : public BaseField
{
public:
    typedef void * CPPType;
    const char *targetClass;

    PointerField(int    _id,
            int         _offset,
            void *    /*_dummy*/,
            const char *_name,
            const char *_decription,
            const char *_comment,
            const char *_targetClass)
        : BaseField(_id, TYPE_POINTER, _name, _decription, _comment, _offset)
        , targetClass(_targetClass)
    {}

    PointerField(int    _id,
            int         _offset,
            void *     /*_dummy*/,
            const ReflectionNaming &_nameing,
            const char *_targetClass)
        : BaseField(_id, TYPE_POINTER, _nameing, _offset)
        , targetClass(_targetClass)
    {}

#ifdef REFLECTION_WITH_VIRTUAL_SUPPORT
    virtual ~PointerField() {}
#endif

};


/**
 * Now traits that allow getting reflection from basic type
 **/
template<typename InputType>
struct ReflectionHelper {  typedef CompositeField Type; };

template<> struct ReflectionHelper<int>         { typedef IntField       Type; };
template<> struct ReflectionHelper<int16_t>     { typedef IntField       Type; };
template<> struct ReflectionHelper<uint32_t>    { typedef IntField       Type; };
template<> struct ReflectionHelper<uint16_t>    { typedef IntField       Type; };
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
    static int        dummy;

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


class DynamicObject
{
public:
    Reflection *reflection;
    void       *rawObject;

    DynamicObject() :
        reflection(NULL),
        rawObject(NULL)
    {}

    template<typename Object>
    DynamicObject(Object *object):
        reflection(BaseReflection<Object>::getReflection()),
        rawObject((void *) object)
    {}

    template<typename Type>
    Type *getField(int fieldId)
    {
        return (Type *)&(((uint8_t*)rawObject)[reflection->fields[fieldId]->offset]);
    }

    const BaseField* getFieldReflection(int fieldId)
    {
        return reflection->fields[fieldId];
    }

};

} //namespace corecvs

#endif  //REFLECTION_H_

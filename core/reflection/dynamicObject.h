#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include "core/reflection/reflection.h"

namespace corecvs {

/**
 * \brief Dynamic Object is an object accessed by pointer, whose type is cleared form C++ point of view,
 * the object is compleatly managed be the reflection
 *
 **/
class DynamicObjectWrapper
{
public:
    const Reflection *reflection;
    void             *rawObject;
    bool              ownsObject = false;

    DynamicObjectWrapper(
        const Reflection *reflection = NULL,
        void       *rawObject = NULL
    ) :
        reflection(reflection),
        rawObject(rawObject)
    {}

    template<typename Object>
    DynamicObjectWrapper(Object *object):
        reflection(BaseReflection<Object>::getReflection()),
        rawObject((void *) object)
    {}

    /**
     *   Please note due to stupid alignment issues on ARM there could be some problems
     **/
    template<typename Type>
    Type *getField(int fieldId)
    {
        return (Type *)&(((uint8_t*)rawObject)[reflection->fields[fieldId]->offset]);
    }

    const BaseField* getFieldReflection(int fieldId)
    {
        return reflection->fields[fieldId];
    }

    bool simulateConstructor();

    template<typename VisitorType>
    void accept(VisitorType &visitor)
    {
        if (reflection == NULL || rawObject == NULL)
            return;

        for (int count = 0; count < reflection->fieldNumber(); count++)
        {
            const BaseField *field = reflection->fields[count];
            switch (field->type) {

            case BaseField::TYPE_BOOL:
            {
                const corecvs::BoolField *descr = static_cast<const BoolField *>(field);
                bool &value = *getField<bool>(count);
                visitor.visit(value,  descr);
                break;
            }
            case BaseField::TYPE_INT:
            {
                visitor.visit(*getField<int>(count),  static_cast<const IntField *>(field));
                break;
            }
            case BaseField::TYPE_ENUM:
            {
                visitor.visit(*getField<int>(count),  static_cast<const EnumField *>(field));
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                visitor.visit(*getField<double>(count),  static_cast<const DoubleField *>(field));
                break;
            }
            case BaseField::TYPE_STRING:
            {
                visitor.visit(*getField<std::string>(count),  static_cast<const StringField *>(field));
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                visitor.visit(*getField<std::wstring>(count),  static_cast<const WStringField *>(field));
                break;
            }
            case (BaseField::FieldType)(BaseField::TYPE_VECTOR_BIT | BaseField::TYPE_DOUBLE) :
            {
                visitor.visit(*getField<vector<double> >(count),  static_cast<const DoubleVectorField *>(field));
                break;
            }

            case BaseField::TYPE_COMPOSITE:
            {
                const CompositeField *tfield = static_cast<const CompositeField *>(field);
                if (tfield->reflection == NULL)
                {
                    SYNC_PRINT(("DynamicObject::simulateConstructor(%s): composite field (%d : %s) has empty reflection",
                                reflection->name.name, count, tfield->name.name
                                ));
                    break;
                }
                DynamicObjectWrapper obj(tfield->reflection, getField<void *>(count));
                visitor.visit(obj, tfield->name.name);
                break;
            }

            default:
                break;
            }
        }
    }

    void printRawObject()
    {
        if (reflection == NULL)
        {
            printf("(No Reflection)");
            return;
        }

        if (rawObject == NULL)
        {
            printf("(NULL)");
            return;
        }

        printf( "Addr: %p\n", rawObject );
        printf( "% 4d |", reflection->objectSize);

        for (int i = 0; i < reflection->objectSize; i++)
        {
            printf( "%02X%s",  ((uint8_t *)rawObject)[i], i % 4 == 3 ? " | " : " " );
            if ( i % 48 == 47 ) {
                printf("\n     |");
            }
        }

         printf("\n");
    }



#if 0
    ~DynamicObject()
    {
        if (ownsObject) {
            delete(rawObject);
        }
    }
#endif

};

class DynamicObject {
public:
    const Reflection *reflection = NULL;
    void       *rawObject = NULL;

    DynamicObject(
        const Reflection *reflection = NULL,
        bool construct = true
    ) :
        reflection(reflection)
    {
        if (reflection != NULL)
        {
            rawObject = malloc(reflection->objectSize);
            if (construct) {
                DynamicObjectWrapper(reflection, rawObject).simulateConstructor();
            }
        }
    }

    ~DynamicObject();

    template<typename Object>
    DynamicObject(Object *object):
        reflection(BaseReflection<Object>::getReflection())
    {
        rawObject = malloc(std::max((size_t)reflection->objectSize, sizeof(Object)));
        *static_cast<Object *>(rawObject) = *object;
    }

    DynamicObject(DynamicObject&& other)
    {
        reflection = other.reflection;
        rawObject = other.rawObject;
        other.rawObject = NULL;
        other.reflection = NULL;
    }

    /**
     * I call separate method clone instead of copy constructor/operator =.
     *
     * This is done not to create an illusion that it is a fast operation. It is a deep copy and needs allocation on heap.
     */
    DynamicObject clone()
    {
        DynamicObject toReturn(reflection, false);
        memcpy(toReturn.rawObject, rawObject, reflection->objectSize);
        return  toReturn;
    }


    /**
     *   Please note due to stupid alignment issues on ARM there could be some problems
     **/
    template<typename Type>
    Type *getField(int fieldId)
    {
        return (Type *)&(((uint8_t*)rawObject)[reflection->fields[fieldId]->offset]);
    }

    const BaseField* getFieldReflection(int fieldId)
    {
        return reflection->fields[fieldId];
    }

    bool simulateConstructor();

    template<typename Type>
    bool copyTo(Type *target) const
    {
        Reflection *targetRef = BaseReflection<Type>::getReflection();
        if (targetRef->name.name != reflection->name.name)
        {
            SYNC_PRINT(("Seem to copyTo object of the wrong type"));
            return false;
        }
        target = static_cast<Type *>(rawObject);
        return true;
    }

    template<typename VisitorType>
    void accept(VisitorType &visitor)
    {
         DynamicObjectWrapper(reflection, rawObject).accept<VisitorType>(visitor);
    }

    void printRawObject()
    {
         DynamicObjectWrapper(reflection, rawObject).printRawObject();
    }

private:
    DynamicObject(const DynamicObject& other) = delete;
    DynamicObject& operator=(const DynamicObject& other) = delete;

};

} //namespace corecvs


#endif // DYNAMICOBJECT_H


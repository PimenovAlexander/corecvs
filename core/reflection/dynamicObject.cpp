#include "core/reflection/dynamicObject.h"

namespace corecvs {

bool DynamicObjectWrapper::simulateConstructor()
{
    if (reflection == NULL || rawObject == NULL)
        return false;

    for (int count = 0; count < reflection->fieldNumber(); count++)
    {
        const BaseField *field = reflection->fields[count];
        switch (field->type) {
        case BaseField::TYPE_BOOL:
        {
            const BoolField *tfield = static_cast<const BoolField *>(field);
            *getField<bool>(count) = tfield->defaultValue;
            break;
        }
        case BaseField::TYPE_INT:
        {
            const IntField *tfield = static_cast<const IntField *>(field);
            *getField<int>(count) = tfield->defaultValue;
            break;
        }
        case BaseField::TYPE_DOUBLE:
        {
            const DoubleField *tfield = static_cast<const DoubleField *>(field);
            *getField<double>(count) = tfield->defaultValue;
            break;
        }
        case BaseField::TYPE_STRING:
        {
            const StringField *tfield = static_cast<const StringField *>(field);
            std::string *target = getField<std::string>(count);
            target = new (target) std::string(tfield->defaultValue);
            break;
        }
        case BaseField::TYPE_WSTRING:
        {
            const WStringField *tfield = static_cast<const WStringField *>(field);
            std::wstring *target = getField<std::wstring>(count);
            target = new (target) std::wstring(tfield->defaultValue);;
            break;
        }
        case (BaseField::FieldType)(BaseField::TYPE_VECTOR_BIT | BaseField::TYPE_DOUBLE) :
        {
            const DoubleVectorField *tfield = static_cast<const DoubleVectorField *>(field);
            *getField<vector<double> >(count) = tfield->defaultValue;
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
            obj.simulateConstructor();
            break;
        }

        default:
            break;
        }
    }

    return true;
}


bool DynamicObjectWrapper::compare(DynamicObjectWrapper &d1, DynamicObjectWrapper &d2)
{
    if (d1.reflection != d2.reflection) {
        SYNC_PRINT(("Objects have different classes %s and %s\n", d1.reflection->name.name, d2.reflection->name.name));
        return false;
    }

    bool equal = true;

    for (int count = 0; count < d1.reflection->fieldNumber(); count++)
    {
        const BaseField *field = d1.reflection->fields[count];

        switch (field->type) {
        case BaseField::TYPE_BOOL:
        {
            bool b1 = *d1.getField<bool>(count);
            bool b2 = *d2.getField<bool>(count);

            if (b1 != b2)
            {
                SYNC_PRINT(("Field %s differ %s vs %s\n", field->name.name, b1 ? "true" : "false", b2 ? "true" : "false"));
                equal = false;
            }
            break;
        }
        case BaseField::TYPE_INT:
        {
            int i1 = *(d1.getField<int>(count));
            int i2 = *(d2.getField<int>(count));

            if (i1 != i2)
            {
                SYNC_PRINT(("Field %s differ %d vs %d\n", field->name.name, i1, i2));
                equal = false;
            }
            break;
        }
        case BaseField::TYPE_DOUBLE:
        {
            double f1 = *d1.getField<double>(count);
            double f2 = *d2.getField<double>(count);

            if (f1 != f2)
            {
                SYNC_PRINT(("Field %s differ %lf vs %lf\n", field->name.name, f1, f2));
                equal = false;
            }
            break;
        }
        case BaseField::TYPE_STRING:
        {
            std::string s1 = *d1.getField<std::string>(count);
            std::string s2 = *d2.getField<std::string>(count);

            if (s1 != s2)
            {
                SYNC_PRINT(("Field %s differ %s vs %s\n", field->name.name, s1.c_str(), s2.c_str()));
                equal = false;
            }
            break;
        }
        case BaseField::TYPE_WSTRING:
        {
            std::wstring w1 = *d1.getField<std::wstring>(count);
            std::wstring w2 = *d2.getField<std::wstring>(count);
            if (w1 != w2)
            {
                SYNC_PRINT(("Field %s differ\n", field->name.name));
                equal = false;
            }
            break;
        }
        case (BaseField::FieldType)(BaseField::TYPE_VECTOR_BIT | BaseField::TYPE_DOUBLE) :
        {
            vector<double>* v1 = d1.getField<vector<double> >(count);
            vector<double>* v2 = d2.getField<vector<double> >(count);
            if (v1->size() != v2->size()) {
                SYNC_PRINT(("Field %s size differ [%d vs %d]\n", field->name.name, (int)v1->size(), (int)v2->size() ));
                equal = false;
                break;
            }
            for (size_t vc = 0; vc < v1->size(); vc++) {
                if (v1->operator [](vc) != v2->operator [](vc)) {
                    SYNC_PRINT(("Field %s differ at pos %d [%lf vs %lf]\n", field->name.name, (int)vc,
                                v1->operator [](vc), v2->operator [](vc)));
                    equal = false;
                    break;
                }
            }
            break;
        }
        case BaseField::TYPE_COMPOSITE:
        {
            const CompositeField *tfield = static_cast<const CompositeField *>(field);

            if (tfield == NULL) {
                break;
            }

            void *o1 = d1.getField<void *>(count);
            void *o2 = d2.getField<void *>(count);

            DynamicObjectWrapper obj1(tfield->reflection, o1);
            DynamicObjectWrapper obj2(tfield->reflection, o2);
            if (!compare(obj1, obj2))
            {
                equal = false;
            }
            break;
        }

        default:
            break;
        }
    }
    return equal;
}

bool DynamicObject::compare(DynamicObject &d1, DynamicObject &d2)
{
    DynamicObjectWrapper cmp1(d1.reflection, d1.rawObject);
    DynamicObjectWrapper cmp2(d2.reflection, d2.rawObject);

    return DynamicObjectWrapper::compare(cmp1, cmp2);
}


void DynamicObject::printRawObject()
{
    DynamicObjectWrapper(reflection, rawObject).printRawObject();
}

DynamicObject::DynamicObject(const DynamicObject &other)
{
    this->cloneFrom(DynamicObjectWrapper(other.reflection, other.rawObject));
}

DynamicObject &DynamicObject::operator=(const DynamicObject &other)
{
    this->cloneFrom(DynamicObjectWrapper(other.reflection, other.rawObject));
    return *this;
}

void DynamicObject::purgeObject()
{
    if (reflection != NULL && rawObject != NULL)
    {
        for (int count = 0; count < reflection->fieldNumber(); count++)
        {
            const BaseField *field = reflection->fields[count];
            switch (field->type) {
                case BaseField::TYPE_STRING:
                {
                    std::string *target = getField<std::string>(count);
                    target->~basic_string();
                    break;
                }
                case BaseField::TYPE_WSTRING:
                {
                    std::wstring *target = getField<std::wstring>(count);
                    target->~basic_string();
                    break;
                }
                case BaseField::TYPE_DOUBLE_VECTOR:
                {
                    vector<double> *target = getField<vector<double>>(count);
                    target->~vector();
                    break;
                }
                default:
                    break;
            }

        }
    }
    free(rawObject);
    rawObject = NULL;
}

DynamicObject::~DynamicObject()
{
    purgeObject();
}

DynamicObject DynamicObject::clone()
{   
    DynamicObjectWrapper wrapper(reflection, rawObject);
    return DynamicObject::clone(wrapper);
}

void DynamicObject::cloneFrom(const DynamicObjectWrapper &wrapper)
{
    purgeObject();
    rawObject = malloc(wrapper.reflection->objectSize);
    reflection = wrapper.reflection;

    /** We copy the whole object, even the fields that are not reflected **/
    memcpy(rawObject, wrapper.rawObject, wrapper.reflection->objectSize);

    /** We would need to pass over some fields and fix them for a deep copy **/
    if (wrapper.reflection != NULL && wrapper.rawObject != NULL)
    {
        for (int count = 0; count < wrapper.reflection->fieldNumber(); count++)
        {
            const BaseField *field = wrapper.reflection->fields[count];
            switch (field->type) {
                case BaseField::TYPE_STRING:
                {
                    const std::string *source = wrapper.getField<std::string>(count);
                    std::string *target = getField<std::string>(count);
                    target = new (target) std::string(*source);
                    break;
                }
                case BaseField::TYPE_WSTRING:
                {
                    const std::wstring *source = wrapper.getField<std::wstring>(count);
                    std::wstring *target = getField<std::wstring>(count);
                    target = new (target) std::wstring(*source);
                    break;
                }
                case BaseField::TYPE_DOUBLE_VECTOR:
                {
                    const vector<double> *source = wrapper.getField<vector<double>>(count);
                    vector<double> *target = getField<vector<double>>(count);
                    target = new (target) vector<double>(*source);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

DynamicObject DynamicObject::clone(const DynamicObjectWrapper &wrapper)
{
    DynamicObject toReturn;
    toReturn.cloneFrom(wrapper);
    return  toReturn;
}


} // namespace corecvs


#ifndef CORETOSCRIPT_H
#define CORETOSCRIPT_H

#include "core/reflection/reflection.h"
#include "core/reflection/dynamicObject.h"

#include "core/xml/generated/axisAlignedBoxParameters.h"

#include <QMetaType>
#include <QScriptValue>
#include <QScriptContext>

#ifndef WORK_IN_PROGRESS

/* We have to do it for all types */
Q_DECLARE_METATYPE(AxisAlignedBoxParameters);


class ReflectionToScript
{
public:
    corecvs::Reflection *mReflection = NULL;

    ReflectionToScript(corecvs::Reflection *reflection = NULL) :
        mReflection(reflection)
    {}

template<class ExposeType>
    static QScriptValue createMyStruct(QScriptContext *, QScriptEngine *engine)
    {
        ExposeType s;
        return engine->toScriptValue(s);
    }

template<class ExposeType>
    static QScriptValue toScriptValue(QScriptEngine *engine, const ExposeType &s)
    {
          corecvs::Reflection *reflection = &ExposeType::reflection;

          corecvs::DynamicObjectWrapper input(&s);

          QScriptValue obj = engine->newObject();
          if (reflection == NULL)
          {
              return obj;
          }

          for (size_t i = 0; i < reflection->fields.size(); i++)
          {
              const corecvs::BaseField *field = reflection->fields[i];
              QString name = field->name.name;
              switch (field->type) {
                  case corecvs::BaseField::TYPE_INT:
                  {
                      //const IntField *iField = static_cast<const IntField *>(field);
                      obj.setProperty(name, *(input.template getField<int>((int)i)));
                      break;
                  }
                  case corecvs::BaseField::TYPE_DOUBLE:
                  {
                      //const DoubleField *dField = static_cast<const DoubleField *>(field);
                      obj.setProperty(name, *input.template getField<double>((int)i));
                      break;
                  }
                  case corecvs::BaseField::TYPE_STRING:
                  {
                      //const StringField *sField = static_cast<const StringField *>(field);
                      obj.setProperty(name, QString::fromStdString(*input.template getField<std::string>((int)i)));
                      break;
                  }
                  case corecvs::BaseField::TYPE_WSTRING:
                  {
                      //const StringField *sField = static_cast<const StringField *>(field);
                      obj.setProperty(name, QString::fromStdWString(*input.template getField<std::wstring>((int)i)));
                      break;
                  }
                  case corecvs::BaseField::TYPE_BOOL:
                  {
                      //const BoolField *bField = static_cast<const BoolField *>(field);
                      obj.setProperty(name, *input.template getField<bool>((int)i));
                      break;
                  }
                  case corecvs::BaseField::TYPE_ENUM:
                  {
                      //const corecvs::EnumField *eField = static_cast<const corecvs::EnumField *>(field);
                      //const corecvs::EnumReflection *enumOptions = eField->enumReflection;
                      /* NOT SUPPORTED */
                      break;
                  }
                  case corecvs::BaseField::TYPE_DOUBLE | corecvs::BaseField::TYPE_VECTOR_BIT:
                  {
                      //const corecvs::DoubleVectorField *dField = static_cast<const corecvs::DoubleVectorField *>(field);
                      // DoubleVectorWidget *vectorWidget = new DoubleVectorWidget(this);
                      /* NOT SUPPORTED */
                      break;
                  }

                  /* Composite field */
                  case corecvs::BaseField::TYPE_COMPOSITE:
                  {
                      //const corecvs::CompositeField *cField = static_cast<const corecvs::CompositeField *>(field);
                      //const corecvs::Reflection *subReflection = cField->reflection;
                       /* NOT SUPPORTED */
                      break;
                  }

                  /* Composite field */
                  case corecvs::BaseField::TYPE_POINTER:
                  {
                      /* NOT SUPPORTED */
                      break;
                  }
                  default:
                  {
                      /* NOT SUPPORTED */
                      break;
                  }
            }
      }

      return obj;
    }

template<class ExposeType>
    static void fromScriptValue(const QScriptValue &obj, ExposeType &s)
    {
        corecvs::Reflection *reflection = &ExposeType::reflection;

        corecvs::DynamicObjectWrapper input(&s);

        if (reflection == NULL)
        {
            return;
        }

        for (size_t i = 0; i < reflection->fields.size(); i++)
        {
            const corecvs::BaseField *field = reflection->fields[i];
            QString name = field->name.name;

            QScriptValue value = obj.property(name);

            switch (field->type) {
                case corecvs::BaseField::TYPE_INT:
                {
                    *(input.template getField<int>((int)i)) = value.toInt32();
                    break;
                }
                case corecvs::BaseField::TYPE_DOUBLE:
                {
                    *input.template getField<double>((int)i) = value.toNumber();
                    break;
                }
                case corecvs::BaseField::TYPE_STRING:
                {
                    *input.template getField<std::string>((int)i) = value.toString().toStdString();
                    break;
                }
                case corecvs::BaseField::TYPE_WSTRING:
                {
                    *input.template getField<std::wstring>((int)i) = value.toString().toStdWString();
                    break;
                }
                case corecvs::BaseField::TYPE_BOOL:
                {
                    *input.template getField<bool>((int)i) = value.toBool();
                    break;
                }
                case corecvs::BaseField::TYPE_ENUM:
                {
                    // const corecvs::EnumField *eField = static_cast<const corecvs::EnumField *>(field);
                    // const corecvs::EnumReflection *enumOptions = eField->enumReflection;
                    /* NOT SUPPORTED */
                    break;
                }
                case corecvs::BaseField::TYPE_DOUBLE | corecvs::BaseField::TYPE_VECTOR_BIT:
                {
                    // const corecvs::DoubleVectorField *dField = static_cast<const corecvs::DoubleVectorField *>(field);
                    //DoubleVectorWidget *vectorWidget = new DoubleVectorWidget(this);
                    /* NOT SUPPORTED */
                    break;
                }

                /* Composite field */
                case corecvs::BaseField::TYPE_COMPOSITE:
                {
                    //const corecvs::CompositeField *cField = static_cast<const corecvs::CompositeField *>(field);
                    //const Reflection *subReflection = cField->reflection;
                     /* NOT SUPPORTED */
                    break;
                }

                /* Composite field */
                case corecvs::BaseField::TYPE_POINTER:
                {
                    /* NOT SUPPORTED */
                    break;
                }
                default:
                {
                    /* NOT SUPPORTED */
                    break;
                }
            }
        }
    }
};

#endif
#endif // CORETOSCRIPT_H


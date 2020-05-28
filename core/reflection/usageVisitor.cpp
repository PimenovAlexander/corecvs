#include "reflection/usageVisitor.h"

namespace corecvs {

using namespace std;

const std::string UsagePrinter::SEPARATOR(".");

void UsagePrinter::printUsage(const Reflection *reflection, const std::string &addNamespace)
{
    bool nsSpaces = (addNamespace.find_first_of(" ") != std::string::npos);

    int position = 0;
    for (int i = 0; i < (int)reflection->fields.size(); i++)
    {
        const BaseField *field = reflection->fields[i];
        bool spaced = nsSpaces || (strchr(field->name.name, ' ') != NULL);
        char const *sep = spaced ? "\"" : "";

        switch (field->type) {
            case BaseField::TYPE_INT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=%d%s)\n",
                       sep, addNamespace.c_str(), iField->name.name, sep,
                       iField->name.description, iField->defaultValue,
                       iField->suffixHint ?  iField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                const DoubleField *dField = static_cast<const DoubleField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=%lf%s)\n",
                       sep, addNamespace.c_str(), dField->name.name, sep,
                       dField->name.description, dField->defaultValue,
                       dField->suffixHint ? dField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_STRING:
            {
                const StringField *sField = static_cast<const StringField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=\"%s\"%s)\n",
                       sep, addNamespace.c_str(), sField->name.name, sep,
                       sField->name.description, sField->defaultValue.c_str(),
                       sField->suffixHint ? sField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                const WStringField *sField = static_cast<const WStringField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s\n",
                       sep, addNamespace.c_str(), sField->name.name, sep, sField->name.description);
                break;
            }
            case BaseField::TYPE_BOOL:
            {
                const BoolField *bField = static_cast<const BoolField *>(field);
                printf("  [%s--%s%s%s]  - %s (default %s)\n",
                       sep, addNamespace.c_str(), bField->name.name, sep,
                       bField->name.description, bField->defaultValue ? "active" : "inactive");

                break;
            }
            case BaseField::TYPE_ENUM:
            {
                const EnumField *eField = static_cast<const EnumField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=%d%s)\n",
                       sep, addNamespace.c_str(), eField->name.name, sep,
                       eField->name.description, eField->defaultValue,
                       eField->suffixHint ?  eField->suffixHint : "");
                if (eField->enumReflection != NULL)
                {
                    for (const EnumOption *opt : eField->enumReflection->options)
                    {
                        if (opt != NULL) {
                            printf("         %d - %s (%s)\n", opt->id, opt->name.name, opt->name.description);
                        }
                    }
                }

                break;
            }

            case BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT:
            {
                //const DoubleVectorField *dField = static_cast<const DoubleVectorField *>(field);
                break;
            }

            /* Composite field */
            case BaseField::TYPE_COMPOSITE:
            {
                const CompositeField *cField = static_cast<const CompositeField *>(field);
                printUsage(cField->reflection, addNamespace + cField->name.name + SEPARATOR);
                break;
            }

            /* Composite field */
            case BaseField::TYPE_POINTER:
            {
                // const PointerField *pField = static_cast<const PointerField *>(field);
                break;
            }

            /* Well something is not supported */
            default:
                break;
        }

        position++;
    }

}

void UsagePrinter::printUsage(DynamicObjectWrapper *wrapper, const string &addNamespace)
{
    void *rawObject = wrapper->rawObject;
    const Reflection *reflection = wrapper->reflection;

    bool nsSpaces = (addNamespace.find_first_of(" ") != std::string::npos);

    int position = 0;
    for (int count = 0; count < (int)reflection->fields.size(); count++)
    {
        const BaseField *field = reflection->fields[count];
        bool spaced = nsSpaces || (strchr(field->name.name, ' ') != NULL);
        char const *sep = spaced ? "\"" : "";

        switch (field->type) {
            case BaseField::TYPE_INT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=%d%s)\n",
                       sep, addNamespace.c_str(), iField->name.name, sep,
                       iField->name.description, (rawObject == NULL) ? iField->defaultValue : *wrapper->getField<int>(count),
                       iField->suffixHint ?  iField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                const DoubleField *dField = static_cast<const DoubleField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=%lf%s)\n",
                       sep, addNamespace.c_str(), dField->name.name, sep,
                       dField->name.description, (rawObject == NULL) ? dField->defaultValue : *wrapper->getField<double>(count),
                       dField->suffixHint ? dField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_STRING:
            {
                const StringField *sField = static_cast<const StringField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=\"%s\"%s)\n",
                       sep, addNamespace.c_str(), sField->name.name, sep,
                       sField->name.description, (rawObject == NULL) ? sField->defaultValue.c_str() : wrapper->getField<std::string>(count)->c_str(),
                       sField->suffixHint ? sField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                const WStringField *sField = static_cast<const WStringField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s\n",
                       sep, addNamespace.c_str(), sField->name.name, sep, sField->name.description);
                break;
            }
            case BaseField::TYPE_BOOL:
            {
                const BoolField *bField = static_cast<const BoolField *>(field);
                printf("  [%s--%s%s=<on|off|yes|no|true|false>%s]  - %s (default %s)\n",
                       sep, addNamespace.c_str(), bField->name.name, sep,
                       bField->name.description, ((rawObject == NULL) ? (bField->defaultValue) : *wrapper->getField<bool>(count)) ? "true" : "false");

                break;
            }
            case BaseField::TYPE_ENUM:
            {
                const EnumField *eField = static_cast<const EnumField *>(field);
                printf("  [%s--%s%s=<value>%s]  - %s (default=%d%s)\n",
                       sep, addNamespace.c_str(), eField->name.name, sep,
                       eField->name.description, (rawObject == NULL) ? eField->defaultValue : *wrapper->getField<int>(count),
                       eField->suffixHint ?  eField->suffixHint : "");
                if (eField->enumReflection != NULL)
                {
                    for (const EnumOption *opt : eField->enumReflection->options)
                    {
                        if (opt != NULL) {
                            printf("         %d - %s (%s)\n", opt->id, opt->name.name, opt->name.description);
                        }
                    }
                }

                break;
            }

            case BaseField::TYPE_DOUBLE | BaseField::TYPE_VECTOR_BIT:
            {
                //const DoubleVectorField *dField = static_cast<const DoubleVectorField *>(field);
                break;
            }

            /* Composite field */
            case BaseField::TYPE_COMPOSITE:
            {
                const CompositeField *cField = static_cast<const CompositeField *>(field);

                if (rawObject == NULL) {
                    printUsage(cField->reflection, addNamespace + cField->name.name + SEPARATOR);
                } else {
                    DynamicObjectWrapper subWrapper(cField->reflection, wrapper->getField<void *>(count));
                    printUsage(&subWrapper, addNamespace + cField->name.name + SEPARATOR);
                }
                break;
            }

            /* Composite field */
            case BaseField::TYPE_POINTER:
            {
                // const PointerField *pField = static_cast<const PointerField *>(field);
                break;
            }

            /* Well something is not supported */
            default:
                break;
        }

        position++;
    }
}




} //namespace corecvs

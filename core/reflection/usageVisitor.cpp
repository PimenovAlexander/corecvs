#include "core/reflection/usageVisitor.h"

namespace corecvs {

const std::string UsagePrinter::SEPARATOR(".");

void UsagePrinter::printUsage(const Reflection *reflection, const std::string &addNamespace)
{
    int position = 0;
    for (int i = 0; i < (int)reflection->fields.size(); i++)
    {
        const BaseField *field = reflection->fields[i];
        switch (field->type) {
            case BaseField::TYPE_INT:
            {
                const IntField *iField = static_cast<const IntField *>(field);
                printf("  [--%s%-10s=<value>]  - %s (default=%d%s)\n", addNamespace.c_str(), iField->name.name, iField->name.description, iField->defaultValue,
                       iField->suffixHint ?  iField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_DOUBLE:
            {
                const DoubleField *dField = static_cast<const DoubleField *>(field);
                printf("  [--%s%-10s=<value>]  - %s (default=%lf%s)\n", addNamespace.c_str(), dField->name.name, dField->name.description, dField->defaultValue,
                       dField->suffixHint ? dField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_STRING:
            {
                const StringField *sField = static_cast<const StringField *>(field);
                printf("  [--%s%-10s=<value>]  - %s (default=\"%s\"%s)\n", addNamespace.c_str(), sField->name.name, sField->name.description, sField->defaultValue.c_str(),
                       sField->suffixHint ? sField->suffixHint : "");
                break;
            }
            case BaseField::TYPE_WSTRING:
            {
                const WStringField *sField = static_cast<const WStringField *>(field);
                printf("  [--%s%-10s=<value>]  - %s\n", addNamespace.c_str(), sField->name.name, sField->name.description);
                break;
            }
            case BaseField::TYPE_BOOL:
            {
                const BoolField *bField = static_cast<const BoolField *>(field);
                printf("  [--%s%-10s]  - %s (default %s)\n", addNamespace.c_str(), bField->name.name, bField->name.description, bField->defaultValue ? "active" : "inactive");

                break;
            }
            case BaseField::TYPE_ENUM:
            {
                const EnumField *eField = static_cast<const EnumField *>(field);
                printf("  [--%s%-10s=<value>]  - %s (default=%d%s)\n", addNamespace.c_str(), eField->name.name, eField->name.description, eField->defaultValue,
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




} //namespace corecvs

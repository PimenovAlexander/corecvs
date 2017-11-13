#include "core/reflection/reflection.h"

namespace corecvs {

/* Well... we expect some compiler magic to work and init it before the first use */
std::unique_ptr<ReflectionDirectory> ReflectionDirectoryHolder::reflectionDirectory;

ReflectionDirectory *ReflectionDirectoryHolder::getReflectionDirectory()
{
    if (!reflectionDirectory)
        reflectionDirectory.reset(new ReflectionDirectory());

    /* We lose uniquness here. That is not a big problem. Quite obvious that ReflectionDirectory has global ownership */
    return reflectionDirectory.get();
}


bool BaseField::isInputPin() const {
    const char *prefix = "in";
    return strncmp(name.name, prefix, strlen(prefix)) == 0;
}

bool BaseField::isOuputPin() const {
    const char *prefix = "out";
    return strncmp(name.name, prefix, strlen(prefix)) == 0;
}

bool Reflection::isActionBlock() const
{
    for (int fieldId = 0; fieldId < fieldNumber(); fieldId++)
    {
        const BaseField *field = fields[fieldId];
        if (field->isInputPin()) {
            return true;
        }
        if (field->isOuputPin()) {
            return false;
        }
    }
    return false;
}




} // namespace corecvs


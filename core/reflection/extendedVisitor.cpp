#include "core/reflection/extendedVisitor.h"
#include "core/reflection/reflection.h"
#include "core/reflection/dynamicObject.h"
#include "core/reflection/defaultSetter.h"

namespace corecvs {

ExtendedVisitor::ExtendedVisitor()
{

}

void *CreatorFabric::createObject(std::string &name)
{
    ReflectionDirectory &directory = *ReflectionDirectoryHolder::getReflectionDirectory();

    auto it = directory.find(name);
    if (it == directory.end()) {
        return NULL;
    }
    Reflection *reflection = (*it).second;
    if (reflection->objectSize <= 0)
    {
        SYNC_PRINT(("CreatorFabric::createObject(%s): reflection doesn't allow object creation", name.c_str()));
        return NULL;
    }

    void *result = malloc(reflection->objectSize);

    DynamicObjectWrapper object(reflection, result);
    object.simulateConstructor();

    return result;
}

} // namespace corecvs


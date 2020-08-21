#ifndef LOCKABLEOBJECT_H
#define LOCKABLEOBJECT_H

#include <mutex>
#include "core/reflection/reflection.h"
#include "core/reflection/dynamicObject.h"

namespace corecvs {

class LockableObject : public corecvs::DynamicObjectWrapper
{
    //QMutex mutex;
    std::mutex mutex;
public:
    LockableObject() {}

    template<typename Object>
    LockableObject(Object *object) : corecvs::DynamicObjectWrapper(object)
    {}

    void lock() {
        mutex.lock();
    }

    void unlock() {
        mutex.unlock();
    }
};

} // namespace corecvs


#endif // LOCKABLEOBJECT_H

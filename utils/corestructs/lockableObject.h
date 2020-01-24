#ifndef LOCKABLEOBJECT_H
#define LOCKABLEOBJECT_H

#include <QMutex>
#include <mutex>
#include "core/reflection/reflection.h"
#include "core/reflection/dynamicObject.h"

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


#endif // LOCKABLEOBJECT_H

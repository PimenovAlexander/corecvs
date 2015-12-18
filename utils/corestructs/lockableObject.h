#ifndef LOCKABLEOBJECT_H
#define LOCKABLEOBJECT_H

#include <QMutex>
#include "reflection.h"

class LockableObject : public corecvs::DynamicObject
{
    QMutex mutex;
public:
    LockableObject() {}

    template<typename Object>
    LockableObject(Object *object) : corecvs::DynamicObject(object)
    {}

    void lock() {
        mutex.lock();
    }

    void unlock() {
        mutex.unlock();
    }
};


#endif // LOCKABLEOBJECT_H

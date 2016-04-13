#include "statusTracker.h"

void corecvs::StatusTracker::readLock() const
{
#ifdef WITH_TBB
    const_cast<tbb::reader_writer_lock&>(lock).lock_read();
#endif
}
void corecvs::StatusTracker::writeLock()
{
#ifdef WITH_TBB
    lock.lock();
#endif
}
void corecvs::StatusTracker::unlock() const
{
#ifdef WITH_TBB
    const_cast<tbb::reader_writer_lock&>(lock).unlock();
#endif
}

void corecvs::StatusTracker::incrementStarted()
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.startedActions++;
        CORE_ASSERT_TRUE_S(currentStatus.startedActions <= currentStatus.totalActions);
    unlock();
}

void corecvs::StatusTracker::incrementCompleted()
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.completedActions++;
        CORE_ASSERT_TRUE_S(currentStatus.completedActions <= currentStatus.totalActions);
        CORE_ASSERT_TRUE_S(currentStatus.completedActions >= currentStatus.startedActions);
    unlock();
}

void corecvs::StatusTracker::reset(const std::string &action, size_t totalActions)
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.completedActions = currentStatus.startedActions = 0;
        currentStatus.totalActions = totalActions;
    unlock();
}

corecvs::Status  corecvs::StatusTracker::getStatus() const
{
    if (this == nullptr)
        return corecvs::Status();

    readLock();
        auto status = currentStatus;
    unlock();
    return status;
}

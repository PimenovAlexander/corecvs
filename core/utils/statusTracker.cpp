#include "core/utils/statusTracker.h"
#include "core/tbbwrapper/tbbWrapper.h"

#include <iostream>

namespace corecvs {

AutoTracker::AutoTracker(StatusTracker* st) : st(st)
{
    st->incrementStarted();
}

AutoTracker::~AutoTracker()
{
    st->incrementCompleted();
}

void corecvs::StatusTracker::setTotalActions(size_t totalActions)
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.completedGlobalActions = 0;
        currentStatus.totalGlobalActions = totalActions;
    unlock();
    std::cout << "StatusTracker::setTotalActions " << totalActions << std::endl;
}

void corecvs::StatusTracker::reset(const std::string &action, size_t totalActions)
{
    if (this == nullptr)
        return;
    writeLock();
        if (currentStatus.startedGlobalActions > 0) {
            currentStatus.completedGlobalActions++;
        }
        currentStatus.startedGlobalActions++;
        currentStatus.currentAction    = action;
        currentStatus.completedActions = currentStatus.startedActions = 0;
        currentStatus.totalActions     = totalActions;
        auto status = currentStatus;
    unlock();
    std::cout << "StatusTracker::reset " << status << std::endl;
}

void StatusTracker::incrementStarted()
{
    if (this == nullptr)
        return;

    checkToCancel();	// Note: it can throw a cancelation exception

    writeLock();
        currentStatus.startedActions++;
        std::cout << "Started: " << currentStatus.startedActions << std::endl;
        auto startedActions = currentStatus.startedActions;
        auto totalActions   = currentStatus.totalActions;
    unlock();

    CORE_ASSERT_TRUE_S(startedActions <= totalActions);
}

void StatusTracker::incrementCompleted()
{
    if (this == nullptr)
        return;

    //checkToCancel(); // Note: don't call it here as this method is called by ~AutoTracker that dues to terminating app!!!
    if (isToCancel())
    {
        std::cout << "StatusTracker::incrementCompleted is canceling" << std::endl;
        return;
    }

    writeLock();
        currentStatus.completedActions++;
        std::cout << "StatusTracker::incrementCompleted " << currentStatus << std::endl;
        auto completedActions = currentStatus.completedActions;
        auto startedActions   = currentStatus.startedActions;
        auto totalActions     = currentStatus.totalActions;
    unlock();

    CORE_ASSERT_TRUE_S(completedActions <= totalActions);
    CORE_ASSERT_TRUE_S(completedActions <= startedActions);
}

AutoTracker StatusTracker::createAutoTrackerCalculationObject()
{
    if (this == nullptr)
        return 0;
    return AutoTracker(this);
}

bool_t corecvs::StatusTracker::isCompleted() const
{
    if (this == nullptr)
        return false;
    readLock();
        auto flag = currentStatus.isCompleted;
    unlock();
    return flag;
}

bool_t corecvs::StatusTracker::isFailed() const
{
    if (this == nullptr)
        return false;
    readLock();
        auto flag = currentStatus.isFailed;
    unlock();
    return flag;
}

bool_t corecvs::StatusTracker::isToCancel() const
{
    if (this == nullptr)
        return false;
    readLock();
        auto flag = currentStatus.isToCancel;
    unlock();
    return flag;
}

bool_t corecvs::StatusTracker::isCanceled() const
{
    if (this == nullptr)
        return false;
    readLock();
        auto flag = currentStatus.isCanceled;
    unlock();
    return flag;
}

void corecvs::StatusTracker::setCompleted()
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.isCompleted = true;
    unlock();
    std::cout << "StatusTracker::setCompleted" << std::endl;
}

void corecvs::StatusTracker::setFailed()
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.isFailed = true;
    unlock();
    std::cout << "StatusTracker::setFailed" << std::endl;
}

void corecvs::StatusTracker::setToCancel()
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.isToCancel = true;
    unlock();
    std::cout << "StatusTracker::setToCancel" << std::endl;
}

void corecvs::StatusTracker::setCanceled()
{
    if (this == nullptr)
        return;
    writeLock();
        currentStatus.isCanceled = true;
    unlock();
    std::cout << "StatusTracker::setCanceled" << std::endl;
}

void corecvs::StatusTracker::checkToCancel() const
{
    if (isToCancel())
    {
        std::cout << "StatusTracker::checkToCancel cancel_group_execution" << std::endl;
        task::self().cancel_group_execution();

        std::cout << "StatusTracker::checkToCancel throw..." << std::endl;
        throw CancelExecutionException("Cancel");
    }
}

bool corecvs::StatusTracker::isActionCompleted(const std::string &action) const
{
    if (this == nullptr)
        return false;
    readLock();
        bool flag = (action == currentStatus.currentAction &&
                     currentStatus.totalActions == currentStatus.completedActions);
    unlock();
    return flag;
}

corecvs::Status corecvs::StatusTracker::getStatus() const
{
    if (this == nullptr)
        return Status();

    readLock();
        auto status = currentStatus;
    unlock();
    return status;
}

void StatusTracker::readLock() const
{
#ifdef WITH_TBB
    const_cast<tbb::reader_writer_lock&>(lock).lock_read();
#endif
}
void StatusTracker::writeLock()
{
#ifdef WITH_TBB
    lock.lock();
#endif
}
void StatusTracker::unlock() const
{
#ifdef WITH_TBB
    const_cast<tbb::reader_writer_lock&>(lock).unlock();
#endif
}

} // namespace corecvs 

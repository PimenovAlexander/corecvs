#include "core/utils/statusTracker.h"
#include "core/tbbwrapper/tbbWrapper.h"

#include <iostream>

namespace corecvs {

void        StatusTracker::SetTotalActions(StatusTracker *st, size_t totalActions)                 { if (st) st->setTotalActions(totalActions); }
void        StatusTracker::Reset(StatusTracker *st, const std::string &action, size_t totalActions){ if (st) st->reset(action, totalActions); }
void        StatusTracker::IncrementStarted(StatusTracker *st)                                     { if (st) st->incrementStarted();   }
void        StatusTracker::IncrementCompleted(StatusTracker *st)                                   { if (st) st->incrementCompleted(); }
AutoTracker StatusTracker::CreateAutoTrackerCalculationObject(StatusTracker *st)                   { return st ? st->createAutoTrackerCalculationObject() : AutoTracker(nullptr); }
void        StatusTracker::SetCompleted(StatusTracker *st)                                         { if (st) st->setCompleted(); }
void        StatusTracker::SetFailed(StatusTracker *st, const char* error)                         { if (st) st->setFailed(error); }
void        StatusTracker::SetToCancel(StatusTracker *st)                                          { if (st) st->setToCancel();  }
void        StatusTracker::SetCanceled(StatusTracker *st)                                          { if (st) st->setCanceled();  }
bool        StatusTracker::IsCompleted(const StatusTracker *st)                                    { return st ? st->isCompleted() : false; }
bool        StatusTracker::IsFailed(const StatusTracker *st)                                       { return st ? st->isFailed()    : false; }
bool        StatusTracker::IsToCancel(const StatusTracker *st)                                     { return st ? st->isToCancel()  : false; }
bool        StatusTracker::IsCanceled(const StatusTracker *st)                                     { return st ? st->isCanceled()  : false; }
void        StatusTracker::CheckToCancel(StatusTracker *st)                                  { if (st) st->checkToCancel(); }
bool        StatusTracker::IsActionCompleted(const StatusTracker *st, const std::string &action)   { return st ? st->isActionCompleted(action) : false; }
Status      StatusTracker::GetStatus(const StatusTracker *st)                                      { return st ? st->getStatus() : Status(); }

AutoTracker::AutoTracker(StatusTracker* st) : st(st)
{
    StatusTracker::IncrementStarted(st);
}

AutoTracker& AutoTracker::operator=(AutoTracker &&other)
{
    // other is a complete object => started is already incremented,
    // so the only action that is needed is to steal ST pointer from
    // an argument
    std::swap(st, other.st);
    return *this;
}

AutoTracker::AutoTracker(AutoTracker&& other) : st(other.st)
{
    // other is a complete object => started is already incremented,
    // so the only action that is needed is to steal ST pointer from
    // an argument
    other.st = nullptr;
}

AutoTracker::~AutoTracker()
{
    StatusTracker::IncrementCompleted(st);
}

#define WRITE_LOCK write_lock l(lock)
#define READ_LOCK  read_lock  l(lock)

void corecvs::StatusTracker::setTotalActions(size_t totalActions)
{
    {
        WRITE_LOCK;
        currentStatus.completedGlobalActions = 0;
        currentStatus.totalGlobalActions = totalActions;
    }
    std::cout << "StatusTracker::setTotalActions " << totalActions << std::endl;
}

void corecvs::StatusTracker::reset(const std::string &action, size_t totalActions)
{
    Status status;
    {
        WRITE_LOCK;
        if (currentStatus.startedGlobalActions > 0) {
            currentStatus.completedGlobalActions++;
        }
        currentStatus.startedGlobalActions++;
        currentStatus.currentAction = action;
        currentStatus.completedActions = currentStatus.startedActions = 0;
        currentStatus.totalActions = totalActions;
        status = currentStatus;
    }
    if (onProgress) {
        onProgress(status.getProgressGlobal(), status.getProgressLocal());
    }
    std::cout << "StatusTracker::reset " << status << std::endl;
}

void StatusTracker::incrementStarted()
{
    checkToCancel();    // Note: it can throw a cancelation exception

    size_t startedActions, totalActions;
    {
        WRITE_LOCK;
        startedActions = ++currentStatus.startedActions;
        totalActions   = currentStatus.totalActions;
    }
    CORE_ASSERT_TRUE_S(startedActions <= totalActions);
    std::cout << "Started: " << startedActions << std::endl;
}

void StatusTracker::incrementCompleted()
{
    if (isToCancel())
    {
        std::cout << "StatusTracker::incrementCompleted is canceling" << std::endl;
        return;
    }

    Status status;
    {
        WRITE_LOCK;
        currentStatus.completedActions++;
        status = currentStatus;
    }
    if (onProgress) {
        onProgress(status.getProgressGlobal(), status.getProgressLocal());
    }
    std::cout << "StatusTracker::incrementCompleted " << status << std::endl;

    CORE_ASSERT_TRUE_S(status.completedActions <= status.totalActions);
    CORE_ASSERT_TRUE_S(status.completedActions <= status.startedActions);
}

AutoTracker StatusTracker::createAutoTrackerCalculationObject()
{
    return AutoTracker(this);
}

bool  corecvs::StatusTracker::isCompleted() const
{
    READ_LOCK;
    return currentStatus.isCompleted;
}

bool  corecvs::StatusTracker::isFailed() const
{
    READ_LOCK;
    return currentStatus.isFailed;
}

bool  corecvs::StatusTracker::isToCancel() const
{
    READ_LOCK;
    return currentStatus.isToCancel;
}

bool  corecvs::StatusTracker::isCanceled() const
{
    READ_LOCK;
    return currentStatus.isCanceled;
}

void corecvs::StatusTracker::setCompleted()
{
    {
        WRITE_LOCK;
        currentStatus.isCompleted = true;
        currentStatus.completedGlobalActions++;
        //CORE_ASSERT_TRUE_S(currentStatus.completedGlobalActions <= currentStatus.totalGlobalActions); // we are not right often with global steps estimation for some methods...
    }
    std::cout << "StatusTracker::setCompleted" << std::endl;
    
    {
        READ_LOCK;
        if (currentStatus.completedGlobalActions != currentStatus.totalGlobalActions) {
            std::cout << "StatusTracker::setCompleted: globalCounter error, STATUS::: " << currentStatus << std::endl;
        }
    }
    if (onFinished) {
        onFinished();
    }
}

void corecvs::StatusTracker::setFailed(const char* error)
{
    {
        WRITE_LOCK;
        currentStatus.isFailed = true;
    }
    if (onError) {
        onError(error);
    }
    std::cout << "StatusTracker::setFailed" << std::endl;
}

void corecvs::StatusTracker::setToCancel()
{
    {
        WRITE_LOCK;
        currentStatus.isToCancel = true;
    }
    std::cout << "StatusTracker::setToCancel" << std::endl;
}

void corecvs::StatusTracker::setCanceled()
{
    {
        WRITE_LOCK;
        currentStatus.isCanceled = true;
    }

    std::cout << "StatusTracker::setCanceled" << std::endl;
}

void corecvs::StatusTracker::cancelExecution() const
{
    std::cout << "StatusTracker::checkToCancel cancel_group_execution" << std::endl;
#ifdef WITH_TBB
    task::self().cancel_group_execution();
#endif
    if (onFinished) {
        onFinished();
    }
    std::cout << "StatusTracker::checkToCancel throw..." << std::endl;
    throw CancelExecutionException("Cancel");
}

void corecvs::StatusTracker::checkToCancel()
{
    if (onCheckToCancel && onCheckToCancel()) {//controlled outside using cvsdk taskcontrol
        setToCancel();
        cancelExecution();
    }
    else {
        if (isToCancel())
        {
            cancelExecution();
        }
    }
}

bool corecvs::StatusTracker::isActionCompleted(const std::string &action) const
{
    READ_LOCK;
    return (action == currentStatus.currentAction && currentStatus.totalActions == currentStatus.completedActions);
}

corecvs::Status corecvs::StatusTracker::getStatus() const
{
    READ_LOCK;
    return currentStatus;
}

} // namespace corecvs

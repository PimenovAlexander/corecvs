#ifndef STATUS_TRACKER
#define STATUS_TRACKER

#include <string>
#ifdef WITH_TBB
#include <tbb/tbb.h>
#endif

#include <global.h>

namespace corecvs {

struct Status
{
    std::string currentAction;
    size_t      completedActions, totalActions, startedActions;

    Status() : currentAction("NONE"), completedActions(0), totalActions(0), startedActions(0)
    {}
};

class StatusTracker
{
public:
    void    incrementStarted();
    void    incrementCompleted();
    void    reset(const std::string &action, size_t totalActions);

    Status  getStatus() const;

private:
    Status  currentStatus;

#ifdef WITH_TBB
    tbb::reader_writer_lock lock;
#endif
    void readLock() const;
    void writeLock();
    void unlock() const;
};

} // namespace corecvs

#endif // STATUS_TRACKER

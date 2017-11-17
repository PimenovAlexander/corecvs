#ifndef STATUS_TRACKER
#define STATUS_TRACKER

#include <string>
#include <ostream>
#include <functional>

#include "core/utils/global.h"

#include "core/tbbwrapper/tbbWrapper.h"

struct CancelExecutionException : public AssertException
{
    CancelExecutionException(const char* codeExpr) : AssertException(codeExpr) {}
};

namespace corecvs {

struct Status
{
    std::string currentAction;
    size_t      completedActions, totalActions, startedActions, completedGlobalActions, totalGlobalActions, startedGlobalActions;
    bool        isCompleted,
                isFailed,
                isToCancel,
                isCanceled;

    Status() : currentAction("NONE")
        , completedActions(0), totalActions(0), startedActions(0)
        , completedGlobalActions(0), totalGlobalActions(0), startedGlobalActions(0)
        , isCompleted(false), isFailed(false)
        , isToCancel(false), isCanceled(false)
    {}

    friend std::ostream& operator<<(std::ostream& os, const Status &status)
    {
        os << "\tglobal: " << status.startedGlobalActions << "/" << status.completedGlobalActions << "/" << status.totalGlobalActions
            << "\tlocal: " << status.currentAction << " " << status.startedActions << "/" << status.completedActions << "/" << status.totalActions;
        return os;
    }

    std::ostream& progress(std::ostream& os)
    {
        os << *this << std::endl;
        return os;
    }

    double getProgressGlobal() {
        return (double)completedGlobalActions / CORE_MAX(1, totalGlobalActions);
    }

    double getProgressLocal() {
        return (double)completedActions / CORE_MAX(1, totalActions);
    }
};

class StatusTracker;

struct AutoTracker
{
    AutoTracker(StatusTracker* st);
    AutoTracker(AutoTracker &&other);
    AutoTracker& operator=(AutoTracker &&other);
   ~AutoTracker();

private:
    AutoTracker(const AutoTracker &other) = delete;
    AutoTracker& operator=(const AutoTracker &other) = delete;

    StatusTracker* st;
};

class StatusTracker
{
public:
    static void         SetTotalActions(StatusTracker *tracker, size_t totalActions);
    static void         Reset(StatusTracker *tracker, const std::string &action, size_t totalActions);

    static void         IncrementStarted(StatusTracker *tracker);
    static void         IncrementCompleted(StatusTracker *tracker);
    static AutoTracker  CreateAutoTrackerCalculationObject(StatusTracker *tracker);

    static void         SetCompleted(StatusTracker *tracker);
    static void         SetFailed(StatusTracker *tracker, const char* error);
    static void         SetToCancel(StatusTracker *tracker);
    static void         SetCanceled(StatusTracker *tracker);

    static bool         IsCompleted(const StatusTracker *tracker);
    static bool         IsFailed(const StatusTracker *tracker);

    static bool         IsToCancel(const StatusTracker *tracker);

    static bool         IsCanceled(const StatusTracker *tracker);
    static void         CheckToCancel(StatusTracker *tracker);

    static bool         IsActionCompleted(const StatusTracker *tracker, const std::string &action);

    static Status       GetStatus(const StatusTracker *tracker);

    std::function<void(std::string)>                    onError;
    std::function<void()>                               onFinished;
    std::function<void(double global, double local)>    onProgress;
    std::function<bool()>                               onCheckToCancel;

    StatusTracker() : onError(0), onFinished(0), onProgress(0), onCheckToCancel(0)
    {}

    StatusTracker& operator=(const StatusTracker&) = delete;
    StatusTracker(StatusTracker&) = delete;

protected:
    void                setTotalActions(size_t totalActions);
    void                reset(const std::string &action, size_t totalActions);

    void                incrementStarted();
    void                incrementCompleted();
    AutoTracker         createAutoTrackerCalculationObject();

    void                setCompleted();
    void                setFailed(const char* error);
    void                setToCancel();
    void                setCanceled();

    bool                isCompleted() const;
    bool                isFailed() const;
    ///
    /// \brief isToCancel
    /// \return Returns whether the processing task should be canceled asap
    ///
    bool                isToCancel() const;
    ///
    /// \brief isCanceled
    /// \return Returns whether the processing task was canceled and has been stopped
    ///
    bool                isCanceled() const;
    void                checkToCancel();

    bool                isActionCompleted(const std::string &action) const;

    Status              getStatus() const;

private:
    Status              currentStatus;
    void                cancelExecution() const;

#ifdef WITH_TBB
    typedef tbb::reader_writer_lock::scoped_lock_read read_lock;
    typedef tbb::reader_writer_lock::scoped_lock write_lock;
#else
    typedef int read_lock;
    typedef int write_lock;
#endif

#ifdef WITH_TBB
    mutable tbb::reader_writer_lock
#else
    int
#endif
    lock;
};

} // namespace corecvs

#endif // STATUS_TRACKER

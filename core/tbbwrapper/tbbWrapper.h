#ifndef TBBWRAPPER_H
#define TBBWRAPPER_H
/**
 * \file tbbWrapper.h
 * \brief This is a class that provides common TBB interface
 *
 * This class allows system to work transparently with or without TBB
 *
 **/
#include <cstddef>
#include <iostream>

#ifdef WITH_TBB
/*
 * This jump around is beacause tbb indirectly includes windows.h
   This is a fail. TBB needs to be abolished
*/
#define Polygon Polygon_
#define Ellipse Ellipse_
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <tbb/reader_writer_lock.h>
#include <tbb/spin_mutex.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_sort.h>
#include <tbb/tbb_stddef.h>

#undef Ellipse
#undef Polygon

using namespace tbb;
#endif

#include "core/utils/global.h"

namespace corecvs {

template <typename IndexType>
class BlockedRange {
public:
    typedef std::size_t size_type;

    BlockedRange( IndexType begin, IndexType end, size_type grainsize=1 ) :
        myBegin(begin),
        myEnd(end),
        myGrainsize(grainsize)
    {
    }

    IndexType end() const
    {
        return myEnd;
    }

    IndexType begin() const
    {
        return myBegin;
    }

    size_type size() const
    {
        return size_type(myEnd - myBegin);
    }

    bool empty() const
    {
        return !(myBegin < myEnd);
    }

    bool is_divisible() const
    {
        return myGrainsize < size();
    }

    /**
     * Please not the design here -
     *
     * During the split the input range r is transformed into the first part of the split,
     * and the newly constructed object becomes the second part.
     *
     * \attention
     *   For a newly created part to finish at the appropriate place we need to _first_ initialize
     *   the myEnd , and then myBegin.
     *
     *   TBB blocked_range<Type> version does it by defining end before begin. I don't think
     *   one should give up semantics for precisely following C++ guidelines of using
     *   initialization list.
     *
     **/
#ifdef WITH_TBB
    static const bool is_splittable_in_proportion = true;

    BlockedRange(BlockedRange& r, tbb::split) :
        myGrainsize(r.myGrainsize)
    {
        myEnd = r.myEnd;
        myBegin = do_split(r);
    }

    BlockedRange(BlockedRange &r, tbb::proportional_split &proportion)
        : myGrainsize(r.myGrainsize)
    {
        myEnd = r.myEnd;
        myBegin = do_split(r, proportion);
    }

    static IndexType do_split(BlockedRange &r, tbb::proportional_split &proportion)
    {
        CORE_ASSERT_TRUE(r.is_divisible(), "TBB Wrapper internal error\n");
        auto middle = r.myBegin + (r.myEnd - r.myBegin) / 2u;
        r.myEnd = middle;
        return middle;
    }
#endif

    static IndexType do_split( BlockedRange& r ) {
        CORE_ASSERT_TRUE(r.is_divisible(), "TBB Wrapper internal error\n");
        IndexType middle = r.myBegin + (r.myEnd - r.myBegin) / 2u;
        r.myEnd = middle;
        return middle;
    }

    template<typename R, typename C>
    friend class BlockedRange2d;
private:
    IndexType myBegin;
    IndexType myEnd;
    size_type myGrainsize;

};

template<typename R, typename C>
class BlockedRange2d
{
public:
    typedef BlockedRange<R> row_range_type;
    typedef BlockedRange<C> col_range_type;

    bool empty() const
    {
        return rows.empty() || cols.empty();
    }

    bool is_divisible() const
    {
        return rows.is_divisible() || cols.is_divisible();
    }

    static const bool is_splittable_in_proportion = true;
private:

    template <typename Split>
    void do_split (BlockedRange2d &r, Split &split)
    {
        if (rows.size() * double(cols.grainsize()) < cols.size() * double(rows.grainsize()))
            cols.begin = col_range_type::do_split(r.cols, split);
        else
            rows.begin = row_range_type::do_split(r.cols, split);
    }
    row_range_type rows;
    col_range_type cols;
};

/**
 *   This template provides the dummy call of the function over
 *   the interval without TBB.
 **/
template <typename IndexType, class Function>
void parallelable_for_notbb(IndexType begin, IndexType end, const Function &f)
{
    f(BlockedRange<IndexType>(begin, end));
}


/**
 *   This template provides the parallelable call of the function over
 *   the interval with or without TBB. Tring to be faster
 *   @{
 **/
#ifdef WITH_TBB
template <typename IndexType, class Function>
void parallelable_for(IndexType begin, IndexType end, const Function &f, bool shouldParallel = true)
{
    if (shouldParallel)
        parallel_for(BlockedRange<IndexType>(begin,end), f);
    else
        parallelable_for_notbb(begin, end, f);
}

template <typename IndexType, class Function>
void parallelable_for(IndexType begin, IndexType end, std::size_t grainsize, const Function &f, bool shouldParallel = true)
{
    if (shouldParallel)
        parallel_for(BlockedRange<IndexType>(begin,end,grainsize), f, simple_partitioner());
    else
        parallelable_for_notbb(begin, end, f);
}
#else // WITH_TBB
template <typename IndexType, class Function>
void parallelable_for(IndexType begin, IndexType end, const Function &f, bool /*shouldParallel*/ = true)
{
    parallelable_for_notbb(begin, end, f);
}

template <typename IndexType, class Function>
void parallelable_for(IndexType begin, IndexType end, std::size_t /*grainsize*/, const Function &f, bool /*shouldParallel*/ = true)
{
    parallelable_for_notbb(begin, end, f);
}
#endif // !WITH_TBB
/**@}*/

template <typename RR, typename V, typename F, typename R>
V parallelable_reduce(const RR& range, const V& id, const F &f, const R &r, bool shouldParallel = true)
{
#ifndef WITH_TBB
    return f(range, id);
#else
    if (shouldParallel) {
        return tbb::parallel_reduce(range, id, f, r);
    } else {
        return f(range, id);
    }
#endif
}

template <typename Index, typename Value, typename MapFunction, typename ReductionFunction>
Value parallelable_reduce(const Index& from, const Index& to, const Value& identity, const MapFunction &f, const ReductionFunction &r, bool shouldParallel = true)
{
    return parallelable_reduce(corecvs::BlockedRange<Index>(from, to), identity, f, r, shouldParallel);
}

template <typename Index, typename Value, typename MapFunction, typename ReductionFunction>
Value parallelable_reduce(const Index& from, const Index& to,
                          const typename BlockedRange<Index>::size_type &grainsize,
                          const Value& id,
                          const MapFunction &f, const ReductionFunction &r,
                          bool shoudParallel = true)
{
    return parallelable_reduce(corecvs::BlockedRange<Index>(from, to, grainsize), id, f, r, shoudParallel);
}

#ifdef WITH_TBB
template <typename IndexType, class Function>
void parallelable_reduce(IndexType begin, IndexType end, Function &f, bool shouldParallel = true)
{
    if (shouldParallel) {
        parallel_reduce(BlockedRange<IndexType>(begin,end), f);
    } else {
        f(BlockedRange<IndexType>(begin, end));
    }
}
#else
template <typename IndexType, class Function>
void parallelable_reduce(IndexType begin, IndexType end, Function &f)
{
    f(BlockedRange<IndexType>(begin, end));
}
#endif

/**
 *  Sorting
 **/

#ifdef WITH_TBB
template<typename RandomAccessIterator, typename Compare>
void parallelable_sort( RandomAccessIterator begin, RandomAccessIterator end, const Compare& comp) {
    parallel_sort(begin, end, comp);
}
#else
template<typename RandomAccessIterator, typename Compare>
void parallelable_sort( RandomAccessIterator begin, RandomAccessIterator end, const Compare& comp) {
    sort(begin, end, comp);
}
#endif



inline std::string tbbInfo()
{
    char info[256];
#ifdef WITH_TBB
    snprintf2buf(info, "TBB is ON: %d.%d compatVer:%d ifcVer:%d runtimeVer:%d useExcp:%d useCapExcp:%d"
        , TBB_VERSION_MAJOR, TBB_VERSION_MINOR
        , TBB_COMPATIBLE_INTERFACE_VERSION, TBB_INTERFACE_VERSION
        , TBB_runtime_interface_version()
        , TBB_USE_EXCEPTIONS, TBB_USE_CAPTURED_EXCEPTION
        );
#else
    snprintf2buf(info, "TBB is OFF");
#endif
    return info;
}

/** Useful class to setmaximum number of working threads for the whole TBB environment for any application on startup
 */
#ifdef WITH_TBB
    class TbbSchedulerInitializer : public tbb::task_scheduler_init
    {
    public:
        TbbSchedulerInitializer(const char* varName = "CALIBRATION_TEST_THREAD_LIMIT")
            : tbb::task_scheduler_init(std::getenv(varName) ? std::stoi(std::getenv(varName)) : automatic)
        {
            char* var = std::getenv(varName);
            if (var) {
                std::cout << "Detected thread limit env.var: " << var << std::endl;
            }
        }
    };
#else
    class TbbSchedulerInitializer {
    public:
        TbbSchedulerInitializer(const char* /*var*/ = "") {}
    };
#endif

} //namespace corecvs

#endif // TBBWRAPPER_H

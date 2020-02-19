#ifndef WISDOM_H
#define WISDOM_H

#include <array>
#include <iostream>
#include <vector>
#include <mutex>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <tuple>
#include <iomanip>
#include <functional>

#include "utils/global.h"
#include "tbbwrapper/tbbWrapper.h"

namespace corecvs
{

template<typename T>
struct Characterizator
{
    static const int CHARACTERISTIC_WIDTH = 1;
    typedef std::array<int, CHARACTERISTIC_WIDTH> characteristic_type;

    static characteristic_type Characterize(const T&)
    {
        return characteristic_type({0});
    }
};

template <size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<int, N> &a)
{
    os << "[";
    for (auto& i: a)
        os << i << (&i == &*a.rbegin() ? "" : ", ");
    os << "]";
    return os;
}

enum class AcceleratorTypes
{
    CPU,
    CUDA
};

std::ostream& operator<<(std::ostream& os, const AcceleratorTypes &type);

struct AcceleratorControllerImpl
{
    typedef std::unique_lock<std::mutex> lock_type;

    virtual lock_type tryLock(int id) = 0;
    virtual lock_type lock(int id) = 0;
    virtual int getNumber() = 0;

    virtual ~AcceleratorControllerImpl() {}
};

struct CpuController : AcceleratorControllerImpl
{
    lock_type tryLock(int id);
    lock_type lock(int id);
    int getNumber();

    CpuController();

    std::mutex mutex;
};

#ifdef WITH_CUDA
struct CudaGpuController : AcceleratorControllerImpl
{
    lock_type tryLock(int id);
    lock_type lock(int id);
    int getNumber();

    CudaGpuController();

    std::unique_ptr<std::mutex[]> cudaGpus;
    int N;
};
#endif

extern "C"
{
void* cpuptr();
void* gpuptr();
}


struct AcceleratorController
{
    typedef AcceleratorControllerImpl::lock_type lock_type;

    static lock_type    Lock(const AcceleratorTypes &a, int id)
    {
        return GetInstance().lock(a, id);
    }
    static lock_type TryLock(const AcceleratorTypes &a, int id)
    {
        return GetInstance().tryLock(a, id);
    }
    static int GetNumber(const AcceleratorTypes &a)
    {
        return GetInstance().getNumber(a);
    }
    static void Register(const AcceleratorTypes &a, AcceleratorControllerImpl* impl)
    {
        auto &v = GetInstance().controllers;
        for (auto vv : v)
            if (vv.first == a) {
                CORE_ASSERT_TRUE_S(false);
            }
        v.emplace_back(a, impl);
    }
private:
    int getNumber(const AcceleratorTypes &a)
    {
        for (auto& p: controllers)
            if (p.first == a)
                return p.second->getNumber();
        return 0;
    }
    lock_type    lock(const AcceleratorTypes &a, int id)
    {
        for (auto& p: controllers)
            if (p.first == a)
                return p.second->lock(id);

        CORE_ASSERT_TRUE_S(false);
        // This stuff never happens
        std::mutex m;
        return lock_type(m);
    }
    lock_type tryLock(const AcceleratorTypes &a, int id)
    {
        for (auto& p: controllers)
            if (p.first == a)
                return p.second->tryLock(id);

        CORE_ASSERT_TRUE_S(false);
        // This stuff never happens
        std::mutex m;
        return lock_type(m);
    }
    static AcceleratorController& GetInstance()
    {
        static AcceleratorController controller;
#ifdef WITH_CUDA
        if (!gpuptr())
            std::cout << "AcceleratorController: No gpu!?" << std::endl;
#endif
        if (!cpuptr())
            std::cout << "AcceleratorController: No cpu!?" << std::endl;
        return controller;
    }
    AcceleratorController() {}
    std::vector<std::pair<AcceleratorTypes, AcceleratorControllerImpl*>> controllers;
};

template<typename T, typename Implementation, typename R, typename... A>
struct Wisdom
{
    static R WiseRun(T& t, A... a)
    {
        return GetInstance().wiseRun(t, a...);
    }

    static void Register(AcceleratorTypes accelerator, Implementation implementation, std::function<R(int,T&,A...)> f)
    {
        GetInstance().registered[std::make_tuple(accelerator, implementation)] = f;
    }
private:
    static Wisdom& GetInstance()
    {
        static Wisdom w;
        return w;
    }
    R wiseRun(T& t, A... a)
    {
        auto cc = Characterizator<T>::Characterize(t);
        if (finalOrder.count(cc))
        {
            auto &order = finalOrder[cc];
            int acc_id = 0;
            while (true)
            {
                auto acc = order[acc_id];
                acc_id = (acc_id + 1) % order.size();
                auto lock = AcceleratorController::TryLock(std::get<0>(acc), std::get<2>(acc));
                if (lock)
                    return singleRun(std::get<0>(acc), std::get<1>(acc), std::get<2>(acc), t, a...);
            }
        }
        std::cout << "Updating " << typeid(*this).name() << " wisdom" << std::endl;

        int totalTasks = 0;
        std::map<AcceleratorTypes, std::vector<std::tuple<Implementation, int, int>>> amap;
        std::vector<std::tuple<AcceleratorTypes, Implementation, int>> tasks;
        std::vector<double> timings_;
        for (auto &t: registered)
        {
            auto acc = std::get<0>(t.first);
            auto imp = std::get<1>(t.first);
            int accs = AcceleratorController::GetNumber(acc);
            for (int i = 0; i < accs; ++i)
            {
                amap[acc].emplace_back(imp, i, totalTasks++);
                tasks.emplace_back(acc, imp, i);
            }
        }

        CORE_ASSERT_TRUE_S(totalTasks);

        std::vector<AcceleratorTypes> acc;
        for (auto &p: amap)
            acc.emplace_back(p.first);
        timings_.resize(totalTasks);

        int N = (int)acc.size();
        R res;
        parallelable_for(0, N, [&](const BlockedRange<int> &rr)
            {
                R r;
                for (int i = rr.begin(); i != rr.end(); ++i)
                {
                    auto& tasks = amap[acc[i]];
                    for (auto &tt: tasks)
                    {
                        auto accId = std::get<1>(tt);
                        auto accel = acc[i];
                        auto impl = std::get<0>(tt);
                        //auto tid = std::get<2>(tt);

                        auto start = std::chrono::high_resolution_clock::now();
                        auto lock = AcceleratorController::Lock(accel, accId);
                        r = singleRun(accel, impl, accId, t, a...);
                        auto stop = std::chrono::high_resolution_clock::now();

                        auto dlta = (stop - start).count() / 1e9;
                        timings_[std::get<2>(tt)] = dlta;
                    }
                }
                if (rr.begin() == 0)
                    res = r;
            });

        auto& map = timings[cc];
        for (int i = 0; i < totalTasks; ++i)
            map[tasks[i]].push_back(timings_[i]);
        if (map[tasks[0]].size() == RETRIES)
        {
            for (auto& r: map)
                std::sort(r.second.begin(), r.second.end());
            std::vector<int> order(totalTasks);
            for (int i = 0; i < totalTasks; ++i)
                order[i] = i;
            std::sort(order.begin(), order.end(), [&](const int &a, const int &b) { return map[tasks[a]][RETRIES / 2] < map[tasks[b]][RETRIES / 2]; });
            auto &fo = finalOrder[cc];
            auto last = map[tasks[order[totalTasks - 1]]][RETRIES / 2];
            std::cout << "Final order for " << cc << " is :";
            for (auto& i: order)
            {
                fo.emplace_back(tasks[i]);
                auto curr = map[tasks[i]][RETRIES / 2];
                std::cout << std::get<0>(tasks[i]) << "+" << std::get<1>(tasks[i]) << "@" << std::get<2>(tasks[i]) << ":"  << double(last) / curr << " ";
            }
            std::cout << std::endl;
        }
        return res;
    }

    R singleRun(AcceleratorTypes &a, Implementation &i, int id, T& t, A... aa)
    {
        for (auto& p: registered)
            if (std::get<0>(p.first) == a || std::get<1>(p.first) == i)
                return p.second(id, t, aa...);

        std::cout << "Wisdom tried to schedule unknown solver" << std::endl;
        CORE_ASSERT_TRUE_S(false);
        return R();
    }

    Wisdom() {}

    static const int RETRIES = 5;
    std::map<std::tuple<AcceleratorTypes, Implementation>, std::function<R(int,T&,A...)>> registered;
    std::map<typename Characterizator<T>::characteristic_type, std::map<std::tuple<AcceleratorTypes, Implementation, int>, std::vector<double>>> timings;
    std::map<typename Characterizator<T>::characteristic_type, std::vector<std::tuple<AcceleratorTypes, Implementation, int>>> finalOrder;
};

}

#endif // WISDOM_H

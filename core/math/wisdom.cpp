#include "core/math/wisdom.h"

#include <iostream>

#ifdef WITH_CUDA
#include <cuda_runtime.h>
#endif

namespace corecvs
{

std::ostream& operator<<(std::ostream& os, const AcceleratorTypes &type)
{
    switch(type)
    {
    case AcceleratorTypes::CPU:
        os << "\x1b[36mintel-cpu\x1b[0m";
        break;
    case AcceleratorTypes::CUDA:
        os << "\x1b[32mnvidia-gpu\x1b[0m";
        break;
    }
    return os;
}

static std::unique_ptr<CpuController> ptr_c(new CpuController());
void* cpuptr() { return ptr_c.get(); }

#ifdef WITH_CUDA
static std::unique_ptr<CudaGpuController> ptr_g(new CudaGpuController());
void* gpuptr() { return ptr_g.get(); }
#endif


CpuController::lock_type CpuController::tryLock(int id)
{
    CORE_ASSERT_TRUE_S(id == 0);
    return lock_type(mutex, std::try_to_lock);
}

CpuController::lock_type CpuController::lock(int id)
{
    CORE_ASSERT_TRUE_S(id == 0);
    return lock_type(mutex);
}

int CpuController::getNumber()
{
    return 1;
}

CpuController::CpuController()
{
    AcceleratorController::Register(AcceleratorTypes::CPU, this);
}

#ifdef WITH_CUDA
CudaGpuController::CudaGpuController()
{
    std::cout << "=== CUDA-GPU Controller initialization ===" << std::endl;
	N = 0;
    cudaGetDeviceCount(&N);
    std::cout << " Found " << N << " cuda-enabled devices:" << std::endl;
    cudaGpus = std::unique_ptr<std::mutex[]> (new std::mutex[N]);
    for (int i = 0; i < N; ++i)
    {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);
        std::cout << "  " << i << ": " << prop.name << std::endl;
    }
    std::cout << "=== CUDA-GPU Controller initialization ===" << std::endl;

    AcceleratorController::Register(AcceleratorTypes::CUDA, this);
}

CudaGpuController::lock_type CudaGpuController::tryLock(int id)
{
    CORE_ASSERT_TRUE_S(id < N);
    return lock_type(cudaGpus[id], std::try_to_lock);
}

CudaGpuController::lock_type CudaGpuController::lock(int id)
{
    CORE_ASSERT_TRUE_S(id < N);
    return lock_type(cudaGpus[id]);
}

int CudaGpuController::getNumber()
{
    return N;
}


#endif

}

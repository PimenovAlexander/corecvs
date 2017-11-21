#include "windowsMemoryUsageCalculator.h"

#include <windows.h>
#include <Psapi.h>

void WindowsMemoryUsageCalculator::getMemoryUsageImpl()
{
    // nothing here yet
    PROCESS_MEMORY_COUNTERS_EX pmc;
    pmc.cb = sizeof(pmc);
    GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));


    mVirtualSize = pmc.PrivateUsage / 1024.0 / 1024.0;
    mResidentSize = pmc.WorkingSetSize / 1024.0 / 1024.0;
}

void WindowsMemoryUsageCalculator::getTotalMemoryImpl()
{
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);

    mTotalMemory = mem.ullTotalPhys / 1024.0 / 1024.0;
}

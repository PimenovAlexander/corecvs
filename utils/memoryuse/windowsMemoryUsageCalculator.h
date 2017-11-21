#pragma once
#include "memoryUsageCalculator.h"

class WindowsMemoryUsageCalculator : public MemoryUsageCalculator
{
    Q_OBJECT
protected:
    virtual void getMemoryUsageImpl();
    virtual void getTotalMemoryImpl();
};


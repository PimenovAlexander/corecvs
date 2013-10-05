#pragma once

#include "memoryUsageCalculator.h"

class LinuxMemoryUsageCalculator : public MemoryUsageCalculator
{
    Q_OBJECT
protected:
    virtual void getMemoryUsageImpl();
};


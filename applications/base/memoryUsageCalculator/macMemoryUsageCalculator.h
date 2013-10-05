#pragma once
#include "memoryUsageCalculator.h"

class MacMemoryUsageCalculator : public MemoryUsageCalculator
{
    Q_OBJECT
protected:
    virtual void getMemoryUsageImpl();
};



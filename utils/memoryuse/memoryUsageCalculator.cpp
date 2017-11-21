#include <QtCore/QDebug>
#include "memoryUsageCalculator.h"

const double MemoryUsageCalculator::CRITICAL_FRACTION = 0.95;

void MemoryUsageCalculator::getMemoryUsage()
{
    getMemoryUsageImpl();
    getTotalMemoryImpl();

    if (mResidentSize > MemoryUsageCalculator::CRITICAL_FRACTION * mTotalMemory)
    {
        qDebug() << "We are using " << mResidentSize << "Mb which is more than high limit of " << MemoryUsageCalculator::CRITICAL_FRACTION << " * " << mTotalMemory << " = " << MemoryUsageCalculator::CRITICAL_FRACTION * mTotalMemory << "Mb";
        emit memoryOverflow();
    }
}

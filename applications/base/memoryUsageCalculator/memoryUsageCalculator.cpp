#include <QtCore/QDebug>
#include "memoryUsageCalculator.h"

void MemoryUsageCalculator::getMemoryUsage()
{
    getMemoryUsageImpl();

    if (mResidentSize > upperMemoryUsageLimit)
    {
        qDebug() << "We are using " << mResidentSize << "Mb with is bigger then high limit of " << upperMemoryUsageLimit << "Mb";
        emit memoryOverflow();
    }
}

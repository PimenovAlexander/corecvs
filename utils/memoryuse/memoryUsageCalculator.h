#pragma once
#include <QtCore/QObject>


/// Class that gets memory usage info from OS and checks if it is within allowed range or not
class MemoryUsageCalculator : public QObject
{
    Q_OBJECT

public slots:
    /// Recalculate memory usage
    void getMemoryUsage();

signals:
    /// Is thrown if memory usage hits the limit
    void memoryOverflow();

protected:
    /// Platform-specific implementation
    virtual void getMemoryUsageImpl() = 0;
    virtual void getTotalMemoryImpl() { mTotalMemory = 2400; }

public:
    static const double CRITICAL_FRACTION;
    double mTotalMemory;
    double mVirtualSize;
    double mResidentSize;
};


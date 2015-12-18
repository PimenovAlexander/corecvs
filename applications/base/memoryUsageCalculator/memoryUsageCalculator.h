#pragma once
#include <QtCore/QObject>

/// When this limit is hit, memoryOverflow() signal is thrown
const double upperMemoryUsageLimit = 2400.0; // in Mb

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

    double mVirtualSize;
    double mResidentSize;
};


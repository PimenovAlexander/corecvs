#pragma once

// This interface is necessary to make possible class implementing both FeatureDetector/DescriptorExtractor interfaces
class AlgoBase
{
public:
	virtual double getProperty(const std::string &name) const = 0;
	virtual void   setProperty(const std::string &name, const double &value) = 0;

	virtual bool   isParallelable() { return true; }

    virtual ~AlgoBase() {}
};

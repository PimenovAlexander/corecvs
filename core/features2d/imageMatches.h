#ifndef IMAGEMATCHES_H
#define IMAGEMATCHES_H

#include <array>
#include <string>
#include <iostream>
#include <stdint.h>
#include <deque>


struct RawMatch
{
    typedef uint32_t FeatureId;

    FeatureId featureQ;
    FeatureId featureT;

	double distance;
	double best2ndBest;
    static const FeatureId INVALID_MARKER;

    RawMatch(const FeatureId &featureQ = INVALID_MARKER, const FeatureId &featureT = INVALID_MARKER, const double &distance = 1000.0, double best2ndBest = 1.0)
		: featureQ(featureQ), featureT(featureT), distance(distance), best2ndBest(best2ndBest)
	{
	}

	bool isValid() const
	{
		return featureQ != INVALID_MARKER && featureT != INVALID_MARKER;
	}

	friend std::istream& operator>>(std::istream& is, RawMatch &rm);
	friend std::ostream& operator<<(std::ostream& os, const RawMatch &rm);
};

struct Match
{
	uint16_t imgA;
	uint16_t imgB;

    RawMatch::FeatureId featureA;
    RawMatch::FeatureId featureB;

	double distance;
	double best2ndBest;

	Match()
	{
	}
    Match(const uint16_t &imgA, const uint16_t &imgB, const RawMatch::FeatureId &featureA, const RawMatch::FeatureId &featureB, const double &distance, double best2ndBest = 1.0) : distance(distance), best2ndBest(best2ndBest)
	{
		if (imgA < imgB)
		{
			this->imgA = imgA;
			this->featureA = featureA;
			this->imgB = imgB;
			this->featureB = featureB;
		} else
		{
			this->imgA = imgB;
			this->featureA = featureB;
			this->imgB = imgA;
			this->featureB = featureA;
		}
	}
};

struct RefinedMatchSet
{
	size_t imgA;
	size_t imgB;

	std::deque<Match> matches;

	RefinedMatchSet() : imgA(~(size_t)0), imgB(~(size_t)0)
	{
	}

	RefinedMatchSet(const size_t &_imgA, const size_t &_imgB)
	{
		imgB = std::max(_imgA, _imgB);
		imgA = std::min(_imgA, _imgB);
	}
	RefinedMatchSet(const size_t &_imgA, const size_t &_imgB, const std::deque<Match> &_matches) : matches(_matches)
	{
		imgB = std::max(_imgA, _imgB);
		imgA = std::min(_imgA, _imgB);
	}
};

struct RefinedMatches
{
	std::deque<RefinedMatchSet> matchSets;

	void load(std::istream &is);
	void save(std::ostream &os) const;

	void load(const std::string &filename);
	void save(const std::string &filename) const;
};

struct RawMatches
{
	std::deque<std::deque<std::array<RawMatch,2>>> matches;

	void load(std::istream &is);
	void save(std::ostream &os) const;

	void load(const std::string &filename);
	void save(const std::string &filename) const;
};


#endif

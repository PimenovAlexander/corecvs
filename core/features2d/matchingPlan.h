#pragma once

#include <deque>
#include <iostream>
#include <stdint.h>

struct MatchPlanEntry
{
	bool isBetween(uint16_t imgA, uint16_t imgB);

	void save(std::ostream &os) const;
	void load(std::istream &is);

	friend std::ostream& operator<<(std::ostream& os, const MatchPlanEntry& mp);
	friend std::istream& operator>>(std::istream& is, MatchPlanEntry& mp);

	uint16_t queryImg;
	uint16_t trainImg;

    std::deque<uint32_t> queryFeatures;
    std::deque<uint32_t> trainFeatures;
};

struct MatchPlan
{
	void save(std::ostream &os) const;
	void load(std::istream &is);
	void save(const std::string &filename) const;
	void load(const std::string &filename);

	friend std::ostream& operator<<(std::ostream& os, const MatchPlan& mp);
	friend std::istream& operator>>(std::istream& is, MatchPlan& mp);

	std::deque<MatchPlanEntry> plan;
};

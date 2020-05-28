#include "features2d/imageMatches.h"

#include "utils/global.h"

#include <fstream>
#include <iomanip>

const RawMatch::FeatureId RawMatch::INVALID_MARKER = ~((RawMatch::FeatureId)0);

std::istream& operator>>(std::istream& is, RawMatch &rm)
{
	is >> rm.featureQ >> rm.featureT >> rm.distance;
	return is;
}

std::ostream& operator<<(std::ostream& os, const RawMatch &rm)
{
	os << std::setprecision(15) << std::scientific;
	os << std::setw(20) << rm.featureQ << "\t" <<
		std::setw(20) << rm.featureT << "\t" <<
		std::setw(20) << rm.distance << "\t";
	return os;
}

void RawMatches::load(const std::string &filename)
{
	std::ifstream is;
	is.open(filename, std::ifstream::in);

	load(is);
}

void RawMatches::save(const std::string &filename) const
{
	std::ofstream os;
	os.open(filename, std::ofstream::out);

	save(os);
}

void RawMatches::load(std::istream &ifs)
{
	size_t M, K;
	ifs >> M;

	matches.clear();
	matches.resize(M);

	for (size_t i = 0; i < M; ++i)
	{
        CORE_ASSERT_TRUE_S(ifs);

		size_t L;

		ifs >> L;

		matches[i].resize(L);

		for (size_t j = 0; j < L; ++j)
		{
			ifs >> K;
			for (size_t k = 0; k < K; ++k)
			{
				ifs >> matches[i][j][k];
			}

		}
	}
}

void RawMatches::save(std::ostream &ofs) const
{
	ofs.precision(9);

	size_t M = matches.size(), K ;
	ofs << M << std::endl;

	for (size_t i = 0; i < M; ++i)
	{
        CORE_ASSERT_TRUE_S(ofs);

		size_t L = matches[i].size();

		ofs << L << std::endl;

		for (size_t j = 0; j < L; ++j)
		{
			ofs << (K = matches[i][j].size()) << "\t";
			for (size_t k = 0; k < K; ++k)
			{
				ofs << matches[i][j][k];
			}
			ofs << std::endl;
		}
	}
}

void RefinedMatches::load(const std::string &filename)
{
	std::ifstream is;
	is.open(filename, std::ifstream::in);

	load(is);
}

void RefinedMatches::save(const std::string &filename) const
{
	std::ofstream os;
	os.open(filename, std::ofstream::out);

	save(os);
}

void RefinedMatches::load(std::istream &ifs)
{
	size_t M;
	ifs >> M;

	matchSets.clear();
	for (size_t i = 0; i < M; ++i)
	{
		size_t I, J, K;
        CORE_ASSERT_TRUE_S(ifs);
		ifs >> I >> J >> K;

		RefinedMatchSet set(I, J);
		for (size_t j = 0; j < K; ++j)
		{
			size_t mi, mj;
			double dist;
			ifs >> mi >> mj >> dist;
			set.matches.push_back(Match((uint16_t)I, (uint16_t)J, (uint16_t)mi, (uint16_t)mj, dist));
		}
		matchSets.push_back(set);
	}
}

void RefinedMatches::save(std::ostream &ofs) const
{
	ofs.precision(9);

	size_t M = matchSets.size();
	ofs << M << std::endl;

	for (size_t i = 0; i < M; ++i)
	{
		size_t I = matchSets[i].imgA, J = matchSets[i].imgB, K = matchSets[i].matches.size();
		ofs << I << " " << J << " " << K << std::endl;

		for (size_t j = 0; j < K; ++j)
		{
			ofs << std::setw(15) << matchSets[i].matches[j].featureA
				<< std::setw(15) << matchSets[i].matches[j].featureB
				<< std::setw(15) << matchSets[i].matches[j].distance
				<< std::endl;
		}
	}

}

#ifndef VSFMIO_H
#define VSFMIO_H

#include <array>
#include <stdint.h>
#include <vector>
#include <iostream>

struct SiftFeature
{
	float x;
	float y;
	float color;
	float scale;
	float orientation;

	static const size_t DESCRIPTOR_WIDTH = 128;
	static const size_t BINARY_FLOATS = 5;
	std::array<uint8_t, DESCRIPTOR_WIDTH> data;

	double importance;

	SiftFeature() : x(0.f), y(0.f), color(0.f), scale(0.f), orientation(0.f), importance(0.0)
	{
	}

	SiftFeature(float x, float y, float scale = 0.f, float orientation = 0.f, float color = 0.f, double importance = 0.0) : x(x), y(y), color(color), scale(scale), orientation(orientation), importance(importance)
	{
	}

	SiftFeature(float x, float y, uint8_t *data, float scale = 0.f, float orientation = 0.f, float color = 0.f, double importance = 0.0) : x(x), y(y), color(color), scale(scale), orientation(orientation), importance(importance)
	{
		std::copy(data, data + DESCRIPTOR_WIDTH, this->data.begin());
	}
};

std::ostream& operator<<(std::ostream& os, const SiftFeature& f);
std::istream& operator>>(std::istream& is, SiftFeature& f);

struct VsfmSiftIO
{
	static void writeAscii(std::ostream& os, const std::vector<SiftFeature>& features);
	static void readAscii(std::istream& os, std::vector<SiftFeature>& features);
	static void writeBinary(std::ostream& os, const std::vector<SiftFeature>& features);
	static void readBinary(std::istream& os, std::vector<SiftFeature>& features);

	static const uint32_t MAGIC_SIFT;
	static const uint32_t MAGIC_V40;
	static const uint32_t MAGIC_EOF;
};

struct SiftHeader
{
	uint32_t magicName;
	uint32_t magicVersion;
	uint32_t npoint;
	uint32_t keyPointSize;
	uint32_t descriptorSize;

	SiftHeader() :
		magicName(VsfmSiftIO::MAGIC_SIFT),
		magicVersion(VsfmSiftIO::MAGIC_V40),
		npoint(0),
		keyPointSize(SiftFeature::BINARY_FLOATS),
		descriptorSize(SiftFeature::DESCRIPTOR_WIDTH)
	{
	}
};


#endif

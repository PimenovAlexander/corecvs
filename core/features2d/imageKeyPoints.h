#pragma once

#include <vector>
#include <iostream>

#include "runtimeTypeBuffer.h"
#include "rgbColor.h"

typedef std::string DescriptorType;
typedef std::string DetectorType;
typedef std::string MatcherType;

struct KeyPoint
{
	KeyPoint(double x = 0.0, double y = 0.0, double size = 0.0, double angle = -1.0, double response = 0.0, int octave = 0)
		: x(x), y(y), size(size), angle(angle), response(response), octave(octave)
	{}

	friend std::ostream& operator<<(std::ostream& os, const KeyPoint &kp);
	friend std::istream& operator>>(std::istream& is, KeyPoint &kp);

	double x;
	double y;
	double size;
	double angle;
	double response;
	int    octave;
	corecvs::RGBColor color;

    template<typename V>
    void accept(V &visitor)
    {
        visitor.visit(x,         0.0, "x");
        visitor.visit(y,         0.0, "y");
        visitor.visit(size,      0.0, "size");
        visitor.visit(angle,    -1.0, "angle");
        visitor.visit(response,  0.0, "response");
        visitor.visit(octave,      0, "octave");
    }
};

struct ImageKeyPoints
{
	void save(const std::string &filename) const;
	void load(const std::string &filename);
	void save(std::ostream &os) const;
	void load(std::istream &is);

	friend std::ostream& operator<<(std::ostream& os, const ImageKeyPoints& kp);
	friend std::istream& operator>>(std::istream& is, ImageKeyPoints& kp);

	std::vector<KeyPoint> keyPoints;
};


struct ImageDescriptors
{
	RuntimeTypeBuffer mat;
	DescriptorType    type;

	void load(std::istream &is);
	void save(std::ostream &os) const;
	void load(const std::string &filename);
	void save(const std::string &filename) const;
	friend std::ostream& operator<<(std::ostream &os, const ImageDescriptors &b);
	friend std::istream& operator>>(std::istream &is, ImageDescriptors &b);
};

struct Image
{
	ImageDescriptors descriptors;
	ImageKeyPoints   keyPoints;

	size_t           id;
	std::string      filename;

	Image(const size_t &id);
	Image(const size_t &id, const std::string &filename);
};

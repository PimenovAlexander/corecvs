#ifndef IMAGEKEYPOINTS_H
#define IMAGEKEYPOINTS_H

#include <vector>
#include <iostream>

#include "buffers/runtimeTypeBuffer.h"
#include "buffers/rgb24/rgbColor.h"

#include "features2d/algoBase.h"

struct KeyPointArea
{
    double size;
    double angle;
    double response;
    int    octave;
    corecvs::RGBColor color;

    template<typename V>
    void accept(V &visitor)
    {
        visitor.visit(size,      0.0, "size");
        visitor.visit(angle,    -1.0, "angle");
        visitor.visit(response,  0.0, "response");
        visitor.visit(octave,      0, "octave");
    }

    KeyPointArea(
            double size = 0.0,
            double angle = -1.0,
            double response = 0.0,
            int octave = 0)
        : size(size), angle(angle), response(response), octave(octave)
    {}
};

struct KeyPoint : public KeyPointArea
{
    KeyPoint(
            double x = 0.0,
            double y = 0.0,
            double size = 0.0,
            double angle = -1.0,
            double response = 0.0,
            int octave = 0)
        : KeyPointArea(size, angle, response, octave),
          position(x,y)
	{}

	friend std::ostream& operator<<(std::ostream& os, const KeyPoint &kp);
	friend std::istream& operator>>(std::istream& is, KeyPoint &kp);

    corecvs::Vector2dd position;

    inline double &x()
    {
        return position.x();
    }

    inline double &y()
    {
        return position.y();
    }


    template<typename V>
    void accept(V &visitor)
    {
        visitor.visit(position.x(), 0.0, "x");
        visitor.visit(position.y(), 0.0, "y");
        KeyPointArea::accept<V>(visitor);
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
    corecvs::RuntimeTypeBuffer mat;
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
	void*            remapCache;

	Image(const size_t &id);
	Image(const size_t &id, const std::string &filename, void* remapCache = 0);
};

#endif // IMAGEKEYPOINTS_H

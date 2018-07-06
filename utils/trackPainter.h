#ifndef PARALLELTRACKPAINTER_H
#define PARALLELTRACKPAINTER_H

#include <string>
#include <vector>
#include <atomic>
#include <functional>

#include "core/camerafixture/sceneFeaturePoint.h"

namespace cvs {

typedef std::vector<Vector2dd>* Match;

class TrackPainter {
public:
    TrackPainter(std::vector<std::string> images_,
                 std::vector<Match> matches_)
        : images(images_), matches(matches_) { }

	void paintTracksOnImages(bool pairs = true, std::string suffix = "tracks", std::string outDir = "");
    std::vector<std::string> images;
    std::vector<Match> matches;
};

struct ParallelTrackPainter
{
    ParallelTrackPainter(
            std::vector<std::string> &images_,
            std::vector<Match> scene_,
            std::unordered_map<Match, RGBColor> &colorizer_,
            bool pairs_ = false,
			std::string suffix_ = "tracks",
			std::string outDir = "")
			: colorizer(colorizer_), images(images_), scene(scene_), pairs(pairs_), suffix(suffix_), dir(outDir)
    {}

    void operator() (const corecvs::BlockedRange<int> &r) const;

    std::unordered_map<Match, RGBColor> &colorizer;
    std::vector<std::string>            &images;
    std::vector<Match>                  scene;
    bool                                pairs;
	std::string							suffix;
	std::string							dir;
};

}

//namespace std {

//template <>
//struct hash<Match> {
//    std::size_t operator(const &cvs::Match key) const {
//        return std::hash(key.first);
//    }
//};

//}

#endif // PARALLELTRACKPAINTER_H

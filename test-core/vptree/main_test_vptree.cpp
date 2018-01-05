#include "core/utils/global.h"
#include "core/vptree/Vantage-Point tree.h"
#include "gtest/gtest.h"
#include <bitset>

struct GeoCor {
	double latitude;
	double longitude;

	double dist(const GeoCor& p2) {
		double a = (latitude - p2.latitude);
		double b = (longitude - p2.longitude);
		return sqrt(a * a + b * b);
	}
};

struct ImageGrayScaleBits {
	std::bitset<256> bits;

	double dist(const ImageGrayScaleBits &y) { // Hamming distance
		return (bits ^ y.bits).count();
	}
};

TEST(VPTreeTest, BuildingLight) {
	std::vector<GeoCor> points;

	for (double i = 0; i < 100; i++) {
		points.push_back({ i, i });
	}

	VpTree<GeoCor> tree;
	tree.create(points);

	for (int i = 0; i < 100; i++) {
		std::vector<GeoCor> results;
		std::vector<double> distances;
		GeoCor searchPoint = { i, i };
		tree.search(searchPoint, 1, &results, &distances);

		ASSERT_EQ(results.size(), 1);
		ASSERT_LE(abs(distances[0]), 1e-9);
	}
}

TEST(VPTreeTest, InsertingLight) {
	std::vector<GeoCor> points;

	for (double i = 0; i < 100; i++) {
		points.push_back({ i, i });
	}

	VpTree<GeoCor> tree;
	for (int i = 0; i < points.size(); i++) {
		tree.insert(points[i]);
	}

	for (int i = 0; i < 100; i++) {
		std::vector<GeoCor> results;
		std::vector<double> distances;
		GeoCor searchPoint = { i, i };
		tree.search(searchPoint, 1, &results, &distances);

		ASSERT_EQ(results.size(), 1);
		ASSERT_LE(abs(distances[0]), 1e-9);
	}
}

TEST(VPTreeTest, BuildingHard) {
	std::vector<GeoCor> points;

	for (double i = 0; i < 1e5; i++) {
		points.push_back({ i, i });
	}

	VpTree<GeoCor> tree;
	tree.create(points);

	std::vector<GeoCor> results;
	std::vector<double> distances;
	GeoCor searchPoint = { 4.5, 4.5 };
	tree.search(searchPoint, 12, &results, &distances);

	ASSERT_EQ(results.size(), 12);
	for (double i = 0; i < 12; i++) {
		GeoCor cur = { i, i };
		bool check = false;
		for (int j = 0; j < results.size(); j++) {
			if (abs(results[j].latitude - cur.latitude) + abs(results[j].longitude - cur.longitude) < 1e9) {
				check = true;
			}
		}
		ASSERT_TRUE(check);
	}
	
}

TEST(VPTreeTest, BigDimension) {
	std::vector<ImageGrayScaleBits> points;

	ImageGrayScaleBits inclusive;
	inclusive.bits.set(0);
	points.push_back(inclusive);
	for (double i = 0; i < 128; i++) {
		ImageGrayScaleBits img;
		img.bits.set(2 * i);
		img.bits.set(2 * i + 1);
		points.push_back(img);
	}

	VpTree<ImageGrayScaleBits> tree;
	tree.create(points);

	std::vector<ImageGrayScaleBits> results;
	std::vector<double> distances;
	ImageGrayScaleBits zero;
	tree.search(zero, 1, &results, &distances);

	ASSERT_EQ(results.size(), 1);
	ASSERT_EQ(distances[0], 1);
}

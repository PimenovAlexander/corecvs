
#ifdef WITH_EIGEN
#include <Eigen/Dense>

#include "gtest/gtest.h"

TEST(Eigen3Integration, Basic)
{
    Eigen::Matrix3d test;
    test.setZero();
    ASSERT_EQ(test.determinant(), 0.0);
}

#endif // WITH_EIGEN

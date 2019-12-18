#ifndef ASSERTS
#define ASSERTS
#endif

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/polynomial/monom.h"

using namespace corecvs;

using Monom_ = Monom<double, u_int>;


TEST(MonomTests, testRelationalOperators) {
    Monom_ x1y2z3{2, {1, 2, 3}};
    Monom_ x1{12, {1}};
    Monom_ y2{12, {0, 1}};
    Monom_ zero{0, {}};

    ASSERT_EQ(x1y2z3, (Monom_{2, {1, 2, 3}}));
    ASSERT_EQ(x1, (Monom_{12, {1}}));
    ASSERT_EQ(y2, (Monom_{12, {0, 1}}));
    ASSERT_EQ(zero, (Monom_{0, {}}));

    ASSERT_NE(x1y2z3, x1);
    ASSERT_NE(x1y2z3, y2);
    ASSERT_NE(x1y2z3, zero);
    ASSERT_NE(x1, y2);
    ASSERT_NE(x1, zero);
    ASSERT_NE(y2, zero);

    // zero < y2 < x1 < x1y2z3
    ASSERT_LT(zero, y2);
    ASSERT_LT(y2, x1);
    ASSERT_LT(x1, x1y2z3);

    // x1y2z3 > x1 > y2 > zero
    ASSERT_GT(x1y2z3, x1);
    ASSERT_GT(x1, y2);
    ASSERT_GT(y2, zero);
}

TEST(MonomTests, testAddition) {
    Monom_ zero{};

    const auto test = [](Monom_ arg1, Monom_ arg2, const Monom_& res) {
        ASSERT_TRUE(arg1.isSimilarTo(arg2) && arg2.isSimilarTo(arg1));
        ASSERT_EQ(arg1 + arg2, res);
        ASSERT_EQ(arg2 + arg1, res);
        ASSERT_EQ(arg1 += arg2, res);
        ASSERT_EQ(arg2 += arg1, res);
    };

    test(zero, zero, zero);
    test(Monom_{1, {1}}, Monom_{-1, {1}}, zero);
    test(Monom_{1, {1}}, Monom_{1, {1}}, Monom_{2, {1}});
    test(Monom_{1, {1, 2}}, Monom_{1, {1, 2}}, Monom_{2, {1, 2}});
    test(Monom_{1, {0, 0, 2}}, Monom_{2, {0, 0, 2}}, Monom_{3, {0, 0, 2}});
}

TEST(MonomTests, testSubtraction) {
    Monom_ zero{};

    const auto test = [](Monom_ arg1, Monom_ arg2, const Monom_& res) {
        ASSERT_TRUE(arg1.isSimilarTo(arg2) && arg2.isSimilarTo(arg1));
        ASSERT_EQ(arg1 - arg2, res);
        ASSERT_EQ(arg2 - arg1, res);
        ASSERT_EQ(arg1 -= arg2, res);
        ASSERT_EQ(arg2 -= arg1, res);
    };

    test(zero, zero, zero);
    test(Monom_{1, {1}}, Monom_{1, {1}}, zero);
    test(Monom_{1, {1, 2}}, Monom_{1, {1, 2}}, zero);
    test(Monom_{2, {1}}, Monom_{1, {1}}, Monom_{1, {1}});
    test(Monom_{2, {1, 2}}, Monom_{1, {1, 2}}, Monom_{1, {1, 2}});
    test(Monom_{4, {0, 0, 2}}, Monom_{2, {0, 0, 2}}, Monom_{2, {0, 0, 2}});
}

TEST(MonomTests, testMultiplication) {
    Monom_ zero{};

    const auto test = [](Monom_ arg1, Monom_ arg2, const Monom_& res) {
        ASSERT_EQ(arg1 * arg2, res);
        ASSERT_EQ(arg2 * arg1, res);
        ASSERT_EQ(arg1 *= arg2, res);
        ASSERT_EQ(arg2 *= arg1, res);
    };

    test(zero, zero, zero);
    test(Monom_{1, {1}}, zero, zero);
    test(zero, Monom_{1, {1}}, zero);
    test(Monom_{1, {1}}, Monom_{1, {1}}, Monom_{1, {2}});
    test(Monom_{2, {1}}, Monom_{3, {1}}, Monom_{6, {2}});
    test(zero, Monom_{2, {1, 2}}, zero);
    test(Monom_{2, {1, 2}}, Monom_{3, {1, 2}}, Monom_{6, {2, 4}});
    test(Monom_{2, {1, 2}}, Monom_{3, {1}}, Monom_{6, {2, 2}});
    test(Monom_{2, {1}}, Monom_{3, {1, 2}}, Monom_{6, {2, 2}});
    test(Monom_{1, {1, 2}}, Monom_{0, {1, 2}}, zero);
    test(Monom_{2, {1, 2, 3}}, Monom_{3, {0, 1, 2, 3}},
         Monom_{6, {1, 3, 5, 3}});
}

TEST(MonomTests, testDivision) {
    Monom_ zero{};

    const auto test = [](Monom_ arg1, const Monom_& arg2, const Monom_& res) {
        ASSERT_TRUE(arg1.isDividableBy(arg2));
        ASSERT_EQ(arg1 / arg2, res);
        ASSERT_EQ(arg1 /= arg2, res);
    };

    test(zero, Monom_{1, {1}}, zero);
    test(Monom_{1, {1}}, Monom_{1, {1}}, Monom_{1, {}});
    test(Monom_{3, {1}}, Monom_{2, {1}}, Monom_{1.5, {}});
    test(zero, Monom_{2, {1, 2}}, zero);
    test(Monom_{3, {1, 2}}, Monom_{2, {1, 1}}, Monom_{1.5, {0, 1}});
    test(Monom_{0, {1, 2}}, Monom_{1, {1}}, zero);
    test(Monom_{3, {1, 2}}, Monom_{2, {1}}, Monom_{1.5, {0, 2}});
    test(Monom_{3, {1, 2, 2, 3}}, Monom_{2, {1, 0, 1}},
         Monom_{1.5, {0, 2, 1, 3}});
}

TEST(MonomTests, leastCommonMultiple) {
    ASSERT_EQ(lcm(Monom_{1, {1}}, Monom_{1, {1}}), (Monom_{1, {1}}));
    ASSERT_EQ(lcm(Monom_{2, {1}}, Monom_{3, {1}}), (Monom_{6, {1}}));
    ASSERT_EQ(lcm(Monom_{2, {1, 2}}, Monom_{3, {1, 2}}), (Monom_{6, {1, 2}}));
    ASSERT_EQ(lcm(Monom_{2, {1, 2}}, Monom_{3, {1, 3}}), (Monom_{6, {1, 3}}));
    ASSERT_EQ(lcm(Monom_{2, {2, 2}}, Monom_{3, {1, 2}}), (Monom_{6, {2, 2}}));
    ASSERT_EQ(lcm(Monom_{2, {1, 2}}, Monom_{3, {1}}), (Monom_{6, {1, 2}}));
    ASSERT_EQ(lcm(Monom_{2, {1}}, Monom_{3, {1, 2}}), (Monom_{6, {1, 2}}));
    ASSERT_EQ(lcm(Monom_{2, {1, 2, 3}}, Monom_{3, {0, 1, 2, 3}}),
              (Monom_{6, {1, 2, 3, 3}}));
}
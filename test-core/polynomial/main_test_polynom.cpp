#ifndef ASSERTS
#define ASSERTS
#endif

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/polynomial/monom.h"
#include "core/polynomial/polynom.h"

using namespace corecvs;

using Polynom_ = Polynom<double, u_int>;
using Monom_ = Monom<double, u_int>;


TEST(PolynomTests, testAddition) {
    Polynom_ zero{};
    Polynom_ a{Monom_{1, {1}}, Monom_{2, {1, 1}}};    //  x + 2xy
    Polynom_ b{Monom_{3, {0, 1}}, Monom_{1, {1, 1}}}; // 3y +  xy
    Polynom_ c{Monom_{1, {0, 2, 1}}};                 //       y^2*z
    Polynom_ d{Monom_{2, {1}}, Monom_{3, {0, 2, 1}}}; // 2x + 3y^2*z

    const auto test = [](Polynom_ arg1, Polynom_ arg2, const Polynom_& res) {
        ASSERT_EQ(arg1 + arg2, res);
        ASSERT_EQ(arg2 + arg1, res);
        ASSERT_EQ(arg1 += arg2, res);
        ASSERT_EQ(arg2 += arg1, res);
    };

    test(zero, zero, zero);
    test(a, zero, a);
    test(b, zero, b);
    test(c, zero, c);
    test(d, zero, d);
    test(a, b, Polynom_{Monom_{1, {1}}, Monom_{3, {0, 1}}, Monom_{3, {1, 1}}});
    test(c, d, Polynom_{Monom_{2, {1}}, Monom_{4, {0, 2, 1}}});
    test(a + b, c + d, Polynom_{Monom_{3, {1}}, Monom_{3, {0, 1}}, Monom_{3, {1, 1}}, Monom_{4, {0, 2, 1}}});
    test(a, (-2) * b, Polynom_{Monom_{1, {1}}, Monom_{-3, {0, 1}}});
    test(d, (-3) * c, Polynom_{Monom_{2, {1}}});
    test((-2) * a, d + (-3) * c, Polynom_{Monom_{6, {0, 1}}});
}

TEST(PolynomTests, testSubtraction) {
    Polynom_ zero{};
    Polynom_ a{Monom_{1, {1}}, Monom_{2, {1, 1}}};    //  x + 2xy
    Polynom_ b{Monom_{3, {0, 1}}, Monom_{1, {1, 1}}}; // 3y +  xy
    Polynom_ c{Monom_{1, {0, 2, 1}}};                 //       y^2*z
    Polynom_ d{Monom_{2, {1}}, Monom_{3, {0, 2, 1}}}; // 2x + 3y^2*z

    const auto test = [](Polynom_ arg1, Polynom_ arg2, const Polynom_& res) {
        ASSERT_EQ(arg1 - arg2, res);
        ASSERT_EQ(arg2 - arg1, res);
        ASSERT_EQ(arg1 -= arg2, res);
        ASSERT_EQ(arg2 -= arg1, res);
    };

    test(zero, zero, zero);
    test(zero, zero, zero);
    test(a, zero, a);
    test(b, zero, b);
    test(c, zero, c);
    test(d, zero, d);
    test(a, b, Polynom_{Monom_{1, {1}}, Monom_{-3, {0, 1}}, Monom_{1, {1, 1}}});
    test(c, d, Polynom_{Monom_{-2, {1}}, Monom_{-2, {0, 2, 1}}});
    test(a + b, c + d, Polynom_{Monom_{-1, {1}}, Monom_{3, {0, 1}}, Monom_{3, {1, 1}}, Monom_{-4, {0, 2, 1}}});
    test(a, 2 * b, Polynom_{Monom_{1, {1}}, Monom_{-3, {0, 1}}});
    test(d, 3 * c, Polynom_{Monom_{2, {1}}});
    test(d + (-3) * c, 2 * a, Polynom_{Monom_{6, {0, 1}}});
}

TEST(PolynomTests, testMultiplication) {
    Polynom_ zero{};

    const auto test = [](Polynom_ arg1, Polynom_ arg2, const Polynom_& res) {
        ASSERT_EQ(arg1 * arg2, res);
        ASSERT_EQ(arg2 * arg1, res);
        ASSERT_EQ(arg1 *= arg2, res);
        ASSERT_EQ(arg2 *= arg1, res);
    };

    test(zero, zero, zero);
//    test(Polynom_{1, {1}}, zero, zero);
//    test(zero, Polynom_{1, {1}}, zero);
//    test(Polynom_{1, {1}}, Polynom_{1, {1}}, Polynom_{1, {2}});
//    test(Polynom_{2, {1}}, Polynom_{3, {1}}, Polynom_{6, {2}});
//    test(zero, Polynom_{2, {1, 2}}, zero);
//    test(Polynom_{2, {1, 2}}, Polynom_{3, {1, 2}}, Polynom_{6, {2, 4}});
//    test(Polynom_{2, {1, 2}}, Polynom_{3, {1}}, Polynom_{6, {2, 2}});
//    test(Polynom_{2, {1}}, Polynom_{3, {1, 2}}, Polynom_{6, {2, 2}});
//    test(Polynom_{1, {1, 2}}, Polynom_{0, {1, 2}}, zero);
//    test(Polynom_{2, {1, 2, 3}}, Polynom_{3, {0, 1, 2, 3}},
//         Polynom_{6, {1, 3, 5, 3}});
}

TEST(PolynomTests, testDivision) {
    Polynom_ zero{};

    const auto test = [](Polynom_ arg1, const Polynom_& arg2, const Polynom_& res) {
        ASSERT_TRUE(arg1.isDividableBy(arg2));
        ASSERT_EQ(arg1 / arg2, res);
        ASSERT_EQ(arg1 /= arg2, res);
    };

//    test(zero, Polynom_{1, {1}}, zero);
//    test(Polynom_{1, {1}}, Polynom_{1, {1}}, Polynom_{1, {}});
//    test(Polynom_{3, {1}}, Polynom_{2, {1}}, Polynom_{1.5, {}});
//    test(zero, Polynom_{2, {1, 2}}, zero);
//    test(Polynom_{3, {1, 2}}, Polynom_{2, {1, 1}}, Polynom_{1.5, {0, 1}});
//    test(Polynom_{0, {1, 2}}, Polynom_{1, {1}}, zero);
//    test(Polynom_{3, {1, 2}}, Polynom_{2, {1}}, Polynom_{1.5, {0, 2}});
//    test(Polynom_{3, {1, 2, 2, 3}}, Polynom_{2, {1, 0, 1}},
//         Polynom_{1.5, {0, 2, 1, 3}});
}

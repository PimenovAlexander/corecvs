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
        Polynom_ tmp{arg1};
        ASSERT_EQ(arg1 += arg2, res);
        ASSERT_EQ(arg2 += tmp, res);
    };

    test(zero, zero, zero);
    test(a, zero, a);
    test(b, zero, b);
    test(c, zero, c);
    test(d, zero, d);
    test(a, b, Polynom_{Monom_{1, {1}}, Monom_{3, {0, 1}}, Monom_{3, {1, 1}}});
    test(c, d, Polynom_{Monom_{2, {1}}, Monom_{4, {0, 2, 1}}});
    test(a + b, c + d, Polynom_{Monom_{3, {1}}, Monom_{3, {0, 1}}, Monom_{3, {1, 1}}, Monom_{4, {0, 2, 1}}});
    test(a, (-2) * b, Polynom_{Monom_{1, {1}}, Monom_{-6, {0, 1}}});
    test(d, (-3) * c, Polynom_{Monom_{2, {1}}});
    test((-2) * a, d + (-3) * c, Polynom_{Monom_{-4, {1, 1}}});
}

TEST(PolynomTests, testSubtraction) {
    Polynom_ zero{};
    Polynom_ a{Monom_{1, {1}}, Monom_{2, {1, 1}}};    //  x + 2xy
    Polynom_ b{Monom_{3, {0, 1}}, Monom_{1, {1, 1}}}; // 3y +  xy
    Polynom_ c{Monom_{1, {0, 2, 1}}};                 //       y^2*z
    Polynom_ d{Monom_{2, {1}}, Monom_{3, {0, 2, 1}}}; // 2x + 3y^2*z

    const auto test = [](Polynom_ arg1, Polynom_ arg2, const Polynom_& res) {
        ASSERT_EQ(arg1 - arg2, res);
        ASSERT_EQ(arg2 - arg1, (-1) * res);
        Polynom_ tmp{arg1};
        ASSERT_EQ(arg1 -= arg2, res);
        ASSERT_EQ(arg2 -= tmp, (-1) * res);
    };

    test(zero, zero, zero);
    test(a, zero, a);
    test(b, zero, b);
    test(c, zero, c);
    test(d, zero, d);
    test(a, b, Polynom_{Monom_{1, {1}}, Monom_{-3, {0, 1}}, Monom_{1, {1, 1}}});
    test(c, d, Polynom_{Monom_{-2, {1}}, Monom_{-2, {0, 2, 1}}});
    test(a + b, c + d, Polynom_{Monom_{-1, {1}}, Monom_{3, {0, 1}}, Monom_{3, {1, 1}}, Monom_{-4, {0, 2, 1}}});
    test(a, 2 * b, Polynom_{Monom_{1, {1}}, Monom_{-6, {0, 1}}});
    test(d, 3 * c, Polynom_{Monom_{2, {1}}});
    test(d + (-3) * c, 2 * a, Polynom_{Monom_{-4, {1, 1}}});
}

TEST(PolynomTests, testMultiplication) {
    Polynom_ zero{};
    Polynom_ a{Monom_{1, {1}}, Monom_{2, {1, 1}}};    //  x + 2xy
    Polynom_ b{Monom_{3, {0, 1}}, Monom_{1, {1, 1}}}; // 3y +  xy
    Polynom_ c{Monom_{1, {0, 2, 1}}};                 //       y^2*z
    Polynom_ d{Monom_{2, {1}}, Monom_{3, {0, 2, 1}}}; // 2x + 3y^2*z

    const auto test = [](Polynom_ arg1, Polynom_ arg2, const Polynom_& res) {
        ASSERT_EQ(arg1 * arg2, res);
        ASSERT_EQ(arg2 * arg1, res);
        Polynom_ tmp{arg1};
        ASSERT_EQ(arg1 *= arg2, res);
        ASSERT_EQ(arg2 *= tmp, res);
    };

    test(zero, zero, zero);
    test(a, zero, zero);
    test(b, zero, zero);
    test(c, zero, zero);
    test(d, zero, zero);
    test(a, a, Polynom_{Monom_{4, {2, 2}}, Monom_{4, {2, 1}}, Monom_{1, {2, 0}}});
    test(a, b, Polynom_{Monom_{2, {2, 2}}, Monom_{1, {2, 1}}, Monom_{6, {1, 2}}, Monom_{3, {1, 1}}});
    test(a, c, Polynom_{Monom_{2, {1, 3, 1}}, Monom_{1, {1, 2, 1}}});
    test(a, d, Polynom_{Monom_{4, {2, 1}}, Monom_{2, {2}}, Monom_{6, {1, 3, 1}}, Monom_{3, {1, 2, 1}}});
    test(b, b, Polynom_{Monom_{1, {2, 2}}, Monom_{6, {1, 2}}, Monom_{9, {0, 2}}});
    test(b, c, Polynom_{Monom_{1, {1, 3, 1}}, Monom_{3, {0, 3, 1}}});
    test(b, d, Polynom_{Monom_{2, {2, 1}}, Monom_{3, {1, 3, 1}}, Monom_{6, {1, 1}}, Monom_{9, {0, 3, 1}}});
    test(c, c, Polynom_{Monom_{1, {0, 4, 2}}});
    test(c, d, Polynom_{Monom_{2, {1, 2, 1}}, Monom_{3, {0, 4, 2}}});
    test(d, d, Polynom_{Monom_{4, {2}}, Monom_{12, {1, 2, 1}}, Monom_{9, {0, 4, 2}}});
    test(a + b + c, d + 2 * a - b, Polynom_{
            Monom_{9, {2, 2}}, Monom_{15, {2, 1}}, Monom_{4, {2}}, Monom_{12, {1, 3, 1}}, Monom_{7, {1, 2, 1}},
            Monom_{9, {1, 1}}, Monom_{3, {0, 4, 2}}, Monom_{6, {0, 3, 1}}, Monom_{-9, {0, 2}}
    });
    test(a + b + c + d, a - b - c - d, Polynom_{
            Monom_{3, {2, 2}}, Monom_{-3, {2}}, Monom_{-8, {1, 3, 1}}, Monom_{-16, {1, 2, 1}}, Monom_{-6, {1, 2}},
            Monom_{-12, {1, 1}}, Monom_{-16, {0, 4, 2}}, Monom_{-24, {0, 3, 1}}, Monom_{-9, {0, 2}}
    });
}

TEST(PolynomTests, testDivision) {
    Polynom_ zero{};
    Polynom_ a{Monom_{1, {1}}, Monom_{2, {1, 1}}};    //  x + 2xy
    Polynom_ b{Monom_{3, {0, 1}}, Monom_{1, {1, 1}}}; // 3y +  xy
    Polynom_ c{Monom_{1, {0, 2, 1}}};                 //       y^2*z
    Polynom_ d{Monom_{2, {1}}, Monom_{3, {0, 2, 1}}}; // 2x + 3y^2*z

    const auto test = [](const Polynom_& mul1, const Polynom_& mul2, Polynom_ product) {
        ASSERT_TRUE(product.isDividableBy(mul1));
        ASSERT_TRUE(product.isDividableBy(mul2));
        ASSERT_EQ(product / mul1, mul2);
        ASSERT_EQ(product / mul2, mul1);
        Polynom_ tmp{product};
        ASSERT_EQ(product /= mul1, mul2);
        ASSERT_EQ(tmp /= mul2, mul1);
    };

    ASSERT_EQ(zero / a, zero);
    ASSERT_EQ(zero / b, zero);
    ASSERT_EQ(zero / c, zero);
    ASSERT_EQ(zero / d, zero);
    test(a, a, Polynom_{Monom_{4, {2, 2}}, Monom_{4, {2, 1}}, Monom_{1, {2, 0}}});
    test(a, b, Polynom_{Monom_{2, {2, 2}}, Monom_{1, {2, 1}}, Monom_{6, {1, 2}}, Monom_{3, {1, 1}}});
    test(a, c, Polynom_{Monom_{2, {1, 3, 1}}, Monom_{1, {1, 2, 1}}});
    test(a, d, Polynom_{Monom_{4, {2, 1}}, Monom_{2, {2}}, Monom_{6, {1, 3, 1}}, Monom_{3, {1, 2, 1}}});
    test(b, b, Polynom_{Monom_{1, {2, 2}}, Monom_{6, {1, 2}}, Monom_{9, {0, 2}}});
    test(b, c, Polynom_{Monom_{1, {1, 3, 1}}, Monom_{3, {0, 3, 1}}});
    test(b, d, Polynom_{Monom_{2, {2, 1}}, Monom_{3, {1, 3, 1}}, Monom_{6, {1, 1}}, Monom_{9, {0, 3, 1}}});
    test(c, c, Polynom_{Monom_{1, {0, 4, 2}}});
    test(c, d, Polynom_{Monom_{2, {1, 2, 1}}, Monom_{3, {0, 4, 2}}});
    test(d, d, Polynom_{Monom_{4, {2}}, Monom_{12, {1, 2, 1}}, Monom_{9, {0, 4, 2}}});
    test(a + b + c, d + 2 * a - b, Polynom_{
            Monom_{9, {2, 2}}, Monom_{15, {2, 1}}, Monom_{4, {2}}, Monom_{12, {1, 3, 1}}, Monom_{7, {1, 2, 1}},
            Monom_{9, {1, 1}}, Monom_{3, {0, 4, 2}}, Monom_{6, {0, 3, 1}}, Monom_{-9, {0, 2}}
    });
    test(a + b + c + d, a - b - c - d, Polynom_{
            Monom_{3, {2, 2}}, Monom_{-3, {2}}, Monom_{-8, {1, 3, 1}}, Monom_{-16, {1, 2, 1}}, Monom_{-6, {1, 2}},
            Monom_{-12, {1, 1}}, Monom_{-16, {0, 4, 2}}, Monom_{-24, {0, 3, 1}}, Monom_{-9, {0, 2}}
    });
}

#ifndef ASSERTS
#define ASSERTS
#endif

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/polynomial/basis.h"
#include "core/polynomial/finiteField.h"

using namespace corecvs;


TEST(BasisTests, testNormalizedMinRedGroebner_OverDoubleType) {
    using Basis_ = Basis<double, u_int>;
    using Monom_ = Monom<double, u_int>;
    using Polynom_ = Polynom<double, u_int>;

    Polynom_ x{Monom_{1, {1}}};
    Polynom_ y{Monom_{1, {0, 1}}};
    Polynom_ z{Monom_{1, {0, 0, 1}}};

    // --------------------------------------------------------
    // from https://www.mccme.ru/free-books/dubna/arjantsev.pdf
    // section 4, tasks 4.7
    // --------------------------------------------------------
    // 1
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - 1, (x - 1) * y, (x + 1) * z}),
            (Basis_{(x ^ 2u) - 1, (x - 1) * y, (x + 1) * z, y * z})
    );
    // 2
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - 1, (x - 1) * y, (x - 1) * z}),
            (Basis_{(x ^ 2u) - 1, (x - 1) * y, (x - 1) * z})
    );
    // 3
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 3u) * y * z - x * (z ^ 2u),
                    x * (y ^ 2u) * z - x * y * z,
                    (x ^ 2u) * (y ^ 2u) - z}),
            (Basis_{
                    x * (y ^ 2u) * z - x * y * z,
                    (x ^ 2u) * (y ^ 2u) - z,
                    (x ^ 2u) * y * z - (z ^ 2u),
                    y * (z ^ 2u) - (z ^ 2u),
                    (x ^ 2u) * (z ^ 2u) - (z ^ 3u)
            })
    );
    // 4
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 2u) * y + x * z + (y ^ 2u) * z,
                    x * (z ^ 2u) - y * z,
                    x * y * z - (y ^ 2u)
            }),
            (Basis_{
                    (x ^ 2u) * y + x * z + (y ^ 2u) * z,
                    x * (z ^ 2u) - y * z,
                    x * y * z - (y ^ 2u),
                    (y ^ 3u) + (y ^ 2u) * (z ^ 3u) + y * (z ^ 2u),
                    x * (y ^ 2u) + (y ^ 2u) * (z ^ 2u) + y * z
            })
    );
    // 5
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * (y ^ 2u) - z - (z ^ 2u),
                    (x ^ 2u) * y - y,
                    (y ^ 2u) - (z ^ 2u)
            }),
            (Basis_{
                    (x ^ 2u) * y - y,
                    (y ^ 2u) + 0.5 * z,
                    (z ^ 2u) + 0.5 * z,
                    x * z + z
            })
    );
    // 6
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y + (x ^ 2u) * z,
                    x * z + y * (z ^ 3u),
                    y * z - (y ^ 2u) * (z ^ 3u)
            }),
            (Basis_{
                    x * z + y * (z ^ 3u),
                    x * y + y * (z ^ 3u),
                    y * (z ^ 4u) - y * z,
                    (y ^ 2u) * z - y * (z ^ 2u)
            })
    );

    // --------------------------------------------------------
    // from https://www.mccme.ru/free-books/dubna/arjantsev.pdf
    // section 5, tasks 5.6
    // --------------------------------------------------------

    // 1
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - 1, x * y - y, x * z + z}),
            (Basis_{(x ^ 2u) - 1, x * y - y, x * z + z, y * z})
    );
    // 2
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) + (y ^ 2u) + (z ^ 2u), x + y - z, y + (z ^ 2u)}),
            (Basis_{x - (z ^ 2u) - z, y + (z ^ 2u), (z ^ 4u) + (z ^ 3u) + (z ^ 2u)})
    );
    // 3
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{x * z - 2 * y + 1, y * z + z - 1, x * y * z + y * z + z}),
            (Basis_{x + 4 * z - 1, y + 2 * z - 1.5, (z ^ 2u) - 1.25 * z + 0.5})
    );
    // 4
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 3u) * y * z - x * (z ^ 2u),
                    x * (y ^ 2u) * z - x * y * z,
                    (x ^ 2u) * (y ^ 2u) - z
            }),
            (Basis_{
                    (x ^ 2u) * (y ^ 2u) - z,
                    (x ^ 2u) * y * z - (z ^ 2u),
                    (x ^ 2u) * (z ^ 2u) - (z ^ 3u),
                    x * (y ^ 2u) * z - x * y * z,
                    (z ^ 2u) * y - (z ^ 2u)
            })
    );
    // 5
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{x * (y ^ 2u) - (z ^ 2u) - z, (x ^ 2u) * y - y, (y ^ 2u) - (z ^ 2u)}),
            (Basis_{(x ^ 2u) * y - y, x * z + z, (y ^ 2u) + 0.5 * z, (z ^ 2u) + 0.5 * z})
    );
    // 8
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y - x * z + (y ^ 2u),
                    (x ^ 2u) * y - (x ^ 2u) + y * z,
                    x * y - x - y
            }),
            (Basis_{
                    x + y * z + y,
                    (y ^ 2u) + (z ^ 2u) * y,
                    y * z + (z ^ 2u) * y + (z ^ 3u) * y
            })
    );
    // 9
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 2u) + y * z + z,
                    x * y * z + x * z - (y ^ 3u),
                    x * z + (y ^ 2u)
            }),
            (Basis_{
                    (x ^ 2u) + y * z + z,
                    x * z - 2 * (z ^ 3u),
                    (y ^ 2u) + 2 * (z ^ 3u),
                    (z ^ 2u) * y + 0.5 * (z ^ 2u),
                    (z ^ 5u) + 0.125 * (z ^ 2u)
            })
    );
}

/*
 * This test requires Monom::PRECISION be no less than 1e-2,
 * otherwise it diverges.
 * */
TEST(BasisTests, DISABLED_testNormalizedMinRedGroebner_OverDoubleType_SmallPrecisionRequired) {
    using Basis_ = Basis<double, u_int>;
    using Monom_ = Monom<double, u_int>;
    using Polynom_ = Polynom<double, u_int>;

    Polynom_ x{Monom_{1, {1}}};
    Polynom_ y{Monom_{1, {0, 1}}};
    Polynom_ z{Monom_{1, {0, 0, 1}}};

    // 6
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y + z - 1,
                    x - y - (z ^ 2u),
                    (x ^ 2u) - 2 * y + 1
            }),
            (Basis_{
                    x - 0.05882 * (z ^ 5u) - 0.3529 * (z ^ 4u) - 0.14705 * (z ^ 3u) - 0.70588 * (z ^ 2u) + 0.5 * z - 1,
                    y - 0.05882 * (z ^ 5u) - 0.3529 * (z ^ 4u) - 0.14705 * (z ^ 3u) + 0.29411 * (z ^ 2u) + 0.5 * z - 1,
                    (z ^ 6u) + 0.5 * (z ^ 4u) - 3 * (z ^ 3u) + 4.5 * (z ^ 2u)
            })
    );
}

/*
 * This test requires Monom::PRECISION to be at most 1e3,
 * otherwise it fails.
 * */
TEST(BasisTests, testNormalizedMinRedGroebner_OverDoubleType_HighPrecisionRequired) {
    using Basis_ = Basis<double, u_int>;
    using Monom_ = Monom<double, u_int>;
    using Polynom_ = Polynom<double, u_int>;

    Polynom_ x{Monom_{1, {1}}};
    Polynom_ y{Monom_{1, {0, 1}}};
    Polynom_ z{Monom_{1, {0, 0, 1}}};

    // 10
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 2u) + z * (y ^ 2u) + y * z,
                    (-1) * x * y + x + (z ^ 2u),
                    x * z + (y ^ 2u) - 1
            }),
            (Basis_{
                    (x ^ 2u) + z * (y ^ 2u) + y * z,
                    x * y - x - (z ^ 2u),
                    x * z + (y ^ 2u) - 1,
                    (y ^ 3u) - (y ^ 2u) - y + (z ^ 3u) + 1,
                    (y ^ 2u) * (z ^ 2u) - (z ^ 2u),
                    z ^ 4u
            })
    );
}

/**
 * Examples results were computed on the Macaulay2 system.
 */
TEST(BasisTests, testNormalizedMinRedGroebner_OverFiniteField) {
    using f_ = FiniteField<int, 101>;
    using Basis_ = Basis<f_, u_int>;
    using Monom_ = Monom<f_, u_int>;
    using Polynom_ = Polynom<f_, u_int>;

    Polynom_ x{Monom_{f_{1}, {1}}};
    Polynom_ y{Monom_{f_{1}, {0, 1}}};
    Polynom_ z{Monom_{f_{1}, {0, 0, 1}}};

    // --------------------------------------------------------
    // from https://www.mccme.ru/free-books/dubna/arjantsev.pdf
    // section 4, tasks 4.7
    // --------------------------------------------------------
    // 1
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - f_{1}, (x - f_{1}) * y, (x + f_{1}) * z}),
            (Basis_{(x ^ 2u) - f_{1}, (x - f_{1}) * y, (x + f_{1}) * z, y * z})
    );
    // 2
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - f_{1}, (x - f_{1}) * y, (x - f_{1}) * z}),
            (Basis_{(x ^ 2u) - f_{1}, (x - f_{1}) * y, (x - f_{1}) * z})
    );
    // 3
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 3u) * y * z - x * (z ^ 2u),
                    x * (y ^ 2u) * z - x * y * z,
                    (x ^ 2u) * (y ^ 2u) - z}),
            (Basis_{
                    x * (y ^ 2u) * z - x * y * z,
                    (x ^ 2u) * (y ^ 2u) - z,
                    (x ^ 2u) * y * z - (z ^ 2u),
                    y * (z ^ 2u) - (z ^ 2u),
                    (x ^ 2u) * (z ^ 2u) - (z ^ 3u)
            })
    );
    // 4
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 2u) * y + x * z + (y ^ 2u) * z,
                    x * (z ^ 2u) - y * z,
                    x * y * z - (y ^ 2u)
            }),
            (Basis_{
                    (x ^ 2u) * y + x * z + (y ^ 2u) * z,
                    x * (z ^ 2u) - y * z,
                    x * y * z - (y ^ 2u),
                    (y ^ 3u) + (y ^ 2u) * (z ^ 3u) + y * (z ^ 2u),
                    x * (y ^ 2u) + (y ^ 2u) * (z ^ 2u) + y * z
            })
    );
    // 5
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * (y ^ 2u) - z - (z ^ 2u),
                    (x ^ 2u) * y - y,
                    (y ^ 2u) - (z ^ 2u)
            }),
            (Basis_{
                    (x ^ 2u) * y - y,
                    (y ^ 2u) - f_{50} * z,
                    (z ^ 2u) - f_{50} * z,
                    x * z + z
            })
    );
    // 6
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y + (x ^ 2u) * z,
                    x * z + y * (z ^ 3u),
                    y * z - (y ^ 2u) * (z ^ 3u)
            }),
            (Basis_{
                    x * z + y * (z ^ 3u),
                    x * y + y * (z ^ 3u),
                    y * (z ^ 4u) - y * z,
                    (y ^ 2u) * z - y * (z ^ 2u)
            })
    );

    // --------------------------------------------------------
    // from https://www.mccme.ru/free-books/dubna/arjantsev.pdf
    // section 5, tasks 5.6
    // --------------------------------------------------------

    // 1
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - f_{1}, x * y - y, x * z + z}),
            (Basis_{(x ^ 2u) - f_{1}, x * y - y, x * z + z, y * z})
    );
    // 2
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) + (y ^ 2u) + (z ^ 2u), x + y - z, y + (z ^ 2u)}),
            (Basis_{x - (z ^ 2u) - z, y + (z ^ 2u), (z ^ 4u) + (z ^ 3u) + (z ^ 2u)})
    );
    // 3
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{x * z - f_{2} * y + f_{1}, y * z + z - f_{1}, x * y * z + y * z + z}),
            (Basis_{x + f_{4} * z - f_{1}, y + f_{2} * z + f_{49}, (z ^ 2u) + f_{24} * z - f_{50}})
    );
    // 4
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 3u) * y * z - x * (z ^ 2u),
                    x * (y ^ 2u) * z - x * y * z,
                    (x ^ 2u) * (y ^ 2u) - z
            }),
            (Basis_{
                    (x ^ 2u) * (y ^ 2u) - z,
                    (x ^ 2u) * y * z - (z ^ 2u),
                    (x ^ 2u) * (z ^ 2u) - (z ^ 3u),
                    x * (y ^ 2u) * z - x * y * z,
                    (z ^ 2u) * y - (z ^ 2u)
            })
    );
    // 5
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{x * (y ^ 2u) - (z ^ 2u) - z, (x ^ 2u) * y - y, (y ^ 2u) - (z ^ 2u)}),
            (Basis_{(x ^ 2u) * y - y, x * z + z, (y ^ 2u) - f_{50} * z, (z ^ 2u) - f_{50} * z})
    );
    // 6
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y + z - f_{1},
                    x - y - (z ^ 2u),
                    (x ^ 2u) - f_{2} * y + f_{1}
            }),
            (Basis_{
                    (z ^ 6u) - f_{50} * (z ^ 4u) - f_{3} * (z ^ 3u) - f_{46} * (z ^ 2u),
                    y - f_{6} * (z ^ 5u) - f_{36} * (z ^ 4u) - f_{15} * (z ^ 3u) + f_{30} * (z ^ 2u) - f_{50} * z - f_{1},
                    x - f_{6} * (z ^ 5u) - f_{36} * (z ^ 4u) - f_{15} * (z ^ 3u) + f_{29} * (z ^ 2u) - f_{50} * z - f_{1}
            })
    );
    // 7
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y + x * z - x - y,
                    (x ^ 2u) * y + (x ^ 2u) + y * z - z,
                    (x ^ 2u) - x - y,
            }),
            (Basis_{
                    (z ^ 3u) + f_{2} * (z ^ 2u) - f_{3} * z,
                    y * z - f_{50} * (z ^ 2u) + f_{50} * z,
                    (y ^ 2u) - f_{34} * (z ^ 2u) + f_{34} * z,
                    x - f_{34} * (z ^ 2u) + y + f_{33} * z
            })
    );
    // 8
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    x * y - x * z + (y ^ 2u),
                    (x ^ 2u) * y - (x ^ 2u) + y * z,
                    x * y - x - y
            }),
            (Basis_{
                    x + y * z + y,
                    (y ^ 2u) + (z ^ 2u) * y,
                    y * z + (z ^ 2u) * y + (z ^ 3u) * y
            })
    );
    // 9
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 2u) + y * z + z,
                    x * y * z + x * z - (y ^ 3u),
                    x * z + (y ^ 2u)
            }),
            (Basis_{
                    (z ^ 5u) + f_{38} * (z ^ 2u),
                    x * z + f_{99} * (z ^ 3u),
                    (y ^ 2u) + f_{2} * (z ^ 3u),
                    (x ^ 2u) + y * z + z,
                    y * (z ^ 2u) + f_{51} * (z ^ 2u)
            })
    );
    // 10
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{
                    (x ^ 2u) + z * (y ^ 2u) + y * z,
                    f_{-1} * x * y + x + (z ^ 2u),
                    x * z + (y ^ 2u) - f_{1}
            }),
            (Basis_{
                    (x ^ 2u) + z * (y ^ 2u) + y * z,
                    x * y - x - (z ^ 2u),
                    x * z + (y ^ 2u) - f_{1},
                    (y ^ 3u) - (y ^ 2u) - y + (z ^ 3u) + f_{1},
                    (y ^ 2u) * (z ^ 2u) - (z ^ 2u),
                    z ^ 4u
            })
    );
}
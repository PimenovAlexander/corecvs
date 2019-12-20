#ifndef ASSERTS
#define ASSERTS
#endif

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/polynomial/basis.h"

using namespace corecvs;

using Basis_ = Basis<double, u_int>;
using Monom_ = Monom<double, u_int>;
using Polynom_ = Polynom<double, u_int>;


TEST(BasisTests, testMinRedGroebner) {
    Polynom_ x{Monom_{1, {1}}};
    Polynom_ y{Monom_{1, {0, 1}}};
    Polynom_ z{Monom_{1, {0, 0, 1}}};

    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - 1, (x - 1) * y, (x + 1) * z}),
            (Basis_{(x ^ 2u) - 1, (x - 1) * y, (x + 1) * z, y * z})
    );
    ASSERT_EQ(
            makeNormalizedMinRedGroebner(Basis_{(x ^ 2u) - 1, (x - 1) * y, (x - 1) * z}),
            (Basis_{(x ^ 2u) - 1, (x - 1) * y, (x - 1) * z})
    );
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
}

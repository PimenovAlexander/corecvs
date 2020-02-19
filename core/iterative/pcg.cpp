#include "iterative/pcg.h"

namespace corecvs
{
std::ostream& operator<<(std::ostream& o, const PCGStatus &s)
{
#define C(c, s) \
        case PCGStatus::c: \
            o << s; \
            break;

    switch (s)
    {
        C(RUNNING,     "solver is running")
        C(ZERO,        "x = 0 is an exact solution")
        C(CONVERGED,   "solved Ax=b, given rtol")
        C(STAGNATED,   "stagnated for two consecutive iterations")
        C(ITERATION,   "reached iteration limit")
        C(DIVERGED,    "scalar quantites become too small or too large")
        C(BAD_PRECON,  "Bad (non-posdef) preconditioner?!")
    }
    return o;
#undef C
}

};

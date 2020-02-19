#include "iterative/minresQLP.h"

namespace corecvs
{
std::ostream& operator<<(std::ostream& o, const MinresQLPStatus &s)
{
#define C(c, s) \
        case MinresQLPStatus::c: \
            o << s; \
            break;

    switch (s)
    {
        C(RUNNING,    "solver is running")
        C(BOTH_EIGEN, "b and x are eigenvectors")
        C(ZERO,       "x = 0 is an exact solution")
        C(SOLVED_RTOL,"solved Ax=b, given rtol")
        C(MINLEN_RTOL,"found min-length solution for singular problem, given rtol")
        C(SOLVED_EPS, "solved Ax=b, given eps")
        C(MINLEN_EPS, "found min-length solution for singular problem, given eps")
        C(EIGEN,      "x converged to eigenvector")
        C(XNORM,      "xnorm exceeded maxXNorm")
        C(ACOND,      "Acond has exceeded ACondLim")
        C(ITERATION,  "reached iteration limit")
        C(LSQ_NOCONV, "LSQ problem, not converged")
        C(BAD_PRECON, "Bad (non-posdef) preconditioner?!")
    }
    return o;
#undef C
}

};

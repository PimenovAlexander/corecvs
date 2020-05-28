#ifndef PCG_H
#define PCG_H

#include <cmath>
#include <chrono>
#include <iomanip>
#include <map>
#include <functional>

#include "wrappers/cblasLapack/cblasLapackeWrapper.h"
#include "math/vector/vector.h"

namespace corecvs
{

enum class PCGStatus
{
    RUNNING      = -1,
    ZERO         =  0,
    CONVERGED    =  1,
    STAGNATED    =  2,
    ITERATION    =  3,
    DIVERGED     =  4,
    BAD_PRECON   = 10
};
std::ostream& operator<<(std::ostream &o, const PCGStatus &s);

struct PCGParams
{
    double rtol     = 2.3e-16,
           eps      = 2.3e-16,
           norm2Lim = 1e100,
           zero2Lim = 1e-100;
    int maxIt = -1;
};

template<typename M>
class PCG : PCGParams
{
    public:
    static PCGStatus Solve(const M &a, const Vector &b, Vector &x, const PCGParams &params = PCGParams())
    {
        PCG solver(a, b, x, params);
        return solver.run();
    }
    static PCGStatus Solve(const M &a, std::function<Vector(const Vector&)> p, const Vector &b, Vector &x, const PCGParams &params = PCGParams())
    {
        PCG solver(a, p, b, x, params);
        return solver.run();
    }
    PCG (const M &A, const Vector &b, Vector &x, const PCGParams &params = PCGParams()) : PCG(A, b, x, (int)b.size(), params)
    {
    }
    PCG (const M &A, std::function<Vector(const Vector&)> P, const Vector &b, Vector &x, const PCGParams &params = PCGParams()) : PCG(A, b, x, (int)b.size(), params)
    {
        this->P = P;
        usePreconditioner = true;
    }
    PCGStatus run()
    {
        xmin = x = Vector(N);
        auto normBefore = !(A * x - b);

        bnorm2 = b & b;
        r = b - A * x;
        rnorm2 = r.sumAllElementsSq();
        tolb = rtol * rtol * bnorm2;

        if (bnorm2 == 0.0 || rnorm2 < tolb)
        {
            flag = PCGStatus::ZERO;
        }
        rho = 1.0;

        while (flag == PCGStatus::RUNNING && iter < maxIt)
        {
            if (!singleStep())
                break;
        }
        auto normAfter = !(A * x - b);
        auto relBefore = normBefore / !b, relAfter = normAfter / !b;
        std::cout << normBefore << " (" << relBefore << ") > " << normAfter << " (" << relAfter << ") [" << normBefore / normAfter << "] @ " << iter << std::endl;
        std::cout << (usePreconditioner ? "PRECONDITIONED-" : "") << "CONJUGATED-GRADIENT status: " << flag << std::endl;

        return flag;
    }
    bool singleStep()
    {
        ++iter;

        if (!residualUpdate())
            return false;

        pcgStep();

        checkExit();
        return true;
    }
private:
    PCG (const M &A, const Vector &b, Vector &x, int N, const PCGParams &params) :
        PCGParams(params),
        A(A), b(b), xmin(x),
        N(N),
        r(N), y(N), z(N), p(N), q(N), st(N), x(N)
    {
        if (maxIt < 0)
            maxIt = N;
    }

    bool residualUpdate()
    {
        if (usePreconditioner)
            y = P(r);
        else
            y = r;
        if (y.sumAllElementsSq() > norm2Lim)
        {
            flag = PCGStatus::BAD_PRECON;
            return false;
        }
        z = y;
        return true;
    }
    void pcgStep()
    {
#define SCALAR_CHECK(a) \
        { \
            auto aa = std::abs(a); \
            if (aa < zero2Lim || aa > norm2Lim) \
            { \
                flag = PCGStatus::DIVERGED; \
                return; \
            } \
        }
        double rho1 = rho;
        rho = r & z;

        SCALAR_CHECK(rho)
        if (iter == 1)
            p = z;
        else
        {
            double beta = rho / rho1;
            SCALAR_CHECK(beta)
            p = z + beta * p;
        }

        q = A * p;
        double pq = p & q;
        SCALAR_CHECK(pq)
        alpha = rho / pq;

        if (alpha == 0.0)
        {
            flag = PCGStatus::STAGNATED;
            return;
        }

        SCALAR_CHECK(alpha)
        checkStagnation();

#ifndef WITH_BLAS
        x = x + alpha * p;
#else
        cblas_daxpy(x.size(), alpha, &p[0], 1, &x[0], 1);
#endif
        rnorm2 = (b - A * x).sumAllElementsSq();

        if (rnorm2 < rnormMin)
        {
            xmin = x;
            rnormMin = rnorm2;
        }

#ifndef WITH_BLAS
        r -= alpha * q;
#else
        cblas_daxpy(r.size(), -alpha, &r[0], 1, &q[0], 1);
#endif
#undef SCALAR_CHECK
    }

    void checkStagnation()
    {
        if (flag == PCGStatus::STAGNATED)
            return;
        double maxS = 0.0;
        for (int i = 0; i < N; ++i)
        {
            double val = 0.0;
            if (x[i] != 0.0)
                val = p[i] / x[i];
            else
                val = p[i] == 0.0 ? 0.0 : std::numeric_limits<double>::infinity();
            maxS = std::max(maxS, std::abs(val));
        }
        if (std::abs(alpha) * maxS < eps)
            flag = PCGStatus::STAGNATED;
    }

    void checkExit()
    {
        if (flag != PCGStatus::RUNNING)
            return;
        if (rnorm2 < tolb)
        {
            flag = PCGStatus::CONVERGED;
            return;
        }
        if (iter >= maxIt)
        {
            flag = PCGStatus::ITERATION;
            return;
        }
    }


    const M& A;
    std::function<Vector(const Vector&)> P = [](const Vector& v) { return v; };
    bool usePreconditioner = false;
    const Vector &b;
    Vector &xmin;
    int N;
    Vector r, y, z, p, q, st, x;
    double Arnorm = 0.0, ynorm = 0.0;

    // Here we have some supplementary state
    PCGStatus flag = PCGStatus::RUNNING;
    int iter = 0;
    double bnorm2, rnorm2, tolb, rho, alpha, rnormMin = norm2Lim;
#if 0
    double beta1 = 0.0,
         beta   =   0.0,   tau = 0.0,    taul = 0.0,  phi = 0.0,
        betan  = 0.0,  gmin = 0.0,      cs =-1.0,   sn = 0.0,
        cr1   =  -1.0,   sr1 = 0.0,     cr2 =-1.0,  sr2 = 0.0,
        dltan =   0.0, eplnn = 0.0,    gamma = 0.0, gammal= 0.0,
        gammal2=   0.0,   eta = 0.0,    etal = 0.0, etal2= 0.0,
        vepln =   0.0, veplnl= 0.0,  veplnl2= 0.0,  ul3 = 0.0,
            ul2 =   0.0,    ul =   0.0,     u = 0.0,
    gammal_QLP = 0.0, vepln_QLP = 0.0, gamma_QLP = 0.0,
    u_QLP = 0.0, ul_QLP = 0.0,
    gminl = 0.0, rnorm  = 0.0, xnorm = 0.0, xl2norm = 0.0, Axnorm = 0.0,
    Anorm = 0.0, Acond = 1.0,
    relres = 0.0,

    alpha = 0.0, pnorm = 0.0,
    dlta, epln, gbar, dlta_QLP,
    gammal3, gamma_tmp, taul2,
    Acondl, rnorml, relresl, relAresl;
#endif
};
}
#endif

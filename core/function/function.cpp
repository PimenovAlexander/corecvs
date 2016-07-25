#include "function.h"

namespace corecvs {

Matrix FunctionArgs::getLSQHessian(const double* in, double delta)
{
    Vector v(inputs, in), F(outputs);
    (*this)(&v[0], &F[0]);
    auto J = getJacobian(v, delta);
    auto Hs= getHessians(v, delta);
    Matrix H(inputs, inputs);
    for (int i = 0; i < inputs; ++i)
    {
        for (int j = 0; j < inputs; ++j)
        {
            double hij = 0.0;
            for (int k = 0; k < outputs; ++k)
            {
                hij += Hs[k].a(i, j) * F[k];
                hij += J.a(k, i) * J.a(k, j);
            }
            H.a(i, j) = hij;
        }
    }
    return H;
}

Matrix FunctionArgs::getJacobian(const double *in, double delta)
{
    Matrix result(outputs, inputs);
    vector<double> xc(inputs);
    vector<double> y_minus(outputs);
    vector<double> y_plus (outputs);

    for (int i = 0; i < inputs; i++)
    {
        xc[i] = in[i];
    }

    for (int i = 0; i < inputs; i++)
    {
        double delta_loc = std::max(1.0, std::abs(in[i])) * delta;
        double xm = xc[i] = in[i] - delta_loc;
        operator()(&xc[0], &y_minus[0]);
        double xp = xc[i] = in[i] + delta_loc;
        operator()(&xc[0], &y_plus[0]);
        xc[i] = in[i];

        // Note: this stuff is not equal to 2 * delta
        double dx = xp - xm;
        for (int j = 0; j < outputs; j++)
        {
            result.element(j,i) = (y_plus[j] - y_minus[j]) / dx;
        }
    }
    return result;
}

std::vector<Matrix> FunctionArgs::getHessians(const Vector &in, double delta)
{
    // actual values of x_i+-\Delta x_i and x_i+-2\Delta x_i
    Vector dxp(inputs), dxm(inputs), dxp2(inputs), dxm2(inputs);
    for (int i = 0; i < inputs; ++i)
    {
        auto delta_xi = std::max(1.0, std::abs(in[i])) * delta;
        dxp[i] =  in[i] + delta_xi;
        dxm[i] =  in[i] - delta_xi;
        dxp2[i] = in[i] + 2.0 * delta_xi;
        dxm2[i] = in[i] - 2.0 * delta_xi;
    }

    Matrix f0(inputs, outputs),
            fp(inputs, outputs),
            fm(inputs, outputs),
            fp2(inputs, outputs),
            fm2(inputs, outputs);
    for (int i = 0; i < inputs; ++i)
    {
        Vector input = in;
        (*this)(&input[0], &f0.a(i, 0));
#define FILL(s) \
        input[i] = dx ## s[i]; \
        (*this)(&input[0], &f ## s.a(i, 0));
        FILL(p);
        FILL(m);
        FILL(p2);
        FILL(m2);
#undef FILL
    }
    std::vector<Matrix> dpp, dpm, dmp, dmm;
    for (int i = 0; i < inputs; ++i)
    {
        Matrix mpp(inputs, outputs), mpm(inputs, outputs), mmp(inputs, outputs), mmm(inputs, outputs);
        for (int j = 0; j < inputs; ++j)
        {
            if (i > j)
            {
#define FILL_PM(MACRO) \
                MACRO(p, p); \
                MACRO(p, m); \
                MACRO(m, p); \
                MACRO(m, m);
#define FILL_SWAP(sA, sB) \
                Matrix& m ## sA ## sB ## _ = d ## sA ## sB [j]; \
                for (int k = 0; k < outputs; ++k) \
                    m ## sB ## sA.a(j, k) = m ## sA ## sB ## _.a(i, k);
                FILL_PM(FILL_SWAP);
#undef FILL_SWAP
                continue;
            }
            if (i == j) continue;

            Vector input(in);
#define FILL_IJ(sI, sJ) \
            input[i] = dx ## sI [i]; \
            input[j] = dx ## sJ [j]; \
            (*this)(&input[0], &m ## sI ## sJ.a(j, 0));
            FILL_PM(FILL_IJ);
#undef FILL_IJ
        }
#define EMPLACE_PM(sP, sM) \
        d##sP##sM.emplace_back(m##sP##sM);
        FILL_PM(EMPLACE_PM);
#undef FILL_PM
#undef EMPLACE_PM
    }
    // Now we've computed all desired data and it's the time to compute hessians
    std::vector<Matrix> hessians;
    for (int ii = 0; ii < outputs; ++ii)
    {
        Matrix hessian(inputs, inputs);
        for (int i = 0; i < inputs; ++i)
        {
            for (int j = 0; j < inputs; ++j)
            {
                if (i == j)
                {
                    // too hard to use exact delta values
                    double di = (dxp[i] - dxm[i]) / 2.0;
                    hessian.a(i, j) = (-fp2.a(i, ii) - fm2.a(i, ii) + 16.0 * (fp.a(i, ii) + fm.a(i, ii)) - 30.0 * f0.a(i, ii)) / 12.0 / di / di;
                    continue;
                }
                hessian.a(i, j) = (dpp[i].a(j, ii) + dmm[i].a(j, ii) - dpm[i].a(j, ii) - dmp[i].a(j, ii)) / (dxp[i] - dxm[i]) / (dxp[j] - dxm[j]);
            }
        }
        hessians.push_back(hessian);
    }
    return hessians;
}

SparseMatrix SparseFunctionArgs::getNativeJacobian(const double *in, double delta)
{
    std::vector<std::vector<double>> values(inputs);
    for (int i = 0; i < inputs; ++i)
        values[i].reserve(dependencyList[i].size());

    int N = (int)groupInputs.size();
    std::vector<double> xp(inputs), xm(inputs), deltaS(inputs);
    double curr_eval = 0.0, curr_subscale = 0.0;
    auto start_evals = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i)
    {
        auto& group = groupInputs[i];
        auto& idxs = groupOutputs[i];
        auto& remap = remapIdx[i];

        int M = (int)group.size();

        for (int j = 0; j < inputs; ++j)
            xp[j] = xm[j] = in[j];
        for (int j = 0; j < M; ++j)
        {
            double x = in[group[j]];
            double delta_loc = std::max(1.0, std::abs(x)) * delta;
            double xxm = x - delta_loc;
            double xxp = x + delta_loc;
            xp[group[j]] = xxp;
            xm[group[j]] = xxm;
            deltaS[j] = xxp - xxm;
        }

        std::vector<double> yp(idxs.size()), ym(idxs.size());
        auto begin_eval = std::chrono::high_resolution_clock::now();
        operator()(&xp[0], &yp[0], idxs);
        operator()(&xm[0], &ym[0], idxs);
        auto end_eval = std::chrono::high_resolution_clock::now();
        curr_eval += (end_eval - begin_eval).count() / 1e9;

        auto begin_subscale = std::chrono::high_resolution_clock::now();
        int K = (int)idxs.size();
#ifdef WITH_BLAS
        cblas_daxpy(K, -1.0, &ym[0], 1, &yp[0], 1);
#endif
        for (int j = 0; j < K; ++j)
        {
            int id = remap[idxs[j]];
#ifdef WITH_BLAS
            double v = yp[j] / deltaS[id];
#else
            double v = (yp[j] - ym[j]) / deltaS[id];
#endif
            values[group[id]].push_back(v);
        }
        auto end_subscale = std::chrono::high_resolution_clock::now();
        curr_subscale += (end_subscale - begin_subscale).count() / 1e9;
    }
    auto end_evals = std::chrono::high_resolution_clock::now();
    double eval_f = curr_eval;
    double eval_other = (end_evals - start_evals).count() / 1e9 - eval_f - curr_subscale;


    auto begin_prepare = std::chrono::high_resolution_clock::now();
    std::vector<double> sparseValues;
    std::vector<int> sparseColumns, sparseRowPointers(inputs + 1);
    for (int i = 0; i < inputs; ++i)
    {
        int N = (int)dependencyList[i].size();
        CORE_ASSERT_TRUE_S(N == (int)values[i].size());
        for (int j = 0; j < N; ++j)
        {
            int jj = dependencyList[i][j];
            sparseColumns.push_back(jj);
            sparseValues.push_back(values[i][j]);
        }
        sparseRowPointers[i + 1] = (int)sparseValues.size();
    }
    auto end_prepare = std::chrono::high_resolution_clock::now();
    auto begin_construct = std::chrono::high_resolution_clock::now();
    auto sm = SparseMatrix(inputs, outputs, std::move(sparseValues), std::move(sparseColumns), std::move(sparseRowPointers));
    auto end_construct = std::chrono::high_resolution_clock::now();
    auto begin_t = std::chrono::high_resolution_clock::now();
    auto sm_t = sm.t();
    auto end_t = std::chrono::high_resolution_clock::now();

    double prepare_t = (end_prepare - begin_prepare).count() / 1e9;
    double construct_t = (end_construct - begin_construct).count() / 1e9;
    double t_t = (end_t - begin_t).count() / 1e9;
    feval += eval_f;
    transp += t_t;
    construct += construct_t;
    prepare += prepare_t;
    other += eval_other;
    subscale += curr_subscale;
    return sm_t;
}

void SparseFunctionArgs::minify()
{
    std::vector<int> usedO(outputs);
    std::vector<int> usedI(inputs);
    CORE_ASSERT_TRUE_S((int)dependencyList.size() == inputs);

    for (int i = 0; i < inputs; ++i)
    {
        if (usedI[i])
            continue;
        std::vector<int> currentGroup = {i}, currentRemap(outputs, -1), currentOutputs = {};
        usedO.clear();
        usedO.resize(outputs);
        for (auto& id: dependencyList[i])
        {
            CORE_ASSERT_TRUE_S(id < outputs);
            usedO[id] = 1;
        }
        for (int j = i + 1; j < inputs; ++j)
        {
            if (usedI[j])
                continue;
            bool isOk = true;
            for (auto& id: dependencyList[j])
                if (usedO[id])
                {
                    isOk = false;
                    break;
                }
            if (!isOk)
                continue;
            currentGroup.push_back(j);
            usedI[j] = 1;
            for (auto& id: dependencyList[j])
                usedO[id] = 1;
        }
        CORE_ASSERT_TRUE_S((int)currentRemap.size() == outputs);
        for (auto& id: currentGroup)
            for (auto& ido: dependencyList[id])
            {
                currentOutputs.push_back(ido);
                currentRemap[ido] = &id - &*currentGroup.begin();
            }
        groupInputs.push_back(currentGroup);
        groupOutputs.push_back(currentOutputs);
        remapIdx.push_back(currentRemap);
    }
    std::cout << "REMAPANAL: " << inputs << "->" << groupInputs.size() << std::endl;
}

} // namespace corecvs

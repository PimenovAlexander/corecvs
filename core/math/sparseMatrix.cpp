#include "core/math/sparseMatrix.h"
#include "wrappers/cblasLapack/cblasLapackeWrapper.h"

#include <chrono>
#include <thread>

#ifdef WITH_TBB
#include <tbb/tbb.h>
#endif

//using namespace corecvs;
using corecvs::SparseMatrix;
using corecvs::Vector;

#define STRONG_TRIAG

void SparseMatrix::checkCorrectness() const
{
    CORE_ASSERT_TRUE_S(h + 1 == (int)rowPointers.size());
    CORE_ASSERT_TRUE_S(rowPointers.size());
    CORE_ASSERT_TRUE_S(*rowPointers.rbegin() == (int)columns.size());
    CORE_ASSERT_TRUE_S(*rowPointers.rbegin() == (int)values.size());
    for (size_t i = 0; i + 1 < rowPointers.size(); ++i)
        CORE_ASSERT_TRUE_S(rowPointers[i] <= rowPointers[i + 1]);
    for (int i = 0; i < h; ++i)
        for (int j = rowPointers[i]; j + 1 < rowPointers[i + 1]; ++j)
        {
            CORE_ASSERT_TRUE_S(columns[j] < columns[j + 1]);
            CORE_ASSERT_TRUE_S(columns[j] >= 0 && columns[j] < w);
        }
}

struct Registrant
{
    Registrant()
    {
        std::cout << "Registering in spmvwisdom" << std::endl;
        corecvs::SPMVWisdom::Register(corecvs::AcceleratorTypes::CPU, corecvs::SparseImplementations::HOMEBREW, [](int, const SparseMatrix &m, const Vector &v, bool trans) { return m.spmv_homebrew(v, trans); });
#ifdef WITH_CUSPARSE
        corecvs::SPMVWisdom::Register(corecvs::AcceleratorTypes::CUDA, corecvs::SparseImplementations::CUSPARSE, [](int id, const SparseMatrix &m, const Vector &v, bool trans) { return m.spmv_cusparse(v, trans, id); });
#endif
#ifdef WITH_MKL
        corecvs::SPMVWisdom::Register(corecvs::AcceleratorTypes::CPU, corecvs::SparseImplementations::MKL,      [](int, const SparseMatrix &m, const Vector &v, bool trans) { return m.spmv_mkl(v, trans); });
#endif
        std::cout << "Registering in trsvwisdom" << std::endl;
        corecvs::TRSVWisdom::Register(corecvs::AcceleratorTypes::CPU, corecvs::SparseImplementations::HOMEBREW, [](int, const SparseMatrix &m, const Vector &v, const char* trans, bool up, int N) { return m.trsv_homebrew(v, trans, up, N); });
#if 1
#ifdef WITH_CUSPARSE
        corecvs::TRSVWisdom::Register(corecvs::AcceleratorTypes::CUDA, corecvs::SparseImplementations::CUSPARSE, [](int dev, const SparseMatrix &m, const Vector &v, const char* trans, bool up, int N) { return m.trsv_cusparse(v, trans, up, N, dev); });
#endif
#ifdef WITH_MKL
        corecvs::TRSVWisdom::Register(corecvs::AcceleratorTypes::CPU, corecvs::SparseImplementations::MKL, [](int, const SparseMatrix &m, const Vector &v, const char* trans, bool up, int N) { return m.trsv_mkl(v, trans, up, N); });
#endif
#endif
        std::cout << "Registering in spmmwisdom" << std::endl;
        corecvs::SPMMWisdom::Register(corecvs::AcceleratorTypes::CPU, corecvs::SparseImplementations::HOMEBREW, [](int, const corecvs::SPMMC::inner_type& t) { return std::get<0>(t).spmm_homebrew(std::get<1>(t), std::get<2>(t), std::get<3>(t)); });
#if 1
#ifdef WITH_CUSPARSE
        corecvs::SPMMWisdom::Register(corecvs::AcceleratorTypes::CUDA, corecvs::SparseImplementations::CUSPARSE, [](int dev, const corecvs::SPMMC::inner_type& t) { return std::get<0>(t).spmm_cusparse(std::get<1>(t), std::get<2>(t), std::get<3>(t), dev); });
#endif
#endif
#ifdef WITH_MKL
        corecvs::SPMMWisdom::Register(corecvs::AcceleratorTypes::CPU, corecvs::SparseImplementations::MKL, [](int, const corecvs::SPMMC::inner_type& t) { return std::get<0>(t).spmm_mkl(std::get<1>(t), std::get<2>(t), std::get<3>(t)); });
#endif
    }
};

std::unique_ptr<Registrant> reg(new Registrant);

std::pair<bool, SparseMatrix> SparseMatrix::incompleteCholseky(bool allow_parallel)
{
    // Here we will consturct incomplete upper-triangular cholesky-factor for matrix
    // If we do not succeed, we'll return <false, ()> 'cause it is 2016 now,
    // we are retrograds (and do not use boost)
    // and std::optional from C++17 is not yet ready (and usage of std::experimental
    // is prohibited)
    // We will start from the same NNZ-structure

    SparseMatrix A(*this);
    int N = h;
    //int n = N;
    for (int k = 0; k < N; ++k)
    {
        int i_k_k = getIndex(k, k);

        double pivot = i_k_k >= 0 ? A.values[i_k_k] : 0.0;
        if (pivot <= 0.0)
        {
            std::cout << "Non-positive pivot R" << std::endl;
            return std::make_pair(false, SparseMatrix());
        }
        CORE_ASSERT_TRUE_S(A.a(k, k) == pivot);
        CORE_ASSERT_TRUE_S(columns[i_k_k] == k);
        pivot = std::sqrt(pivot);

        A.values[i_k_k] = pivot;

        for (int i_k_i = i_k_k + 1; i_k_i < A.rowPointers[k + 1]; ++i_k_i)
            A.values[i_k_i] /= pivot;

        if (!allow_parallel)
        {
            int i_k_j = i_k_k + 1;
            for (; i_k_j < A.rowPointers[k + 1]; ++i_k_j)
            {
                int j = A.columns[i_k_j];
                int i_j_i = getUBIndex(j, j);

                double a_k_j = A.values[i_k_j];
                if (a_k_j == 0.0)
                    continue;

                int i_k_i = i_k_j;
                for (; i_j_i < A.rowPointers[j + 1]; ++i_j_i)
                {
                    int i = A.columns[i_j_i];
                    while (i_k_i < A.rowPointers[k + 1] && A.columns[i_k_i] < i) ++i_k_i;
                    if (i_k_i == A.rowPointers[k + 1])
                        break;
                    if (A.columns[i_k_i] > i)
                        continue;
                    double a_k_i = A.values[i_k_i];
                    A.values[i_j_i] -= a_k_i * a_k_j;
                }
            }
        }
        else
        {
            int i_k_j_ = i_k_k + 1;
            corecvs::parallelable_for(i_k_j_, A.rowPointers[k + 1], 512, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i_k_j = r.begin(); i_k_j != r.end(); ++i_k_j)
                {
                    int j = A.columns[i_k_j];
                    int i_j_i = getUBIndex(j, j);

                    double a_k_j = A.values[i_k_j];
                    if (a_k_j == 0.0)
                        continue;

                    int i_k_i = i_k_j;
                    for (; i_j_i < A.rowPointers[j + 1]; ++i_j_i)
                    {
                        int i = A.columns[i_j_i];
                        while (i_k_i < A.rowPointers[k + 1] && A.columns[i_k_i] < i) ++i_k_i;
                        if (i_k_i == A.rowPointers[k + 1])
                            break;
                        if (A.columns[i_k_i] > i)
                            continue;
                        double a_k_i = A.values[i_k_i];
                        A.values[i_j_i] -= a_k_i * a_k_j;
                    }
                }
            });
        }
    }

    int nnz = 0;
    for (int i = 0; i < N; ++i)
    {
        for (int j = getUBIndex(i, i); j < A.rowPointers[i + 1]; ++j)
            if (A.columns[j] >= i)
            {
                A.columns[nnz] = A.columns[j];
                A.values[nnz] = A.values[j];
                ++nnz;
            }
        A.rowPointers[i + 1] = nnz;
    }
    A.columns.resize(nnz);
    A.values.resize(nnz);

    return std::make_pair(true, A);
}

Vector SparseMatrix::dtrsv(const Vector &rhs, bool upper, bool notrans) const
{

    return upper ? notrans ? dtrsv_un(rhs) : dtrsv_ut(rhs) : notrans ? dtrsv_ln(rhs) : dtrsv_lt(rhs);
}

Vector SparseMatrix::dtrsv_ut(const Vector &rhs) const
{
    /*
     *                         /# # # # #\
     * /                     \ |  # # # #|   /                     \
     * | x_1 x_2 x_3 x_4 x_5 | |    # # #| = | b_1 b_2 b_3 b_4 b_5 |
     * \                     / |      # #|   \                     /
     *                         \        #/
     */
    CORE_ASSERT_TRUE_S(h == w);
    CORE_ASSERT_TRUE_S(h == rhs.size());
    Vector res(rhs.size());

    std::vector<int> first(h), id(h), next(h);
    for (int i = 0; i < h; ++i)
    {
        first[i] = i;
#ifndef STRONG_TRIAG
        id[i] = getUBIndex(i, i);
#else
        id[i] = rowPointers[i];
#endif
        next[i] = -1;
        CORE_ASSERT_TRUE_S(columns[id[i]] == i && id[i] < rowPointers[i + 1]);
    }

    int vstd = 0;
    for (int j = 0; j < h; ++j)
    {
        int idx = first[j];
        double pivot = 0.0, sum = rhs[j];
        while (idx != -1)
        {
            vstd++;
            int i = idx;
            int i_i_j = id[idx];
            CORE_ASSERT_TRUE_S(columns[i_i_j] == j);
            if (i == j)
                pivot = values[i_i_j];
            else
                sum -= values[i_i_j] * res[i];
            int nid = next[idx];
            if (i_i_j + 1 < rowPointers[i + 1])
            {
                int c = columns[i_i_j + 1];
                next[idx] = first[c];
                first[c] = idx;
                id[idx] = i_i_j + 1;
            }
            idx = nid;
        }
        CORE_ASSERT_TRUE_S(std::abs(pivot) > 0.0);
        res[j] = sum / pivot;
    }
    return res;
}

Vector SparseMatrix::dtrsv_lt(const Vector &rhs) const
{
    /*
     *                         /#        \
     * /                     \ |# #      |   /                     \
     * | x_1 x_2 x_3 x_4 x_5 | |# # #    | = | b_1 b_2 b_3 b_4 b_5 |
     * \                     / |# # # #  |   \                     /
     *                         \# # # # #/
     */
    CORE_ASSERT_TRUE_S(h == w);
    CORE_ASSERT_TRUE_S(h == rhs.size());
    Vector res(rhs.size());

    std::vector<int> first(h), id(h), next(h);
    for (int i = 0; i < h; ++i)
    {
        first[i] = i;
#ifndef STRONG_TRIAG
        id[i] = getUBIndex(i, i);
#else
        id[i] = rowPointers[i + 1] - 1;
#endif
        next[i] = -1;
        CORE_ASSERT_TRUE_S(columns[id[i]] == i && id[i] < rowPointers[i + 1]);
    }

    for (int j = h - 1; j >= 0; --j)
    {
        int idx = first[j];
        double pivot = 0.0, sum = rhs[j];
        while (idx != -1)
        {
            int i = idx;
            int i_i_j = id[idx];
            CORE_ASSERT_TRUE_S(columns[i_i_j] == j);
            if (i == j)
                pivot = values[i_i_j];
            else
                sum -= values[i_i_j] * res[i];
            int nid = next[idx];
            if (i_i_j > rowPointers[i])
            {
                int c = columns[i_i_j - 1];
                next[idx] = first[c];
                first[c] = idx;
                id[idx] = i_i_j - 1;
            }
            idx = nid;
        }
        CORE_ASSERT_TRUE_S(std::abs(pivot) > 0.0);
        res[j] = sum / pivot;
    }
    return res;
}

corecvs::Vector SparseMatrix::dtrsv_ln(const Vector &rhs) const
{
    /*
     * /#        \ / x_1 \   / b_1 \
     * |# #      | | x_2 |   | b_2 |
     * |# # #    | | x_3 | = | b_3 |
     * |# # # #  | | x_4 |   | b_4 |
     * \# # # # #/ \ x_5 /   \ b_5 /
     */

    CORE_ASSERT_TRUE_S(h == w);
    CORE_ASSERT_TRUE_S(h == rhs.size());
    Vector res(rhs.size());
    for (int i = 0; i < h; ++i)
    {
        double sum = rhs[i];
        int i_i_j = rowPointers[i];
        for (; i_i_j < rowPointers[i + 1] && columns[i_i_j] < i; ++i_i_j)
            sum -= values[i_i_j] * res[columns[i_i_j]];
        CORE_ASSERT_TRUE_S(columns[i_i_j] == i && i_i_j < rowPointers[i + 1]);
        res[i] = sum / values[i_i_j];
    }
    return res;
}

Vector SparseMatrix::dtrsv_un(const Vector &rhs) const
{
//    std::cout << "[UN]" << std::flush;

    /*
     * /# # # # #\ / x_1 \   / b_1 \
     * |  # # # #| | x_2 |   | b_2 |
     * |    # # #| | x_3 | = | b_3 |
     * |      # #| | x_4 |   | b_4 |
     * \        #/ \ x_5 /   \ b_5 /
     */

    Vector res(rhs.size());
    CORE_ASSERT_TRUE_S(h == w);
    CORE_ASSERT_TRUE_S(h == rhs.size());
    for (int i = h - 1; i >= 0; --i)
    {
        double sum = rhs[i];
#ifndef STRONG_TRIAG
        int i_i_i = getUBIndex(i, i);
#else
        int i_i_i = rowPointers[i];
#endif
        CORE_ASSERT_TRUE_S(columns[i_i_i] == i && rowPointers[i + 1]  > i_i_i);
        for (int i_i_j = i_i_i + 1; i_i_j < rowPointers[i + 1]; ++i_i_j)
            sum -= values[i_i_j] * res[columns[i_i_j]];
        res[i] = sum / values[i_i_i];
    }
    return res;
}


void SparseMatrix::spyPlot() const
{

    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            double v = a(i, j);
            std::cout << (v == 0.0 ? " " : "*");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int SparseMatrix::nnz() const
{

    return (int)values.size();
}

double SparseMatrix::fillin() const
{

    return double(nnz()) / w / h;
}

SparseMatrix::SparseMatrix(const SparseMatrix &src, int x1, int y1, int x2, int y2)
{
#if 0
    src.swapCoords(x1, y1, x2, y2);

    h = y2 - y1;
    w = x2 - x1;
    CORE_ASSERT_TRUE_S(h >= 0 && w >= 0);
    rowPointers.resize(h + 1);
    for (int i = y1; i < y2; ++i)
    {
        const int* cbegin = std::lower_bound(&src.columns[src.rowPointers[i]], &src.columns[src.rowPointers[i + 1]], x1),
               *   cend = std::lower_bound(&src.columns[src.rowPointers[i]], &src.columns[src.rowPointers[i + 1]], x2);
        for (auto ptr = cbegin; ptr < cend; ++ptr)
        {
            columns.push_back(*ptr - x1);
            values.push_back(src.values[ptr - &src.columns[0]]);
        }
        rowPointers[i - y1 + 1] = (int)values.size();
    }
#else

    src.swapCoords(x1, y1, x2, y2);

    h = y2 - y1;
    w = x2 - x1;
    CORE_ASSERT_TRUE_S(h >= 0 && w >= 0);
    rowPointers.resize(h + 1);
    for (int i = y1; i < y2; ++i)
    {
        for (int j = src.rowPointers[i]; j < src.rowPointers[i + 1]; ++j)
        {
            if (src.columns[j] >= x2)
                break;
            if (src.columns[j] <  x1)
                continue;
            columns.push_back(src.columns[j] - x1);
            values.push_back(src.values[j]);
        }
        rowPointers[i - y1 + 1] = (int)values.size();
    }

#endif
}

void SparseMatrix::denseSubMatrix(int x1, int y1, int x2, int y2, double* output, int stride) const
{
    swapCoords(x1, y1, x2, y2);

    int hh = y2 - y1,
        ww = x2 - x1;
    if (stride < 0)
        stride = ww;
    CORE_ASSERT_TRUE_S(hh >= 0 && ww >= 0);

    for (int i = y1; i < y2; ++i)
    {
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            if (columns[j] >= x2)
                break;
            if (columns[j] <  x1)
                continue;
            output[(i - y1) * stride + (columns[j] - x1)] = values[j];
        }
    }

}

corecvs::Matrix SparseMatrix::denseSubMatrix(int x1, int y1, int x2, int y2) const
{

    swapCoords(x1, y1, x2, y2);

    int hh = y2 - y1,
        ww = x2 - x1;
    CORE_ASSERT_TRUE_S(hh >= 0 && ww >= 0);
    corecvs::Matrix res(hh, ww);

    denseSubMatrix(x1, y1, x2, y2, &res.a(0, 0), res.stride);

    return res;
}

SparseMatrix::SparseMatrix(const corecvs::Matrix &dense, double threshold) : h(dense.h), w(dense.w)
{
    rowPointers.resize(h + 1);
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            double val = dense.a(i, j);
            if (std::abs(val) > threshold)
            {
                values.push_back(val);
                columns.push_back(j);
            }
        }
        rowPointers[i + 1] = (int)values.size();
    }
}

SparseMatrix::SparseMatrix(int h, int w, const std::vector<double> &values, const std::vector<int> &columns, const std::vector<int> &rowPointers)
    : h(h), w(w), values(values), columns(columns), rowPointers(rowPointers)
{
    CORE_ASSERT_TRUE_S(h + 1 == (int)rowPointers.size());
    CORE_ASSERT_TRUE_S(values.size() == columns.size());
    CORE_ASSERT_TRUE_P(*rowPointers.rbegin() == (int)values.size(), ("there is a problem with cusparse"));
}

SparseMatrix::SparseMatrix(int h, int w, const std::map<std::pair<int, int>, double> &data) : h(h), w(w)
{
    // Here we will use lexicographical order of std::map<std::pair<X,X>,Y>
    rowPointers.resize(h + 1);

    int rowPrev = 0;
    for (auto& val: data)
    {
        auto idx  = val.first;
        auto value= val.second;

        int r = idx.first, c = idx.second;
        CORE_ASSERT_TRUE_S(r < h && c < w);

        if (r != rowPrev)
        {
            for (int i = rowPrev + 1; i <= r; ++i)
                rowPointers[i] = (int)values.size();
            rowPrev = r;
        }

        values.push_back(value);
        columns.push_back(c);
    }
    for (int i = rowPrev + 1; i <= h; ++i)
        rowPointers[i] = (int)values.size();

}

double SparseMatrix::a(int y, int x) const
{
#if 0
    int i = std::lower_bound(&columns[rowPointers[y]], &columns[rowPointers[y + 1]], x) - &columns[0];
    if (i < rowPointers[y + 1] && columns[i] == x)
        return values[i];
    return 0.0;
#else
    for (int i = rowPointers[y]; i < rowPointers[y + 1]; ++i)
        if (columns[i] == x)
            return values[i];

    return 0.0;
#endif
}

corecvs::Matrix SparseMatrix::denseRows(int x1, int y1, int x2, int y2, std::vector<int> &colIdx)
{
    swapCoords(x1, y1, x2, y2);
    int h = y2 - y1;

    colIdx.clear();
    std::vector<int> rPtr(h);
    for (int i = 0; i < h; ++i)
    {
        rPtr[i] = rowPointers[i + y1];
        while (columns[rPtr[i]] < x2 && rPtr[i] < rowPointers[i + y1])
            rPtr[i]++;
    }
    std::vector<int> rStartPtr = rPtr;

    while (1)
    {
        int nextCol = -1;
        for (int i = 0; i < h; ++i)
        {
            if (rPtr[i] == rowPointers[i + y1 + 1] || columns[rPtr[i]] >= x2)
                continue;
            if (nextCol < 0)
                nextCol = columns[rPtr[i]];
            nextCol = std::min(nextCol, columns[rPtr[i]]);
        }
        if (nextCol == -1)
            break;
        CORE_ASSERT_TRUE_S(nextCol >= x1 && nextCol < x2);
        colIdx.push_back(nextCol);
        for (int i = 0; i < h; ++i)
            if (rPtr[i] != rowPointers[i + y1 + 1] && columns[rPtr[i]] < x2 && columns[rPtr[i]] == nextCol)
                ++rPtr[i];
    }
    int w = (int)colIdx.size();
    rPtr = rStartPtr;

    corecvs::Matrix M(h, w);
    for (int nnzCols = 0; nnzCols < w; ++nnzCols)
    {
        int nextCol = colIdx[nnzCols];
        CORE_ASSERT_TRUE_S(nextCol >= x1 && nextCol < x2);
        for (int i = 0; i < h; ++i)
            if (rPtr[i] != rowPointers[i + y1 + 1] && columns[rPtr[i]] < x2 && columns[rPtr[i]] == nextCol)
            {
                M.a(i, nnzCols) = values[rPtr[i]];
                ++rPtr[i];
            }
    }

    return M;
}

corecvs::Matrix SparseMatrix::denseCols(int x1, int y1, int x2, int y2, std::vector<int> &rowIdx)
{
    swapCoords(x1, y1, x2, y2);

    int w = x2 - x1;
    rowIdx.clear();
    std::vector<int*> b, e;
    b.reserve(y2-y1);
    e.reserve(y2-y1);
    for (int i = y1; i < y2; ++i)
    {
#ifdef _MSC_VER
        int* cbegin = std::lower_bound(&columns[rowPointers[i]], columns.data() + rowPointers[i + 1], x1), 
           *   cend = std::lower_bound(&columns[rowPointers[i]], columns.data() + rowPointers[i + 1], x2);  
#else
        int* cbegin = std::lower_bound(&columns[rowPointers[i]], &columns[rowPointers[i + 1]], x1),
           *   cend = std::lower_bound(&columns[rowPointers[i]], &columns[rowPointers[i + 1]], x2);
#endif
        if (cbegin < cend)
        {
            rowIdx.push_back(i);
            b.push_back(cbegin);
            e.push_back(cend);
        }
    }

    int h = (int)rowIdx.size();
    Matrix M(h, w);
    for (int y = 0; y < h; ++y)
    {
        for (int* ptr = b[y]; ptr < e[y]; ++ptr)
            M.a(y, *ptr - x1) = values[ptr - &columns[0]];
    }

    return M;
}

void SparseMatrix::swapCoords(int &x1, int &y1, int &x2, int &y2)  const
{
    x1 = std::max(0, std::min(w, x1));
    x2 = std::max(0, std::min(w, x2));
    y1 = std::max(0, std::min(h, y1));
    y2 = std::max(0, std::min(h, y2));
    if (x2 < x1)
        std::swap(x1, x2);
    if (y2 < y1)
        std::swap(y1, y2);
}

double& SparseMatrix::a(int y, int x)
{
#if 0
    int i = std::lower_bound(&columns[rowPointers[y]], &columns[rowPointers[y + 1]], x) - &columns[0];
    if (i < rowPointers[y + 1] && columns[i] == x)
        return values[i];
    CORE_ASSERT_TRUE_S(rowPointers[y + 1] == i || columns[i] > x);
    columns.insert(columns.begin() + i, x);
    values.insert(values.begin() + i, 0.0);
    for (int j = y + 1; j <= h; ++j)
        ++rowPointers[j];
#else

    int i = 0;
    for (i = rowPointers[y]; i < rowPointers[y + 1] && columns[i] < x; ++i);
    if (i < rowPointers[y + 1] && columns[i] == x)
    {

       return values[i];
    }
    CORE_ASSERT_TRUE_S(rowPointers[y + 1] == i || columns[i] > x);
    columns.insert(columns.begin() + i, x);
    values.insert(values.begin() + i, 0.0);
    for (int j = y + 1; j <= h; ++j)
        ++rowPointers[j];
#endif

    return values[i];
}

SparseMatrix::operator corecvs::Matrix() const
{
    corecvs::Matrix m(h, w);

    for (int i = 0; i < h; ++i)
    {
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            m.a(i, columns[j]) = values[j];
        }
    }

    return m;
}

SparseMatrix corecvs::operator -(const SparseMatrix &a)
{
    SparseMatrix b = a;
    for (auto &v: b.values)
        v = -v;

    return b;
}

SparseMatrix corecvs::operator *(const double &lhs, const SparseMatrix &rhs)
{

    SparseMatrix res = rhs;
    for (auto &v: res.values)
        v *= lhs;

    return res;
}

SparseMatrix corecvs::operator *(const SparseMatrix &lhs, const double &rhs)
{
    return rhs * lhs;
}

SparseMatrix corecvs::operator /(const SparseMatrix &lhs, const double &rhs)
{
    SparseMatrix res = lhs;
    for (auto &v: res.values)
        v /= rhs;

    return res;
}

SparseMatrix corecvs::operator +(const SparseMatrix &lhs, const SparseMatrix &rhs)
{
    CORE_ASSERT_TRUE_S(lhs.h == rhs.h && lhs.w == rhs.w);
    std::vector<double> values;
    std::vector<int> columns;
    std::vector<int> rowPointers(lhs.h + 1);

    for (int i = 0; i < lhs.h; ++i)
    {
        int lhs_l = lhs.rowPointers[i], lhs_r = lhs.rowPointers[i + 1];
        int rhs_l = rhs.rowPointers[i], rhs_r = rhs.rowPointers[i + 1];

        int lhs_p = lhs_l, rhs_p = rhs_l;
        while (lhs_p < lhs_r && rhs_p < rhs_r)
        {
            int cl = lhs.columns[lhs_p], cr = rhs.columns[rhs_p];
            if (cl < cr)
            {
                values.push_back(lhs.values[lhs_p]);
                columns.push_back(cl);
                ++lhs_p;
                continue;
            }
            if (cr < cl)
            {
                values.push_back(rhs.values[rhs_p]);
                columns.push_back(cr);
                rhs_p++;
                continue;
            }
            values.push_back(lhs.values[lhs_p] + rhs.values[rhs_p]);
            columns.push_back(cl);
            ++rhs_p;
            ++lhs_p;
        }
        while (lhs_p < lhs_r)
        {
            values.push_back(lhs.values[lhs_p]);
            columns.push_back(lhs.columns[lhs_p]);
            ++lhs_p;
        }
        while (rhs_p < rhs_r)
        {
            values.push_back(rhs.values[rhs_p]);
            columns.push_back(rhs.columns[rhs_p]);
            ++rhs_p;
        }
        rowPointers[i + 1] = (int)values.size();
    }

    return SparseMatrix(lhs.h, lhs.w, values, columns, rowPointers);
}

SparseMatrix corecvs::operator -(const SparseMatrix &lhs, const SparseMatrix &rhs)
{
    CORE_ASSERT_TRUE_S(lhs.h == rhs.h && lhs.w == rhs.w);
    std::vector<double> values;
    std::vector<int> columns;
    std::vector<int> rowPointers(lhs.h + 1);

    for (int i = 0; i < lhs.h; ++i)
    {
        int lhs_l = lhs.rowPointers[i], lhs_r = lhs.rowPointers[i + 1];
        int rhs_l = rhs.rowPointers[i], rhs_r = rhs.rowPointers[i + 1];

        int lhs_p = lhs_l, rhs_p = rhs_l;
        while (lhs_p < lhs_r && rhs_p < rhs_r)
        {
            int cl = lhs.columns[lhs_p], cr = rhs.columns[rhs_p];
            if (cl < cr)
            {
                values.push_back(lhs.values[lhs_p]);
                columns.push_back(cl);
                ++lhs_p;
                continue;
            }
            if (cr < cl)
            {
                values.push_back(-rhs.values[rhs_p]);
                columns.push_back(cr);
                rhs_p++;
                continue;
            }
            values.push_back(lhs.values[lhs_p] - rhs.values[rhs_p]);
            columns.push_back(cl);
            ++rhs_p;
            ++lhs_p;
        }
        while (lhs_p < lhs_r)
        {
            values.push_back(lhs.values[lhs_p]);
            columns.push_back(lhs.columns[lhs_p]);
            ++lhs_p;
        }
        while (rhs_p < rhs_r)
        {
            values.push_back(-rhs.values[rhs_p]);
            columns.push_back(rhs.columns[rhs_p]);
            ++rhs_p;
        }
        rowPointers[i + 1] = (int)values.size();
    }

    return SparseMatrix(lhs.h, lhs.w, values, columns, rowPointers);
}

SparseMatrix SparseMatrix::spmm_homebrew(const SparseMatrix &rhs, bool transA, bool transB) const
{
    if (transA)
    {
        SparseMatrix smT = this->t();
        return smT.spmm_homebrew(rhs, false, transB);
    }
    if (transB)
    {
        SparseMatrix smT = rhs.t();
        return spmm_homebrew(smT, false, false);
    }
    CORE_ASSERT_TRUE_S(w == rhs.h);
    int N = std::max(std::max(this->h, this->w), std::max(rhs.h, rhs.w));
    std::vector<double> values, acc(N);
    std::vector<int> columns, rowPointers(this->h + 1), index(N, -10);
    int h = this->h;
    int w = rhs.w;

    for (int i = 0; i < h; ++i)
    {
        int ii = -1, l = 0;
        for (int jj = this->rowPointers[i]; jj < this->rowPointers[i + 1]; ++jj)
        {
            int j = this->columns[jj];
            for (int k = rhs.rowPointers[j]; k < rhs.rowPointers[j + 1]; ++k)
            {
                if (index[rhs.columns[k]] == -10)
                {
                    index[rhs.columns[k]] = ii;
                    ii = rhs.columns[k];
                    ++l;
                }
            }
        }
        rowPointers[i + 1] = rowPointers[i] + l;
        columns.resize(rowPointers[i + 1]);
        values.resize(rowPointers[i + 1]);
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            columns[j] = ii;
            ii = index[ii];
            index[columns[j]] = -10;
        }
        index.clear();
        index.resize(N, -10);
    }
    rowPointers[0] = 0;

    for (int i = 0; i < h; ++i)
    {
#ifdef _MSC_VER
        std::sort(&columns[rowPointers[i]], columns.data() + rowPointers[i + 1]);
#else
        std::sort(&columns[rowPointers[i]], &columns[rowPointers[i + 1]]);
#endif
        for (int jj = this->rowPointers[i]; jj < this->rowPointers[i + 1]; ++jj)
        {
            int j = this->columns[jj];
            double val = this->values[jj];
            for (int k = rhs.rowPointers[j]; k < rhs.rowPointers[j + 1]; ++k)
            {
                acc[rhs.columns[k]] += val * rhs.values[k];
            }
        }
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            values[j] = acc[columns[j]];
            acc[columns[j]] = 0.0;
        }
    }
    return SparseMatrix(h, w, values, columns, rowPointers);
}

#ifdef WITH_MKL
SparseMatrix SparseMatrix::spmm_mkl(const SparseMatrix &rhs, bool transA, bool transB) const
{
    if (transB)
    {
        std::cout << "MKL: EXPLICIT TRANSPOSE" << std::endl;
        SparseMatrix smT = rhs.t();
        return spmm_mkl(smT, transA, false);
    }
    if (&rhs == this)
    {
        std::cout << "MKL: ATA? DETECTED" << std::endl;
        auto lhs_mkl = (sparse_matrix_t)*this;
        sparse_matrix_t res;
        mkl_sparse_spmm(transA ? SPARSE_OPERATION_TRANSPOSE : SPARSE_OPERATION_NON_TRANSPOSE, lhs_mkl, lhs_mkl, &res);
        mkl_sparse_destroy(lhs_mkl);
        SparseMatrix ress(res);
        mkl_sparse_destroy(res);
        return ress;
    }
    auto lhs_mkl = (sparse_matrix_t)*this;
    auto rhs_mkl = (sparse_matrix_t)rhs;
    sparse_matrix_t res;
    mkl_sparse_spmm(transA ? SPARSE_OPERATION_TRANSPOSE : SPARSE_OPERATION_NON_TRANSPOSE, lhs_mkl, rhs_mkl, &res);
    mkl_sparse_destroy(lhs_mkl);
    mkl_sparse_destroy(rhs_mkl);
    SparseMatrix ress(res);
    mkl_sparse_destroy(res);
    return ress;
}
#endif

#ifdef WITH_CUSPARSE
SparseMatrix SparseMatrix::spmm_cusparse(const SparseMatrix &rhs, bool transA, bool transB, int dev) const
{
    cudaSetDevice(dev);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    promoteToGpu(dev);
    rhs.promoteToGpu(dev);

    cusparseHandle_t   handle = 0;
    cusparseMatDescr_t descrA = 0, descrB = 0, descrC = 0;

    auto status = cusparseCreate(&handle);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    if (status != CUSPARSE_STATUS_SUCCESS)
    {
        std::cout << "CUSPARSE INIT FAILED" << std::endl;
    }

    cusparseCreateMatDescr(&descrA);
    cusparseSetMatType(descrA, CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descrA, CUSPARSE_INDEX_BASE_ZERO);
    cusparseCreateMatDescr(&descrB);
    cusparseSetMatType(descrB, CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descrB, CUSPARSE_INDEX_BASE_ZERO);
    cusparseCreateMatDescr(&descrC);
    cusparseSetMatType(descrC, CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descrC, CUSPARSE_INDEX_BASE_ZERO);

    int baseC, nnzC;
    int *nnzP = &nnzC;
    cusparseSetPointerMode(handle, CUSPARSE_POINTER_MODE_HOST);
    int *rowPointersC;
    int m = transA ? w : h, k = transA ? h : w, n = transB ? rhs.h : rhs.w;
    cudaMalloc((void**)&rowPointersC, sizeof(int) * (1 + m));
    auto ctransA = transA ? CUSPARSE_OPERATION_TRANSPOSE : CUSPARSE_OPERATION_NON_TRANSPOSE,
         ctransB = transB ? CUSPARSE_OPERATION_TRANSPOSE : CUSPARSE_OPERATION_NON_TRANSPOSE;

    cusparseXcsrgemmNnz(handle, ctransA, ctransB, m, n, k,
            descrA,     nnz(),     gpuPromotion->basicPromotions[dev].dev_rowPointers.get(),     gpuPromotion->basicPromotions[dev].dev_columns.get(),
            descrB, rhs.nnz(), rhs.gpuPromotion->basicPromotions[dev].dev_rowPointers.get(), rhs.gpuPromotion->basicPromotions[dev].dev_columns.get(),
            descrC, rowPointersC, nnzP);

    if (nnzP)
        nnzC = *nnzP;
    else
    {
        cudaMemcpy(&nnzC, rowPointersC + m, sizeof(int), cudaMemcpyDeviceToHost);
        cudaMemcpy(&baseC, rowPointersC, sizeof(int), cudaMemcpyDeviceToHost);
        nnzC -= baseC;
    }
    double *valuesC;
    int *columnsC;
    cudaMalloc((void**)&valuesC, sizeof(double) * nnzC);
    cudaMalloc((void**)&columnsC, sizeof(int) * nnzC);

    cusparseDcsrgemm(handle, ctransA, ctransB, m, n, k,
            descrA, nnz(),
                gpuPromotion->basicPromotions[dev].dev_values.get(),     gpuPromotion->basicPromotions[dev].dev_rowPointers.get(),     gpuPromotion->basicPromotions[dev].dev_columns.get(),
            descrB, rhs.nnz(),
            rhs.gpuPromotion->basicPromotions[dev].dev_values.get(), rhs.gpuPromotion->basicPromotions[dev].dev_rowPointers.get(), rhs.gpuPromotion->basicPromotions[dev].dev_columns.get(),
            descrC,
            valuesC, rowPointersC, columnsC);

    std::vector<int> crp(m + 1), cc(nnzC);
    std::vector<double> cv(nnzC);

    cudaMemcpy(&crp[0], rowPointersC, sizeof(int) * (m + 1), cudaMemcpyDeviceToHost);
    cudaMemcpy(&cc[0], columnsC, sizeof(int) * nnzC, cudaMemcpyDeviceToHost);
    cudaMemcpy(&cv[0], valuesC, sizeof(double) * nnzC, cudaMemcpyDeviceToHost);

    cudaFree(rowPointersC);
    cudaFree(columnsC);
    cudaFree(valuesC);

    return SparseMatrix(m, n, cv, cc, crp);
}
#endif

Vector SparseMatrix::spmv_homebrew(const Vector &rhs, bool trans) const
{
    if (trans)
    {
        CORE_ASSERT_TRUE_S(rhs.size() == h);
        Vector ans(w);

        for (int i = 0; i < h; ++i)
        {
            for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
                ans[columns[j]] += values[j] * rhs[i];
        }

        return ans;
    }
    CORE_ASSERT_TRUE_S(w == rhs.size());
    Vector ans(h);
    int N = h;
    std::size_t bs = 512;

    corecvs::parallelable_for(0, N, bs, [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i != r.end(); ++i)
        {
            double res = 0.0;
            for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
                res += values[j] * rhs[columns[j]];
            ans[i] = res;
        }
    });
    return ans;
}

Vector SparseMatrix::trsv(const Vector &rhs, const char* trans, bool up, int N) const
{
    return TRSVWisdom::WiseRun(*this, rhs, trans, up, N);
}

#ifdef WITH_CUSPARSE
Vector SparseMatrix::spmv_cusparse(const Vector &rhs, bool trans, int dev) const
{
    cudaSetDevice(dev);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    promoteToGpu(dev);
    int ansSize = trans ? w : h;
    Vector ans(ansSize);
    CORE_ASSERT_TRUE_S(rhs.size() == w + h - ansSize);

    double *dev_rhs, *dev_res;
    cudaMalloc(&(void*&)dev_rhs, rhs.size() * sizeof(double));
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaMalloc(&(void*&)dev_res, ansSize * sizeof(double));
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);

    cusparseHandle_t   handle = 0;
    cusparseMatDescr_t descr  = 0;

    auto status = cusparseCreate(&handle);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    if (status != CUSPARSE_STATUS_SUCCESS)
    {
        std::cout << "CUSPARSE INIT FAILED" << std::endl;
    }

    status = cusparseCreateMatDescr(&descr);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    if (status != CUSPARSE_STATUS_SUCCESS)
    {
        std::cout << "CUSPARSE MD FAILED" << std::endl;
    }

    cusparseSetMatType(descr, CUSPARSE_MATRIX_TYPE_GENERAL);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cusparseSetMatIndexBase(descr, CUSPARSE_INDEX_BASE_ZERO);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);

    cudaMemcpy(dev_rhs, &rhs[0], rhs.size() * sizeof(double), cudaMemcpyHostToDevice);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaMemset(dev_res, 0, ansSize * sizeof(double));
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);

    double alpha = 1.0, beta = 0.0;
    status = cusparseDcsrmv(handle, trans ? CUSPARSE_OPERATION_TRANSPOSE : CUSPARSE_OPERATION_NON_TRANSPOSE, h, w, nnz()
#       if (__CUDA_API_VERSION == 5050)
        ,  alpha, descr, gpuPromotion->basicPromotions[dev].dev_values.get(), gpuPromotion->basicPromotions[dev].dev_rowPointers.get(), gpuPromotion->basicPromotions[dev].dev_columns.get(), dev_rhs,  beta, dev_res);
#else
        , &alpha, descr, gpuPromotion->basicPromotions[dev].dev_values.get(), gpuPromotion->basicPromotions[dev].dev_rowPointers.get(), gpuPromotion->basicPromotions[dev].dev_columns.get(), dev_rhs, &beta, dev_res);
#endif
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);

    if (status != CUSPARSE_STATUS_SUCCESS)
    {
        std::cout << "CUSPARSE SPMV FAILED" << std::endl;
        fprintf(stderr, "SPMV launch failed: %s\n", cudaGetErrorString(cudaGetLastError()));
    }
    cudaMemcpy(&ans[0], dev_res, sizeof(double) * ansSize, cudaMemcpyDeviceToHost);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);

    cudaFree(dev_rhs);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaFree(dev_res);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    return ans;
}
#endif

#ifdef WITH_MKL
Vector SparseMatrix::spmv_mkl(const Vector &rhs, bool trans) const
{
    // Cause mkl does not support non-square matrix in spmv through basic interface
    if (w != h)
        return spmv_homebrew(rhs, trans);
    int ansSize = trans ? w : h;
    Vector ans(ansSize);
    CORE_ASSERT_TRUE_S(rhs.size() == w + h - ansSize);

    mkl_cspblas_dcsrgemv(trans ? "T" : "N", &w, &values[0], &rowPointers[0], &columns[0], &rhs[0], &ans[0]);
    return ans;
}
#endif

Vector SparseMatrix::trsv_homebrew(const Vector &rhs, const char* trans, bool up, int N) const
{
    CORE_ASSERT_TRUE_S(w == h && w == rhs.size());

    Vector ans = rhs;
    for (int i = 0; i < N; ++i)
    {
        auto res = dtrsv(ans, up, trans[i] == 'N');
        ans = res;
    }
    return ans;
}

#ifdef WITH_MKL
Vector SparseMatrix::trsv_mkl(const Vector &rhs, const char* trans, bool up, int N) const
{
    CORE_ASSERT_TRUE_S(w == h && w == rhs.size());

    Vector ans[2] = {rhs, Vector(rhs.size())};
    for (int i = 0; i < N; ++i)
    {
        int id = i % 2;
        mkl_cspblas_dcsrtrsv(up ? "U" : "L", trans[i] == 'N' ? "N" : "T", "N", &h, &values[0], &rowPointers[0], &columns[0], &(ans[id][0]), &(ans[1 - id][0]));
    }
    return ans[N % 2];
}
#endif

#ifdef WITH_CUSPARSE
Vector SparseMatrix::trsv_cusparse(const Vector &rhs, const char* trans, bool up, int N, int gpuId) const
{
    CORE_ASSERT_TRUE_S(w == h && w == rhs.size());
    cudaSetDevice(gpuId);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    promoteToGpu(gpuId);
    gpuPromotion->triangularAnalysis(up, gpuId);

    CORE_ASSERT_TRUE_S(gpuPromotion->basicPromotions.count(gpuId));
    auto &basic = gpuPromotion->basicPromotions[gpuId];
    CORE_ASSERT_TRUE_S(gpuPromotion->triangularPromotions.count(gpuId));
    auto &trian = gpuPromotion->triangularPromotions[gpuId];

    double *ans_gpu[2];
    cudaMalloc(&(void*&)ans_gpu[0], w * sizeof(double));
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaMalloc(&(void*&)ans_gpu[1], w * sizeof(double));
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaMemcpy(ans_gpu[0], &rhs[0], w * sizeof(double), cudaMemcpyHostToDevice);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaMemset(ans_gpu[1], 0, w * sizeof(double));
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);

    double alpha = 1.0;
    for (int i = 0; i < N; ++i)
    {
        int id = i % 2;
        if (trans[i] == 'N')
            cusparseDcsrsv2_solve(trian.handle, CUSPARSE_OPERATION_NON_TRANSPOSE, w, nnz(), &alpha, trian.descr, basic.dev_values.get(), basic.dev_rowPointers.get(), basic.dev_columns.get(), trian.infoNoTrans, ans_gpu[id], ans_gpu[1 - id], trian.policy, trian.bufferNoTrans.get());
        else
            cusparseDcsrsv2_solve(trian.handle, CUSPARSE_OPERATION_TRANSPOSE, w, nnz(), &alpha, trian.descr, basic.dev_values.get(), basic.dev_rowPointers.get(), basic.dev_columns.get(), trian.infoTrans, ans_gpu[id], ans_gpu[1 - id], trian.policy, trian.bufferTrans.get());
    }
    Vector ans(w);
    cudaMemcpy(&ans[0], ans_gpu[N % 2], w * sizeof(double), cudaMemcpyDeviceToHost);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaFree(ans_gpu[0]);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    cudaFree(ans_gpu[1]);
    CUDAPromoter::TriangularPromotion::checkError(__FILE__,  __LINE__);
    return ans;
}
#endif

Vector corecvs::operator *(const SparseMatrix &lhs, const Vector &rhs)
{
    if (!reg)
        CORE_ASSERT_FAIL("Wrong Registrant usage");

    return SPMVWisdom::WiseRun(lhs, rhs, false);
}

Vector corecvs::operator *(const Vector &lhs, const SparseMatrix &rhs)
{
#if 0
    CORE_ASSERT_TRUE_S(lhs.size() == rhs.h);
    Vector ans(rhs.w);
    for (int i = 0; i < rhs.w; ++i)
        ans[i] = 0.0;

    for (int i = 0; i < rhs.h; ++i)
    {
        for (int j = rhs.rowPointers[i]; j < rhs.rowPointers[i + 1]; ++j)
            ans[rhs.columns[j]] += rhs.values[j] * lhs[i];
    }

    return ans;
#else
    return SPMVWisdom::WiseRun(rhs, lhs, true);
#endif
}

SparseMatrix corecvs::operator *(const SparseMatrix &lhs, const SparseMatrix &rhst)
{
#if 1
    SPMMC::inner_type ref(lhs, rhst, false, false);
    return SPMMWisdom::WiseRun(ref);
#else
    CORE_ASSERT_TRUE_S(lhs.w == rhst.h);
#ifndef WITH_MKL
    auto& rhs = rhst;
    int N = std::max(std::max(lhs.h, lhs.w), std::max(rhs.h, rhs.w));
    std::vector<double> values, acc(N);
    std::vector<int> columns, rowPointers(lhs.h + 1), index(N, -10);
    int h = lhs.h;
    int w = rhs.w;

    for (int i = 0; i < h; ++i)
    {
        int ii = -1, l = 0;
        for (int jj = lhs.rowPointers[i]; jj < lhs.rowPointers[i + 1]; ++jj)
        {
            int j = lhs.columns[jj];
            for (int k = rhs.rowPointers[j]; k < rhs.rowPointers[j + 1]; ++k)
            {
                if (index[rhs.columns[k]] == -10)
                {
                    index[rhs.columns[k]] = ii;
                    ii = rhs.columns[k];
                    ++l;
                }
            }
        }
        rowPointers[i + 1] = rowPointers[i] + l;
        columns.resize(rowPointers[i + 1]);
        values.resize(rowPointers[i + 1]);
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            columns[j] = ii;
            ii = index[ii];
            index[columns[j]] = -10;
        }
        index.clear();
        index.resize(N, -10);
    }
    rowPointers[0] = 0;

    for (int i = 0; i < h; ++i)
    {
#ifdef _MSC_VER
        std::sort(&columns[rowPointers[i]], columns.data() + rowPointers[i + 1]]);
#else
        std::sort(&columns[rowPointers[i]], &columns[rowPointers[i + 1]]);
#endif
        for (int jj = lhs.rowPointers[i]; jj < lhs.rowPointers[i + 1]; ++jj)
        {
            int j = lhs.columns[jj];
            double val = lhs.values[jj];
            for (int k = rhs.rowPointers[j]; k < rhs.rowPointers[j + 1]; ++k)
            {
                acc[rhs.columns[k]] += val * rhs.values[k];
            }
        }
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            values[j] = acc[columns[j]];
            acc[columns[j]] = 0.0;
        }
    }
    return SparseMatrix(h, w, values, columns, rowPointers);
#else


    auto lhs_mkl = (sparse_matrix_t)lhs;
    auto rhs_mkl = (sparse_matrix_t)rhst;
    sparse_matrix_t res;
    mkl_sparse_spmm(SPARSE_OPERATION_NON_TRANSPOSE, lhs_mkl, rhs_mkl, &res);
    mkl_sparse_destroy(lhs_mkl);
    mkl_sparse_destroy(rhs_mkl);
    SparseMatrix ress(res);
    mkl_sparse_destroy(res);



    return ress;
#endif
#endif
}

SparseMatrix SparseMatrix::ata() const
{
#if 1
    SPMMC::inner_type ref(*this, *this, true, false);
	CORE_ASSERT_TRUE_S(this - &std::get<0>(ref) == 0);
	CORE_ASSERT_TRUE_S(this - &std::get<1>(ref) == 0);
    return SPMMWisdom::WiseRun(ref);
#else
#ifndef WITH_MKL
    return t() * (*this);
#else

    auto lhs_mkl = (sparse_matrix_t)(*this);
    sparse_matrix_t res;
    mkl_sparse_spmm(SPARSE_OPERATION_TRANSPOSE, lhs_mkl, lhs_mkl, &res);
    mkl_sparse_destroy(lhs_mkl);
    SparseMatrix ress(res);
    mkl_sparse_destroy(res);

    return ress;
#endif
#endif
}

SparseMatrix SparseMatrix::t() const
{
    std::vector<int> tRowPointers(w + 1);
    std::vector<double> tValues(values.size());
    std::vector<int> tColumns(values.size());

    for (auto& id: tRowPointers)
        id = 0;
    tRowPointers[0] = 0;
    for (int i = 0; i < h; ++i)
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
            tRowPointers[columns[j]+1]++;
    for (int i = 0; i < w; ++i)
        tRowPointers[i + 1] += tRowPointers[i];
    for (int i = 0; i < h; ++i)
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            int idx = columns[j];
            tColumns[tRowPointers[idx]] = i;
            tValues[tRowPointers[idx]] = values[j];
            tRowPointers[idx]++;
        }
    for (int i = w - 1; i >= 1; --i)
        tRowPointers[i] = tRowPointers[i - 1];
    tRowPointers[0] = 0;
    *tRowPointers.rbegin() = (int)tValues.size();

    return SparseMatrix(w, h, tValues, tColumns, tRowPointers);
}

#ifdef WITH_MKL

SparseMatrix::operator sparse_matrix_t() const
{
    sparse_matrix_t mkl_sparse;
    mkl_sparse_d_create_csr(&mkl_sparse, SPARSE_INDEX_BASE_ZERO, h, w, const_cast<int*>(&rowPointers[0]), const_cast<int*>(&rowPointers[1]), const_cast<int*>(&columns[0]), const_cast<double*>(&values[0]));
    return mkl_sparse;
}

SparseMatrix::SparseMatrix(const sparse_matrix_t &mklSparse)
{
    double *vals;
    int *rowB, *rowE, *cols;
    int mh, mw;
    sparse_index_base_t indexType;
    sparse_matrix_t copy;

    mkl_sparse_convert_csr(mklSparse, SPARSE_OPERATION_NON_TRANSPOSE, &copy);
    mkl_sparse_d_export_csr(copy, &indexType, &mh, &mw, &rowB, &rowE, &cols, &vals);

    CORE_ASSERT_TRUE_S(indexType == SPARSE_INDEX_BASE_ZERO);

    rowPointers.resize(mh + 1);
    h = mh;
    w = mw;

    std::vector<int> order;
    for (int i = 0; i < mh; ++i)
    {
        int N;
        order.resize(N = rowE[i] - rowB[i]);
        for (int j = 0; j < N; ++j)
            order[j] = rowB[i] + j;
        std::sort(order.begin(), order.end(), [&](const int &a, const int &b) { return cols[a] < cols[b]; });
        for (int j = 0; j < N; ++j)
        {
            values.push_back(vals[order[j]]);
            columns.push_back(cols[order[j]]);
        }
        rowPointers[i + 1] = (int)values.size();
    }


    mkl_sparse_destroy(copy);
}

#endif // WITH_MKL

int SparseMatrix::getUBIndex(int i, int j) const
{
#ifdef _MSC_VER
    return std::lower_bound(&columns[rowPointers[i]], columns.data() + rowPointers[i + 1], j) - &columns[0];
#else
    return std::lower_bound(&columns[rowPointers[i]], &columns[rowPointers[i + 1]], j) - &columns[0];
#endif
}

int SparseMatrix::getIndex(int i, int j) const
{
    auto id = getUBIndex(i, j);
    return id < rowPointers[i + 1] && columns[id] == j ? id : -1;
}


SparseMatrix SparseMatrix::upper() const
{
    SparseMatrix copy(*this);
    int valueIdx = 0;
    for (int i = 0; i < h; ++i)
    {
        bool diagExists = false;
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            if (columns[j] >= i)
            {
                if (columns[j] == i)
                    diagExists = true;
                if (columns[j] > i && !diagExists)
                {
                    copy.values.resize(copy.values.size() + 1);
                    copy.columns.resize(copy.columns.size() + 1);
                    copy.values[valueIdx] = 0.0;
                    copy.columns[valueIdx] = i;
                    diagExists = true;
                    valueIdx++;
                }
                copy.values[valueIdx] = values[j];
                copy.columns[valueIdx] = columns[j];
                valueIdx++;
            }
        }
        copy.rowPointers[i + 1] = valueIdx;
    }
    copy.values.resize(valueIdx);
    copy.columns.resize(valueIdx);
    return copy;
}

bool SparseMatrix::linSolve(const Vector &rhs, Vector &res, bool symmetric, bool posDef) const
{
    if (symmetric)
#ifdef WITH_MKL
        return LinSolve(symmetric ? upper() : (*this), rhs, res, true, posDef);
#else
        return LinSolve(*this, rhs, res, true, posDef);
#endif
    return LinSolve(*this, rhs, res, false, posDef);
}

bool SparseMatrix::LinSolve(const SparseMatrix &m, const Vector &rhs, Vector &res, bool symmetric, bool posDef)
{
    CORE_ASSERT_TRUE_S(rhs.size() == m.h);
#ifndef WITH_MKL
    CORE_UNUSED(posDef);
    return Matrix::LinSolve((Matrix)m, rhs, res, symmetric);
#else
    res = Vector(m.w);
    auto& sol = res;
    for (int i = 0; i < m.w; ++i)
        sol[i] = 0.0;
    _MKL_DSS_HANDLE_t dss_handle;
    int options = MKL_DSS_DEFAULTS + MKL_DSS_ZERO_BASED_INDEXING;
    int nnz     = (int)m.values.size();
    int retval  = dss_create(dss_handle, options);
    int order   = MKL_DSS_AUTO_ORDER;
    int *pivot  = 0;
    int nrhs    = 1;
    int solve_options = MKL_DSS_DEFAULTS;
    int delOptions = 0;

    CORE_ASSERT_TRUE_S(retval == MKL_DSS_SUCCESS);
    options = symmetric ? MKL_DSS_SYMMETRIC : MKL_DSS_NON_SYMMETRIC;

    retval = dss_define_structure(dss_handle, options, &m.rowPointers[0], m.h, m.w, &m.columns[0], nnz);
    CORE_ASSERT_TRUE_S(retval == MKL_DSS_SUCCESS);

    retval = dss_reorder(dss_handle, order, pivot);
    CORE_ASSERT_TRUE_S(retval == MKL_DSS_SUCCESS);

    int decompose_options = symmetric ?  posDef ? MKL_DSS_POSITIVE_DEFINITE : MKL_DSS_INDEFINITE : 0;
    retval = dss_factor_real(dss_handle, decompose_options, &m.values[0]);
    if (retval != MKL_DSS_SUCCESS)
    {
       dss_delete(dss_handle, delOptions);
       return false;
    }


    retval = dss_solve_real(dss_handle, solve_options, &rhs[0], nrhs, &sol[0]);
    if (retval != MKL_DSS_SUCCESS)
    {
       dss_delete(dss_handle, delOptions);
       return false;
    }

    dss_delete(dss_handle, delOptions);
    return true;
#endif // WITH_MKL
}

std::ostream& corecvs::operator<< (std::ostream &out, const SparseMatrix &sm)
{
    out << (Matrix)sm;
    return out;
}

void SparseMatrix::print(std::ostream& out) const
{
    out << *this;
}

#if !defined(_WIN32) || defined(_WIN64)

bool SparseMatrix::LinSolveSchurComplementInv(const SparseMatrix &M, const corecvs::Vector &Bv
    , const std::vector<int> &diagBlocks
    , corecvs::Vector &res
    , bool symmetric
    , bool posDef)
{
    /*
     * So we partition M and B into
     * +---+---+   /   \   /   \
     * | A | B |   | x |   | a |
     * +---+---+ * +---+ = +---+
     * | C | D |   | y |   | b |
     * +---+---+   \   /   \   /
     * Where D is block-diagonal well-conditioned matrix
     *
     * Then we invert D explicitly and solve
     * x = (A-BD^{-1}C)^{-1}(a-BD^{-1}b)
     * y = D^{-1}(b-Cx)
     *
     * Note that M is symmetric => D is symmetric, (A-BD^{-1}C) is symmetric
     *           M is posdef    => D is posdef,    (A-BD^{-1}C) is symmetric (TODO: isposdef)
     */

#ifndef WITH_BLAS
#  if defined(_MSC_VER)
#    pragma message ("SparseMatrix::LinSolveSchurComplementInv(): NIY")
#  else
#	 warning SparseMatrix::LinSolveSchurComplementInv(): NIY
#  endif
    CORE_UNUSED(M);
    CORE_UNUSED(Bv);
    CORE_UNUSED(diagBlocks);
    CORE_UNUSED(res);
    CORE_UNUSED(symmetric);
    CORE_UNUSED(posDef);
    return false;
#else

    auto Ah = diagBlocks[0],
         Aw = diagBlocks[0];
    auto Bw = M.w - Aw,
         Bh = Ah;
    auto Cw = Aw,
         Ch = M.h - Ah;
    auto Dw = Bw;

    /*
     * The same as above, but with fancy LAPACK
     */
    int N = (int)diagBlocks.size() - 1;
    std::vector<int> pivots(Dw);

    auto startBlocks = std::chrono::high_resolution_clock::now();
    std::vector<int> rptr(N);
    for (int i = 0; i < N; ++i)
    {
        auto s = diagBlocks[i + 1] - diagBlocks[i];
        rptr[i] = s * s + (i ? rptr[i - 1] : 0);
    }
    auto  stopBlocks = std::chrono::high_resolution_clock::now();

    auto  startDiagFactor = std::chrono::high_resolution_clock::now();

    int NNZDinv = *rptr.rbegin();
    std::vector<int> rowPointers(Bw + 1), columns(NNZDinv);
    std::vector<double> values(NNZDinv);

    corecvs::parallelable_for(0, (int)N, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i < r.end(); ++i)
                {
                    auto offset = i ? rptr[i - 1] : 0;
                    double* ptr = &values[offset];
                    auto blockE = diagBlocks[i + 1], blockS = diagBlocks[i];
                    auto h = blockE - blockS;
                    auto p = blockS - diagBlocks[0];

                    M.denseSubMatrix(blockS, blockS, blockE, blockE, ptr);

                    if (!symmetric)
                    {
                        LAPACKE_dgetrf(LAPACK_ROW_MAJOR, h, h, ptr, h, &pivots[p]);
                        LAPACKE_dgetri(LAPACK_ROW_MAJOR, h,    ptr, h, &pivots[p]);
                    }
                    else
                    {
                        if (posDef)
                        {
                            LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'U', h, ptr, h);
                            LAPACKE_dpotri(LAPACK_ROW_MAJOR, 'U', h, ptr, h);
                        }
                        else
                        {
                            LAPACKE_dsytrf(LAPACK_ROW_MAJOR, 'U', h, ptr, h, &pivots[p]);
                            LAPACKE_dsytri(LAPACK_ROW_MAJOR, 'U', h, ptr, h, &pivots[p]);
                        }
                    }
                    for (int ii = 0; ii < h; ++ii)
                    {
                        for (int jj = 0; jj < h; ++jj)
                            columns[offset++] = p + jj;
                        rowPointers[p + ii + 1] = offset;
                    }

                }
            });
    auto  stopDiagFactor = std::chrono::high_resolution_clock::now();

    // Computing BD^{-1}
    auto startSparsePrep = std::chrono::high_resolution_clock::now();
    SparseMatrix A(M ,   0,  0,      Aw,      Ah),
                          B(M,   Aw,  0, Aw + Bw,      Bh),
                          C(M,    0, Ah,      Cw, Ah + Ch);
    CORE_ASSERT_TRUE_S(A.h == Ah && A.w == Aw);
    CORE_ASSERT_TRUE_S(B.h == Bh && B.w == Bw);
    CORE_ASSERT_TRUE_S(C.h == Ch && C.w == Cw);
    auto stopSparsePrep = std::chrono::high_resolution_clock::now();

    // Computing BD^{-1}
    //recvs::Matrix BDinv(Bh, Dw);
    auto startDinvBt = std::chrono::high_resolution_clock::now();
    SparseMatrix Dinv(Bw, Bw, values, columns, rowPointers);
    auto BDinv = B * Dinv;
    auto stopDinvBt = std::chrono::high_resolution_clock::now();
    // Computing lhs/rhs
    auto startLhsRhs = std::chrono::high_resolution_clock::now();

    corecvs::Vector a(Ah, &Bv[0]), b(Ch, &Bv[Ah]), rhs;
    corecvs::Matrix lhs;

#ifdef WITH_TBB
    tbb::task_group g;
    g.run([&]() { rhs = a - BDinv * b; });
    g.run([&]() { lhs = (corecvs::Matrix)A - (corecvs::Matrix)(BDinv * C); });
    g.wait();
#else
    rhs = a - BDinv * b;
    lhs = (corecvs::Matrix)A - (corecvs::Matrix)(BDinv * C);

#endif
    std::cout << "Fillin: A=" << A.fillin() << ", BDinv*C: " << (BDinv*C).fillin() << ", lhs: " << (SparseMatrix(lhs)).fillin() << std::endl;
    auto stopLhsRhs = std::chrono::high_resolution_clock::now();

    auto startX = std::chrono::high_resolution_clock::now();
    // Solving for x
    corecvs::Vector x(Aw), y(Bw);
    bool foo = lhs.linSolve(rhs, x, symmetric, false);
    auto stopX = std::chrono::high_resolution_clock::now();

    if (!foo) return false;

    auto startY = std::chrono::high_resolution_clock::now();
    // Solving for y
    auto yrhs = b - C * x;
    y = Dinv * yrhs;
    auto stopY = std::chrono::high_resolution_clock::now();

    double blocks = (stopBlocks - startBlocks).count(),
           diagFac= (stopDiagFactor - startDiagFactor).count(),
           sparseP= (stopSparsePrep - startSparsePrep).count(),
           dinvBt = (stopDinvBt - startDinvBt).count(),
           lhsRhs = (stopLhsRhs - startLhsRhs).count(),
           xxx    = (stopX - startX).count(),
           yyy    = (stopY - startY).count();
    static int statcnt = 0;
    statcnt++;
    if (statcnt % 1000 == 0)
    {
    double total = blocks + diagFac + sparseP + dinvBt + lhsRhs + xxx + yyy;
    total /= 100.0;
#define SPRINT(T, V) \
        << "\t" << T << ": " << V / total << " % / " << V << "ns" << std::endl
    std::cout << "LSC Stats: " << total * 100.0 << "ns"  << std::endl
        SPRINT("Blocks", blocks)
        SPRINT("DiagFactor", diagFac)
        SPRINT("SparsePrep", sparseP)
        SPRINT("DinvBt", dinvBt)
        SPRINT("LhsRhs", lhsRhs)
        SPRINT("X", xxx)
        SPRINT("Y", yyy);
    }

    for (int i = 0; i < Aw; ++i)
        res[i] = x[i];
    for (int j = 0; j < Bw; ++j)
        res[j + Aw] = y[j];
    return true;
#endif
}

bool SparseMatrix::LinSolveSchurComplementNew(const SparseMatrix &M, const corecvs::Vector &Bv
    , const std::vector<int> &diagBlocks
    , corecvs::Vector &res
    , bool symmetric
    , bool posDef)
{
    /*
     * So we partition M and B into
     * +---+---+   /   \   /   \
     * | A | B |   | x |   | a |
     * +---+---+ * +---+ = +---+
     * | C | D |   | y |   | b |
     * +---+---+   \   /   \   /
     * Where D is block-diagonal well-conditioned matrix
     *
     * Then we invert D explicitly and solve
     * x = (A-BD^{-1}C)^{-1}(a-BD^{-1}b)
     * y = D^{-1}(b-Cx)
     *
     * Note that M is symmetric => D is symmetric, (A-BD^{-1}C) is symmetric
     *           M is posdef    => D is posdef,    (A-BD^{-1}C) is symmetric (TODO: isposdef)
     */

#ifndef WITH_BLAS
#  if defined(_MSC_VER)
#    pragma message ("SparseMatrix::LinSolveSchurComplementNew(): NIY")
#  else
#	 warning SparseMatrix::LinSolveSchurComplementNew(): NIY
#  endif
    CORE_UNUSED(M);
    CORE_UNUSED(Bv);
    CORE_UNUSED(diagBlocks);
    CORE_UNUSED(res);
    CORE_UNUSED(symmetric);
    CORE_UNUSED(posDef);
    return false;
#else

    auto Ah = diagBlocks[0],
         Aw = diagBlocks[0];
    auto Bw = M.w - Aw,
         Bh = Ah;
    auto Cw = Aw,
         Ch = M.h - Ah;
    auto Dw = Bw;

    /*
     * The same as above, but with fancy LAPACK
     */
    auto N = diagBlocks.size() - 1;
    std::vector<int> pivots(Dw), pivotIdx(N);

    std::vector<corecvs::Matrix> qrd(N);
    auto startBlocks = std::chrono::high_resolution_clock::now();
    corecvs::parallelable_for(0, (int)N, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    qrd[i] = M.denseSubMatrix(diagBlocks[i], diagBlocks[i], diagBlocks[i + 1], diagBlocks[i + 1]);
                    pivotIdx[i] = diagBlocks[i] - diagBlocks[0];
                }
            });
    auto  stopBlocks = std::chrono::high_resolution_clock::now();

    auto  startDiagFactor = std::chrono::high_resolution_clock::now();
    // Factorizing (without "\"")
    corecvs::parallelable_for(0, (int)N, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i < r.end(); ++i)
                {
                    auto& MM = qrd[i];
                    if (!symmetric)
                    {
                        LAPACKE_dgetrf(LAPACK_ROW_MAJOR, MM.h, MM.w, MM.data, MM.stride, &pivots[pivotIdx[i]]);
                    }
                    else
                    {
                        if (posDef)
                            LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'U', MM.h, MM.data, MM.stride);
                        else
                            LAPACKE_dsytrf(LAPACK_ROW_MAJOR, 'U', MM.h, MM.data, MM.stride, &pivots[pivotIdx[i]]);
                    }
                }
            });
    auto  stopDiagFactor = std::chrono::high_resolution_clock::now();

    // Computing BD^{-1}
    auto startSparsePrep = std::chrono::high_resolution_clock::now();
    SparseMatrix A(M ,   0,  0,      Aw,      Ah),
                 B(M,   Aw,  0, Aw + Bw,      Bh),
                 C(M,    0, Ah,      Cw, Ah + Ch);
    CORE_ASSERT_TRUE_S(A.h == Ah && A.w == Aw);
    CORE_ASSERT_TRUE_S(B.h == Bh && B.w == Bw);
    CORE_ASSERT_TRUE_S(C.h == Ch && C.w == Cw);
    auto stopSparsePrep = std::chrono::high_resolution_clock::now();

    // Computing BD^{-1}
    //recvs::Matrix BDinv(Bh, Dw);
    auto startDinvBt = std::chrono::high_resolution_clock::now();


    int ND = (int)qrd.size();
    int BS = std::max(ND / 48, 1);
    std::vector<corecvs::Matrix> dBlocks(ND);
    std::vector<std::vector<int>> denseCols(ND);
    std::vector<int> cols(ND), rows(ND);

    corecvs::parallelable_for(0, ND, BS, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    auto begin = diagBlocks[i] - diagBlocks[0], end = diagBlocks[i + 1] - diagBlocks[0];
                    dBlocks[i] = B.denseCols(begin, 0, end, B.h, denseCols[i]).t();
                    cols[i] = dBlocks[i].w;
                    rows[i] = dBlocks[i].h;
                }
            });
    int nnz = 0;
    std::vector<int> cumC(ND);
    for (int i = 0; i < ND; ++i)
    {
        int s = cols[i] * rows[i];
        nnz += s;
        cumC[i] = i == 0 ? s : s + cumC[i - 1];
    }
    corecvs::Matrix m(1,1);
    SparseMatrix DinvtBt(m);
    {
        int h = B.w, w = B.h;
        std::vector<int> rowPointers(h + 1), columns(nnz);
        std::vector<double> values(nnz);

        corecvs::parallelable_for(0, ND, BS, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    auto begin = diagBlocks[i] - diagBlocks[0], end = diagBlocks[i + 1] - diagBlocks[0];
                    auto len = end - begin;
                    auto& MM = qrd[i];
                    auto& MB = dBlocks[i];
                    CORE_ASSERT_TRUE_S(MB.h == MM.h);
                    if (!symmetric)
                    {
                        LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'T', len, MB.w, MM.data, MM.stride, &pivots[pivotIdx[i]], MB.data, MB.stride);
                    }
                    else
                    {
                        if (posDef)
                            LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', len, MB.w, MM.data, MM.stride, MB.data, MB.stride);
                        else
                            LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', len, MB.w, MM.data, MM.stride, &pivots[pivotIdx[i]], MB.data, MB.stride);
                    }

                    auto& idx= denseCols[i];
                    int NC = (int)idx.size();
                    rowPointers[begin] = i == 0 ? 0 : cumC[i - 1];
                    for (int ii = 0; ii < len; ++ii)
                    {
                        int cid = rowPointers[ii + begin];
                        memcpy(&values [cid], &MB.a(ii, 0), sizeof(double) * NC);
                        memcpy(&columns[cid], &idx[0], sizeof(int) * NC);
                        if (ii + 1 < len || i + 1 == ND)
                            rowPointers[ii + begin + 1] = rowPointers[ii + begin] + NC;
                    }
                }
            });
        DinvtBt = SparseMatrix(h, w, values, columns, rowPointers);
    }

    auto stopDinvBt = std::chrono::high_resolution_clock::now();
    // Computing lhs/rhs
    auto startLhsRhs = std::chrono::high_resolution_clock::now();

    corecvs::Vector a(Ah, &Bv[0]), b(Ch, &Bv[Ah]), rhs;
    corecvs::Matrix lhs;

#ifdef WITH_TBB
    tbb::task_group g;
    g.run([&]() { rhs = a - b * DinvtBt; });
    g.run([&]() { lhs = (corecvs::Matrix)A - (corecvs::Matrix)(C.t() * DinvtBt).t(); });
    g.wait();
#else
    rhs = a - b * DinvtBt;
    lhs = (corecvs::Matrix)A - (corecvs::Matrix)(C.t() * DinvtBt).t();
#endif
    auto stopLhsRhs = std::chrono::high_resolution_clock::now();

    auto startX = std::chrono::high_resolution_clock::now();
    // Solving for x
    corecvs::Vector x(Aw), y(Bw);
    bool foo = lhs.linSolve(rhs, x, symmetric, false);
    auto stopX = std::chrono::high_resolution_clock::now();

    if (!foo) return false;

    auto startY = std::chrono::high_resolution_clock::now();
    // Solving for y
    rhs = b - C * x;
    corecvs::parallelable_for(0, (int)qrd.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;
            auto& MM = qrd[i];
            corecvs::Vector bcx(len);
            for (int j = 0; j < len; ++j)
                bcx[j] = rhs[j + begin - Aw];
            if (!symmetric)
            {
                LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', len, 1, MM.data, MM.stride, &pivots[pivotIdx[i]], &bcx[0], 1);
            }
            else
            {
                if (posDef)
                    LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', len, 1, MM.data, MM.stride, &bcx[0], 1);
                else
                    LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', len, 1, MM.data, MM.stride, &pivots[pivotIdx[i]], &bcx[0], 1);
            }
            auto res = bcx;
            for (int j = begin; j < end; ++j)
                y[j - Aw] = res[j - begin];
        }
    });
    auto stopY = std::chrono::high_resolution_clock::now();

    double blocks = (stopBlocks - startBlocks).count(),
           diagFac= (stopDiagFactor - startDiagFactor).count(),
           sparseP= (stopSparsePrep - startSparsePrep).count(),
           dinvBt = (stopDinvBt - startDinvBt).count(),
           lhsRhs = (stopLhsRhs - startLhsRhs).count(),
           xxx    = (stopX - startX).count(),
           yyy    = (stopY - startY).count();
    static int statcnt = 0;
    statcnt++;
    if (statcnt % 1000 == 0)
    {
    double total = blocks + diagFac + sparseP + dinvBt + lhsRhs + xxx + yyy;
    total /= 100.0;
#define SPRINT(T, V) \
        << "\t" << T << ": " << V / total << " % / " << V << "ns" << std::endl
    std::cout << "LSC Stats: " << total * 100.0 << "ns"  << std::endl
        SPRINT("Blocks", blocks)
        SPRINT("DiagFactor", diagFac)
        SPRINT("SparsePrep", sparseP)
        SPRINT("DinvBt", dinvBt)
        SPRINT("LhsRhs", lhsRhs)
        SPRINT("X", xxx)
        SPRINT("Y", yyy);
    }

    for (int i = 0; i < Aw; ++i)
        res[i] = x[i];
    for (int j = 0; j < Bw; ++j)
        res[j + Aw] = y[j];
    return true;
#endif
}

bool SparseMatrix::LinSolveSchurComplementOld(const SparseMatrix &M, const corecvs::Vector &Bv
    , const std::vector<int> &diagBlocks
    , corecvs::Vector &res
    , bool symmetric
    , bool posDef)
{
    /*
     * So we partition M and B into
     * +---+---+   /   \   /   \
     * | A | B |   | x |   | a |
     * +---+---+ * +---+ = +---+
     * | C | D |   | y |   | b |
     * +---+---+   \   /   \   /
     * Where D is block-diagonal well-conditioned matrix
     *
     * Then we invert D explicitly and solve
     * x = (A-BD^{-1}C)^{-1}(a-BD^{-1}b)
     * y = D^{-1}(b-Cx)
     *
     * Note that M is symmetric => D is symmetric, (A-BD^{-1}C) is symmetric
     *           M is posdef    => D is posdef,    (A-BD^{-1}C) is symmetric (TODO: isposdef)
     */

#ifndef WITH_BLAS
#  if defined(_MSC_VER)
#    pragma message ("SparseMatrix::LinSolveSchurComplementOld(): NIY")
#  else
#	 warning SparseMatrix::LinSolveSchurComplementOld(): NIY
#  endif
    CORE_UNUSED(M);
    CORE_UNUSED(Bv);
    CORE_UNUSED(diagBlocks);
    CORE_UNUSED(res);
    CORE_UNUSED(symmetric);
    CORE_UNUSED(posDef);
    return false;
#else

    auto Ah = diagBlocks[0],
         Aw = diagBlocks[0];
    auto Bw = M.w - Aw,
         Bh = Ah;
    auto Cw = Aw,
         Ch = M.h - Ah;
    auto Dw = Bw;

    /*
     * The same as above, but with fancy LAPACK
     */
    auto N = diagBlocks.size() - 1;
    std::vector<int> pivots(Dw), pivotIdx(N);

    std::vector<corecvs::Matrix> qrd(N);
    auto startBlocks = std::chrono::high_resolution_clock::now();
    corecvs::parallelable_for(0, (int)N, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    qrd[i] = M.denseSubMatrix(diagBlocks[i], diagBlocks[i], diagBlocks[i + 1], diagBlocks[i + 1]);
                    pivotIdx[i] = diagBlocks[i] - diagBlocks[0];
                }
            });
    auto  stopBlocks = std::chrono::high_resolution_clock::now();

    auto  startDiagFactor = std::chrono::high_resolution_clock::now();
    // Factorizing (without "\"")
    corecvs::parallelable_for(0, (int)N, [&](const corecvs::BlockedRange<int> &r)
            {
                for (int i = r.begin(); i < r.end(); ++i)
                {
                    auto& MM = qrd[i];
                    if (!symmetric)
                    {
                        LAPACKE_dgetrf(LAPACK_ROW_MAJOR, MM.h, MM.w, MM.data, MM.stride, &pivots[pivotIdx[i]]);
                    }
                    else
                    {
                        if (posDef)
                            LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'U', MM.h, MM.data, MM.stride);
                        else
                            LAPACKE_dsytrf(LAPACK_ROW_MAJOR, 'U', MM.h, MM.data, MM.stride, &pivots[pivotIdx[i]]);
                    }
                }
            });
    auto  stopDiagFactor = std::chrono::high_resolution_clock::now();

    // Computing BD^{-1}
    auto startSparsePrep = std::chrono::high_resolution_clock::now();
    SparseMatrix A(M ,   0,  0,      Aw,      Ah),
                 B(M,   Aw,  0, Aw + Bw,      Bh),
                 C(M,    0, Ah,      Cw, Ah + Ch);
    CORE_ASSERT_TRUE_S(A.h == Ah && A.w == Aw);
    CORE_ASSERT_TRUE_S(B.h == Bh && B.w == Bw);
    CORE_ASSERT_TRUE_S(C.h == Ch && C.w == Cw);
    auto stopSparsePrep = std::chrono::high_resolution_clock::now();

    // Computing BD^{-1}
    //recvs::Matrix BDinv(Bh, Dw);
    auto startDinvBt = std::chrono::high_resolution_clock::now();
    auto DinvtBt = (corecvs::Matrix)(!symmetric ? B.t() : C);
    CORE_ASSERT_TRUE_S(DinvtBt.h == Dw && DinvtBt.w == Bh);
    corecvs::parallelable_for(0, (int)qrd.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;
            auto& MM = qrd[i];
            auto ptr = &DinvtBt.a(diagBlocks[i] - diagBlocks[0], 0);
            if (!symmetric)
            {
                LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'T', len, Bh, MM.data, MM.stride, &pivots[pivotIdx[i]], ptr, DinvtBt.stride);
            }
            else
            {
                if (posDef)
                    LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', len, Bh, MM.data, MM.stride, ptr, DinvtBt.stride);
                else
                    LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', len, Bh, MM.data, MM.stride, &pivots[pivotIdx[i]], ptr, DinvtBt.stride);
            }
        }
    });

    auto stopDinvBt = std::chrono::high_resolution_clock::now();
    // Computing lhs/rhs
    auto startLhsRhs = std::chrono::high_resolution_clock::now();
#if 0
    corecvs::Vector a(Ah, &Bv[0]), b(Ch, &Bv[Ah]), rhs;
    corecvs::Matrix lhs;

    tbb::task_group g;
    g.run([&]() { rhs = a - b * DinvtBt; });
    g.run([&]() { lhs = (corecvs::Matrix)A - (corecvs::Matrix)(C.t() * DinvtBt).t(); });
    g.wait();
#else
    corecvs::Vector a(Ah, &Bv[0]), b(Ch, &Bv[Ah]), rhs;
    corecvs::Matrix lhs;
    rhs = a - b * DinvtBt;

    lhs = (corecvs::Matrix)A - (corecvs::Matrix)(C.t() * DinvtBt).t();
#endif
    auto stopLhsRhs = std::chrono::high_resolution_clock::now();

    auto startX = std::chrono::high_resolution_clock::now();
    // Solving for x
    corecvs::Vector x(Aw), y(Bw);
    bool foo = lhs.linSolve(rhs, x, symmetric, false);
    auto stopX = std::chrono::high_resolution_clock::now();

    if (!foo) return false;

    auto startY = std::chrono::high_resolution_clock::now();
    // Solving for y
    rhs = b - C * x;
    corecvs::parallelable_for(0, (int)qrd.size(), [&](const corecvs::BlockedRange<int> &r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            auto begin = diagBlocks[i], end = diagBlocks[i + 1];
            auto len = end - begin;
            auto& MM = qrd[i];
            corecvs::Vector bcx(len);
            for (int j = 0; j < len; ++j)
                bcx[j] = rhs[j + begin - Aw];
            if (!symmetric)
            {
                LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', len, 1, MM.data, MM.stride, &pivots[pivotIdx[i]], &bcx[0], 1);
            }
            else
            {
                if (posDef)
                    LAPACKE_dpotrs(LAPACK_ROW_MAJOR, 'U', len, 1, MM.data, MM.stride, &bcx[0], 1);
                else
                    LAPACKE_dsytrs(LAPACK_ROW_MAJOR, 'U', len, 1, MM.data, MM.stride, &pivots[pivotIdx[i]], &bcx[0], 1);
            }
            auto res = bcx;
            for (int j = begin; j < end; ++j)
                y[j - Aw] = res[j - begin];
        }
    });
    auto stopY = std::chrono::high_resolution_clock::now();

    double blocks = (stopBlocks - startBlocks).count(),
           diagFac= (stopDiagFactor - startDiagFactor).count(),
           sparseP= (stopSparsePrep - startSparsePrep).count(),
           dinvBt = (stopDinvBt - startDinvBt).count(),
           lhsRhs = (stopLhsRhs - startLhsRhs).count(),
           xxx    = (stopX - startX).count(),
           yyy    = (stopY - startY).count();
    static int statcnt = 0;
    statcnt++;
    if (statcnt % 1000 == 0)
    {
    double total = blocks + diagFac + sparseP + dinvBt + lhsRhs + xxx + yyy;
    total /= 100.0;
#define SPRINT(T, V) \
        << "\t" << T << ": " << V / total << " % / " << V << "ns" << std::endl
    std::cout << "LSC Stats: " << total * 100.0 << "ns"  << std::endl
        SPRINT("Blocks", blocks)
        SPRINT("DiagFactor", diagFac)
        SPRINT("SparsePrep", sparseP)
        SPRINT("DinvBt", dinvBt)
        SPRINT("LhsRhs", lhsRhs)
        SPRINT("X", xxx)
        SPRINT("Y", yyy);
    }

    for (int i = 0; i < Aw; ++i)
        res[i] = x[i];
    for (int j = 0; j < Bw; ++j)
        res[j + Aw] = y[j];
    return true;
#endif
}

bool SparseMatrix::LinSolveSchurComplement(const SparseMatrix &M, const corecvs::Vector &Bv, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric, bool posDef, bool explicitInv)
{
    if (explicitInv)
        return LinSolveSchurComplementInv(M, Bv, diagBlocks, res, symmetric, posDef);
    else
        return LinSolveSchurComplementNew(M, Bv, diagBlocks, res, symmetric, posDef);
}

bool SparseMatrix::linSolveSchurComplement(const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric, bool posDef)
{
    return SparseMatrix::LinSolveSchurComplement(*this, B, diagBlocks, res, symmetric, posDef);
}

#endif // #	if !defined(_WIN32) || defined(_WIN64)

std::ostream& corecvs::operator<<(std::ostream& os, const corecvs::SparseImplementations &si)
{
    switch (si)
    {
    case HOMEBREW:
        os << "\x1b[44mhomebrew\x1b[0m";
        break;
    case MKL:
        os << "\x1b[46mintel-mkl\x1b[0m";
        break;
    case CUSPARSE:
        os << "\x1b[42mnvidia-cusparse\x1b[0m";
        break;
    }
    return os;
}

corecvs::Characterizator<const SparseMatrix&>::characteristic_type corecvs::Characterizator<const SparseMatrix&>::Characterize(const SparseMatrix &sm)
{
    return characteristic_type({(int)(std::log(sm.h) / std::log(2)), (int)(std::log(sm.w) / std::log(2)), (int)(std::log(sm.nnz()) / std::log(2))});
}

corecvs::SPMMC::characteristic_type corecvs::SPMMC::Characterize(const corecvs::SPMMC::inner_type &v)
{
    auto &lhs = std::get<0>(v);
    auto &rhs = std::get<1>(v);
    auto transA = std::get<2>(v);
    auto transB = std::get<3>(v);
    return characteristic_type({
        (int)(std::log(lhs.h) / std::log(2)),
        (int)(std::log(lhs.w) / std::log(2)),
        (int)(std::log(lhs.nnz()) / std::log(2)),
        (int)(std::log(rhs.h) / std::log(2)),
        (int)(std::log(rhs.w) / std::log(2)),
        (int)(std::log(rhs.nnz()) / std::log(2)),
        transA ? 1 : 0,
        transB ? 1 : 0});
}

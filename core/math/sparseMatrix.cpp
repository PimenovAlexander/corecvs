#include "sparseMatrix.h"

using namespace corecvs;

SparseMatrix::SparseMatrix(const Matrix &dense, double threshold) : h(dense.h), w(dense.w)
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
                rowPointers[i + 1] = (int)values.size();
            }
        }
    }
}

SparseMatrix::SparseMatrix(int h, int w, const std::vector<double> &values, const std::vector<int> &columns, const std::vector<int> &rowPointers) : h(h), w(w), values(values), columns(columns), rowPointers(rowPointers)
{
    CORE_ASSERT_TRUE_S(h + 1 == (int)rowPointers.size());
    CORE_ASSERT_TRUE_S(values.size() == columns.size());
    CORE_ASSERT_TRUE_S(*rowPointers.rbegin() == (int)values.size());
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
    for (int i = rowPointers[y]; i < rowPointers[y + 1]; ++i)
        if (columns[i] == x)
            return values[i];
    return 0.0;
}

double& SparseMatrix::a(int y, int x)
{
    int i = 0;
    for (i = rowPointers[y]; i < rowPointers[y + 1] && columns[i] < x; ++i);
    if (i < rowPointers[y + 1] && columns[i] == x)
        return values[i];
    columns.insert(columns.begin() + i, x);
    values.insert(values.begin() + i, 0.0);
    for (int j = y + 1; j <= h; ++j)
        ++rowPointers[j];
    return values[i];
}

SparseMatrix::operator Matrix() const
{
    Matrix m(h, w);

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

Vector corecvs::operator *(const SparseMatrix &lhs, const Vector &rhs)
{
    CORE_ASSERT_TRUE_S(lhs.w == rhs.size());
    Vector ans(lhs.h);
    for (int i = 0; i < lhs.h; ++i)
    {
        double res = 0.0;
        for (int j = lhs.rowPointers[i]; j < lhs.rowPointers[i + 1]; ++j)
            res += lhs.values[j] * rhs[lhs.columns[j]];
        ans[i] = res;
    }
    return ans;
}

Vector corecvs::operator *(const Vector &lhs, const SparseMatrix &rhs)
{
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
}

#include <fstream>
SparseMatrix corecvs::operator *(const SparseMatrix &lhs, const SparseMatrix &rhst)
{
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
        std::sort(&columns[rowPointers[i]], &columns[rowPointers[i + 1]]);
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
}

SparseMatrix SparseMatrix::ata() const
{
#if 1
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
#else
#if 0
//    const auto& rhs = *this;
    std::vector<double> values;
    std::vector<int> columns, rowPointers(h + 1);
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            double total = 0.0;
            int cnt = 0;
            int lhs_l = rowPointers[i], lhs_r = rowPointers[i + 1];
            int rhs_l = rowPointers[j], rhs_r = rowPointers[j + 1];
            while (lhs_l < lhs_r && rhs_l < rhs_r)
            {
                int cl = columns[lhs_l], cr = columns[rhs_l];
                if (cl < cr)
                {
                    ++lhs_l;
                    continue;
                }
                if (cr < cl)
                {
                    ++rhs_l;
                    continue;
                }
                total += values[lhs_l] * values[rhs_l];
                lhs_l++;
                rhs_l++;
                cnt++;
            }
            if (cnt > 0 && total != 0.0)
            {
                values.push_back(total);
                columns.push_back(j);
            }
        }
        rowPointers[i + 1] = values.size();
    }
    return SparseMatrix(h, h, values, columns, rowPointers);
#else
    return t() * (*this);
#endif
#endif
}

SparseMatrix SparseMatrix::t() const
{
#if 0
    std::map<std::pair<int, int>, double> map;
    for (int i = 0; i < h; ++i)
    {
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            map[std::make_pair(columns[j], i)] = values[j];
        }
    }
    return SparseMatrix(w, h, map);
#else
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
#endif
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

    for (int i = 0; i < mh; ++i)
    {
        for (int j = rowB[i]; j < rowE[i]; ++j)
        {
            values.push_back(vals[j]);
            columns.push_back(cols[j]);
        }
        rowPointers[i + 1] = (int)values.size();
    }

    mkl_sparse_destroy(copy);
}
#endif

#include <chrono>

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
#endif
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

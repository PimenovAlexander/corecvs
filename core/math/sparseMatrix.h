#ifndef SPARSEMATRIX
#define SPARSEMATRIX

#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <atomic>

#include "math/matrix/matrix.h"
#include "math/vector/vector.h"

#ifdef WITH_MKL
#include <mkl.h>
#endif

#ifdef WITH_CUSPARSE
#include "cuda_runtime_api.h"
#include "cusparse.h"
#include "cuda.h"
#endif

#include "math/wisdom.h"

namespace corecvs
{

/**
* Here we store sparse matrix in 3-array compressed row storage
* http://netlib.org/linalg/html_templates/node91.html
*
* The only difference is that we prefer zero-based indicies
* Note that current implementation may be highly inefficient, but
* it proposed for using in problems with < 1% non-zero entries, so
* O(nnz^2) should be significantly better O(n^2) even with poor
* implementation
*/
class SparseMatrix
{
public:
    //! \brief Creates sparse matrix from dense with specified threshold
    SparseMatrix(const Matrix &dense, double threshold = 0.0);
    //! \brief Creates sparse matrix from CRS data
    SparseMatrix(int h = 0, int w = 0, const std::vector<double> &values = {}, const std::vector<int> &columns = {}, const std::vector<int> &rowPointers = {0});
    //! \brief Creates sparse matrix from {point, value} data
    SparseMatrix(int h, int w, const std::map<std::pair<int, int>, double> &data);
    //! \brief Creates dense submatrix and return column idx
    Matrix denseRows(int x1, int y1, int x2, int y2, std::vector<int> &colIdx);
    //! \brief Creates dense submatrix and return row idx
    Matrix denseCols(int x1, int y1, int x2, int y2, std::vector<int> &rowIdx);
    //! \brief Cast to dense matrix
    explicit operator Matrix() const;
    SparseMatrix(const SparseMatrix &src, int x1, int y1, int x2, int y2);
    SparseMatrix(const SparseMatrix &M) : h(M.h), w(M.w), values(M.values), columns(M.columns), rowPointers(M.rowPointers)
    {
    }
    SparseMatrix &operator=(const SparseMatrix &M)
    {
        if (this == &M)
            return *this;
#ifdef WITH_CUSPARSE
        gpuPromotion = nullptr;
#endif
        h = M.h;
        w = M.w;
        values = M.values;
        columns = M.columns;
        rowPointers = M.rowPointers;
        return *this;
    }
    corecvs::Matrix denseSubMatrix(int x1, int y1, int x2, int y2) const;
    void denseSubMatrix(int x1, int y1, int x2, int y2, double* output, int stride = -1) const;
    void checkCorrectness() const;
    int getUBIndex(int i, int j) const;
    int getIndex(int i, int j) const;

    Vector trsv(const Vector &rhs, const char* trans, bool up, int N) const;
    Vector trsv_homebrew(const Vector &rhs, const char* trans,  bool up, int N) const;
    Vector spmv_homebrew(const Vector &rhs, bool trans) const;
    SparseMatrix spmm_homebrew(const SparseMatrix &rhs, bool transA, bool transB) const;
#ifdef WITH_MKL
    Vector spmv_mkl     (const Vector &rhs, bool trans) const;
    Vector trsv_mkl(const Vector &rhs, const char* trans,  bool up, int N) const;
    SparseMatrix spmm_mkl(const SparseMatrix &rhs, bool transA, bool transB) const;
#endif
#ifdef WITH_CUSPARSE
    Vector spmv_cusparse(const Vector &rhs, bool trans, int gpuId) const;
    Vector trsv_cusparse(const Vector &rhs, const char* trans,  bool up, int N, int gpuId) const;
    SparseMatrix spmm_cusparse(const SparseMatrix &rhs, bool transA, bool transB, int devId) const;
#endif


#ifdef WITH_MKL
    //! \brief Note: deletion of MKL's deletions is your problem
    explicit operator sparse_matrix_t() const;
    SparseMatrix(const sparse_matrix_t &mklSparse);
#endif
#if 0
    bool isValid(bool full = false) const;
    void sort();
#endif
    /**
     * \brief Accesses element.
     * Note that it does not return reference and
     * is not efficient (it may scan all non-zero entries
     * in row y before returning 0)
     */
    double a(int y, int x) const;
    double&a(int y, int x);
    int nnz() const;
    double fillin() const;
    void spyPlot() const;
    Vector dtrsv(const Vector &v, bool upper = true, bool notrans = true) const;

    friend SparseMatrix operator -(const SparseMatrix &a);
    friend SparseMatrix operator *(const double       &lhs, const SparseMatrix &rhs);
    friend SparseMatrix operator *(const SparseMatrix &lhs, const double       &rhs);
    friend SparseMatrix operator /(const SparseMatrix &lhs, const double       &rhs);

    friend SparseMatrix operator *(const SparseMatrix &lhs, const SparseMatrix &rhs);
    friend SparseMatrix operator +(const SparseMatrix &lhs, const SparseMatrix &rhs);
    friend SparseMatrix operator -(const SparseMatrix &lhs, const SparseMatrix &rhs);

    friend Vector       operator *(const SparseMatrix &lhs, const Vector       &rhs);
    friend Vector       operator *(const Vector       &lhs, const SparseMatrix &rhs);

    //! \brief Transposes matrix
    SparseMatrix t() const;
    SparseMatrix upper() const;
    SparseMatrix ata() const;
    bool linSolve(const Vector &rhs, Vector &res, bool symmetric = false, bool posDef = false) const;
    static bool LinSolve(const SparseMatrix &m, const Vector &rhs, Vector &res, bool symmetric = false, bool posDef = false);
    /*
     * Linear system solving with use of schur-complement structure (only with block-diagonal lower-right part)
     * Note that you shoul use it only when you are sure that lower (block-diagonal) part is well-conditioned
     */
#if !defined(_WIN32) || defined(_WIN64) // requires x86 blas
    static bool LinSolveSchurComplementInv(const corecvs::SparseMatrix &A, const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false);
    static bool LinSolveSchurComplementOld(const corecvs::SparseMatrix &A, const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false);
    static bool LinSolveSchurComplementNew(const corecvs::SparseMatrix &A, const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false);
	static bool LinSolveSchurComplement(const corecvs::SparseMatrix &A, const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false, bool explicitInv = false);
    bool        linSolveSchurComplement(const corecvs::Vector &B, const std::vector<int> &diagBlocks, corecvs::Vector &res, bool symmetric = false, bool posDef = false);
#endif 

    std::pair<bool, SparseMatrix> incompleteCholseky(bool allow_parallel = true);

    void print(std::ostream& out = std::cout) const;
    friend std::ostream& operator<< (std::ostream &out, const SparseMatrix &sm);

    int h, w;
    Vector dtrsv_un(const Vector &v) const;
    Vector dtrsv_ut(const Vector &v) const;
    Vector dtrsv_ln(const Vector &v) const;
    Vector dtrsv_lt(const Vector &v) const;

#ifdef WITH_CUSPARSE
    void promoteToGpu(int gpuId = 0) const
    {
        if (!gpuPromotion)
            gpuPromotion = std::unique_ptr<CUDAPromoter>(new CUDAPromoter(*this));
        gpuPromotion->promote(gpuId);
    }
    static const int SPMV_RETRY = 3;
#endif
private:
#ifdef WITH_CUSPARSE
    struct CUDAPromoter
    {
        typedef std::unique_ptr<double, decltype(&cudaFree)> GpuDoublePtr;
        typedef std::unique_ptr<int, decltype(&cudaFree)> GpuIntPtr;
        typedef std::unique_ptr<void, decltype(&cudaFree)> GpuVoidPtr;
        const SparseMatrix *matrix;

        CUDAPromoter(const SparseMatrix &m);

        struct TriangularPromotion
        {
            GpuVoidPtr bufferTrans  = GpuVoidPtr(nullptr, cudaFree),
                       bufferNoTrans= GpuVoidPtr(nullptr, cudaFree);
            cusparseSolvePolicy_t policy = CUSPARSE_SOLVE_POLICY_USE_LEVEL;
            cusparseMatDescr_t descr;
            cusparseHandle_t handle;
            csrsv2Info_t infoTrans, infoNoTrans;

            TriangularPromotion();
            TriangularPromotion(TriangularPromotion &&rhs);
            TriangularPromotion& operator=(TriangularPromotion &&rhs);
            operator bool() const;
            TriangularPromotion(const SparseMatrix &m, bool upper, int gpuId);

            static void checkError(const char *bar="", int baz = -1);

            ~TriangularPromotion();
        };

        struct BasicPromotion
        {
            GpuDoublePtr dev_values = GpuDoublePtr(nullptr, cudaFree);
            GpuIntPtr dev_columns = GpuIntPtr(nullptr, cudaFree), dev_rowPointers = GpuIntPtr(nullptr, cudaFree);

            BasicPromotion();
            BasicPromotion(const SparseMatrix &m, int gpuId);
#if defined(_MSC_VER) && (_MSC_VER < 1900)   // needs only for older than msvc2015 compiler

            BasicPromotion& operator=(BasicPromotion &&rhs);
#endif
            operator bool() const;
        };

        std::map<int, BasicPromotion>       basicPromotions;
        std::map<int, TriangularPromotion> triangularPromotions;
        void promote(int gpuId);
        void triangularAnalysis(bool upper, int gpuId);

    };
    mutable std::unique_ptr<CUDAPromoter> gpuPromotion = nullptr;
#endif
    void swapCoords(int &x1, int &y1, int &x2, int &y2) const;
    //! All non-zero entries of matrix
    std::vector<double> values;
    //! Column-indicies (of values)
    std::vector<int> columns;
    /**
     * Row start pointers; the last one equals to total number of NNZ
     * elements
     */
    std::vector<int> rowPointers;
};

enum SparseImplementations { HOMEBREW, MKL, CUSPARSE };
std::ostream& operator<< (std::ostream& out, const SparseImplementations &si);

template<>
struct Characterizator<const SparseMatrix&>
{
    static const int CHARACTERISTIC_WIDTH = 3;
    typedef std::array<int, CHARACTERISTIC_WIDTH> characteristic_type;

    static characteristic_type Characterize(const SparseMatrix& sm);
};

template<>
struct Characterizator<const std::tuple<const SparseMatrix&, const SparseMatrix&, bool, bool>&>
{
    static const int CHARACTERISTIC_WIDTH = 8;
    typedef std::array<int, CHARACTERISTIC_WIDTH> characteristic_type;
    typedef std::tuple<const SparseMatrix&, const SparseMatrix&, bool, bool> inner_type;

    static characteristic_type Characterize(const inner_type &v);
};
typedef Characterizator<const std::tuple<const SparseMatrix&, const SparseMatrix&, bool, bool>&> SPMMC;

typedef Wisdom<const SparseMatrix&, SparseImplementations, Vector, const Vector&, bool> SPMVWisdom;
typedef Wisdom<const SparseMatrix&, SparseImplementations, Vector, const Vector&, const char *, bool, int> TRSVWisdom;
typedef Wisdom<const SPMMC::inner_type&, SparseImplementations, SparseMatrix> SPMMWisdom;

SparseMatrix operator -(const SparseMatrix &a);
SparseMatrix operator *(const double       &lhs, const SparseMatrix &rhs);
SparseMatrix operator *(const SparseMatrix &lhs, const double       &rhs);
SparseMatrix operator /(const SparseMatrix &lhs, const double       &rhs);

SparseMatrix operator *(const SparseMatrix &lhs, const SparseMatrix &rhs);
SparseMatrix operator +(const SparseMatrix &lhs, const SparseMatrix &rhs);
SparseMatrix operator -(const SparseMatrix &lhs, const SparseMatrix &rhs);

Vector       operator *(const SparseMatrix &lhs, const Vector       &rhs);
Vector       operator *(const Vector       &lhs, const SparseMatrix &rhs);
std::ostream& operator<< (std::ostream &out, const SparseMatrix &sm);

}

#endif // SPARSEMATRIX

#ifndef MATRIX44_H_
#define MATRIX44_H_
/**
 * \file matrix44.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Aug 10, 2010
 * \author alexander
 */

#include "core/math/vector/fixedVector.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/vector/vector4d.h"

namespace corecvs {


/**
 *  This class is mostly for representing Projectrive Transformations in 3D
 *
 * \f[ P=\pmatrix{
 *        a_{0,0} & a_{0,1} & a_{0,2} & a_{0,3} \cr
 *        a_{1,0} & a_{1,1} & a_{1,2} & a_{1,3} \cr
 *        a_{2,0} & a_{2,1} & a_{2,2} & a_{2,3} \cr
 *        a_{3,0} & a_{3,1} & a_{3,2} & a_{3,3}}
 * \f]
 *
 *
 * **/
//class Matrix44 : public FixedVectorBase<Matrix44, double, 16>
class Matrix44 : public FixedVectorBase<Matrix44, double, 16>
{
public:
    // typedef FixedVectorBase<Matrix44, double, 16> BaseClass;
    typedef FixedVectorBase<Matrix44, double, 16> BaseClass;

    static const int H = 4;
    static const int W = H;
    static const int ELEM_NUM = H*W;

    Matrix44();


    /**
     * Constructs the 4 by 4 Matrix with the given diagonal element
     * In terms of projective transform that will always be an identity matrix
     *
     * \f[ P=\pmatrix{
     *        d & 0 & 0 & 0 \cr
     *        0 & d & 0 & 0 \cr
     *        0 & 0 & d & 0 \cr
     *        0 & 0 & 0 & d }
     * \f]
     *
     *
     **/
    explicit Matrix44(double d) : BaseClass(0.0)
    {
        for (int i = 0; i < H; i++)
            a(i,i) = d;
    }

    Matrix44(const FixedVector<double, 16> &data) : BaseClass(data)
    { }

    Matrix44(double _a00, double _a01, double _a02, double _a03,
             double _a10, double _a11, double _a12, double _a13,
             double _a20, double _a21, double _a22, double _a23,
             double _a30, double _a31, double _a32, double _a33
             );

	Matrix44(const Vector4dd& row0, const Vector4dd& row1, const Vector4dd& row2, const Vector4dd& row3);

    Matrix44(const Matrix33 &_m, const Vector3dd &_shift = Vector3dd(0,0,0));
    static void FillWithArgs(
            Matrix44 &m,
            const double &a00, const double &a01, const double &a02, const double &a03,
            const double &a10, const double &a11, const double &a12, const double &a13,
            const double &a20, const double &a21, const double &a22, const double &a23,
            const double &a30, const double &a31, const double &a32, const double &a33);

    static void FillWithArgsT(
            Matrix44 &m,
            const double &a00, const double &a01, const double &a02, const double &a03,
            const double &a10, const double &a11, const double &a12, const double &a13,
            const double &a20, const double &a21, const double &a22, const double &a23,
            const double &a30, const double &a31, const double &a32, const double &a33);

    static Matrix44 Diagonal(double d0, double d1, double d2, double d3)
    {
        Matrix44 toReturn(0);
        toReturn.a(0,0) = d0;
        toReturn.a(1,1) = d1;
        toReturn.a(2,2) = d2;
        toReturn.a(3,3) = d3;
        return toReturn;
    }

    static Matrix44 FromRawRows(const double *data) {
        Matrix44 result;
        memcpy(result.element, data, sizeof(double) * ELEM_NUM);
        return result;
    }

    /* Mirror matrix */
    static Matrix44 MirrorXY();
    static Matrix44 MirrorYZ();
    static Matrix44 MirrorXZ();

    Matrix33 topLeft33() const;
    Vector3dd translationPart() const;
	Vector4dd row(int i) const
	{
		return Vector4dd(a(i, 0), a(i, 1), a(i, 2), a(i, 3));
	}

	Vector4dd column(int i) const
	{
		return Vector4dd(a(0, i), a(1, i), a(2, i), a(3, i));
	}

    double &a(int i,int j);
    const double &a(int i,int j) const;

    double &operator ()(int i,int j);
    const double &operator ()(int i,int j) const;

    void transpose();
    Matrix44 transposed() const;
    Matrix44 t() const;

	// if successful, returns 4x4 matrix decomposion [ Matrix = Translation * Rotation * Scale ]
	bool decomposeTRS(Vector3dd& scale, Vector3dd& translate, Matrix33& rotate);

    Matrix44 inverted() const;
    double trace() const;
    double det() const;

    double frobeniusNorm() const;

    friend Matrix44 operator *(const Matrix44 &M1, const Matrix44 &M2);

    friend Matrix44 operator *= (Matrix44 &M1, const Matrix44 &M2);

    friend Matrix44 operator *(const Matrix44 &M1, const Matrix33 &M2);
    friend Matrix44 operator *(const Matrix33 &M1, const Matrix44 &M2);
    friend Matrix44 operator *= (Matrix44 &M1, const Matrix33 &M2);


    friend Vector3dd operator *(const Matrix44 &M1, const Vector3dd &v);
    friend FixedVector<double, 4> operator *(const Matrix44 &m, const FixedVector<double, 4> &v);    
    friend Vector4dd operator *(const Matrix44 &m, const Vector4dd &v);

    static Matrix44 Identity();
    static Matrix44 ProjectParallelToZ();
    static Matrix44 Shift(double x, double y, double z);
    static Matrix44 Shift(const Vector3dd &v);
    static Matrix44 Scale(const double &d);
    static Matrix44 Scale(const double &d1, const double &d2, const double &d3);

    static Matrix44 RotationX(double angle);
    static Matrix44 RotationY(double angle);
    static Matrix44 RotationZ(double angle);

    static Matrix44 Frustum(double fovY, double aspect, double zNear = 1.0, double zFar = 1000.0);

    ~Matrix44();


    friend std::ostream & operator <<(std::ostream &out, const Matrix44 &matrix)
    {
        streamsize wasPrecision = out.precision(6);
        for (int i = 0; i < matrix.H; i++)
        {
            out << "[";
            for (int j = 0; j < matrix.W; j++)
            {
                out << (j == 0 ? "" : " ");
                out.width(6);
                out << matrix.a(i,j);
            }
            out << "]\n";
        }
        out.precision(wasPrecision);
        return out;
    }

    friend std::istream & operator >>(std::istream &in, Matrix44 &matrix)
    {
        for (int i = 0; i < matrix.H; i++)
        {
            for (int j = 0; j < matrix.W; j++)
            {
                while (true)
                {
                    in >> matrix.a(i,j);
                    // cout << "Read:" << i << "," << j << " " << matrix.a(i,j) << endl;
                    if (in.good())
                        break;
                    if (in.eof())
                       return in;
                    /* Not a number clean the error and advance*/
                    in.clear();
                    // cout << "Skipped:" << ((char)in.peek()) << endl;
                    in.ignore();
                }

            }
       }
       return in;
    }

    /**
     * Visitor. So far lazy version
     **/
    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            const char *names[] = {"a00", "a01", "a02", "a03",
                                   "a10", "a11", "a12", "a13",
                                   "a20", "a21", "a22", "a23",
                                   "a30", "a31", "a32", "a33"};

            for (int i = 0; i < LENGTH; i++) {
                visitor.visit(element[i], 0.0, names[i]);
            }
        }


    /**
     * Matrix Operations interface
     **/

    double &atm(int i, int j) {
        return a(i, j);
    }
    const double &atm(int i, int j) const {
        return a(i, j);
    }
    int height() {return H; }
    int width()  {return W; }
    static Matrix44 createMatrix(int /*h*/, int /*w*/) {return Matrix44(); }



};

inline Matrix44::Matrix44(
         double _a00, double _a01, double _a02, double _a03,
         double _a10, double _a11, double _a12, double _a13,
         double _a20, double _a21, double _a22, double _a23,
         double _a30, double _a31, double _a32, double _a33
         )
{
    a(0,0) = _a00;   a(0,1) = _a01;   a(0,2) = _a02;   a(0,3) = _a03;
    a(1,0) = _a10;   a(1,1) = _a11;   a(1,2) = _a12;   a(1,3) = _a13;
    a(2,0) = _a20;   a(2,1) = _a21;   a(2,2) = _a22;   a(2,3) = _a23;
    a(3,0) = _a30;   a(3,1) = _a31;   a(3,2) = _a32;   a(3,3) = _a33;
}

inline Matrix44::Matrix44(const Vector4dd& row0, const Vector4dd& row1, const Vector4dd& row2, const Vector4dd& row3)
{
	a(0, 0) = row0.x();   a(0, 1) = row0.y();   a(0, 2) = row0.z();   a(0, 3) = row0.w();
	a(1, 0) = row1.x();   a(1, 1) = row1.y();   a(1, 2) = row1.z();   a(1, 3) = row1.w();
	a(2, 0) = row2.x();   a(2, 1) = row2.y();   a(2, 2) = row2.z();   a(2, 3) = row2.w();
	a(3, 0) = row3.x();   a(3, 1) = row3.y();   a(3, 2) = row3.z();   a(3, 3) = row3.w();
}

/**
 * Constructs the matrix form linear part presented by Matrix33 and affine shift
 * presented by Vector3dd
 *
 *
 *
 *  \f[
 *    \left( \begin{array}{ccc|c}
 *        \multicolumn{3}{c|}{\multirow{3}{*}{M}} & x \\
 *          &   &   & y \\
 *          &   &   & z \\ \hline
 *        0 & 0 & 0 & 1
 *    \end{array} \right)
 *  \f]
 *
 * */
inline Matrix44::Matrix44(const Matrix33 &_matrix, const Vector3dd &_shift)
{
    a(0,0)  = _matrix.a(0,0);
    a(0,1)  = _matrix.a(0,1);
    a(0,2)  = _matrix.a(0,2);
    a(0,3)  = _shift.x();
    a(1,0)  = _matrix.a(1,0);
    a(1,1)  = _matrix.a(1,1);
    a(1,2)  = _matrix.a(1,2);
    a(1,3)  = _shift.y();
    a(2,0)  = _matrix.a(2,0);
    a(2,1)  = _matrix.a(2,1);
    a(2,2)  = _matrix.a(2,2);
    a(2,3)  = _shift.z();
    a(3,0)  = 0;
    a(3,1)  = 0;
    a(3,2)  = 0;
    a(3,3)  = 1;
}

inline void Matrix44::FillWithArgs(Matrix44 &m,
                    const double &a00, const double &a01, const double &a02, const double &a03,
                    const double &a10, const double &a11, const double &a12, const double &a13,
                    const double &a20, const double &a21, const double &a22, const double &a23,
                    const double &a30, const double &a31, const double &a32, const double &a33)
{
    #define  F(i, j) m.a(i, j) = a ## i ## j;
    #define FF(i) F(i, 0) F(i, 1) F(i, 2) F(i, 3)
         FF(0)
         FF(1)
         FF(2)
         FF(3)
    #undef FF
    #undef F
}

inline void Matrix44::FillWithArgsT(
        Matrix44 &m,
        const double &a00, const double &a01, const double &a02, const double &a03,
        const double &a10, const double &a11, const double &a12, const double &a13,
        const double &a20, const double &a21, const double &a22, const double &a23,
        const double &a30, const double &a31, const double &a32, const double &a33)
{
    #define  F(i, j) m.a(i, j) = a ## j ## i;
    #define FF(i) F(i, 0) F(i, 1) F(i, 2) F(i, 3)
        FF(0)
        FF(1)
        FF(2)
        FF(3)
    #undef FF
    #undef F
}

inline Matrix33 Matrix44::topLeft33() const
{
    Matrix33 b;
    b(0,0) = a(0,0);
    b(0,1) = a(0,1);
    b(0,2) = a(0,2);

    b(1,0) = a(1,0);
    b(1,1) = a(1,1);
    b(1,2) = a(1,2);

    b(2,0) = a(2,0);
    b(2,1) = a(2,1);
    b(2,2) = a(2,2);
    return b;
}

inline Vector3dd Matrix44::translationPart() const
{
    Matrix33 b;
    b(0,1) = a(0,1);
    b(0,0) = a(0,0);
    b(0,2) = a(0,2);

    return Vector3dd(a(0,3), a(1,3), a(2,3));
}


/**
 *  Geting Matrix element functions block
 **/

inline double &Matrix44::a(int i,int j)
{
    return (*this)[i * W + j];
}

inline const double &Matrix44::a(int i,int j) const
{
    return (*this)[i * W + j];
}

inline double &Matrix44::operator ()(int i,int j)
{
    return (*this)[i * W + j];
}

inline const double &Matrix44::operator ()(int i,int j) const
{
    return (*this)[i * W + j];
}


} //namespace corecvs
#endif /* MATRIX44_H_ */


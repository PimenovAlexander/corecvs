#ifndef _MATRIX33_H_
#define _MATRIX33_H_
/**
 * \file matrix33.h
 * \brief This file holds the header for the commonly used structure -
 * a 3 by 3 matrix.
 *
 * \f[ P=\pmatrix{
 *        a_{1,1} & a_{1,2} & a_{1,3}\cr
 *        a_{2,1} & a_{2,2} & a_{2,3}\cr
 *        a_{3,1} & a_{3,2} & a_{3,3}}
 * \f]
 *
 * \ingroup cppcorefiles
 * \date Jan 27, 2007
 * \author Alexander Pimenov
 **/

#include <iostream>
#include <limits>
#include <string.h>

#include "global.h"

#include "vector3d.h"
#include "fixedVector.h"

namespace corecvs {

using std::numeric_limits;
using std::streamsize;

/**
 *  Class describes the 3 by 3 matrix.
 *  This matrix could represent the linear transformation in 3D or a
 *  projective transformation in 2D
 *
 * \f[ P=\pmatrix{
 *        a_{0,0} & a_{0,1} & a_{0,2}\cr
 *        a_{1,0} & a_{1,1} & a_{1,2}\cr
 *        a_{2,0} & a_{2,1} & a_{2,2}
 *        }
 * \f]
 *
 *
 **/


class  Matrix33 : public FixedVectorBase<Matrix33, double, 9>
{
public:
    typedef FixedVectorBase<Matrix33, double, 9> BaseClass;
    typedef double ElementType;

    static const int H = 3;
    static const int W = H;
    static const int ELEM_NUM = H * W;


    explicit Matrix33(const double* _data) : BaseClass(_data) {}
    explicit Matrix33(double _A);

    Matrix33(double _a00, double _a01, double _a02,
             double _a10, double _a11, double _a12,
             double _a20, double _a21, double _a22);

    Matrix33() {}

    Matrix33(const FixedVector<double, 9> &_M) : BaseClass(&_M.at(0)) {}
    Matrix33(const Vector3dd &_V1, const Vector3dd &_V2, const Vector3dd &_V3);

//  Matrix33& operator =(const Matrix33& V);

    void invert();
    // TODO: Rename to inverted
    Matrix33 inv() const;

    double trace() const;
    double det() const;
    inline double &a(int i, int j);
    inline const double &a(int i, int j) const;
    double &operator ()(int i,int j);
    const double &operator ()(int i,int j) const;

    void transpose();
    void swapRows(int r1, int r2);
    void swapColumns(int r1, int r2);

    Matrix33 t() const;
    Matrix33 transposed() const;

    void neg();
    Vector3dd aV(int i) const;
    Vector3dd row(int i) const;

    Vector3dd aW(int i) const;
    Vector3dd column(int i) const;


    Vector3dd diagonal() const;

    double frobeniusNorm() const;

    friend Matrix33 operator * (const Matrix33 &M1, const Matrix33 &M2);
    friend Matrix33 operator *=(      Matrix33 &M1, const Matrix33 &M2);

    friend inline Vector3dd operator *(const Matrix33 &matrix, const Vector3dd &V);
    friend inline Vector3dd operator *(const Vector3dd &V, const Matrix33 &matrix);

    Vector3dd mulBy2dRight(const Vector2dd &V) const;
    Vector3dd mulBy2dLeft (const Vector2dd &V) const;

    friend inline Vector2dd operator *(const Matrix33 &matrix, const Vector2dd &V);
    friend inline Vector2dd operator *(const Vector2dd &V, const Matrix33 &matrix);

    /* These could be done by explicit constructors also. But what for? */

    static Matrix33 Identity();
    static Matrix33 RotationX(double angle);
    static Matrix33 RotationY(double angle);
    static Matrix33 RotationZ(double angle);
    static Matrix33 RotationZ90();
    static Matrix33 RotationZ180();
    static Matrix33 RotationZ270();

    static inline Matrix33 RotateProj(double angle)
    {
        return RotationZ(angle);
    }

    static Matrix33 Rotate(const Vector3dd& v, double angle);

    /* Mirror matrix */
    static Matrix33 MirrorXY();
    static Matrix33 MirrorYZ();
    static Matrix33 MirrorXZ();

    /* Constructing 3D scale matrix */
    static Matrix33 Scale3(const Vector3dd& v);
    static Matrix33 Scale3(double s1, double s2, double s3);
    static Matrix33 Scale3(double v);


    /* Constructing 2D scale matrix */

    static Matrix33 Scale2(double x, double y);

    static inline Matrix33 Scale2(const Vector2dd& v)
    {
        return Scale2(v.x(), v.y());
    }

    static inline Matrix33 Scale2(double v)
    {
        return Scale2(v,v);
    }

    static inline Matrix33 ScaleProj(double v)
    {
        return Scale2(v);
    }

    /* Shifting */

    static Matrix33 ShiftProj(double x, double y);
    static inline Matrix33 ShiftProj(const Vector2dd& v)
    {
        return ShiftProj(v.x(), v.y());
    }

    static Matrix33 Projective(double x, double y);
    static inline Matrix33 Projective(const Vector2dd& v)
    {
        return Projective(v.x(), v.y());
    }

    static Matrix33 CrossProductRight(const Vector3dd &vector);
    static Matrix33 CrossProductLeft (const Vector3dd &vector);

    static Matrix33 VectorByVector(const Vector3dd &a, const Vector3dd &b);

    static Matrix33    FromRows(const Vector3dd &a, const Vector3dd &b, const Vector3dd &c);
    static Matrix33 FromColumns(const Vector3dd &a, const Vector3dd &b, const Vector3dd &c);


    void print() const
    {
        cout << *this;
    }

    void serialise (ostream &out) const
    {
        streamsize oldPrec = out.precision(numeric_limits<double>::digits10 + 3);
        out << *this;
        out.precision(oldPrec);
    }

    friend ostream & operator <<(ostream &out, const Matrix33 &matrix)
    {
        //streamsize wasPrecision = out.precision(6);
        for (int i = 0; i < matrix.H; i++)
        {
            out << "[";
            for (int j = 0; j < matrix.W; j++)
            {
                out << (j == 0 ? "" : " ");
                //out.width(6);
                out << matrix.a(i,j);
            }
            out << "]\n";
        }
        //out.precision(wasPrecision);
        return out;
    }


    friend istream & operator >>(istream &in, Matrix33 &matrix)
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


    static Matrix33 ProjectiveByPoints(Vector2dd points[4], Vector2dd images[4]);
    static Matrix33 ProjectiveByPoints(int pointNumber, Vector2dd points[], Vector2dd images[]);

    /* Matrix Operations interface */
    double &atm(int i, int j) {
        return a(i, j);
    }
    const double &atm(int i, int j) const {
        return a(i, j);
    }
    int height() {return H; }
    int width()  {return W; }
    static Matrix33 createMatrix(int /*h*/, int /*w*/) {return Matrix33(); }

    /* Additional helper function */
    void fillWithArgs(
             double _a00, double _a01, double _a02,
             double _a10, double _a11, double _a12,
             double _a20, double _a21, double _a22
         );


};



/*****************************************************************************/
/*  Inline functions block                                                   */
/*****************************************************************************/

/**
 *  Geting Matrix element functions block
 **/

inline double & Matrix33::a(int i,int j)
{
   return (*this)[i * W + j];
}

inline const double &Matrix33::a(int i,int j) const
{
   return (*this)[i * W + j];
}

inline double &Matrix33::operator ()(int i,int j)
{
    return (*this)[i * W + j];
}

inline const double &Matrix33::operator ()(int i,int j) const
{
    return (*this)[i * W + j];
}




/**
 *   Matrix construction via 3 vectors
 *
 *   \param _V1
 *       first row
 *   \param _V2
 *       second row
 *   \param _V3
 *       third row
 *
 *   \f[ P=\pmatrix{
 *        V_1\cr
 *        V_2\cr
 *        V_3}
 *   \f]
 *
 **/
inline Matrix33::Matrix33(const Vector3dd& _V1, const  Vector3dd& _V2, const Vector3dd& _V3)
{
   a(0,0) = _V1.x();   a(0,1) = _V1.y();   a(0,2) = _V1.z();
   a(1,0) = _V2.x();   a(1,1) = _V2.y();   a(1,2) = _V2.z();
   a(2,0) = _V3.x();   a(2,1) = _V3.y();   a(2,2) = _V3.z();
}

/**
 *   Synonym to Matrix33(const Vector3dd& _V1,const  Vector3dd& _V2,const Vector3dd& _V3)
 **/
inline Matrix33 Matrix33::FromRows(const Vector3dd& _V1, const  Vector3dd& _V2, const Vector3dd& _V3)
{
    return Matrix33(_V1, _V2, _V3);
}


/**
 *   Matrix construction via 3 columns
 *
 *   \param W1
 *       first column
 *   \param W2
 *       second column
 *   \param W3
 *       third column
 *
 *   \f[ P=\pmatrix{
 *        W_1 & W_2 & W_3
 *        }
 *   \f]
 *
 *
 **/
inline Matrix33 Matrix33::FromColumns(const Vector3dd &W1, const Vector3dd &W2, const Vector3dd &W3)
{
    return Matrix33(
            W1.x(), W2.x(), W3.x(),
            W1.y(), W2.y(), W3.y(),
            W1.z(), W2.z(), W3.z()
    );
}


/**
 *   Matrix construction via diagonal element.
 *
 *   \attention Please use this only to create zero and identity matrix, for real scaling use scaling function
 *
 *   \param _A
 *       An element that will fill the diagonal
 *
 *   \f[ P=\pmatrix{
 *        _A &  0 &  0 \cr
 *         0 & _A &  0 \cr
 *         0 &  0 & _A}
 *   \f]
 *
 *     \note this constructor is explicit. Or many warnings will appear with
 *   GCC due to ISO C++ conversion rules.
 *
 **/

inline Matrix33::Matrix33(double _A)
{
   a(0,0) = _A;
   a(0,1) =  0.0;
   a(0,2) =  0.0;

   a(1,0) =  0.0;
   a(1,1) = _A;
   a(1,2) =  0.0;

   a(2,0) =  0.0;
   a(2,1) =  0.0;
   a(2,2)   = _A;
}




inline Matrix33::Matrix33(
         double _a00, double _a01, double _a02,
         double _a10, double _a11, double _a12,
         double _a20, double _a21, double _a22)
{
    a(0,0) = _a00;  a(0,1) = _a01;   a(0,2) = _a02;
    a(1,0) = _a10;  a(1,1) = _a11;   a(1,2) = _a12;
    a(2,0) = _a20;  a(2,1) = _a21;   a(2,2) = _a22;
}

inline void Matrix33::fillWithArgs(
         double _a00, double _a01, double _a02,
         double _a10, double _a11, double _a12,
         double _a20, double _a21, double _a22
     )
{
    a(0,0) = _a00;  a(0,1) = _a01;   a(0,2) = _a02;
    a(1,0) = _a10;  a(1,1) = _a11;   a(1,2) = _a12;
    a(2,0) = _a20;  a(2,1) = _a21;   a(2,2) = _a22;
}

/* *************************************************************************** */
/*  Matrix                                                                     */
/* *************************************************************************** */


/**
 *  Vector by Matrix left multiplication \f$M \overrightarrow{v}\f$
 *
 *  \note ACCURATE version is slower with debug on
 *
 *  \param matrix
 *      input matrix
 *  \param V
 *      input V
 *
 **/

inline Vector3dd operator * (const Matrix33 &matrix, const Vector3dd &V)
{
  double a,b,c;
  a = matrix.a(0,0) * V.x() +  matrix.a(0,1) * V.y() + matrix.a(0,2) * V.z();
  b = matrix.a(1,0) * V.x() +  matrix.a(1,1) * V.y() + matrix.a(1,2) * V.z();
  c = matrix.a(2,0) * V.x() +  matrix.a(2,1) * V.y() + matrix.a(2,2) * V.z();
  return Vector3dd(a,b,c);
}

/**
 *  Vector by Matrix right multiplication \f$\overrightarrow{v} M\f$
 *
 *  \note ACCURATE version is slower with debug on
 *
 *  \param V
 *      input vector
 *
 *  \param matrix
 *      input matrix
 *
 *
 **/

inline Vector3dd operator * (const Vector3dd &V,const Matrix33 &matrix)
{
  double a,b,c;
  a = matrix.a(0,0) * V.x() + matrix.a(1,0) * V.y() + matrix.a(2,0) * V.z();
  b = matrix.a(0,1) * V.x() + matrix.a(1,1) * V.y() + matrix.a(2,1) * V.z();
  c = matrix.a(0,2) * V.x() + matrix.a(1,2) * V.y() + matrix.a(2,2) * V.z();
  return Vector3dd(a,b,c);
}


/**
 *  We expect that  * 1.0 will be optimized out
 **/
inline Vector3dd Matrix33::mulBy2dRight(const Vector2dd &V) const
{
    return (*this) * Vector3dd(V.x(), V.y(), 1.0);
}

inline Vector3dd Matrix33::mulBy2dLeft(const Vector2dd &V) const
{
    return Vector3dd(V.x(), V.y(), 1.0) * (*this);
}


/**
 *  Apply projective transformation to 2D vector
 *     \f{eqnarray*}
 *
 *        Y &=& M \pmatrix{
 *        V_x \cr
 *        V_y \cr
 *        1 } \\
 *        V_{out} &=& ({\frac {Y_x} {Y_z}}, {\frac {Y_y} {Y_z}})
 *     \f}
 *
 *
 *  \param matrix
 *      input matrix
 *  \param V
 *      input V
 *
 *  \return \f$V_{out}\f$
 **/
inline Vector2dd operator * (const Matrix33 &matrix, const Vector2dd &V)
{
    return matrix.mulBy2dRight(V).project();
}

/**
 *  Apply projective transformation to 2D vector (vector is form the right)
 *     \f{eqnarray*}
 *        Y &=& (V_x, V_y, 1)  M \\
 *        V_{out} &=& ({\frac {Y_x} {Y_z}}, {\frac {Y_y} {Y_z}})
 *     \f}
 *
 *
 *  \param matrix
 *      input matrix
 *  \param V
 *      input V
 *
 *  \return \f$V_{out}\f$
 **/
inline Vector2dd operator * (const Vector2dd &V,const Matrix33 &matrix)
{
    return matrix.mulBy2dLeft(V).project();
}

} //namespace corecvs

#endif // _MATRIX33_H_

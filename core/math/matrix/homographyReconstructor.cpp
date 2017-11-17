/**
 * \file homographyReconstructor.cpp
 * \brief Add Comment Here
 *
 * \date Jun 26, 2011
 * \author alexander
 */

#include <limits>

#include "core/math/matrix/homographyReconstructor.h"
#include "core/math/matrix/matrix.h"
#include "core/geometry/line.h"
#include "../vector/vector.h"
#include "../../kalman/classicKalman.h"
#include "../levenmarq.h"

namespace corecvs {

HomographyReconstructor::HomographyReconstructor()
{
}

void HomographyReconstructor::addPoint2PointConstraint(const Vector2dd &from, const Vector2dd &to)
{
    p2p.push_back(Correspondence(from,to));
}

void HomographyReconstructor::addPoint2PointConstraint(const Correspondence &correspondence)
{
    p2p.push_back(correspondence);
}

void HomographyReconstructor::addPoint2LineConstraint(const Vector2dd &from, const Line2d &line)
{
    p2l.push_back(CorrespondencePointLine(from, line));
}

void HomographyReconstructor::addPoint2SegmentConstraint(const Vector2dd &from, const Segment2d &seg)
{
    p2s.push_back(CorrespondencePointSegment(from, seg));
}

bool HomographyReconstructor::hasEnoughtConstraints()
{
    size_t constraintSize  = p2l.size() + p2s.size() + p2p.size() * 2;
    return (constraintSize >= 8);
}

void HomographyReconstructor::reset(void)
{
   p2p.clear();
   p2l.clear();
   p2s.clear();
}

ostream & operator <<(ostream &out, const HomographyReconstructor &reconstructor)
{
    streamsize wasPrecision = out.precision(6);
    out << "// Points2Points " << endl;
    out << reconstructor.p2p.size() << endl;
    for (unsigned i = 0; i < reconstructor.p2p.size(); i++)
    {
        out << reconstructor.p2p[i].start << " " << reconstructor.p2p[i].end  << endl;
    }
    out << "// Points2Line " << endl;
    out << reconstructor.p2l.size() << endl;
    for (unsigned i = 0; i < reconstructor.p2l.size(); i++)
    {
        out << reconstructor.p2l[i].start << " " << reconstructor.p2l[i].end  << endl;
    }
    out.precision(wasPrecision);
    return out;
}


HomographyReconstructor::~HomographyReconstructor()
{
    // TODO Auto-generated destructor stub
}


/**
 *   TODO: This function doubles the code form rectificator.cpp
 **/
// FIXME
void HomographyReconstructor::normalisePoints(Matrix33 &transformLeft, Matrix33 &transformRight)
{
    Vector2dd lmean(0.0);
    Vector2dd lmeansq(0.0);
    Vector2dd rmean(0.0);
    Vector2dd rmeansq(0.0);

    int lcount = 0;
    int rcount = 0;

    for (unsigned  i = 0; i < p2p.size(); i++)
    {
        lmean += p2p[i].start;
        lmeansq += (p2p[i].start) * (p2p[i].start);

        rmean += p2p[i].end;
        rmeansq += (p2p[i].end) * (p2p[i].end);

        lcount++;
        rcount++;
    }

    for (unsigned  i = 0; i < p2l.size(); i++)
    {
        lmean += p2l[i].start;
        lmeansq += (p2l[i].start) * (p2l[i].start);
        lcount++;
    }

    for (unsigned  i = 0; i < p2s.size(); i++)
    {
        lmean += p2s[i].start;
        lmeansq += (p2s[i].start) * (p2s[i].start);
        lcount++;

        Vector2dd segCenter = p2s[i].end.getCenter();
        rmean += segCenter;
        rmeansq += segCenter * segCenter;
        rcount++;
    }

    lmean /= lcount;
    lmeansq  /= lcount;
    rmean  /= rcount;
    rmeansq  /= rcount;

    Vector2dd lsqmean = lmean * lmean;
    Vector2dd rsqmean = rmean * rmean;

    double lscaler = sqrt (lmeansq.x() - lsqmean.x()  + lmeansq.y() - lsqmean.y());
    double rscaler = sqrt (rmeansq.x() - rsqmean.x()  + rmeansq.y() - rsqmean.y());

    transformLeft     = Matrix33::ScaleProj(sqrt(2.0) / lscaler) * Matrix33::ShiftProj(-lmean);
    transformRight    = Matrix33::ScaleProj(sqrt(2.0) / rscaler) * Matrix33::ShiftProj(-rmean);
    //Matrix33 transformRightInv = Matrix33::ShiftProj(rmean) * Matrix33::ScaleProj(rscaler / sqrt(2.0));


    for (unsigned  i = 0; i < p2p.size(); i++)
    {
       p2p[i].start = transformLeft  * p2p[i].start;
       p2p[i].end   = transformRight * p2p[i].end;
    }

    for (unsigned  i = 0; i < p2l.size(); i++)
    {
       p2l[i].start = transformLeft  * p2l[i].start;
       Vector3dd line = p2l[i].end;
       line = line * transformRight.inv();
       p2l[i].end = Line2d(line[0], line[1], line[2]);
       p2l[i].end.normalise();
    }

    for (unsigned  i = 0; i < p2s.size(); i++)
    {
        p2s[i].start = transformLeft  * p2s[i].start;
        p2s[i].end.a = transformRight * p2s[i].end.a;
        p2s[i].end.b = transformRight * p2s[i].end.b;
    }
}



/**
 *  This block is devoted to LSE optimization
 *
 **/

/**
 *  Calculate the Matrix that transform given points \f$(x,y)\f$ to given images \f$(u,v)\f$
 *  This is done almost the same way as in rectification Rectificator::getFundamantalRansac() using the SVD
 *
 *  Matrix is searched in form
 *
 * \f[ P=\pmatrix{
 *        a_{00} & a_{01} & a_{02}\cr
 *        a_{10} & a_{11} & a_{12}\cr
 *        a_{20} & a_{21} &    1
 *        }
 * \f]
 *
 * \f[ u = \frac {a_{00} x + a_{01} y + a_{02}} {a_{20} x + a_{21} y + 1} \f]
 *
 * \f[ v = \frac {a_{10} x + a_{11} y + a_{12}} {a_{20} x + a_{21} y + 1} \f]
 *
 * \f[
 *
 *  \pmatrix{
 *   x & y & 1 & 0 & 0 & 0 & -x u & -y u \cr
 *   0 & 0 & 0 & x & y & 1 & -x v & -y v
 *   }
 *
 *  \pmatrix{ a_{00} \cr a_{01} \cr a_{02} \cr
 *            a_{10} \cr a_{11} \cr a_{12} \cr
 *            a_{20} \cr a_{21}   } =
 *
 *  \pmatrix{ u \cr v }
 *
 * \f]
 *
 */
void HomographyReconstructor::addPoint2PointConstraintLSE(
        Matrix &A,
        Matrix &B,
        int num,
        const Vector2dd &from, const Vector2dd &to)
{
   double fx = from.x();
   double fy = from.y();
   double tx =   to.x();
   double ty =   to.y();

   A.fillLineWithArgs(num    ,  fx,  fy, 1.0, 0.0, 0.0, 0.0, -fx * tx, -fy * tx);
   B.fillLineWithArgs(num    ,  tx);

   A.fillLineWithArgs(num + 1, 0.0, 0.0, 0.0,  fx,  fy, 1.0, -fx * ty, -fy * ty);
   B.fillLineWithArgs(num + 1,  ty);
}

void HomographyReconstructor::addPoint2PointConstraintLSEUnif(
        Matrix &A,
        int num,
        const Vector2dd &from, const Vector2dd &to)
{
   double fx = from.x();
   double fy = from.y();
   double tx =   to.x();
   double ty =   to.y();

   A.fillLineWithArgs(num    ,  fx,  fy, 1.0, 0.0, 0.0, 0.0, -fx * tx, -fy * tx, -tx);
   A.fillLineWithArgs(num + 1, 0.0, 0.0, 0.0,  fx,  fy, 1.0, -fx * ty, -fy * ty, -ty);
}


/**
 *  Matrix is searched in form
 *
 * \f[ P=\pmatrix{
 *        a_{00} & a_{01} & a_{02}\cr
 *        a_{10} & a_{11} & a_{12}\cr
 *        a_{20} & a_{21} &    1
 *        }
 * \f]
 *
 * We have a point \f$(x,y)\f$ and a line \f$ A u + B v + C = 0 \f$
 *
 * The matrix will transform the point \f$(x,y)\f$ to the \f$(u,v)\f$
 *
 * \f[ u = \frac {a_{00} x + a_{01} y + a_{02}} {a_{20} x + a_{21} y + 1} \f]
 *
 * \f[ v = \frac {a_{10} x + a_{11} y + a_{12}} {a_{20} x + a_{21} y + 1} \f]
 *
 * So we have minimize
 *
 * \f[ A u + B v + C = 0 \f]
 * \f[
 *  A \frac {a_{00} x + a_{01} y + a_{02}} {a_{20} x + a_{21} y + 1} +
 *  B \frac {a_{10} x + a_{11} y + a_{12}} {a_{20} x + a_{21} y + 1} + C = 0
 * \f]
 *
 * We will rewrite it as:
 *
 * \f[
 *  A ({a_{00} x + a_{01} y + a_{02}})  +
 *  B ({a_{10} x + a_{11} y + a_{12}})  +
 *  C ({a_{20} x + a_{21} y + 1}) = 0
 * \f]
 *
 *
 * \f[
 *  \pmatrix{
 *   Ax & Ay & A & Bx & By & B & C x & C y }
 *
 *  \pmatrix{ a_{00} \cr a_{01} \cr a_{02} \cr
 *            a_{10} \cr a_{11} \cr a_{12} \cr
 *            a_{20} \cr a_{21}   } =
 *
 *  \pmatrix{ -C }
 *
 * \f]
 *
 */

void HomographyReconstructor::addPoint2LineConstraintLSE(
        Matrix &A,
        Matrix &B,
        int num,
        const Vector2dd &from, const Line2d &line)
{
    Line2d l = line.normalised();

    double lx =  l.x();
    double ly =  l.y();
    double lz =  l.z();
    double fx = from.x();
    double fy = from.y();

    A.fillLineWithArgs(num,
            lx * fx, lx * fy, lx,
            ly * fx, ly * fy, ly,
            lz * fx, lz * fy);
    B.fillLineWithArgs(num, -lz);
}

void HomographyReconstructor::addPoint2LineConstraintLSEUnif(
        Matrix &A,
        int num,
        const Vector2dd &from, const Line2d &line)
{
    Line2d l = line.normalised();

    double lx =  l.x();
    double ly =  l.y();
    double lz =  l.z();
    double fx = from.x();
    double fy = from.y();

    A.fillLineWithArgs(num,
            lx * fx, lx * fy, lx,
            ly * fx, ly * fy, ly,
            lz * fx, lz * fy, lz);
}

/**
 *  Actual computation
 **/
Matrix33 HomographyReconstructor::getBestHomographyLSE1( void )
{
    int problemSize = (int)(p2l.size() + p2p.size() * 2);

    Matrix mX(problemSize, 8);
    Matrix mB(problemSize, 1);

    int lineNum = 0;
    for (unsigned i = 0; i < p2p.size(); i++)
    {
        addPoint2PointConstraintLSE(mX, mB, lineNum, p2p[i].start, p2p[i].end);
        lineNum += 2;
    }
    for (unsigned i = 0; i < p2l.size(); i++)
    {
        addPoint2LineConstraintLSE(mX, mB, lineNum, p2l[i].start, p2l[i].end);
        lineNum += 1;
    }

//    cout << "X" << endl << mX << endl;
//    cout << "B" << endl << mB << endl;

    Matrix mXTX = mX.t() * mX;
    Matrix mXTB = mX.t() * mB;

    Matrix result = mXTX.invSVD() * mXTB;

    Matrix test = mX * result - mB;
//    cout << "Residual:" << test.frobeniusNorm() << endl;

    Matrix33 toReturn(
        result.a(0,0), result.a(1,0), result.a(2,0),
        result.a(3,0), result.a(4,0), result.a(5,0),
        result.a(6,0), result.a(7,0), 1.0
    );
    return toReturn;
}

Matrix33 HomographyReconstructor::getBestHomographyLSE( void )
{
    int problemSize = (int)(p2l.size() + p2p.size() * 2);

    Matrix mX(problemSize ,8);
    Matrix mB(problemSize ,1);

    int lineNum = 0;
    for (unsigned i = 0; i < p2p.size(); i++)
    {
        addPoint2PointConstraintLSE(mX, mB, lineNum, p2p[i].start, p2p[i].end);
        lineNum += 2;
    }
    for (unsigned i = 0; i < p2l.size(); i++)
    {
        addPoint2LineConstraintLSE(mX, mB, lineNum, p2l[i].start, p2l[i].end);
        lineNum += 1;
    }

 //   cout << "X" << endl << mX << endl;
//    cout << "B" << endl << mB << endl;


    Matrix U(mX);
    Matrix W(1, 8);
    Matrix VT(8, 8);
    Matrix::svd (&U, &W, &VT);

    Matrix UT = U.t();
    Matrix UtB = UT * mB;
    Matrix UtB8(8,1);

    for (int i = 0; i < 8; i++)
    {
       // if (W.a(0,i) > 1e-6)
            UtB8.a(i,0) = UtB.a(i,0) / W.a(0,i);
       // else
       //     UtB8.a(i,0) = 0.0;
    }
    Matrix result = VT * UtB8;

    Matrix test = mX * result - mB;
//    cout << "Residual:" << test.frobeniusNorm() << endl;

    Matrix33 toReturn(
        result.a(0,0), result.a(1,0), result.a(2,0),
        result.a(3,0), result.a(4,0), result.a(5,0),
        result.a(6,0), result.a(7,0), 1.0
    );
    return toReturn;
}


Matrix33 HomographyReconstructor::getBestHomographyLSE2( void )
{
    int problemSize = (int)(p2l.size() + p2p.size() * 2);

    Matrix mX(problemSize ,9);

    int lineNum = 0;
    for (unsigned i = 0; i < p2p.size(); i++)
    {
        addPoint2PointConstraintLSEUnif(mX, lineNum, p2p[i].start, p2p[i].end);
        lineNum += 2;
    }
    for (unsigned i = 0; i < p2l.size(); i++)
    {
        addPoint2LineConstraintLSEUnif(mX, lineNum, p2l[i].start, p2l[i].end);
        lineNum += 1;
    }

//    cout << "X" << endl << mX << endl;

    Matrix U(mX);
    Matrix W(1, 9);
    Matrix VT(9, 9);
    Matrix::svd(&U, &W, &VT);

 //   cout << "Singualar Values" << endl << W << endl;

    int minId = -1;
    double minval = std::numeric_limits<double>::max();
    for (unsigned i = 0; i < 9; i ++)
    {
       if (W.a(0, i) <= minval)
       {
           minId = i;
           minval = W.a(0, i);
       }
    }

    Matrix33 toReturn(
        VT.a(0,minId), VT.a(1,minId), VT.a(2,minId),
        VT.a(3,minId), VT.a(4,minId), VT.a(5,minId),
        VT.a(6,minId), VT.a(7,minId), VT.a(8,minId)
    );
    toReturn /= VT.a(8,minId);
    Matrix result(9, 1, toReturn.element);
    Matrix test = mX * result;

//    cout << "Result" << endl << test << endl;
//    cout << "Residual:" << test.frobeniusNorm() << endl;
    return toReturn;
}




/**
 *   This function computes the classical Euklidian cost.
 *
 *
 **/
double HomographyReconstructor::getCostFunction(Matrix33 &H, double out[])
{
    int argout = 0;
    double cost = 0.0;
    for (unsigned i = 0; i < p2p.size(); i++)
    {
        Vector2dd point = (H * p2p[i].start);
        Vector2dd diff = point - p2p[i].end;
        if (out)
        {
            out[argout++] = diff.x();
            out[argout++] = diff.y();
        }
        cost += (point - p2p[i].end).sumAllElementsSq();
    }

    for (unsigned i = 0; i < p2l.size(); i++)
    {
        Vector2dd point = H * p2l[i].start;
        double distanceSq = p2l[i].end.sqDistanceTo(point);
        if (out)
            out[argout++] = std::sqrt(distanceSq);
        cost += distanceSq;
    }
    return cost;
}

Matrix HomographyReconstructor::getJacobian(const Matrix33 &H) const
{
    CORE_ASSERT_TRUE(((uint)(p2p.size() * 2 + p2l.size())) == (p2p.size() * 2 + p2l.size()), ("too much elements int HomographyReconstructor"));

    Matrix J((uint)(p2p.size() * 2 + p2l.size()), 8);
    int argout = 0;
    for (auto& pp: p2p)
    {
        Vector3dd ppp = H * Vector3dd(pp.start[0], pp.start[1], 1.0);
        auto u = ppp[0] / ppp[2], v = ppp[1] / ppp[2], w = ppp[2],
             x = pp.start[0],     y = pp.start[1];
        J.a(argout, 0) = x / w;      J.a(argout, 1) = y / w;      J.a(argout, 2) = 1.0 / w;
        J.a(argout, 3) = 0.0;        J.a(argout, 4) = 0.0;        J.a(argout, 5) = 0.0;
        J.a(argout, 6) = -x * u / w; J.a(argout, 7) = -y * u / w;
        ++argout;
        J.a(argout, 0) = 0.0;        J.a(argout, 1) = 0.0;        J.a(argout, 2) = 0.0;
        J.a(argout, 3) = x / w;      J.a(argout, 4) = y / w;      J.a(argout, 5) = 1.0 / w;
        J.a(argout, 6) = -x * v / w; J.a(argout, 7) = -y * v / w;
        ++argout;
    }
    for (auto& pp: p2l)
    {
        Vector3dd ppp = H * Vector3dd(pp.start[0], pp.start[1], 1.0);
        auto u = ppp[0] / ppp[2], v = ppp[1] / ppp[2], w = ppp[2],
             x = pp.start[0],     y = pp.start[1];
        auto nx = pp.end.normal()[0], ny = pp.end.normal()[1];
        auto n = std::sqrt(nx * nx + ny * ny);
        auto c = nx / n, s = ny / n;

        J.a(argout, 0) = x * c / w; J.a(argout, 1) = y * c / w; J.a(argout, 2) = 1.0 / (w * n);
        J.a(argout, 3) = x * s / w; J.a(argout, 4) = y * s / w; J.a(argout, 5) = 1.0 / (w * n);
        J.a(argout, 6) = -c * x * u / w - s * x * v / w;
        J.a(argout, 7) = -c * y * u / w - s * y * v / w;
        ++argout;
    }
    CORE_ASSERT_TRUE_S(J.h == argout);
    return J;
}

int HomographyReconstructor::getConstraintNumber()
{
    return (int)p2l.size() + (int)p2s.size() + (int)p2p.size() * 2;
}

void HomographyReconstructor::CostFunction::operator()(const double in[], double out[])
{
    Matrix33 H(in[0], in[1], in[2],
               in[3], in[4], in[5],
               in[6], in[7], 1.0);
    reconstructor->getCostFunction(H, out);
}

Matrix HomographyReconstructor::CostFunction::getJacobian(const double in[], double /*dlta*/)
{
    auto J1 = reconstructor->getJacobian(
       Matrix33(in[0], in[1], in[2],
                in[3], in[4], in[5],
                in[6], in[7], 1.0));
    return J1;
}


/*
void HomographyReconstructor::CostFunctionBack::operator()(const double in[], double out[])
{
    Matrix33 H(in[0], in[1], in[2],
               in[3], in[4], in[5],
               in[6], in[7], 1.0);
    out[0] = reconstructor->getCostFunctionBack(H);
}

void HomographyReconstructor::CostFunctionTwoWay::operator()(const double in[], double out[])
{
    Matrix33 H(in[0], in[1], in[2],
               in[3], in[4], in[5],
               in[6], in[7], 1.0);
    out[0] = reconstructor->getCostFunctionTwoWay(H);
}
*/

void HomographyReconstructor::CostFunctionWize::operator()(const double in[], double out[])
{
    Matrix33 H = matrixFromState(in);
    reconstructor->getCostFunction(H, out);
}

Matrix33 HomographyReconstructor::CostFunctionWize::matrixFromState(const double in[])
{
    Matrix33 R = Matrix33::RotationZ(in[0]);
    Matrix33 T = Matrix33::ShiftProj(in[1], in[2]);
    Matrix33 S = Matrix33::Scale2(in[3]);
    Matrix33 P = Matrix33::Projective(in[4], in[5]);
    return S * P * T * R;
}


#ifdef DEPRECATED
/**
 *  This block is devoted to Kalman reconstruction
 **/

Matrix33 HomographyReconstructor::getBestHomographyClassicKalman()
{
    CostFunction F(this);
    IdentityFunction H(1);

    // We know almost nothing about the homography matrix
    Matrix P(8,8, 1000.0);
    // Our model evolution is precise
    Matrix Q(8,8, 0.0);

    // Our measurement is precise up to 2px.
    Matrix R(1,1, 2.0 * 8.0);

    Matrix33 ident(1.0);
    Vector x(8, ident.element);

    ClassicKalman kalmanFilter(
        &F,
        &H,
        Q,
        R,
        P,
        x);

    for (int step = 0; step < 250; step++)
    {
        double measurement = 0.0;
        kalmanFilter.predict();
        kalmanFilter.z[0] = measurement;
        kalmanFilter.update();
    }

    return Matrix33(
            kalmanFilter.x[0], kalmanFilter.x[1], kalmanFilter.x[2],
            kalmanFilter.x[3], kalmanFilter.x[4], kalmanFilter.x[5],
            kalmanFilter.x[6], kalmanFilter.x[7], 1.0
    );

}
#endif

/**
 *  This block is devoted to LM reconstruction
 **/
// FIXME: implement full cost-function for LM
Matrix33 HomographyReconstructor::getBestHomographyLM(Matrix33 guess)
{
    CostFunction F(this);
    LevenbergMarquardt LMfit;

    LMfit.f = &F;
    LMfit.maxIterations = 250;
    LMfit.trace = trace;
    LMfit.traceProgress = trace;

    vector<double> input(8);
    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            if(i * 3 + j == 8)
                break;
            input[i * 3 + j] = guess.a(i, j) / guess.a(2, 2);
        }
    }

    vector<double> output(getConstraintNumber());

    vector<double> optInput = LMfit.fit(input, output);

    return Matrix33(
            optInput[0], optInput[1], optInput[2],
            optInput[3], optInput[4], optInput[5],
            optInput[6], optInput[7], 1.0
            );
}

Matrix33 HomographyReconstructor::getBestHomographyLSEandLM()
{
    Matrix33 guess = getBestHomographyLSE2();
    return getBestHomographyLM(guess);
}



Matrix33 HomographyReconstructor::getBestHomography(const HomographyAlgorithm::HomographyAlgorithm &method)
{
    switch (method) {
        case HomographyAlgorithm::LSE:  return getBestHomographyLSE(); break;
        case HomographyAlgorithm::LSE1: return getBestHomographyLSE1(); break;
        case HomographyAlgorithm::LSE2: return getBestHomographyLSE2(); break;
        case HomographyAlgorithm::ML:   return getBestHomographyLM(); break;
        default:
        case HomographyAlgorithm::ML_AFTER_LSE: return getBestHomographyLSEandLM(); break;
    }
    return Matrix33::Identity();
}


} //namespace corecvs


#include "similarityReconstructor.h"
#include "levenmarq.h"

namespace corecvs {

SimilarityReconstructor::SimilarityReconstructor() :
    trace(false)
{
}

void SimilarityReconstructor::addPoint2PointConstraint(const Vector3dd &from, const Vector3dd &to)
{
    p2p.push_back(Correspondance3D(from,to));
}

void SimilarityReconstructor::reset(void)
{
    p2p.clear();
}


Matrix44 Similarity::toMatrix() const
{
    return Matrix44::Shift(shiftR) * Matrix44::Scale(scaleR) * rotation.toMatrix() * Matrix44::Scale(1.0 / scaleL) * Matrix44::Shift(-shiftL);
}

double Similarity::getScale()
{
    return scaleL / scaleR;
}

double Similarity::getInvScale()
{
    return scaleR / scaleL;
}

Vector3dd Similarity::getShift()
{
    Vector3dd shift = (shiftR / scaleR);
    shift = rotation.conjugated() * shift;
    shift = shift * scaleL;
    shift -= shiftL;

    return shift;
}

Matrix33 Similarity::getRotation()
{
    return rotation.toMatrix();
}

void Similarity::fillFunctionInput(double in[])
{
    double scale    = getScale();
    Vector3dd shift = getShift();

    in[SHIFT_X] = -shift.x();
    in[SHIFT_Y] = -shift.y();
    in[SHIFT_Z] = -shift.z();
    in[SCALE]   = scale;
    in[ROTATION_X] = rotation.x();
    in[ROTATION_Y] = rotation.y();
    in[ROTATION_Z] = rotation.z();
    in[ROTATION_T] = rotation.t();
}


Similarity Similarity::getInterpolated(double t)
{
    Similarity toReturn;

    using corecvs::lerp;

    toReturn.scaleL = lerp(1.0, scaleL, t);
    toReturn.scaleR = lerp(1.0, scaleR, t);

    toReturn.shiftL = lerp(Vector3dd(0.0), shiftL, t);
    toReturn.shiftR = lerp(Vector3dd(0.0), shiftR, t);

    toReturn.rotation = Quaternion::slerp(Quaternion::RotationIdentity(), rotation, t);

    return toReturn;
}



/**
 *  Move similar code from  SimilarityReconstructor and HomographyReconstructor
 *
 *  http://people.csail.mit.edu/bkph/papers/Absolute_Orientation.pdf
 *
 **/
Similarity SimilarityReconstructor::getBestSimilarity()
{
    Similarity result;
    Vector3dd lmean(0.0);
    Vector3dd lmeansq(0.0);
    Vector3dd rmean(0.0);
    Vector3dd rmeansq(0.0);

    if (trace)
        cout << "Starting optimization with " << p2p.size() << " constraints" << endl;

    for (unsigned  i = 0; i < p2p.size(); i++)
    {
        if (trace)
            cout << p2p[i].start << " ==> " << p2p[i].end << endl;

        lmean += p2p[i].start;
        lmeansq += (p2p[i].start) * (p2p[i].start);

        rmean += p2p[i].end;
        rmeansq += (p2p[i].end) * (p2p[i].end);
    }

    lmean    /= p2p.size();
    lmeansq  /= p2p.size();
    rmean    /= p2p.size();
    rmeansq  /= p2p.size();

    Vector3dd lsqmean = lmean * lmean;
    Vector3dd rsqmean = rmean * rmean;

    double lscaler = sqrt((lmeansq - lsqmean).sumAllElements());
    double rscaler = sqrt((rmeansq - rsqmean).sumAllElements());

    /**
     *  so we need to shift first set to 0
     *  scale and shift back, so first element need to be mutiplied by scale
     **/
    result.scaleL = lscaler;
    result.scaleR = rscaler;
    result.shiftL = lmean;
    result.shiftR = rmean;

    /* Now we estimate the rotation */
    Matrix33 S(0.0);
    for (unsigned  i = 0; i < p2p.size(); i++)
    {
        S += Matrix33::VectorByVector((p2p[i].start - lmean) / lscaler, (p2p[i].end - rmean) / rscaler);
    }

    enum {X = 0, Y = 1, Z = 2};

    Matrix44 N = Matrix44(
        S(X, X) + S(Y, Y) + S(Z, Z),        S(Y,Z) - S(Z,Y)       ,        S(Z,X) - S(X,Z)         ,        S(X,Y) - S(Y,X)         ,
            S(Y,Z) - S(Z,Y)          , S(X, X) - S(Y, Y) - S(Z, Z),        S(X,Y) + S(Y,X)         ,        S(Z,X) + S(X,Z)         ,
            S(Z,X) - S(X,Z)          ,        S(X,Y) + S(Y,X)       , - S(X, X) + S(Y, Y) - S(Z, Z),        S(Z,Y) + S(Y,Z)         ,
            S(X,Y) - S(Y,X)          ,        S(Z,X) + S(X,Z)       ,         S(Z,Y) + S(Y,Z)        , - S(X, X) - S(Y, Y) + S(Z, Z)
    );

    Matrix NA(N);
    DiagonalMatrix D(4);
    Matrix VT(4, 4);
    Matrix::jacobi(&NA, &D, &VT, NULL);

    double max = 0;
    int maxi = 0;
    for (int i = 0; i < 4; i++)
    {
        if (D.a(i) > max) {
            max = D.a(i);
            maxi = i;
        }
    }

    if (trace)
    {
        cout << "Cross Correlation:\n" << S;
        cout << "N Matrix         :\n" << N;
        cout << "Decomposition:\n" << NA << endl;
        cout << D << endl;
        cout << VT << endl;
        cout << "Maximum eigenvalue:" << max << " at row/column " << maxi << endl;
    }

    /* make an eigenvalue cheak */
    FixedVector<double, 4> eigen;
    eigen[0] = VT.a(0, maxi);
    eigen[1] = VT.a(1, maxi);
    eigen[2] = VT.a(2, maxi);
    eigen[3] = VT.a(3, maxi);

    Quaternion rotation(eigen[1], eigen[2], eigen[3], eigen[0]);

    if (trace)
    {
        cout << "Checking eigenvector:" <<  eigen << " " << eigen * D.a(0,maxi) << "  "  << (N * eigen) << endl;
        cout << "Quaternion:" << rotation << endl;
    }

    result.rotation = rotation;
    return result;
}

Similarity SimilarityReconstructor::getBestSimilarityLM(Similarity &firstGuess)
{
    vector<double> guess(Similarity::PARAM_NUMBER);
    firstGuess.fillFunctionInput(&guess[0]);

    CostFunction F(this);
    NormalizeFunction N;
    LevenbergMarquardt LMfit;

    LMfit.f = &F;
    LMfit.normalisation = &N;
    LMfit.maxIterations = 100;
    LMfit.traceProgress = false;

    vector<double> output(1);
    output[0] = 0.0;

    vector<double> optInput = LMfit.fit(guess, output);

    return Similarity(&optInput[0]);
}

Similarity SimilarityReconstructor::getBestSimilarityLMN(Similarity &firstGuess)
{
    vector<double> guess(Similarity::PARAM_NUMBER);
    firstGuess.fillFunctionInput(&guess[0]);

    CostFunctionToN F(this);
    NormalizeFunction N;
    LevenbergMarquardt LMfit;

    LMfit.f = &F;
    LMfit.normalisation = &N;
    LMfit.maxIterations = 1000;

    vector<double> output(F.outputs);
    memset(&output[0], 0, sizeof(double) * F.outputs);

    vector<double> optInput = LMfit.fit(guess, output);

    return Similarity(&optInput[0]);
}

double SimilarityReconstructor::getCostFunction(const Similarity &input)
{
    double diff = 0.0;
    Matrix44 trans = input.toMatrix();
    for (unsigned  i = 0; i < p2p.size(); i++)
    {
        Correspondance3D &corr = p2p[i];
        diff += (corr.end - trans * corr.start).sumAllElementsSq();
    }
    if (p2p.size() != 0) {
        return sqrt(diff / p2p.size());
    }
    return 0.0;
}

SimilarityReconstructor::~SimilarityReconstructor()
{

}


ostream &operator << (ostream &out, const Similarity &reconstructor)
{
    out << "Shift Left  by: "  << reconstructor.shiftL << endl;
    out << "Scale Left  by: "  << reconstructor.scaleL << " (" << (1 / reconstructor.scaleL) << ")"<< endl;

    out << "Shift Right by: "  << reconstructor.shiftR << endl;
    out << "Scale Right by: "  << reconstructor.scaleR << " (" << (1 / reconstructor.scaleR) << ")" << endl;

    out << "Quaternion:" << reconstructor.rotation << endl;
    double angle = reconstructor.rotation.getAngle();
    out << "Rotate by: " << angle << " (" << radToDeg(angle) << "deg) around " << reconstructor.rotation.getAxis() << endl;
    Quaternion rot = reconstructor.rotation;
    rot = -rot;
    out << "Rotate by: " << rot.getAngle() << " around " << rot.getAxis() << endl;
    return  out;
}

void SimilarityReconstructor::CostFunction::operator()(const double in[], double out[])
{
    out[0] = reconstructor->getCostFunction(Similarity(in));
}

void SimilarityReconstructor::NormalizeFunction::operator()(const double in[], double out[])
{
    Similarity s(in);
    s.rotation.normalise();
    s.fillFunctionInput(out);
}

void SimilarityReconstructor::CostFunctionToN::operator()(const double in[], double out[])
{
    Similarity input(in);
    Matrix44 trans = input.toMatrix();
    for (unsigned i = 0; i < reconstructor->p2p.size(); i++)
    {
        Correspondance3D &corr = reconstructor->p2p[i];
        out[i] = sqrt((corr.end - trans * corr.start).sumAllElementsSq());
    }
}



};

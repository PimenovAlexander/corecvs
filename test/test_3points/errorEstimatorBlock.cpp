#include "errorEstimatorBlock.h"

#include "core/geometry/mesh/mesh3d.h"

#include "core/camerafixture/cameraFixture.h"
#include "core/camerafixture/fixtureCamera.h"
#include "core/camerafixture/fixtureScene.h"

#include "core/geometry/ellipticalApproximation.h"
#include "core/math/matrix/similarityReconstructor.h"

#include "core/buffers/rgb24/abstractPainter.h"


#if 0
#ifdef WITH_JSONMODERN
#   include "jsonModernReader.h"
    typedef JSONModernReader JSONReader;
#elif defined(WITH_RAPIDJSON)
#   include "rapidJSONReader.h"
    typedef RapidJSONReader  JSONReader;
#else
#   include "jsonGetter.h"   // it depends on Qt!
    typedef JSONGetter       JSONReader;
#endif
#endif

#include <wrappers/jsonmodern/jsonModernReader.h>
typedef JSONModernReader JSONReader;

ErrorEstimatorBlock::ErrorEstimatorBlock()
{

}


FixtureScene *loadTestScene(const std::string &name)
{
    FixtureScene *result = new FixtureScene;

    JSONReader getter(name);
    if (!getter.hasError()) {
        getter.visit(*result, "scene");
    }
    else {
       SYNC_PRINT(("Unable to parse json %s\n", name.c_str()));
    }

    return result;
}


class AccumulatorBuffer : public AbstractBuffer<Vector3dd>
{
public:
    AccumulatorBuffer(int32_t h, int32_t w,   bool shouldInit = true) : AbstractBuffer<Vector3dd> (h, w, shouldInit) {}
    AccumulatorBuffer(Vector2d<int32_t> size, bool shouldInit = true) : AbstractBuffer<Vector3dd> (size, shouldInit) {}

    void inc(const Vector2dd &pos, Vector3dd val = Vector3dd(1.0))
    {
        Vector2d<int> posi(fround(pos.x()), fround(pos.y()));
        if (isValidCoord(posi))
            element(posi) += val;
    }

    void dec(const Vector2dd &pos, Vector3dd val = Vector3dd(1.0))
    {
        Vector2d<int> posi(fround(pos.x()), fround(pos.y()));
        if (isValidCoord(posi))
            element(posi) -= val;
    }

    virtual ~AccumulatorBuffer() {}
};


vector<Vector3dd> ErrorEstimatorBlock::targetScene(const Error3PointParameters &eParams)
{
    vector<Vector3dd> result;
    switch (eParams.targetType()) {
    case TargetType3Point::SINGLE_POINT:
        result.push_back(Vector3dd(0, 0, eParams.distance()));
        break;
    case TargetType3Point::POIS_FILES:

        break;
    case TargetType3Point::WALL:
        {
            for (int i = -eParams.wallBricks() / 2 ; i < eParams.wallBricks() / 2; i++)
                for (int j = -eParams.wallBricks() / 2 ; j < eParams.wallBricks() / 2; j++)
                {
                    result.push_back(Vector3dd(i, j, 0) * eParams.wallBrickStep());
                }

            Affine3DQ wallMove(Quaternion::RotationY(degToRad(eParams.wallAngleDeg())), Vector3dd(0, 0, eParams.distance()) );
            /* Rotate it and move */
            for (Vector3dd &point : result)
            {
                point = wallMove * point;
            }
        }
        break;
    default:
        break;
    }

    return result;
}

int ErrorEstimatorBlock::operator ()()
{
    SYNC_PRINT(("ErrorEstimatorBlock::operator ()():called\n"));

    std::cout << *this << std::endl;    

    Error3PointParameters eParams = parameters();

    /**/    
#if 0
    FixtureScene *testScene = inScene();
    if(testScene == NULL) {
        testScene = loadTestScene(eParams.sceneFileName());
    }
#endif
    CameraModel modelOrg(PinholeCameraIntrinsics(Vector2dd(4000,3000), degToRad(60)));
    //modelOrg.copyModelFrom(*testScene->fixtures()[0]->cameras[0]);

    modelOrg.setLocation(Affine3DQ::Identity());

    CameraModel model1;
    CameraModel model2;
    model1.copyModelFrom(modelOrg);
    model2.copyModelFrom(modelOrg);

    //cout << model1 << endl;

    model1.setLocation(Affine3DQ(Vector3dd::Zero()));
    model2.setLocation(Affine3DQ(Vector3dd::OrtX() * eParams.baseline()));

   // Vector3dd target = Vector3dd::OrtZ() * eParams.distance();

    Mesh3D mesh;
    mesh.switchColor(true);
    AccumulatorBuffer image(eParams.imageSize(),eParams.imageSize());
    AbstractPainter<AccumulatorBuffer> painterAccum(&image);
    Matrix44 transform = Matrix44::Scale(eParams.imageScale()) * Matrix44::Shift( eParams.imageShift().x(), eParams.imageShift().y(), eParams.imageShift().z());

    EllipticalApproximation3d approx;

    std::mt19937 rng;
    rng.seed(eParams.randomSeed());

    vector<Vector3dd> res;
    /* Simulating 3 point */

    Vector3dd addPoint = Vector3dd::OrtZ() * eParams.point3Distance();

    Similarity similarity;
    Ray3d r1;
    Ray3d r2;
    Vector3dd inter;

    vector<Vector3dd> points = targetScene(eParams);

    for (Vector3dd &target : points)
    {
        for (int i = 0; i < eParams.samplingIterations(); i++)    {

            std::normal_distribution<> dx(0, eParams.stationErrorSigma().x());
            std::normal_distribution<> dy(0, eParams.stationErrorSigma().y());
            std::normal_distribution<> dz(0, eParams.stationErrorSigma().z());

            Vector3dd p1error = Vector3dd(dx(rng), dy(rng), dz(rng));
            Vector3dd p2error = Vector3dd(dx(rng), dy(rng), dz(rng));
            std::normal_distribution<> da(0, degToRad(eParams.tiltErrorDeg()));

            Quaternion tilt1 = Quaternion::Rotation(Vector3dd::OrtX(), da(rng));
            Quaternion tilt2 = Quaternion::Rotation(Vector3dd::OrtX(), da(rng));


            if (eParams.use3rdPoint())
            {
                Vector3dd addPointAs;
                {  /* 3rd point */
                    std::normal_distribution<> dpixu(0, eParams.observationError().x());
                    std::normal_distribution<> dpixv(0, eParams.observationError().y());
                    Vector2dd err1(dpixu(rng), dpixv(rng));

                    Ray3d r1 = model1.rayFromPixel(model1.project(addPoint) + err1).normalised();

                    Vector2dd err2(dpixu(rng), dpixv(rng));
                    Ray3d r2 = model2.rayFromPixel(model2.project(addPoint) + err2).normalised();

                    r1.p += p1error;
                    r2.p += p2error;

                    r1.a = tilt1 * r1.a;
                    r2.a = tilt2 * r2.a;

                    addPointAs = r1.intersect(r2);
                }
                SimilarityReconstructor reconstructor;
                reconstructor.addPoint2PointConstraint(addPointAs, addPoint);
                reconstructor.addPoint2PointConstraint(model1.extrinsics.position + p1error, model1.extrinsics.position + p1error);
                reconstructor.addPoint2PointConstraint(model2.extrinsics.position + p2error, model2.extrinsics.position + p2error);

                similarity = reconstructor.getBestSimilarity();
            }

            /*Real point computation*/
            {
                std::normal_distribution<> dpixu(0, eParams.observationError().x());
                std::normal_distribution<> dpixv(0, eParams.observationError().y());

                Vector2dd err1(dpixu(rng), dpixv(rng));
                Vector2dd orig1 = model1.project(target);
                r1 = model1.rayFromPixel(orig1 + err1).normalised();

                Vector2dd err2(dpixu(rng), dpixv(rng));
                Vector2dd orig2 = model2.project(target);
                r2 = model2.rayFromPixel(orig2 + err2).normalised();


                r1.p += p1error;
                r2.p += p2error;

                r1.a = tilt1 * r1.a;
                r2.a = tilt2 * r2.a;

                inter = r1.intersect(r2);
                inter = similarity.transform(inter);
                approx.addPoint(inter);
                res.push_back(inter);

                /* Draw data */
                image.inc((transform *  r1.p).xz(), Vector3dd::OrtX());
                image.inc((transform *  r2.p).xz(), Vector3dd::OrtY());
                image.inc((transform * inter).xz(), Vector3dd::OrtZ());

                mesh.setColor(RGBColor::Red());
                mesh.addPoint(r1.p);
                mesh.setColor(RGBColor::Green());
                mesh.addPoint(r2.p);
                mesh.setColor(RGBColor::Cyan());
                mesh.addPoint(inter);
            }
        }
    }

    cout << "Camera Model " << modelOrg << endl;
    //cout << "Target " << target << endl;

    cout << "Target at " << eParams.distance() << " 3rd point " << eParams.point3Distance()  << "m baseline is " << eParams.baseline() << "m " << endl;
    cout << "Tilt is set to " << eParams.tiltErrorDeg() << endl;

    cout << "\tApproximation " << approx.getMean() << endl;
    cout << "\tDeviation " << approx.getRadius() << " m" << endl;
    approx.getEllipseParameters();

    cout << "\tX (baseline) " << approx.getRadiusForDim(0) << " m " << endl;
    cout << "\tY (height)   " << approx.getRadiusForDim(1) << " m " << endl;
    cout << "\tZ (distance) " << approx.getRadiusForDim(2) << " m " << endl;

    //cout << sim << endl;

/*    for (int i = 0; i < 3; i++) {
        cout << "Axis " << approx.mAxes[i] << "  val:" << sqrt(approx.mValues[i]) << endl;
    }*/

   /* for (size_t i = 0; i < res.size() ; i++)
    {
        Vector3dd delta = res[i] - target;
        Vector3dd rads = approx.getRadiusPerDim();
        delta = delta / rads + rads / 2.0;
        mesh.setColor( RGBColor::FromDouble(delta) );
        mesh.addPoint(res[i]);
    }*/

    mesh.addOrts(2,true);
    mesh.dumpPLY("point-scatter3.ply");

    /** Forming out mesh **/
    delete mOutMesh;
    mOutMesh = new Mesh3D;
    mOutMesh->switchColor(true);
    mOutMesh->add(mesh, true);

    /** Draw image **/
    /*double min = image.;
    double max = ;*/

    delete mOutImage;
    mOutImage = new RGB24Buffer(image.getSize());
    AbstractPainter<RGB24Buffer> painter(mOutImage);

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    Vector3dd minV = Vector3dd(min);
    Vector3dd maxV = Vector3dd(max);

    for (int i = 0; i < image.h; i++)
        for (int j = 0; j < image.w; j++)
        {
            min = CORE_MIN(min, image.element(i,j).minimum());
            max = CORE_MAX(max, image.element(i,j).maximum());

            minV = minV.cwiseMin(image.element(i,j));
            maxV = maxV.cwiseMax(image.element(i,j));
        }

    for (int i = 0; i < image.h; i++)
        for (int j = 0; j < image.w; j++)
        {
            // Vector3dd val = (image.element(i,j) - Vector3dd(min)) / (Vector3dd(max) - Vector3dd(min));
            Vector3dd val = (image.element(i,j) - minV) / (maxV - minV);

            mOutImage->element(i,j) = RGBColor::FromDouble(val * 255.0);
            /*if (mOutImage->element(i,j).toBRGInt() == 0)
                mOutImage->element(i,j) = RGBColor::White();*/
        }

    for (Vector3dd &target : points)
    {

        Vector3dd org1 = transform * model1.extrinsics.position;
        Vector3dd org2 = transform * model2.extrinsics.position;

        Vector3dd pos =  transform * target;

        mOutImage->drawLine(org1.xz(), pos.xz(), RGBColor::Yellow());
        mOutImage->drawLine(org2.xz(), pos.xz(), RGBColor::Yellow());
        mOutImage->drawLine(org2.xz(), org1.xz(), RGBColor::Yellow());

        mOutMesh->setColor(RGBColor::Yellow());
        mOutMesh->addLine(model1.extrinsics.position, model2.extrinsics.position);
        mOutMesh->addLine(model1.extrinsics.position, target);
        mOutMesh->addLine(target, model2.extrinsics.position);
    }

    {
        Vector3dd org1 = transform * r1.p;
        Vector3dd org2 = transform * r2.p;
        Vector3dd pos =  transform * inter;

        Vector3dd dir1 = transform * r1.getPoint(1.0);
        Vector3dd dir2 = transform * r2.getPoint(1.0);


        mOutImage->drawLine(org1.xz(), pos.xz(), RGBColor::Gray());
        mOutImage->drawLine(org2.xz(), pos.xz(), RGBColor::Gray());
        mOutImage->drawLine(org2.xz(), org1.xz(), RGBColor::Gray());

        mOutImage->drawLine(org1.xz(), dir1.xz(), RGBColor::White());
        mOutImage->drawLine(org2.xz(), dir2.xz(), RGBColor::White());


        mOutMesh->setColor(RGBColor::Gray());
        mOutMesh->addLine(r1.p, r2.p);
        mOutMesh->addLine(r1.p, inter);
        mOutMesh->addLine(inter, r2.p);
    }

    if (eParams.use3rdPoint())
    {
        Vector3dd org1 = transform * r1.p;
        Vector3dd org2 = transform * r2.p;
        Vector3dd pos =  transform * addPoint;

        mOutImage->drawLine(org1.xz(), pos.xz(), RGBColor::Cyan());
        mOutImage->drawLine(org2.xz(), pos.xz(), RGBColor::Cyan());
    }




    //mOutImage->drawDoubleBuffer(image, RGB24Buffer::STYLE_GRAY);
    return 0;
}

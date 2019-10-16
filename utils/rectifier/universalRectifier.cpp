/**
 * \file universalRectifier.cpp
 * \brief Add Comment Here
 *
 * \date Oct 28, 2011
 * \author alexander
 */

#include <QImage>
#include <QPainter>

#include "core/utils/global.h"

#include "universalRectifier.h"
#include "core/rectification/essentialEstimator.h"
#include "core/cammodel/cameraParameters.h"
#include "core/rectification/iterativeEstimator.h"
#include "core/math/eulerAngles.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/rectification/ransacEstimator.h"
#include "core/rectification/stereoAligner.h"
#include "core/math/mathUtils.h"

class LengthFilterPredicate
{
public:
    double filterLength;
    LengthFilterPredicate(double _filterLength) :
        filterLength(_filterLength)
    {}

    bool operator() (const Correspondence &corr) const
    {
        return (corr.correspondenceLength() >= filterLength);
    }
};

class RemoveFilteredPredicate
{
public:
    RemoveFilteredPredicate() {};

    bool operator() (const Correspondence &corr) const
    {
        return !(corr.isFiltered());
    }
};


void UniversalRectifier::ransacMethod( double scale )
{
    int trySize         = params.ransacTestSize();
    int iterations      = params.ransacIterations();
    double threshold    = params.ransacThreshold();

    if (params.computeEssential())
    {
        RansacEstimator ransacEstimator(trySize, iterations, threshold);
        result.F = ransacEstimator.getEssentialRansac1(normalList);
        return;
    }

    RansacEstimator ransacEstimator(trySize, iterations, threshold * scale);
    result.F = ransacEstimator.getFundamentalRansac1(normalList);
}

void UniversalRectifier::iterativeMethod( void )
{

	EssentialEstimator::OptimisationMethod method = EssentialEstimator::METHOD_DEFAULT;
	switch (params.iterativeMethod())
	{
		case 0: method = EssentialEstimator::METHOD_SVD_LSE;          break;
		case 1: method = EssentialEstimator::METHOD_GRAD_DESC;        break;
		case 2: method = EssentialEstimator::METHOD_MARQ_LEV;         break;
		case 3: method = EssentialEstimator::METHOD_CLASSIC_KALMAN;   break;
		default: break;
	}

    // itertive essential estimation
    IterativeEstimator iterativeEstimator(
          params.iterativeIterations()
        , params.iterativeInitialSigma() / params.priorFocal()
        , params.iterativeFactorSigma()
        , method
    );

    result.F = iterativeEstimator.getEssential(*data);
    printf("Essential:\n");
    result.F.print();

    /* Move this somewhere */
    RGB24Buffer *buffer = new RGB24Buffer(right);
    for (unsigned i = 0 ; i < data->size(); i++)
    {
        Correspondence *corr    = data->at(i);
        Correspondence *corrPos = &(filteredList->at(i));

        double value = corr->value / (double)params.iterativeIterations();
        buffer->drawLineSimple(
            corrPos->start.x(),
            corrPos->start.y(),
            corrPos->end.x(),
            corrPos->end.y(),
            RGBColor::rainbow(value)
        );
    }
    BMPLoader().save("iterations.bmp", buffer);

    delete_safe(buffer);
}

void UniversalRectifier::manualMethod( void )
{
    CameraLocationAngles euler(params.manualYaw(), params.manualPitch(), params.manualRoll());
    Matrix33 R = euler.toMatrix();
    Vector3dd direction(params.manualX(), params.manualY(), params.manualZ());
    direction.normalise();
    result.F = EssentialMatrix::compose(R, direction);
    printf("Essential:\n");
    result.F.print();
}


void UniversalRectifier::getZ(void)
{
    if (!params.autoZ())
    {
        direction = Vector3dd(params.zdirX(), params.zdirY(), params.zdirZ());
        return;
    }

    printf("Autoguessing best Z\n");
    const unsigned GRANULARITY = 800;
    double distL[GRANULARITY];
    double distR[GRANULARITY];

    double min =  numeric_limits<double>::max();
    double max = -numeric_limits<double>::max();
    double minsum = numeric_limits<double>::max();
    double minindex  = 0;

    direction =  StereoAligner::getBestZ(result.F, inputList->getSize(), GRANULARITY, distL, distR);

    for (unsigned i = 0; i < GRANULARITY; i++)
    {
        if (min > distL[i])  min = distL[i];
        if (min > distR[i])  min = distR[i];
        if (max < distL[i])  max = distL[i];
        if (max < distR[i])  max = distR[i];

        if (distL[i] + distR[i] <  minsum)
        {
            minsum = distL[i] + distR[i];
            minindex = i;
        }
    }

    int graphH = 500;
    zGraph = new QImage(GRANULARITY, graphH,  QImage::Format_RGB32);
    zGraph->fill(0);
    {
        QPainter painter(zGraph);

        max = min + 10000;
        for (unsigned i = 0; i < GRANULARITY; i++)
        {
            int y;
            painter.setPen(QColor(255,255,0));
            // TODO: Use LERP helpers here
            y = (distL[i] - min) / (max - min) * graphH;
            painter.drawPoint(i, graphH - y - 1);
            painter.setPen(QColor(255,0,255));
            y = (distR[i] - min) / (max - min) * graphH;
            painter.drawPoint(i, graphH - y - 1);
        }

        painter.setPen(QColor(0,255,0));
        painter.drawLine(minindex,0,minindex,graphH - 1);
    }

    printf("Guessed direction:");
    direction.println();
}

void UniversalRectifier::decomposeEssential()
{
    EssentialDecomposition decomposition[4];
    double cost[4];

    cout << "=============================================================" << endl;
    EssentialMatrix(result.F).decompose(decomposition);
    int selected = 0;
    for (selected = 0; selected < 4; selected++)
    {
    	 cout << "Decomposition " << selected << endl << decomposition[selected] << endl;

        double d1;
        double d2;
        double err;
        cost[selected] = 0.0;
        EssentialDecomposition *dec = &(decomposition[selected]);
        for (unsigned i = 0; i < normalList->size(); i++)
        {
            dec->getScaler(normalList->at(i), d1, d2, err);
            if (d1 > 0.0 && d2 > 0.0)
                cost[selected]++;
        }

        cout << "decomposition cost:" << cost[selected] << endl;
    }

    double maxCost = 0.0;
    int finalSelection = 0;
    for (selected = 0; selected < 4; selected++)
    {
        if (cost[selected] > maxCost)
        {
            maxCost = cost[selected];
            finalSelection = selected;
        }
    }
    cout << endl;

    result.decomposition = decomposition[finalSelection];
    cout << "Chosen decomposition" << endl << result.decomposition << endl;
}


void UniversalRectifier::recalculate(void)
{
    //emit matchNeeded();
    if (inputList == NULL) {
        code = 1;
        status = "Could not rectificate. No input matches.";
        return;
    }

    if (inputList->size() < 5)
    {
        code = 3;
        status = "Too few matches at input";
        return;
    }
    list = new DerivedCorrespondenceList(inputList);

    /**
     * Clear the flags
     **/
    inputList->resetFlags();
    list->resetFlags();

    // TODO: Clean this up
    bool normalize     = params.normalise() && (params.estimationMethod() == 0);
    bool useKLT        = params.useKLT();
    double userShift   = params.preShift();
    bool useAutoShift  = params.autoShift();
    bool essntialOnly  = params.computeEssential();
    double focalRight  = params.priorFocal();
    double focalLeft   = params.priorFocal2();


    /* First prepare the common output */
    CameraIntrinsicsLegacy cameraLeft (inputList->getSize(), inputList->getSize() / 2.0, focalLeft , 1.0);
    CameraIntrinsicsLegacy cameraRight(inputList->getSize(), inputList->getSize() / 2.0, focalRight, 1.0);

#if 0
    PrinterVisitor visitor;
    camera.accept<PrinterVisitor>(visitor);
    qDebug("Setting focal to %lf and fov to %lfdeg, buffer [%lfx%lf]", focal, radToDeg(camera.getHFov()), inputList->getSize().x(), inputList->getSize().y() );
#endif

    result.leftCamera  = cameraLeft;
    result.rightCamera = cameraRight;

    result.baseline = params.baselineLength();

    /* Then comes KLT block */
    if (useKLT)
    {
        if (left == NULL || right == NULL) {
            code = 2;
            status = "Could not use KLT no gray value provided";
            return;
        }

        list->makePrecise(right, left);
    }

    RemoveFilteredPredicate kltFiltered;
    postKlt = new DerivedCorrespondenceList(
            list,
            kltFiltered,
            (int)Correspondence::FLAG_FILTERED_KLT);

    /* Then length filtering */
    LengthFilterPredicate lenFilter(params.filterMinimumLength());
    filteredList = new DerivedCorrespondenceList(
            postKlt,
            lenFilter,
            (int)Correspondence::FLAG_FILTERED_SHORT);

    /* Filtering finished we can select correspondences that are left and form input data */
    cout << "Input " << list->size() << " correspondences" << endl;
    cout << "After Klt Filtering " << postKlt->size()      << " correspondences" << endl;
    cout << "After Len Filtering " << filteredList->size() << " correspondences" << endl;

    normalList = new DerivedCorrespondenceList(filteredList);
    data = normalList->toArrayOfPointers();

    /* In case of essential estimation data should be transformed to the view of normalized camera */
    /* In case of fundamental normalizing transform is used for math optimization */
    Matrix33 rightNormalizer(1.0);
    Matrix33 leftNormalizer (1.0);
    double scale = 1.0;

    if (essntialOnly)
    {
        rightNormalizer = result.rightCamera.getInvKMatrix33();
        leftNormalizer  = result.leftCamera .getInvKMatrix33();
    }

    if (normalize) {
        normalList->getNormalizingTransform(rightNormalizer, leftNormalizer, &scale);
    }

    if (normalize || essntialOnly) {
        normalList->transform(rightNormalizer, leftNormalizer);
    }

    qDebug("Normalized mean x: %lf\n", normalList->getMeanDifference().x());

    switch (params.estimationMethod())
    {
        case 0:
            srand(0x5EED);
            ransacMethod(scale);
            break;
        case 1:
            iterativeMethod();
            break;
        case 2:
        default:
            manualMethod();
    }


    decomposeEssential();

    /* Return back to original coordinates*/
    if (normalize || essntialOnly) {
        result.F = rightNormalizer.transposed() * result.F * leftNormalizer;
    }


    printf("Global Fundamental Matrix:\n");
    result.F.print();
    printf("Global Fundamental Matrix cost:");
    vector<Correspondence *> inputListPtr;
    for (unsigned i = 0; i < inputList->size(); i++) {
        inputListPtr.push_back(&(inputList->at(i)));
    }
    EssentialEstimator::CostFunction7to1 cost(&inputListPtr);
    cout << "Error:" << cost.getCost(result.F) << endl;
    cout << "Sq per point:" << (cost.getCost(result.F) / inputListPtr.size()) << endl;
    cout << "Error per point:" << sqrt(cost.getCost(result.F) / inputListPtr.size()) << "px" << endl;


    /* Find epipoles as the debug result */
    EssentialMatrix(result.F).nullspaces(&E1, &E2);

    /* Find a transformation that aligns the rows */
    getZ();
    StereoAligner::getAlignmentTransformation(
            result.F,
            &(result.rightTransform),
            &(result.leftTransform),
            direction);


    cout << "Sanity check" << endl;
    EssentialMatrix Ix;
    cout << (result.rightTransform.transposed() * Ix * result.leftTransform) / result.F << endl;

    /* Adapt the transformation to make a center of the image a fixed point */
    Vector2dd center = inputList->getSize() / 2.0;
    StereoAligner::getLateralCorrectionFixPoint(&(result.rightTransform), &(result.leftTransform), center);

    /* Shift images for best overlapping */
    if (useAutoShift)
    {
        StereoAligner::getLateralCorrection(&(result.rightTransform), &(result.leftTransform), postKlt, params.guessShiftThreshold());
    } else {
        result.rightTransform = Matrix33::ShiftProj( userShift / 2.0, 0.0) * result.rightTransform;
        result.leftTransform  = Matrix33::ShiftProj(-userShift / 2.0, 0.0) * result.leftTransform;
    }

    /* One more cost computation */
    vector<Correspondence>   alignedList;
    vector<Correspondence *> alignedPtrList;
    for (unsigned i = 0; i < inputList->size(); i++)
    {
        Correspondence input = inputList->at(i);
        Correspondence output;
        output.start = result.rightTransform * input.start;
        output.end   = result.leftTransform  * input.end;

        alignedList.push_back(output);
    }
    for (unsigned i = 0; i < inputList->size(); i++)
    {
        alignedPtrList.push_back(&(alignedList[i]));
    }

    EssentialEstimator::CostFunction7to1 costAligned(&alignedPtrList);
    cout << "Error:" << costAligned.getCost(Ix) << endl;
    cout << "Sq per point:" << (costAligned.getCost(Ix) / alignedPtrList.size()) << endl;
    cout << "Error per point:" << sqrt(costAligned.getCost(Ix) / alignedPtrList.size()) << endl;


    if (essntialOnly)
    {
        vector<Vector3dd> points3d;
        double zMax = 0;
        for (unsigned i = 0; i < normalList->size(); i++)
        {
            double d1, d2, err;
            Correspondence corr = normalList->at(i);
            result.decomposition.getScaler(corr , d1, d2, err);
            Vector3dd point(corr.start, 1.0);
            point *= d1;
            point *= params.baselineLength();
            points3d.push_back(point);
            if (zMax < d1) zMax = d1;
        }

        topView = new QImage(800,800, QImage::Format_RGB32);
        topView->fill(0);
        QPainter painter(topView);

        for (unsigned i = 0; i < points3d.size(); i++)
        {
            double x = points3d[i].x();
            double z = points3d[i].z();
            int ix = x + topView->width() / 2;
            int iz = topView->height() - 1 - z;
            painter.setPen(QColor(0xF0, 0xF0, 0xF0));
            painter.drawPoint(ix, iz);
        }

        /* Draw camera lines */
        {
            Vector3dd ll = result.leftCamera .getInvKMatrix33() * Vector3dd(         0.0, inputList->h / 2, 1.0);
            Vector3dd rl = result.leftCamera .getInvKMatrix33() * Vector3dd(inputList->w, inputList->h / 2, 1.0);

            ll *= 100;
            rl *= 100;
            int ix, iz;
            ix = ll.x() + topView->width() / 2;
            iz = topView->height() - 1 - ll.z();
            painter.setPen(QColor(0xFF, 0x00, 0x00));
            painter.drawPoint(ix, iz);

            ix = rl.x() + topView->width() / 2;
            iz = topView->height() - 1 - rl.z();
            painter.setPen(QColor(0xFF, 0x00, 0x00));
            painter.drawPoint(ix, iz);
        }


        FILE *Points = fopen("points.txt", "wt");
        for (unsigned i = 0; i < points3d.size(); i++)
        {
            fprintf(Points, "%lf %lf %lf\n", points3d[i].x(), points3d[i].y(), points3d[i].z());
        }
        fclose(Points);


    }

    /*Finally copy back the flag information*/
    normalList->propagateData();
    filteredList->propagateData();
    postKlt->propagateData();
    list->propagateData();

}

UniversalRectifier::~UniversalRectifier()
{
    delete_safe(data);
    delete_safe(list);
    delete_safe(filteredList);
    delete_safe(postKlt);
    delete_safe(normalList);
}


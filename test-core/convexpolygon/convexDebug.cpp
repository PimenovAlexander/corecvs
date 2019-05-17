#include "convexDebug.h"

ConvexDebug::ConvexDebug()
{

}


/* Temporary function */
bool ConvexDebug::GiftWrap(ProjectivePolygon &points, ProjectivePolygon &output, Mesh3D *debug)
{
    output.clear();

    if (points.empty()) {
        return false;
    }
    if (points.size() == 1) {
        output.push_back(points[0]);
        return true;
    }

#if 1
    /* Find one point in hull */
    size_t iMin = 0;
    double zMin = std::numeric_limits<double>::max();

    for (size_t i = 0; i < points.size(); i++)
    {
        double zcost = points[i].normalised() & Vector3dd::OrtZ();
        if (zcost < zMin) {
            zMin = zcost;
            iMin = i;
        }
    }
    if (debug != NULL) debug->setColor(RGBColor::Red());
    if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);


    /* We have now selected direction let's check if we have lines futher away from  */
    Vector3dd orth  = Vector3dd::OrtZ() ^ points[iMin];
    Vector3dd plane = points[iMin] ^ orth; /* in euclidean coordinates this is a point */

    double cMin = std::numeric_limits<double>::max();
    for (size_t i = 0; i < points.size(); i++)
    {
        double dot = plane & points[i].normalised();
        if (dot < cMin)
        {
            cMin = dot;
            iMin = i;
        }
    }
    if (debug != NULL) debug->setColor(RGBColor::Yellow());
    if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);

    /* We now have a direction to work from */

    output.push_back(points[iMin]);
    Vector3dd prev = points[iMin];

    SYNC_PRINT(("ConvexHull::GiftWrap(): starting with point (%d of %d) (%lf %lf %lf)\n",
                (int)iMin, (int)points.size(),
                prev.x(), prev.y(), prev.z()));
    if (debug != NULL) debug->setColor(RGBColor::Amber());
    if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);


    /* Find second point in hull. A known interior point could save time here */
    int sId = -1;
    for (size_t i = 0; i < points.size(); i++)
    {
        if (i == iMin) {
            continue;
        }
        Vector3dd normal = prev ^ points[i];
        cout << " " << i << " p:" << points[i] << " - " << normal << endl;

        size_t j = 0;
        for (; j < points.size(); j++)
        {
            cout << "   " << j;
            if (j == i || j == iMin) {
                cout << " skip" << endl;
                continue;
            }
            double v = (normal & points[j]);
            cout << "  " << v << endl;
            if (v < 0) {
                break;
            }
        }
        if (j == points.size()) {
            sId = i;
            break;
        }
    }
    if (sId == -1) {
        return false;
    }

    Vector3dd current = points[sId];
    output.push_back(points[sId]);
    SYNC_PRINT(("ConvexHull::GiftWrap(): second point (%d of %d) (%lf %lf %lf)\n",
                (int)iMin, (int)points.size(),
                current.x(), current.y(), current.z()));
#endif

#if 0
    int mini = 0;
    int minj = 1;
    double zMin = std::numeric_limits<double>::max   ();
    double zMax = std::numeric_limits<double>::lowest();

    for (int i = 0; i < points.size(); i++)
    {
        for (int j = 0; j < points.size(); j++)
        {
            if (i == j) continue;
            Vector3dd p1 = points[i].normalised();
            Vector3dd p2 = points[j].normalised();

            if (p1.z() < 0 && p2.z() > 0) continue;
            if (p1.z() > 0 && p2.z() < 0) continue;

            Vector3dd plane = (p1 ^ p2);
            double z = plane.z();
            cout << " " << i << " " << j << " " << plane << endl;
            if (z <= 0) {
                continue;
            }

            if (z < zMin) {
                zMin = z;
                mini = i;
                minj = j;
            }
        }
    }
    cout << "-> " << mini << " - " << minj << endl;

    Vector3dd prev    = points[mini];
    Vector3dd current = points[minj];


    if (debug) {
        debug->setColor(RGBColor::Yellow());
        debug->addLineDash(Vector3dd::Zero(), (prev ^ current) * 10.0);
    }

    output.push_back(prev);
    output.push_back(current);

    if (debug != NULL) {
        debug->setColor(RGBColor::Red());
        debug->addIcoSphere(prev.normalised(), 0.1);

        debug->setColor(RGBColor::Green());
        debug->addIcoSphere(current.normalised(), 0.1);
    }
#endif

    /* Wrap */
    int limit = 10;
    int iteration = 0;
    int mark = 0;
    do {
        iteration++;
        cout << "Iteration " << iteration << endl;
        //if (debug != NULL) debug->addLine(Vector3dd::Zero(), prev.normalised());

        Vector3dd next = Vector3dd::Zero();
        double vmax = std::numeric_limits<double>::lowest();

        /* Search for best point */
        for (const Vector3dd &point : points)
        {
            if ((point.normalised() - current.normalised()).l2Metric() < 1e-7) {
                continue;
            }

            Vector3dd splane = current ^ prev;
            if (iteration > mark) {
                //if (debug != NULL) debug->setColor(RGBColor::rainbow(lerp(0.0, 1.0, iteration, mark, limit)));
                if (debug != NULL) debug->setColor(RGBColor::Green() / 2);
                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), prev.normalised(), 0.03, 0.03);

                if (debug != NULL) debug->setColor(RGBColor::Green());
                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), current.normalised());


                if (debug != NULL) debug->setColor(RGBColor::Yellow());
                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), splane.normalised());

//                if (debug != NULL) debug->setColor(RGBColor::Pink());
//                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), back.normalised());
            }


            Vector3dd p = point.normalised();
            Vector3dd nplane = p ^ current;

            double v = splane.normalised() & nplane.normalised();

            /*if (v < 0) {
                return false;
            }*/
            /*
            bool forward = (p & back) < 0;
            bool part    = (p & current) > 0;
            */

            SYNC_PRINT(("(%lf %lf %lf) - %lf\n", point.x(), point.y(), point.z(), v));

            if (v > vmax /*&& forward*/ /*&& part*/)
            {
                vmax = v;
                next = point;
            }
        }

        if (next == output.front())
        {
            break;
        }
        SYNC_PRINT(("ConvexHull::GiftWrap(): %s next point (%lf %lf %lf)\n",
                    iteration > mark ? "*" : "",
                    next.x(), next.y(), next.z()));

        prev = current;
        current = next;
        output.push_back(next);
        //back = Vector3dd::OrtZ() ^ current;

        if (iteration > mark) {
            if (debug != NULL) debug->setColor(RGBColor::Navy());
            if (debug != NULL) debug->addLine(Vector3dd::Zero(), next.normalised());
        }

        //if (debug != NULL) debug->setColor(RGBColor::Magenta());


    } while (iteration < limit);
    cout << "Remaining polygons:" << output.size() << endl;
    return true;
}


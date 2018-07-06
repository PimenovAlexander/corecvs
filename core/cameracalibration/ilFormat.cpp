#include "ilFormat.h"

#include <fstream>

namespace corecvs {

ILFormat::ILFormat()
{

}

const std::string ILFormat::IL_OMNIDIRECTIONAL = "omnidirectional";
const std::string ILFormat::IL_PROJECTIVE      = "projective";


CameraProjection *ILFormat::loadIntrisics(std::istream &filename)
{
    /**
      Some examples

        // "omnidirectional\n"
        // "1578 1.35292 1.12018 5 0.520776 -0.561115 -0.560149 1.01397 -0.870155";


        // projective
        // fx fy cx cy w h


    **/

    std::string cameraType;
    filename >> cameraType;

    if (cameraType == IL_OMNIDIRECTIONAL)
    {
        double s;
        Vector2dd c;
        int imax;
        double n0;

        filename >> s;
        filename >> c.x() >> c.y();
        filename >> imax;

        filename >> n0;
        vector<double> n;
        for (int i = 0; i < imax-1; i++)
        {
            double v = 0;
            filename >> v;
            n.push_back(v);
        }

#if 0
        cout << "Type: " << cameraType << endl;
        cout << "s: " << s << endl;
        cout << "c: " << c << endl;
        cout << "imax: " << imax << endl;
        cout << "n0: " << n0 << endl;

        for (int i = 0; i < imax-1; i++)
        {
            cout << "n" << i+2 << " " << n[i] << endl;
        }
#endif

        OmnidirectionalProjection *projection = new OmnidirectionalProjection;
        projection->setFocal(s * n0);
        projection->setPrincipalX(s * c.x());
        projection->setPrincipalY(s * c.y());
        projection->setSizeX(projection->principalX() * 2);
        projection->setSizeY(projection->principalY() * 2);

        projection->setDistortedSizeX(projection->principalX() * 2);
        projection->setDistortedSizeY(projection->principalY() * 2);

        projection->mN.clear();

        for (size_t i = 0; i < n.size(); i++)
        {
           projection->mN.push_back(n[i] * pow(n0, i + 1));
        }
        return projection;
    }


    if (cameraType == IL_PROJECTIVE)
    {
        //fx fy cx cy w h
        PinholeCameraIntrinsics *projection = new PinholeCameraIntrinsics;
        Vector2dd focal     = Vector2dd::Zero();
        Vector2dd pripcipal = Vector2dd::Zero();
        Vector2dd size      = Vector2dd::Zero();

        filename >> focal.x()     >> focal.y();
        filename >> pripcipal.x() >> pripcipal.y();
        filename >> size.x()      >> size.y();

        projection->setFocal(focal);
        projection->setPrincipal(pripcipal);
        projection->setSize(size);
        projection->setDistortedSize(size);
        projection->setSkew(0.0);
        return projection;
    }

    return NULL;
}

void ILFormat::saveIntrisics(const CameraProjection &projection, std::ostream &stream)
{
    if (projection.projection == ProjectionType::PINHOLE)
    {
        const PinholeCameraIntrinsics *pinhole = static_cast<const PinholeCameraIntrinsics *>(&projection);
        stream << IL_PROJECTIVE << endl;
        stream << pinhole->fx() << " " << pinhole->fy()  << " ";
        stream << pinhole->cx() << " " << pinhole->cy()  << " ";
        stream << pinhole->size().x() << " " << pinhole->size().y()  << " ";
        return;
    }
    if (projection.projection == ProjectionType::OMNIDIRECTIONAL)
    {
         const OmnidirectionalProjection *omni = static_cast<const OmnidirectionalProjection *>(&projection);

         stream << IL_OMNIDIRECTIONAL << endl;
         stream << omni->focal() << " ";
         stream << (omni->principalX() / omni->focal()) << " ";
         stream << (omni->principalY() / omni->focal()) << " ";
         stream << (omni->mN.size() + 1)  << " ";
         stream << 1.0  << " ";
         for (size_t i = 0; i < omni->mN.size(); i++)
             stream << omni->mN[i]  << " ";
         stream << endl;
         return;
    }
    stream << "unsupported" << endl;
}


CameraProjection *ILFormat::loadIntrisics(const std::string &filename)
{
    std::ifstream is;
    is.open(filename, std::ifstream::in);

    is.imbue(std::locale("C"));

    return ILFormat::loadIntrisics(is);
}



void ILFormat::saveIntrisics(const CameraProjection &projection, const std::string &filename)
{
    std::ofstream os;
    os.open(filename, std::ifstream::out);

    os.imbue(std::locale("C"));
    os << std::setprecision(std::numeric_limits<double>::digits10 + 2);

    ILFormat::saveIntrisics(projection, os);
}

Affine3DQ ILFormat::loadExtrinsics(std::istream &stream)
{
    Affine3DQ transform;
    Matrix44 m(1.0);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            stream >> m.a(i, j);
        }
    }
    transform.shift = m.translationPart();
    transform.rotor = Quaternion::FromMatrix(m.topLeft33());
    return transform;
}

void ILFormat::saveExtrinsics(const Affine3DQ &transform, std::ostream &stream)
{
    Matrix44 m = (Matrix44) transform;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            stream << std::setprecision(16) << std::scientific << m.a(i, j) << " ";
        }
    }
}


} // namespace corecvs


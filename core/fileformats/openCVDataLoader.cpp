#include "openCVDataLoader.h"

using namespace tinyxml2;

namespace corecvs {

OpenCVDataLoader::OpenCVDataLoader()
{

}

int corecvs::OpenCVDataLoader::readInt(XMLNode *node, const char *name)
{
    XMLElement *e = node->FirstChildElement(name);
    if (e == NULL) {
        SYNC_PRINT(("Document is malformed"));
        return 0;
    } else {
        cout << "Text <" << e->GetText() << ">" << endl;
    }
    return std::stoi(e->GetText());
}


Matrix corecvs::OpenCVDataLoader::readMatrix(XMLNode *node)
{
    Matrix toReturn;
    XMLElement *type = node->FirstChildElement("dt");
    if (type == NULL) {
        cout << "type tag not found" << endl;
        return toReturn;
    }
    if (type->GetText() != std::string("d") && type->GetText() != std::string("f")) {
        cout << "not a double or float matrix <" << type->GetText() << ">" << endl;
        return toReturn;
    }

    int h = 0;
    int w = 0;

    h = readInt(node, "rows");
    w = readInt(node, "cols");

    cout << "Matrix of size" << h << "x" << w << endl;

    toReturn = Matrix(h, w);

    XMLElement *data = node->FirstChildElement("data");
    if (data == NULL) {
        cout << "data tag not found" << endl;
        return toReturn;
    } else {
        cout << data->GetText() << endl;
    }
    std::istringstream is(data->GetText());
    for (int i = 0; i < toReturn.h; i++)
    {
        for (int j = 0; j < toReturn.w; j++)
        {
            is >> toReturn.element(i,j);
        }
    }

    return toReturn;
}

OpenCVDataLoader::OpenCVCAL OpenCVDataLoader::parseOpenCVOutput(const char *filename)
{
    OpenCVCAL toReturn;
    SYNC_PRINT(("Parsing <%s>\n", filename));

    XMLDocument* doc = new XMLDocument();
    XMLError errorId = doc->LoadFile(filename);
    if (doc->Error())
    {   cout << "Parsing Error: " << errorId << endl;
        delete doc;
        return toReturn;
    }

    XMLNode *node = doc->FirstChild();
    if (node == NULL) {
        SYNC_PRINT(("Document is empty"));
    }
    cout << node->Value() << endl;

    {
        XMLElement *node = doc->FirstChildElement("opencv_storage");
        if (node == NULL) {
            SYNC_PRINT(("Document is empty"));
        }
        cout << node->Value() << endl;

        toReturn.W  = readInt(node, "image_Width");
        toReturn.H  = readInt(node, "image_Height");
        toReturn.bW = readInt(node, "board_Width");
        toReturn.bH = readInt(node, "board_Height");

        toReturn.sqSize = readInt(node, "square_Size");

        XMLElement *cammat = node->FirstChildElement("Camera_Matrix");
        if (cammat == NULL) {
            cout << "Camera Matrix not found" << endl;
            return toReturn;
        }

        Matrix cameraMatrix = readMatrix(cammat);
        cout << "Camera Matrix:" << endl << cameraMatrix << endl;

        XMLElement *distMat = node->FirstChildElement("Distortion_Coefficients");
        if (distMat == NULL) {
            cout << "Distortion Matrix not found" << endl;
            return toReturn;
        }

        Matrix distMatrix = readMatrix(distMat);
        cout << "Distortion Matrix:" << endl << distMatrix << endl;


        PinholeCameraIntrinsics *intr = new PinholeCameraIntrinsics;
        intr->setFx(cameraMatrix.element(0,0));
        intr->setFy(cameraMatrix.element(1,1));
        intr->setCx(cameraMatrix.element(0,2));
        intr->setCy(cameraMatrix.element(1,2));
        intr->setSize(Vector2dd(toReturn.W, toReturn.H));
        toReturn.camera.intrinsics.reset(intr);

        toReturn.camera.distortion.setPrincipalX(intr->principal().x());
        toReturn.camera.distortion.setPrincipalY(intr->principal().y());
        toReturn.camera.distortion.setAspect(1.0);

        toReturn.camera.distortion.setKoeff({
                                                0,
                                                distMatrix.element(0,0),
                                                0,
                                                distMatrix.element(1,0),
                                                0,
                                                distMatrix.element(4,0)});

        toReturn.camera.distortion.setTangentialX(distMatrix.element(2,0));
        toReturn.camera.distortion.setTangentialY(distMatrix.element(3,0));
        toReturn.camera.distortion.setNormalizingFocal(intr->size().y() / 2);

        {
            XMLElement *extrMat = node->FirstChildElement("Extrinsic_Parameters");
            if (extrMat == NULL) {
                cout << "Distortion Matrix not found" << endl;
                return toReturn;
            }

            Matrix extrinsics = readMatrix(extrMat);
            cout << "Extrinsics Matrix:" << endl << extrinsics << endl;

            for (int i = 0; i < extrinsics.h; i++)
            {
                Vector3dd shift(
                            extrinsics.element(i, 3),
                            extrinsics.element(i, 4),
                            extrinsics.element(i, 5));

                double r1 = extrinsics.element(i, 0);
                double r2 = extrinsics.element(i, 1);
                double r3 = extrinsics.element(i, 2);

                /* We have to guess how opencv stores it */
                CORE_UNUSED(r1);
                CORE_UNUSED(r2);
                CORE_UNUSED(r3);

                Affine3DQ affine = Affine3DQ::Shift(shift);
                toReturn.positions.push_back(affine);


            }
        }



    }

    delete doc;
    return toReturn;

}

} // namespace corecvs

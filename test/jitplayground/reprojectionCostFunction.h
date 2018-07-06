#ifndef REPROJECTIONCOSTFUNCTION_H
#define REPROJECTIONCOSTFUNCTION_H

#include "core/function/function.h"
#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"

class ReprojectionCostFunction : public FunctionArgs
{
public:
    /**
     * Model is packed like
     *
     *  Point1  position X, Y, Z
     *  ....
     *  PointN  position X, Y, Z
     *  Fixture position X, Y, Z
     *  Fixture rotation quaternion X, Y, Z, T
     *  ...
     *  Fixture position X, Y, Z
     *  Fixture rotation quaternion X, Y, Z, T

     *
     **/


    FixtureScene *scene = NULL;

    /**
     * We replace sequence number to correspond cameras and their parameters
     **/
    ReprojectionCostFunction(FixtureScene *scene) :
        FunctionArgs(getInputsS(scene), getOutputsS(scene)),
        scene(scene)
    {}

    static int  getInputsS(FixtureScene *scene);
    static int getOutputsS(FixtureScene *scene);

    int  getInputs() {
        return getInputsS(scene);
    }

    int getOutputs() {
        return getOutputsS(scene);
    }


    template<typename DoubleType>
    void fillModel(DoubleType *in)
    {
        for (size_t targetId = 0; targetId < scene->featurePoints().size(); targetId++)
        {
            Vector3dd p = scene->featurePoints()[targetId]->position;
            in[targetId * Vector3dd::LENGTH]     = DoubleType(p.x());
            in[targetId * Vector3dd::LENGTH + 1] = DoubleType(p.y());
            in[targetId * Vector3dd::LENGTH + 2] = DoubleType(p.z());
        }

        int pointParameters = scene->featurePoints().size() * 3;

        for (size_t fixtureId = 0; fixtureId < scene->fixtures().size(); fixtureId++)
        {
             int offset = pointParameters + fixtureId * 7;
             Vector3dd  pointPosition = scene->fixtures()[fixtureId]->location.shift;
             Quaternion pointRotation = scene->fixtures()[fixtureId]->location.rotor;

             in[offset]     = DoubleType(pointPosition.x());
             in[offset + 1] = DoubleType(pointPosition.y());
             in[offset + 2] = DoubleType(pointPosition.z());

             in[offset + 3] = DoubleType(pointRotation.x());
             in[offset + 4] = DoubleType(pointRotation.y());
             in[offset + 5] = DoubleType(pointRotation.z());
             in[offset + 6] = DoubleType(pointRotation.t());
        }
    }

    template<typename DoubleType>
    void costFunction(const DoubleType *in, DoubleType *out)
    {
        int pointParameters = scene->featurePoints().size() * Vector3dd::LENGTH;
        int outCount = 0;

        for (size_t targetId = 0; targetId < scene->featurePoints().size(); targetId++)
        {
            SceneFeaturePoint &targetPoint = *(scene->featurePoints()[targetId]);

            Vector2d<DoubleType> error = Vector2d<DoubleType>(DoubleType(0.0));
            Vector3d<DoubleType> pointPosition(in[targetId * 3], in[targetId * 3 + 1], in[targetId * 3 + 2]);



            SceneFeaturePoint::ObservContainer::iterator it;
            for (it = targetPoint.observations.begin(); it != targetPoint.observations.end(); ++it)
            {
                FixtureCamera *camera  = it->first;
                CameraFixture *fixture = camera->cameraFixture;

                int sequenceNumber = fixture->sequenceNumber;
                int offset = pointParameters + sequenceNumber * 7;

                Vector3d<DoubleType>          fixturePosition(in[offset], in[offset + 1], in[offset + 2]);
                GenericQuaternion<DoubleType> fixtureRotation(in[offset + 3], in[offset + 4], in[offset + 5], in[offset + 6]);

                GenericCameraLocationData<DoubleType> camLocation(camera->extrinsics);
                camLocation.transform(fixtureRotation, fixturePosition);

                Vector3d<DoubleType>  cameraCoords = camLocation.project(pointPosition);

                Vector2d<DoubleType> fromModel = GenericPinholeCameraIntrinsics<DoubleType>(*camera->getPinhole()).project(cameraCoords);

                Vector2dd undist = it->second.getUndist();
                Vector2d<DoubleType> fromCam(DoubleType(undist.x()), DoubleType(undist.y()));
                error = fromCam - fromModel;

                out[outCount++] = error.x();
                out[outCount++] = error.y();
            }

        }

        //cout << "Totally " << outCount << " outputs" << endl;

    }



    // FunctionArgs interface
public:
    virtual void operator ()(const double in[], double out[]) override
    {
        costFunction<double>(in, out);
    }
};

#endif // REPROJECTIONCOSTFUNCTION_H

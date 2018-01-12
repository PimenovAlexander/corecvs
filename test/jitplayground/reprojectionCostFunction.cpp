#include "reprojectionCostFunction.h"

int ReprojectionCostFunction::getInputsS(FixtureScene *scene)
{
    int pointParameters = scene->featurePoints().size() * 3;


    /*int cameraParameters = 0;
    for (size_t i = 0; i < scene->mFixtures.size(); i++)
    {
        CameraFixture *fixture = scene->mFixtures[i];
        cameraParameters += fixture->cameras.size() * (3 + 4);
    }*/
    int fixtureParameters = scene->fixtures().size() * (3 + 4);


    return pointParameters + fixtureParameters;
}

int ReprojectionCostFunction::getOutputsS(FixtureScene *scene)
{
    return scene->totalObservations() * 2;
}


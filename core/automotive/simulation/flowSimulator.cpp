/**
 * \file flowSimulator.cpp
 * \brief This is an implementation file to simulate flow caused by still standing objects and a moving
 * vehicle.
 *
 * \date Feb 22, 2011
 * \author alexander
 */

#include "flowSimulator.h"
#include "FCostFunction.h"
namespace corecvs {

FlowSimulator::FlowSimulator()
{
}

FlowSimulator::~FlowSimulator()
{
}

std::vector<Vector3dd> *FlowSimulator::generateMockScene()
{
    static const int WALL_H = 20;
    static const int WALL_L = 280;
    static const int FLOOR_WIDTH = 20;

    static const float WALL_DIST = 5000.0;
    static const float BRICK_SIZE = 100.0;

    std::vector<Vector3dd> *result = new std::vector<Vector3dd>();

     /**
      * Construct two walls of points
      * Each column on the matrix in the matrix represent one point WALL_H * WALL_L
      * */

     for (int i = 0; i < WALL_L; i ++)
     {
       for (int j = 0; j < WALL_H; j ++)
       {
         /**
          *   OX -reverse of car forward movement
          *   OY -goes right from car
          *   OZ -orthogonal, goes up into the air
          * */

         double x =  -(i - WALL_L / 3) * BRICK_SIZE;
         double y0 = -WALL_DIST / 2.0;
         double y1 =  WALL_DIST / 2.0;
         double z =   j * BRICK_SIZE - (WALL_H / 3) * BRICK_SIZE;

         result->push_back(Vector3dd(x,y0,z));
         result->push_back(Vector3dd(x,y1,z));
       }
     }

     for (int i = 0; i < WALL_L; i ++)
     {
       for (int j = 0; j < FLOOR_WIDTH; j ++)
       {

         double x =  -(i - WALL_L / 3) * BRICK_SIZE;
         double y =  (WALL_DIST / (FLOOR_WIDTH - 1)) * j -  (WALL_DIST / 2.0);
         double z =  -( 0.5) * BRICK_SIZE - (WALL_H / 3) * BRICK_SIZE;

         result->push_back(Vector3dd(x,y,z));
       }
     }

     return result;

}

std::vector<Vector3dd> *FlowSimulator::unitCube(unsigned gradations)
{
    vector<Vector3dd> *pointsIn3d = new std::vector<Vector3dd>();
    double step = 1.0 / (gradations - 1);

    for (unsigned i = 0; i < gradations; i++)
    {
        for (unsigned j = 0; j < gradations; j++)
        {
            for (unsigned k = 0; k < gradations; k++)
            {
                Vector3dd cubePoint = Vector3dd(i, j, k) * step - Vector3dd(0.5);
                pointsIn3d->push_back(cubePoint);
            }
        }
    }

    return pointsIn3d;
}


std::vector<Vector3dd> *FlowSimulator::applyTransform(std::vector<Vector3dd> * pointsA, const Matrix44 &A)
{
    vector<Vector3dd> *result = new std::vector<Vector3dd>();
    for (unsigned i = 0; i < pointsA->size(); i++)
    {
        result->push_back(A * pointsA->at(i));
    }
    return result;
}

std::vector<FloatFlowVector> *FlowSimulator::simulateFlow(
        const CameraIntrinsics &camIntrinsics,
        const ShiftRotateTransformation &cameraExtrinsics,
        const ShiftRotateTransformation &carMovement,
        double maxZ,
        double minLength )
{
    Vector2dd imageSize = camIntrinsics.center * 2.0;

    std::vector<FloatFlowVector> *result = new std::vector<FloatFlowVector>();

    Matrix44 A = FCostFunction::GetMatrixForTransform(carMovement);

    vector<Vector3dd> *pointsA = FlowSimulator::generateMockScene();
    vector<Vector3dd> *pointsB = FlowSimulator::applyTransform(pointsA, A);

    Matrix44 D = FCostFunction::getDMatrix(FCostFunction::FRONT_VIEW_CAMERA, camIntrinsics, cameraExtrinsics);

    vector<Vector3dd> *imagePointsA = FlowSimulator::applyTransform(pointsA, D);
    vector<Vector3dd> *imagePointsB = FlowSimulator::applyTransform(pointsB, D);

    for (unsigned i = 0; i < pointsA->size(); i++)
    {
        Vector3dd projectiveA = imagePointsA->at(i);
        Vector3dd projectiveB = imagePointsB->at(i);

        if (projectiveA.z() <= 0 || projectiveB.z() <= 0)
            continue;

        if (projectiveA.z() > maxZ || projectiveB.z() > maxZ)
            continue;


        Vector2dd imagePointA = projectiveA.project();
        Vector2dd imagePointB = projectiveB.project();

        if (!imagePointA.isInRect(Vector2dd(0.0,0.0), imageSize - Vector2dd(1.0,1.0)))
            continue;
        if (!imagePointB.isInRect(Vector2dd(0.0,0.0), imageSize - Vector2dd(1.0,1.0)))
            continue;
        if ((imagePointB - imagePointA).sumAllElementsSq() < minLength * minLength)
            continue;

        FloatFlowVector flowVector(imagePointA, imagePointB);
        result->push_back(flowVector);
    }

    delete pointsA;
    delete pointsB;
    delete imagePointsA;
    delete imagePointsB;
    return result;
}


} //namespace corecvs


#include <pcl/registration/icp.h>

#include <core/math/matrix/matrix44.h>

#include "libpclWrapper.h"
typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> PointCloudT;

using namespace corecvs;


Matrix44 toCVS(const Eigen::Matrix4d &m)
{
    return Matrix44::FromRawRows(m.data());
}

Matrix44 LibPCLWrapper::legacyCall(
        std::vector<corecvs::Vector3dd> &scene,
        std::vector<corecvs::Vector3dd> &sample,
        double *cost
        )
{
    PointCloudT::Ptr cloud_scene  (new PointCloudT); // Main point cloud to search in
    PointCloudT::Ptr cloud_sample (new PointCloudT); // A sample to adjust to this point cloud

    PointCloudT::Ptr cloud_icp(new PointCloudT); // ICP output point cloud
    /*PointCloudT::Ptr antenna  (new PointCloudT);*/

    cloud_scene->reserve(scene.size());
    for (auto p : scene) {
        cloud_scene->push_back(PointT(p.x(), p.y(), p.z()));
    }

    cloud_icp->reserve(sample.size());
    for (auto p : sample) {
        cloud_icp->push_back(PointT(p.x(), p.y(), p.z()));
    }


    pcl::IterativeClosestPoint<PointT, PointT> icp;
    const float leaf = 0.015f;

    // Set the maximum number of iterations (criterion 1)
    icp.setMaximumIterations(500);
    icp.setMaxCorrespondenceDistance(0.5);

    // Set the transformation epsilon (criterion 2)
    icp.setTransformationEpsilon(1e-5);//1e-5
    // Set the euclidean distance difference epsregisterCloudilon (criterion 3)
    //icp.setMaxCorrespondenceDistance(0.5);
    icp.setEuclideanFitnessEpsilon(1e-5); //1e-5, 1e-8
    icp.setRANSACOutlierRejectionThreshold(1.5f * leaf);

    icp.setInputSource(cloud_icp);
    icp.setInputTarget(cloud_scene);

    icp.align(*cloud_icp);
    icp.setMaximumIterations(1);  // We set this variable to 1 for the next time we will call .align() function
    //std::cout << "Applied " << iterations << std::endl;

    if (icp.hasConverged())
    {
        if (trace) { std::cout << "\nICP has converged, score is " << icp.getFitnessScore() << std::endl; }
        //std::cout << "\nICP transformation " << iterations << " : cloud_icp -> cloud_in" << std::endl;
        Eigen::Matrix4d T = icp.getFinalTransformation().cast<double>();
        Matrix44 TC = toCVS(T).transposed();
        if (trace) { cout << "Result transform:\n" << TC << std::endl; }

        if (cost != NULL) {
            *cost = icp.getFitnessScore();
        }

        return TC;
    }
    else
    {
        if (trace) { PCL_ERROR("\nICP has not converged.\n"); }
        return Matrix44::Identity();
    }

}


#if 0
Matrix44 LibPCLWrapper::legacyCallPlane(std::vector<Vector3dd> &scene, std::vector<Vector3dd> &sample, double *cost)
{
    PointCloudT::Ptr cloud_scene  (new PointCloudT); // Main point cloud to search in
    PointCloudT::Ptr cloud_sample (new PointCloudT); // A sample to adjust to this point cloud

    PointCloudT::Ptr cloud_icp(new PointCloudT); // ICP output point cloud
    /*PointCloudT::Ptr antenna  (new PointCloudT);*/

    for (auto p : scene) {
        cloud_scene->push_back(PointT(p.x(), p.y(), p.z()));
    }

    for (auto p : sample) {
        cloud_icp->push_back(PointT(p.x(), p.y(), p.z()));
    }


    pcl::IterativeClosestPointWithNormals<PointT, PointT> icp;
    const float leaf = 0.015f;

    // Set the maximum number of iterations (criterion 1)
    icp.setMaximumIterations(500);
    // Set the transformation epsilon (criterion 2)
    icp.setTransformationEpsilon(1e-5);//1e-5
    // Set the euclidean distance difference epsregisterCloudilon (criterion 3)
    //icp.setMaxCorrespondenceDistance(0.5);
    icp.setEuclideanFitnessEpsilon(1e-5); //1e-5, 1e-8
    icp.setRANSACOutlierRejectionThreshold(1.5f * leaf);

    icp.setInputSource(cloud_icp);
    icp.setInputTarget(cloud_scene);

    icp.align(*cloud_icp);
    icp.setMaximumIterations(1);  // We set this variable to 1 for the next time we will call .align() function
    //std::cout << "Applied " << iterations << std::endl;

    if (icp.hasConverged())
    {
        if (trace) { std::cout << "\nICP has converged, score is " << icp.getFitnessScore() << std::endl; }
        //std::cout << "\nICP transformation " << iterations << " : cloud_icp -> cloud_in" << std::endl;
        Eigen::Matrix4d T = icp.getFinalTransformation().cast<double>();
        Matrix44 TC = toCVS(T).transposed();
        if (trace) { cout << "Result transform:\n" << TC << std::endl; }

        if (cost != NULL) {
            *cost = icp.getFitnessScore();
        }

        return TC;
    }
    else
    {
        if (trace) { PCL_ERROR("\nICP has not converged.\n"); }
        return Matrix44::Identity();
    }
}
#endif

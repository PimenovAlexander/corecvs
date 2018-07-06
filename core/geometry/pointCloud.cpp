#include "core/utils/utils.h"
#include "pointCloud.h"

namespace corecvs {


PointCloud::PointCloud()
{

}

void PointCloud::createTree()
{
    tree = new HelperTree();

    tree->submesh.resize(mesh.vertexes.size());
    for (Vector3dd &p : mesh.vertexes)
    {
        tree->submesh.push_back(BSPVector3ddPointer(&p));
    }
    tree->subdivide();
}

void PointCloud::relabel(std::vector<Vector3dd> &points, int id)
{
    createTree();
    for (Vector3dd &p : points)
    {
         // VectorPointer pointer = tree->findClosest(p);
    }
}

void PointCloud::load(const std::string &name)
{

    if (HelperUtils::endsWith(name, ".ply"))
    {
        SYNC_PRINT(("Loading single ply"));

        return;
    }

    if (HelperUtils::endsWith(name, ".xyz"))
    {
        SYNC_PRINT(("Loading single xyz"));

        return;
    }

    if (HelperUtils::isDirectory(name))
    {
        return;
    }



}

void PointCloud::save(const std::string &name)
{

}

}

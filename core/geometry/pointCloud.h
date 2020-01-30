#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <string>
#include <vector>
#include "core/math/vector/vector3d.h"
#include "core/geometry/mesh/mesh3d.h"

#include "core/geometry/kdtree.h"
#include "core/geometry/raytrace/bspTree.h"

namespace corecvs {

class PointCloud
{
public:
    PointCloud();

    //typedef KDTree<Vector3dd, Vector3dd::LENGTH> HelperTree;
    typedef BSPTreeNode<BSPVector3ddPointer> HelperTree;


    /*std::vector<Vector3dd>     points;
    std::vector<int>           ids;*/
    Mesh3D mesh; /*We only use points from here*/

    HelperTree *tree = NULL;

    std::map<int, std::string> labels;

    void createTree();

    void relabel(std::vector<Vector3dd> &points, int id);

    void load(const std::string &name);
    void save(const std::string &name);


};

}


#endif // POINTCLOUD_H

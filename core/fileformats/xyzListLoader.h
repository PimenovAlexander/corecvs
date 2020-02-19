#ifndef XYZLISTLOADER_H
#define XYZLISTLOADER_H

#include <iostream>
#include "geometry/mesh/mesh3d.h"

namespace corecvs {

typedef std::vector<std::pair<Vector3dd, std::string>> LabelList;


class XYZListLoader
{
public:
    bool trace = false;

    XYZListLoader();




    int loadXYZ(std::istream &input, Mesh3D &mesh);
    int saveXYZ(std::ostream &out, Mesh3D &mesh);

    int loadLabelList(std::istream &input, LabelList &list);
    int saveLabelList(std::ostream &out  , LabelList &list);

    ~XYZListLoader();
};

}

#endif // XYZLISTLOADER_H

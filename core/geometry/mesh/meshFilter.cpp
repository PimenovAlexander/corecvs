#include "meshFilter.h"


namespace corecvs
{


class SortedFaceData
{
public:
    unsigned int v[3];
    int fp;

    SortedFaceData(unsigned int v0, unsigned int v1, unsigned int v2, int _fp)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
        fp = _fp;
        std::sort(v, v + 3);
    }

    bool operator < (const SortedFaceData &p) const
    {
        return (v[2] != p.v[2]) ? (v[2] < p.v[2]) :
            (v[1] != p.v[1]) ? (v[1] < p.v[1]) :
            (v[0] < p.v[0]);
    }

    bool operator == (const SortedFaceData &s) const
    {
        if ((v[0] == s.v[0]) && (v[1] == s.v[1]) && (v[2] == s.v[2]))
            return true;
        return false;
    }


};


void MeshFilter::removeDuplicatedFaces(Mesh3DDecorated &mesh)
{

    std::vector<SortedFaceData> trianglesSorted;
        for (size_t i = 0; i != mesh.faces.size(); ++i)
        {
            auto t = mesh.faces[i];
            trianglesSorted.push_back(SortedFaceData(t[0], t[1], t[2], (int)i));
        }

        std::sort(trianglesSorted.begin(), trianglesSorted.end());
        std::vector<Vector3d32> noDuplFaces;
        for (size_t i = 0; i < trianglesSorted.size() - 1; ++i)
        {
            if (!(trianglesSorted[i] == trianglesSorted[i + 1]))
            {
                noDuplFaces.push_back(mesh.faces[trianglesSorted[i].fp]);
            }
        }

        noDuplFaces.push_back(
            mesh.faces[trianglesSorted[mesh.faces.size() - 1].fp]);

        mesh.faces = noDuplFaces;
        if (mesh.hasNormals)
            mesh.normalId = mesh.faces;
        if (mesh.hasTexCoords)
        {
            mesh.texId.clear();
            mesh.texId.reserve(mesh.faces.size());
            for (size_t i = 0; i < mesh.faces.size(); i++)
            {
                mesh.texId.push_back(Vector4d32(mesh.faces[i], 0));
            }
        }

}

void MeshFilter::removeDuplicatedVertices(Mesh3DDecorated &mesh)
{
        std::vector<Vector3dd> newVertices;
        std::vector<Vector3dd> newNormals;
        std::vector<Vector2dd> newTCoords;

        int index = 0;
        std::map<int, int> correspMap;


        for (auto it = mesh.vertexes.begin(); it != mesh.vertexes.end(); it++, ++index)
        {
            auto itInner = std::find(newVertices.begin(), newVertices.end(), *it);
            size_t itIndex = std::distance(newVertices.begin(), itInner);
            bool isNewNormals = false;
            bool isNewTCoords = false;
            if (!mesh.normalCoords.empty())
            {
                if (itIndex < newNormals.size())
                    isNewNormals = !newNormals[itIndex].notTooFar(mesh.normalCoords[index]);
                else
                    isNewNormals = true;
            }
            if (!mesh.textureCoords.empty())
            {
                if (itIndex < newTCoords.size())
                    isNewTCoords = !newTCoords[itIndex].notTooFar(mesh.textureCoords[index]);
                else
                    isNewTCoords = true;
            }

            // if current vertex is new or if its duplicate but his normal or tCoord differs from current -> push_back
            if (itInner == newVertices.end() || isNewNormals || isNewTCoords)
            {
                correspMap[index] = (int)newVertices.size();
                newVertices.push_back(*it);
                if (!mesh.normalCoords.empty())
                    newNormals.push_back(mesh.normalCoords[index]);
                if (!mesh.textureCoords.empty())
                    newTCoords.push_back(mesh.textureCoords[index]);
            }
            else
            {
                correspMap[index] = (int)itIndex;
            }
        }

        mesh.vertexes = newVertices;
        mesh.normalCoords = newNormals;
        mesh.textureCoords = newTCoords;
        for (size_t index = 0; index < mesh.faces.size(); ++index)
        {
            // form new triangle
            Vector3d32 newTr;
            for (int i = 0; i < 3; i++)
                newTr[i] = correspMap[mesh.faces[index][i]];

            // delete triangle from the mesh if triangle is degenerate
            if (newTr[0] == newTr[1] ||
                newTr[1] == newTr[2] ||
                newTr[2] == newTr[0])
            {
                mesh.faces.erase(mesh.faces.begin() + index);
                if (!mesh.normalCoords.empty())
                    mesh.normalId.erase(mesh.normalId.begin() + index);
                if (!mesh.textureCoords.empty())
                    mesh.texId.erase(mesh.texId.begin() + index);
                --index;
            }
            else
            {
                mesh.faces[index] = newTr;
                if (!mesh.normalCoords.empty())
                    mesh.normalId[index] = newTr;
                if (!mesh.textureCoords.empty())
                {
                    mesh.texId[index] = Vector4d32(newTr, 0);
                }
            }
        }
}

void getUnreferencedVertices(
    const Mesh3DDecorated& mesh,
    std::vector<bool>& unreferenced)
{
    unreferenced.resize(mesh.vertexes.size(), true);
    for (size_t i = 0; i < mesh.faces.size(); ++i)
    {
        unreferenced[mesh.faces[i][0]] = false;
        unreferenced[mesh.faces[i][1]] = false;
        unreferenced[mesh.faces[i][2]] = false;
    }
}

void MeshFilter::removeUnreferencedVertices(
    Mesh3DDecorated& mesh)
{
    const size_t vs_size = mesh.vertexes.size();
    const size_t ts_size = mesh.faces.size();

    // mark unreferenced vertices
    std::vector<bool> unreferenced(vs_size, true);
    getUnreferencedVertices(mesh, unreferenced);

    // compute new vertices indices
    std::vector<int> newVerticesIndices(vs_size);
    size_t next = 0;
    for (size_t i = 0; i < vs_size; ++i)
    {
        if (!unreferenced[i])
        {
            newVerticesIndices[i] = (int)next;
            ++next;
        }
    }

    // get new vectors
    // remove unreferenced vertices, normals, texture coordinates
    std::vector<Vector3dd> newVertices;
    std::vector<Vector3dd> newNormals;
    std::vector<Vector2dd> newUV;

    for (size_t i = 0; i < vs_size; ++i)
        if (!unreferenced[i])
            newVertices.push_back(mesh.vertexes[i]);

    if (vs_size == mesh.normalCoords.size())
        for (size_t i = 0; i < vs_size; ++i)
            if (!unreferenced[i])
                newNormals.push_back(mesh.normalCoords[i]);

    if (vs_size == mesh.textureCoords.size())
        for (size_t i = 0; i < vs_size; ++i)
            if (!unreferenced[i])
                newUV.push_back(mesh.textureCoords[i]);

    // write new vectors
    mesh.vertexes = newVertices;
    mesh.normalCoords = newNormals;
    mesh.textureCoords = newUV;

    // reindex triangles
    for (size_t i = 0; i < ts_size; ++i)
    {
        const size_t i0 = mesh.faces[i][0];
        const size_t i1 = mesh.faces[i][1];
        const size_t i2 = mesh.faces[i][2];

        const Vector3d32 face(
            newVerticesIndices[i0],
            newVerticesIndices[i1],
            newVerticesIndices[i2]);
        mesh.faces[i] = face;
    }

    if (mesh.hasNormals)
        mesh.normalId = mesh.faces;
    if (mesh.hasTexCoords) {
        mesh.texId.clear();
        mesh.texId.reserve(mesh.faces.size());
        for (size_t i = 0; i < mesh.faces.size(); i++)
        {
            mesh.texId.push_back(Vector4d32(mesh.faces[i], 0));
        }
    }

}

void MeshFilter::removeIsolatedPieces(Mesh3DDecorated &mesh, unsigned minCountOfFaces)
{
    // for each position get indices of triangles which contains it
        std::vector<std::vector<size_t>> v(mesh.vertexes.size());

        // vector of tr_indexes for every vertex
        for (size_t i = 0; i < mesh.faces.size(); ++i)
        {
            v[mesh.faces[i][0]].push_back(i);
            v[mesh.faces[i][1]].push_back(i);
            v[mesh.faces[i][2]].push_back(i);
        }
        std::vector<size_t> tr_stack; // triangles stack

        // component's number for every triangle
        std::vector<int> components(mesh.faces.size(), -1);

        // go through stack to create one connected component
        auto goThroughStack = [&](std::vector<size_t>& tr_stack,
            int curCompIndex)
        {
            // if stack isn't empty there are not visited triangles in the comp
            while(tr_stack.size() > 0)
            {
                // get triangle index from stack
                const int last = (int)tr_stack.back();
                tr_stack.pop_back();

                // if triangle is not visited, else it has a component number and
                // triangles on it's vertices are already in stack
                if (components[last] == -1) //not visited
                {
                    components[last] = curCompIndex;
                    const Vector3d32 tr =
                        mesh.faces[last];

                    // add not visited triangles
                    size_t i1 = tr[0];
                    for (size_t i = 0; i < v[i1].size(); ++i)
                        if (components[v[i1][i]] == -1)
                            tr_stack.push_back(v[i1][i]);

                    size_t i2 = tr[1];
                    for (size_t i = 0; i < v[i2].size(); ++i)
                        if (components[v[i2][i]] == -1)
                            tr_stack.push_back(v[i2][i]);

                    size_t i3 = tr[2];
                    for (size_t i = 0; i < v[i3].size(); ++i)
                        if (components[v[i3][i]] == -1)
                            tr_stack.push_back(v[i3][i]);
                }
            }
        };

        int curCompIndex = 0;

        // every not visited triangle is the start of new component
        for (size_t i = 0; i < components.size(); ++i)
            if (components[i] == -1) // new component
            {
                tr_stack.push_back(i);
                goThroughStack(tr_stack, curCompIndex);
                ++curCompIndex;
            }

        // compute counts of faces by each component
        std::vector<size_t> trianglesCountsInComponents(curCompIndex, 0);
        for (size_t i = 0; i < components.size(); ++i)
            if (components[i] != -1)
                ++trianglesCountsInComponents[components[i]];

        // fill triangles vector by count of faces
        std::vector<Vector3d32> filteredTriangles;
        for (size_t i = 0; i < components.size(); ++i)
        {
            if (trianglesCountsInComponents[components[i]] < minCountOfFaces)
                continue;
            filteredTriangles.push_back(mesh.faces[i]);
        }


        //rewrite faces
        mesh.faces = filteredTriangles;

        if (mesh.hasNormals)
            mesh.normalId = mesh.faces;
        if (mesh.hasTexCoords) {
            mesh.texId.clear();
            mesh.texId.reserve(mesh.faces.size());
            for (size_t i = 0; i < mesh.faces.size(); i++)
            {
                mesh.texId.push_back(Vector4d32(mesh.faces[i], 0));
            }
        }

        // assume that all component faces and vertices have to be removed
        // now triangles removed, but vertices on them is not
        // this vertices are unreferenced, remove them
        if (mesh.faces.size() == 0)
            MeshFilter::removeUnreferencedVertices(mesh);
}

void MeshFilter::removeZeroAreaFaces(Mesh3DDecorated &mesh)
{
    for (size_t i = 0; i < mesh.faces.size(); ++i)
    {
        if (mesh.getFaceAsTrinagle(i).getArea() <= 1e-5)
        {
            mesh.faces.erase(mesh.faces.begin() + i);
            i--;
        }
    }


    //all triangles remove, but vertices on them is not
    // this vertices are unreferenced, remove them
    if (mesh.faces.size() == 0)
        MeshFilter::removeUnreferencedVertices(mesh);
    if (mesh.hasNormals)
        mesh.normalId = mesh.faces;
    if (mesh.hasTexCoords) {
        mesh.texId.clear();
        mesh.texId.reserve(mesh.faces.size());
        for (size_t i = 0; i < mesh.faces.size(); i++)
        {
            mesh.texId.push_back(Vector4d32(mesh.faces[i], 0));
        }
    }
}

} // namespace corecvs



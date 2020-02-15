#include "meshCache.h"

namespace corecvs
{

Vector3df toFloat(const Vector3dd &v)
{
    return Vector3df(v.x(), v.y(), v.z());
}

Vector2df toFloat(const Vector2dd &v)
{
    return Vector2df(v.x(), v.y());
}


#if 0
size_t SceneShadedOpenGLCache::getMaterialID(Mesh3DDecorated *mesh, size_t mFace)
{
    int material = mesh->texId[faceNum][Mesh3DDecorated::MATERIAL_NUM];
    cout << "M: " << material << endl;
    if (material < 0)
       return mesh->materials.size();
    if (material >= (int)mesh->materials.size() + 1);
        continue;

}
#endif


void SceneShadedOpenGLCache::cache(Mesh3DDecorated *mesh, const Draw3dParameters &mParameters)
{
    /* Vertexes */
    {
        positions.resize(mesh->vertexes.size());
        for (size_t i = 0; i < mesh->vertexes.size(); i++)
        {
            positions[i] = toFloat(mesh->vertexes[i]);
        }
    }

    /* Edges */
    {
        RGBColor edgeColor = RGBColor(mParameters.edgeColor());

        size_t glEdgesNum = mesh->edges.size();
        size_t glFacesNum = mesh->faces.size();

        edgePositions   .resize(glEdgesNum * 2 + glFacesNum * 3);
        edgeVertexColors.resize(glEdgesNum * 2 + glFacesNum * 3);
        edgeColors      .resize(glEdgesNum * 2 + glFacesNum * 3);
        edgeIds         .resize((glEdgesNum + glFacesNum * 3) * 2);


        size_t edgeCount = 0;
        size_t outCount = 0;

        for (size_t edgeNum = 0; edgeNum < mesh->edges.size(); edgeNum++, outCount += 2, edgeCount += 2)
        {
            Vector2d32 pointId = mesh->edges[edgeNum];
            edgePositions[outCount    ] = positions[pointId.x()];
            edgePositions[outCount + 1] = positions[pointId.y()];

            if (mesh->hasColor && !mParameters.edgeColorOverride()) {
                edgeVertexColors[outCount    ] = mesh->vertexesColor[pointId.x()];
                edgeVertexColors[outCount + 1] = mesh->vertexesColor[pointId.y()];

                edgeColors[outCount    ] = mesh->edgesColor[edgeNum];
                edgeColors[outCount + 1] = mesh->edgesColor[edgeNum];
            } else {
                edgeVertexColors[outCount    ] = edgeColor;
                edgeVertexColors[outCount + 1] = edgeColor;

                edgeColors[outCount    ] = edgeColor;
                edgeColors[outCount + 1] = edgeColor;
            }

            edgeIds[edgeCount    ] = outCount;
            edgeIds[edgeCount + 1] = outCount + 1;
        }

        for (size_t faceNum = 0; faceNum < mesh->faces.size(); faceNum++, outCount += 3, edgeCount += 6)
        {
            Vector3d32 pointId = mesh->faces[faceNum];

            edgePositions[outCount    ] = positions[pointId.x()];
            edgePositions[outCount + 1] = positions[pointId.y()];
            edgePositions[outCount + 2] = positions[pointId.z()];


            if (mesh->hasColor && !mParameters.edgeColorOverride()) {
                edgeColors.push_back(mesh->facesColor[faceNum]);
                edgeColors.push_back(mesh->facesColor[faceNum]);
                edgeColors.push_back(mesh->facesColor[faceNum]);
            } else {
                edgeColors.push_back(edgeColor);
                edgeColors.push_back(edgeColor);
                edgeColors.push_back(edgeColor);
            }

            edgeIds[edgeCount    ] = outCount;
            edgeIds[edgeCount + 1] = outCount + 1;
            edgeIds[edgeCount + 2] = outCount + 1;
            edgeIds[edgeCount + 3] = outCount + 2;
            edgeIds[edgeCount + 4] = outCount + 2;
            edgeIds[edgeCount + 5] = outCount;
        }

    }

    /* Faces */
    {

        RGBColor faceColor = RGBColor(mParameters.faceColor());

        /* We should divide faces into materials */

        /* First pass. Compute number of faces for each material */
        vector<int> matrialSizes;
        matrialSizes.resize(mesh->materials.size() + 1);
        for (size_t faceNum = 0; faceNum < mesh->faces.size(); faceNum++)
        {
            int material = mesh->texId[faceNum][Mesh3DDecorated::MATERIAL_NUM];
            //cout << "M: " << material << endl;
            if (material < 0)
                material = mesh->materials.size();
            if (material >= (int)mesh->materials.size() + 1)
                continue;
            matrialSizes[material]++;
        }

        /* Prepare arrays of the caches */
        faceCache.resize(matrialSizes.size());
        for (size_t mCount = 0; mCount < matrialSizes.size(); mCount++)
        {
            size_t glFaceNum = matrialSizes[mCount];
            SceneShadedFaceCache &curFaceCache = faceCache[mCount];

            SYNC_PRINT(( "Material: %d - size - %d <%s>\n", (int)mCount, (int)glFaceNum, (mCount < mesh->materials.size()) ? mesh->materials[mCount].name.c_str() : "Additional" ));

            curFaceCache.facePositions   .reserve(glFaceNum * 3);
            curFaceCache.faceVertexColors.reserve(glFaceNum * 3);
            curFaceCache.faceColors      .reserve(glFaceNum * 3);
            curFaceCache.faceTexCoords   .reserve(glFaceNum * 3);

            curFaceCache.faceTexNums     .reserve(glFaceNum * 3);

            curFaceCache.faceNormals     .reserve(glFaceNum * 3);
            curFaceCache.faceIds         .reserve(glFaceNum * 3);
        }

        /* Second path */
        for (size_t faceNum = 0; faceNum < mesh->faces.size(); faceNum++)
        {
            int material = mesh->texId[faceNum][Mesh3DDecorated::MATERIAL_NUM];
            if (material < 0)
                material = mesh->materials.size();
            if (material >= (int)mesh->materials.size() + 1)
                continue;

            //cout << "Material:" << material << endl;
            SceneShadedFaceCache &curFaceCache = faceCache[material];
            Vector3d32 pointId = mesh->faces[faceNum];

            curFaceCache.facePositions.push_back(positions[pointId.x()]);
            curFaceCache.facePositions.push_back(positions[pointId.y()]);
            curFaceCache.facePositions.push_back(positions[pointId.z()]);

            if (mesh->hasColor && !mParameters.faceColorOverride()) {
                curFaceCache.faceVertexColors.push_back(mesh->vertexesColor[pointId.x()]);
                curFaceCache.faceVertexColors.push_back(mesh->vertexesColor[pointId.y()]);
                curFaceCache.faceVertexColors.push_back(mesh->vertexesColor[pointId.z()]);

                curFaceCache.faceColors.push_back(mesh->facesColor[faceNum]);
                curFaceCache.faceColors.push_back(mesh->facesColor[faceNum]);
                curFaceCache.faceColors.push_back(mesh->facesColor[faceNum]);
            } else {
                curFaceCache.faceVertexColors.push_back(faceColor);
                curFaceCache.faceVertexColors.push_back(faceColor);
                curFaceCache.faceVertexColors.push_back(faceColor);

                curFaceCache.faceColors.push_back(faceColor);
                curFaceCache.faceColors.push_back(faceColor);
                curFaceCache.faceColors.push_back(faceColor);
            }

            if (mesh->hasTexCoords)
            {
                Vector4d32 texId = mesh->texId[faceNum];
                if (texId.x() < 0 || texId.y() < 0 || texId.z() < 0)
                {
                    curFaceCache.faceTexCoords.push_back(Vector2df::Zero());
                    curFaceCache.faceTexCoords.push_back(Vector2df::Zero());
                    curFaceCache.faceTexCoords.push_back(Vector2df::Zero());
                } else {
                    curFaceCache.faceTexCoords.push_back(toFloat(mesh->textureCoords[texId.x()]));
                    curFaceCache.faceTexCoords.push_back(toFloat(mesh->textureCoords[texId.y()]));
                    curFaceCache.faceTexCoords.push_back(toFloat(mesh->textureCoords[texId.z()]));
                }

                curFaceCache.faceTexNums.push_back(texId[3]);
                curFaceCache.faceTexNums.push_back(texId[3]);
                curFaceCache.faceTexNums.push_back(texId[3]);
            } else {
                curFaceCache.faceTexCoords.push_back(Vector2df::Zero());
                curFaceCache.faceTexCoords.push_back(Vector2df::Zero());
                curFaceCache.faceTexCoords.push_back(Vector2df::Zero());
            }

            if (mesh->hasNormals)
            {
                Vector3d32 normalId = mesh->normalId[faceNum];
                curFaceCache.faceNormals.push_back(toFloat(mesh->normalCoords[normalId.x()]));
                curFaceCache.faceNormals.push_back(toFloat(mesh->normalCoords[normalId.y()]));
                curFaceCache.faceNormals.push_back(toFloat(mesh->normalCoords[normalId.z()]));
            } else {
                curFaceCache.faceNormals.push_back(Vector3df::Zero());
                curFaceCache.faceNormals.push_back(Vector3df::Zero());
                curFaceCache.faceNormals.push_back(Vector3df::Zero());

            }

            curFaceCache.faceIds.push_back((uint32_t)(curFaceCache.faceIds.size()));
            curFaceCache.faceIds.push_back((uint32_t)(curFaceCache.faceIds.size()));
            curFaceCache.faceIds.push_back((uint32_t)(curFaceCache.faceIds.size()));
        }
    }
}

void SceneShadedOpenGLCache::clearCache()
{
    positions.clear();
    edgePositions.clear();
    edgeVertexColors.clear();
    edgeColors.clear();
    edgeIds.clear();
    faceCache.clear();
}

} // namespace corecvs

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
        int material = mesh->texId[faceNum][4];
        if (material < 0)
            material = mesh->materials.size();
        if (material >= (int)mesh->materials.size() + 1);
            continue;
        matrialSizes[material]++;
    }

    /* Prepare arrays of the caches */
    faceCache.resize(matrialSizes.size());
    for (size_t mCount = 0; mCount < matrialSizes.size(); mCount++)
    {
        size_t glFaceNum = matrialSizes[mCount];
        SceneShadedFaceCache &curFaceCache = faceCache[mCount];

        curFaceCache.facePositions   .resize(glFaceNum * 3);
        curFaceCache.faceVertexColors.resize(glFaceNum * 3);
        curFaceCache.faceColors      .resize(glFaceNum * 3);
        curFaceCache.faceTexCoords   .resize(glFaceNum * 3);

        curFaceCache.faceTexNums     .resize(glFaceNum * 3);

        curFaceCache.faceNormals     .resize(glFaceNum * 3);
        curFaceCache.faceIds         .resize(glFaceNum * 3);
    }

    /* Second path */
    for (size_t faceNum = 0, outCount = 0; faceNum < mesh->faces.size(); faceNum++, outCount += 3)
    {
        int material = mesh->texId[faceNum][4];
        if (material < 0)
            material = mesh->materials.size();
        if (material >= (int)mesh->materials.size() + 1);
            continue;

        SceneShadedFaceCache &curFaceCache = faceCache[material];
        Vector3d32 pointId = mesh->faces[faceNum];

        curFaceCache.facePositions[outCount    ] = positions[pointId.x()];
        curFaceCache.facePositions[outCount + 1] = positions[pointId.y()];
        curFaceCache.facePositions[outCount + 2] = positions[pointId.z()];

        if (mesh->hasColor && !mParameters.faceColorOverride()) {
            curFaceCache.faceVertexColors[outCount    ] = mesh->vertexesColor[pointId.x()];
            curFaceCache.faceVertexColors[outCount + 1] = mesh->vertexesColor[pointId.y()];
            curFaceCache.faceVertexColors[outCount + 2] = mesh->vertexesColor[pointId.z()];

            curFaceCache.faceColors[outCount    ] = mesh->facesColor[faceNum];
            curFaceCache.faceColors[outCount + 1] = mesh->facesColor[faceNum];
            curFaceCache.faceColors[outCount + 2] = mesh->facesColor[faceNum];
        } else {
            curFaceCache.faceVertexColors[outCount    ] = faceColor;
            curFaceCache.faceVertexColors[outCount + 1] = faceColor;
            curFaceCache.faceVertexColors[outCount + 2] = faceColor;

            curFaceCache.faceColors[outCount    ] = faceColor;
            curFaceCache.faceColors[outCount + 1] = faceColor;
            curFaceCache.faceColors[outCount + 2] = faceColor;
        }

        if (mesh->hasTexCoords)
        {
            Vector4d32 texId = mesh->texId[faceNum];
            curFaceCache.faceTexCoords[outCount    ] = toFloat(mesh->textureCoords[texId.x()]);
            curFaceCache.faceTexCoords[outCount + 1] = toFloat(mesh->textureCoords[texId.y()]);
            curFaceCache.faceTexCoords[outCount + 2] = toFloat(mesh->textureCoords[texId.z()]);

            curFaceCache.faceTexNums[outCount    ] = texId[3];
            curFaceCache.faceTexNums[outCount + 1] = texId[3];
            curFaceCache.faceTexNums[outCount + 2] = texId[3];
        } else {
            curFaceCache.faceTexCoords[outCount    ] = Vector2df::Zero();
            curFaceCache.faceTexCoords[outCount + 1] = Vector2df::Zero();
            curFaceCache.faceTexCoords[outCount + 2] = Vector2df::Zero();
        }

        if (mesh->hasNormals)
        {
            Vector3d32 normalId = mesh->normalId[faceNum];
            curFaceCache.faceNormals[outCount    ] = toFloat(mesh->normalCoords[normalId.x()]);
            curFaceCache.faceNormals[outCount + 1] = toFloat(mesh->normalCoords[normalId.y()]);
            curFaceCache.faceNormals[outCount + 2] = toFloat(mesh->normalCoords[normalId.z()]);
        }

        curFaceCache.faceIds[outCount    ] = (uint32_t)(outCount);
        curFaceCache.faceIds[outCount + 1] = (uint32_t)(outCount + 1);
        curFaceCache.faceIds[outCount + 2] = (uint32_t)(outCount + 2);
    }
}

}
} // namespace corecvs

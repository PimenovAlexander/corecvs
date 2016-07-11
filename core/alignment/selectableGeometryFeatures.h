#pragma once

#include <algorithm>
#include <vector>
#include <sstream>
#include "rgb24Buffer.h"
#include "pointObservation.h"


namespace corecvs {

using std::vector;

class ObservationList : public std::vector<PointObservation>
{
public:
    int patternIdentity = -1;

    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            int sizeToVisit = (int)this->size();
            visitor.visit(sizeToVisit, 0, "size");
            resize(sizeToVisit);

            for (int i = 0; i < sizeToVisit; i++)
            {
                std::ostringstream ss;
                ss << "a[" << i << "]";
                visitor.visit(operator [](i), PointObservation(), ss.str().c_str());
            }
            visitor.visit(patternIdentity, -1, "patternIdentity");
        }
};

struct NamePointObservation : public PointObservation
{
    std::string name;
    void        *payload;
};

class SelectableGeometryFeatures
{
public:
    struct VertexPath;

    struct Vertex
    {
        bool        mSelected;
        Vector2dd   position;
        double      weight;
        VertexPath *ownerPath;

        explicit Vertex(const Vector2dd &_position = Vector2dd(0.0));

        bool isSelected();

        bool isInPath() {
            return (ownerPath != NULL);
        }
    };

    typedef Vector2dd * TargetPoint;

    /** Points **/
    vector<Vertex*> mPoints;
    vector<Vertex*> mSelectedPoints;

    Vertex *findClosest(const Vector2dd &position);
    //Vertex *lastVertex();

    struct VertexPath
    {
        bool mSelected;
        vector<Vertex *> vertexes;

        bool isSelected();
        bool isEmpty();
        int length();
    };

    /** Pathes **/
    vector<VertexPath *> mPaths;
    vector<VertexPath *> mSelectedPaths;

    VertexPath *appendNewPath();
    void deletePath(VertexPath *path);

    void addVertexToPath(Vertex *vertex, VertexPath *path);
    void removeVertexFromPath(Vertex *vertex, bool purgeEmptyPath = true);

    Vertex* appendNewVertex(const Vector2dd &point = Vector2dd(0.0));
    void deleteVertex(const Vector2dd &point);
    void deleteVertex(Vertex *vertex);

    void addSelection   (VertexPath *path);
    void removeSelection(VertexPath *path);

    void addSelection   (Vertex *vertex);
    void removeSelection(Vertex *vertex);

    void clearAll();

    void deselectAllPoints();
    void deselectAllPath();
    void deselectAll();

    void addPathFrom(const SelectableGeometryFeatures &other);
    void addAllLinesFromObservationList(const ObservationList &list);

    bool isEmpty() const { return mPaths.empty(); }


    /****/
    bool hasSinglePointsSelected();

    SelectableGeometryFeatures();
    virtual ~SelectableGeometryFeatures();

    /* Helper function */
    vector<vector<Vector2dd> > getLines();
    /**/
    void print();
    void draw(corecvs::RGB24Buffer &buffer);
};

} // namespace corecvs

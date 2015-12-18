#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <vector>
#include <deque>
#include <string>


class GraphValue
{
public:
    double value;
    bool isValid;

    explicit GraphValue(double _value, bool _isValid = true) :
        value(_value), isValid(_isValid)
    {}
};

class GraphHistory : public std::deque<GraphValue>
{
public:
    bool isSelected;
    std::string name;
};

class GraphData {
public:
    size_t limit;
    std::vector<GraphHistory> mData;

    GraphData() :
        limit(1000)
    {}

    void addGraphPoint(     unsigned graphId, double value, bool isValid);
    void addGraphPoint(std::string graphName, double value, bool isValid);

    unsigned getGraphId(std::string name);

    std::string getGraphName(unsigned id);

};

#endif // GRAPHDATA_H

#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <vector>
#include <deque>
#include <string>
#include <limits>
#include <algorithm>   // for std::min, std::max


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

    double minValue () {
        double result = std::numeric_limits<double>::max();
        for(GraphValue g: *this) {
            if (g.isValid) result = std::min(result, g.value);
        }
        return result;
    }

    double maxValue () {
        double result = std::numeric_limits<double>::lowest();
        for(GraphValue g: *this) {
            if (g.isValid) result = std::max(result, g.value);
        }
        return result;
    }
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

#ifndef FILTERGRAPH_H
#define FILTERGRAPH_H

#include <vector>
#include <algorithm>
#include <string.h>
#include <sstream>

#include "filters/legacy/blocks/filterBlock.h"
#include "filters/legacy/filtersCollection.h"
#include "stats/calculationStats.h"

namespace corecvs
{
using std::vector;
//using namespace tinyxml2;

//class CompoundFilter;

class FilterGraph
{
public:
    vector<FilterBlock*> blocks;
    int blockCounter;
    vector<Pin*> inputs;
    vector<Pin*> outputs;
    FiltersCollection* collection;
    FilterGraph* parent;

    /* Execution statistics */
    Statistics *stats;

    FilterGraph(FiltersCollection* _collection, FilterGraph* _parent) :
        blockCounter(0)
      , collection(_collection)
      , parent(_parent)
      , stats(NULL)
    {}

    ~FilterGraph();
    void clear();

    FilterBlock* addBlock(FilterBlock *block);
    void removeBlock(FilterBlock* block);
    bool connect(Pin* startPin, Pin* endPin);
    static void breakConnection(Pin* startPin, Pin* endPin);
    void removeConnections(Pin* pin);

    class NamePredicate
    {
        const char *mName;
    public:
        NamePredicate(const char *name) : mName(name) {}

        bool operator() (FilterBlock *block)
        {
            return block->getFullName() == mName;
        }
    };

    FilterBlock *blockByName(const char *name)
    {
        vector<FilterBlock *>::iterator block = std::find_if(blocks.begin(), blocks.end(), NamePredicate(name));
        if (block == blocks.end()) {
            return NULL;
        }
        return *block;
    }

    void deserialize(const char*);
    void deserialize(XMLDocument*);
    XMLDocument* serialize();
    void serialize(XMLPrinter*);

    FilterGraph* ultimateParent();
    void renewCompoundFilters(int typeId, FilterBlock* blockToIgnore);

    bool topologicSort();
    void execute();
    void clearAllData();

    void print();
    void saveToFile(const char* filename);
    void loadFromFile(const char* filename);
}; // FilterGraph

} /* namespace corecvs */

#endif // FILTERGRAPH_H

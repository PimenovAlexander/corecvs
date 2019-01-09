#ifndef FILTERSCOLLECTION_H
#define FILTERSCOLLECTION_H

#include "core/filters/legacy/blocks/filterBlock.h"
#include "core/tinyxml2/tinyxml2.h"       // for XMLDocument here
#include <set>

namespace corecvs
{
using std::set;
using tinyxml2::XMLDocument;       // for XMLDocument here

class FiltersCollection
{
public:
    enum FilterTypes{ GAIN_OFFSET_FILTER = 0
                    , BIT_SELECTOR_FILTER
                    , SOBEL_FILTER
                    , CANNY_FILTER
                    , BACKGROUND_FILTER
                    , OPERATION_FILTER
                    , TXT_FILTER
                    , INPUT_FILTER
                    , OUTPUT_FILTER
                    , BINARIZE_BLOCK
                    , MASK_FILTER_BLOCK
                    , THICKENING_BLOCK
    };
    static const char* typenames[];
    static unsigned int numberOfFilterTypes();

    FiltersCollection();
    ~FiltersCollection();

    void getContainedBlocks(const char* xml, int n);
    void getContainedBlocks(XMLDocument* doc, int n);
    int getRowByName(const char*);
    FilterBlock* getBlockFromRow(const int row, int id = -1);
    FilterBlock* getBlockFromName(const char* type, int id = -1);

    void addFilterType(const char* _name, const char* _filename)
    {
        XMLDocument doc;
        XMLError err = doc.LoadFile(_filename);
        if (err != XML_SUCCESS)  return;

        CompoundFilterType ct((int)compoundFilters.size(), _name, _filename);
        compoundFilters.push_back(ct);

        getContainedBlocks(&doc, (int)(compoundFilters.size() - 1));
        recalculateDependencies();
////        changed = true;
    }

    void recalculateDependencies();
    bool dependsOnMe(int me, int filter);

    void saveCompoundFilters(void);

    class CompoundFilterType
    {
    public:
        CompoundFilterType(int _id, const char* _name, const char* _filename) :
            /*id(_id),*/ name(_name), filename(_filename)
        {
            dependOnMe.insert(_id);
        }

 //       int id;
        string name;
        string filename;
        set<int> contain;
        set<int> dependOnMe;
    };

    bool changed;
    vector<CompoundFilterType> compoundFilters;
};

} /* namespace corecvs */

#endif // FILTERSCOLLECTION_H

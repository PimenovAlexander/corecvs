#include "core/filters/filtersCollection.h"

#include "core/filters/sobelFilter.h"
#include "core/filters/gainOffsetFilter.h"
#include "core/filters/bitSelectorFilter.h"
#include "core/filters/cannyFilter.h"
#include "core/filters/backgroundFilter.h"
#include "core/filters/operationFilter.h"
#include "core/filters/inputFilter.h"
#include "core/filters/outputFilter.h"
#include "core/filters/txtFilter.h"
#include "core/filters/binarizeBlock.h"
#include "core/filters/thickeningBlock.h"
#include "core/filters/maskFilterBlock.h"
#include "core/filters/blocks/compoundFilter.h"

namespace corecvs
{

const char* FiltersCollection::typenames[] =
{
     "Gain Offset"          /**< GAIN_OFFSET_FILTER  */
    ,"Bit Selector"         /**< BIT_SELECTOR_FILTER */
    ,"Sobel Filter"         /**< SOBEL_FILTER        */
    ,"Canny Filter"         /**< CANNY_FILTER        */
    ,"Background Filter"    /**< BACKGROUND_FILTER   */
    ,"Operation Filter"     /**< OPERATION_FILTER    */
    ,"Text Filter"          /**< TXT_FILTER          */
    ,"Input"                /**< INPUT_FILTER        */
    ,"Output"               /**< OUTPUT_FILTER       */
    ,"Binarize Block"       /**< BINARIZE_BLOCK      */
    ,"Mask Filter Block"    /**< MASK_FILTER_BLOCK   */
    ,"Thickening Block"     /**< THICKENING_BLOCK    */
};

unsigned int FiltersCollection::numberOfFilterTypes()
{
    return sizeof(typenames)/sizeof(char*);
}

FiltersCollection::FiltersCollection() : changed(false)
{
    XMLDocument doc;
    XMLError err = doc.LoadFile("collection.conf");
    if (err != XML_SUCCESS)  return;

    XMLNode* p1 = doc.FirstChild();
    while(p1)
    {   compoundFilters.push_back(CompoundFilterType(p1->ToElement()->IntAttribute("id"),
                                                     p1->ToElement()->Attribute("typename"),
                                                     p1->ToElement()->Attribute("filename")));
        p1 = p1->NextSibling();
    }
    for (unsigned int i = 0; i < compoundFilters.size(); i++)
    {
        XMLError err = doc.LoadFile(compoundFilters[i].filename.c_str());
        if (err != XML_SUCCESS)
            continue;
        getContainedBlocks(&doc, i);
    }
    recalculateDependencies();
}

void FiltersCollection::getContainedBlocks(XMLDocument* doc, int n)
{
    XMLNode* p1 = doc->FirstChild();
    while(p1)
    {
        int row = getRowByName(p1->ToElement()->Attribute("type"));
        if (row == -1) {
            continue;
        }
        if ((uint)row >= numberOfFilterTypes()) {
            compoundFilters[n].contain.insert(row - numberOfFilterTypes());
        }
        p1 = p1->NextSibling();
    }
}

void FiltersCollection::getContainedBlocks(const char* xml, int n)
{
    XMLDocument doc;
    XMLError errorId = doc.Parse(xml);
    if (doc.Error())
    {
        cout << "Parsing Error: " << errorId << endl;
        return;
    }
    getContainedBlocks(&doc, n);
}

FiltersCollection::~FiltersCollection()
{
    if (!changed)
        return;

    saveCompoundFilters();
}

void FiltersCollection::saveCompoundFilters(void)
{
    XMLDocument doc;
    for (unsigned int i = 0; i < compoundFilters.size(); i++)
    {
        XMLElement* type = doc.NewElement("filter");
        type->SetAttribute("typename", compoundFilters[i].name.c_str());
        type->SetAttribute("filename", compoundFilters[i].filename.c_str());
        doc.InsertEndChild(type);
    }
    doc.SaveFile("collection.conf");
}

int FiltersCollection::getRowByName(const char* type)
{
    for (unsigned int row = 0; row < numberOfFilterTypes(); row++)
    {
        if (!strcmp(typenames[row], type))
            return row;
    }

    for (unsigned int row = 0; row < compoundFilters.size(); row++)
    {
        if (!strcmp(compoundFilters[row].name.c_str(), type))
            return row + numberOfFilterTypes();
    }

    return -1;
}

FilterBlock* FiltersCollection::getBlockFromRow(const int row, int id)
{
    switch (row)
    {
    case -1:
        return NULL;
    case GAIN_OFFSET_FILTER:
        return new GainOffsetFilter(id);
    case BIT_SELECTOR_FILTER:
        return new BitSelectorFilter(id);
    case SOBEL_FILTER:
        return new SobelFilter(id);
    case CANNY_FILTER:
        return new CannyFilter(id);
    case BACKGROUND_FILTER:
        return new BackgroundFilter(id);
    case OPERATION_FILTER:
        return new OperationFilter(id);
    case TXT_FILTER:
        return new TxtFilter(id);
    case INPUT_FILTER:
        return new InputFilter(id);
    case OUTPUT_FILTER:
        return new OutputFilter(id);
    case BINARIZE_BLOCK:
        return new BinarizeBlock(id);
    case MASK_FILTER_BLOCK:
        return new MaskFilterBlock(id);
    case THICKENING_BLOCK:
        return new ThickeningBlock(id);
    default:
        return new CompoundFilter(this, row - numberOfFilterTypes(), id);
    } // switch
}

FilterBlock* FiltersCollection::getBlockFromName(const char* type, int id)
{
    int row = getRowByName(type);
    return getBlockFromRow(row, id);
}

//static set<int> FiltersCollection::setUnion(set<int> s1, set<int> s2)
//{
//set<int> s3;
//set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), insert_iterator<set<int> >(s3, s3.begin()));
//return s3;
//}

void FiltersCollection::recalculateDependencies()
{
    for (unsigned int i = 0; i < compoundFilters.size(); i++){
        compoundFilters[i].dependOnMe.clear();
        compoundFilters[i].dependOnMe.insert(i/*compoundFilters[i].id*/);
    }

    // Relation "depends on" is defined by two properties as follows:
    // 1) Each block depends on itself;
    // 2) If A contains B then A depends on each block B depends on

    // set dependOnMe for a given block - set of all blocks which depend on the given block

    // Here for each block i and for each block j which is contained in i we put
    // all blocks, which depend on block i into j-th block dependencies
\
    int counter;
    do
    {  counter = 0;
       for (unsigned int i = 0; i < compoundFilters.size(); i++)
         for (set<int>::iterator itcontained = compoundFilters[i].contain.begin();
              itcontained != compoundFilters[i].contain.end();
              ++itcontained)
            for (set<int>::iterator itdepend = compoundFilters[i].dependOnMe.begin();
                 itdepend != compoundFilters[i].dependOnMe.end();
                 ++itdepend)
               if (compoundFilters[*itcontained].dependOnMe.insert(*itdepend).second)
                   counter++;
    }while(counter > 0);
    changed = true;
}

bool FiltersCollection::dependsOnMe(int me, int filter)
{
    return compoundFilters[me].dependOnMe.find(filter) != compoundFilters[me].dependOnMe.end();
}


} /* namespace corecvs */

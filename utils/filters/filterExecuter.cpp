/**
 * \file filterExecuter.cpp
 * \brief Add Comment Here
 *
 * \date Jun 15, 2012
 * \author alexander
 */

#include "filterExecuter.h"

namespace corecvs
{

FilterExecuter::FilterExecuter(const FilterSelectorParameters &params)
{
    setParameters(&params);
}

FilterExecuter::FilterExecuter()
{}

bool FilterExecuter::setParameters(const void *newParameters)
{
    clear();
    FilterSelectorParameters *params = (FilterSelectorParameters*) newParameters;
    for (unsigned i = 0; i < params->filters.size(); i ++)
    {
        //printf("Adding filter:%s\n", params->filters[i]);

        AbstractFilter *filter = FilterSelector::executerFromName(params->filters[i]);
        filter->setParameters(params->parameters[i]);
        mFilters.push_back(filter);
    }
    return true;
}

G12Buffer *FilterExecuter::filter (G12Buffer *input)
{
    qDebug("FilterExecuter::filter() called");

    G12Buffer *result = input;
    for (unsigned i = 0; i < mFilters.size(); i ++)
    {
        G12Buffer *processed = mFilters[i]->filter(result);
        if (result != input)
        {
            delete result;
        }
        if (!processed->verify())
            cout << "Executer::filter() executed " << mFilters[i]->getName() << " returned invalid result" << endl;

        result = processed;
    }

    return result;
}

void FilterExecuter::clear()
{
    vector<AbstractFilter *>::iterator filterIterator;
    for (filterIterator = mFilters.begin(); filterIterator != mFilters.end(); ++filterIterator)
    {
        delete (*filterIterator);
        *filterIterator = NULL;
    }
    mFilters.clear();

}

FilterExecuter::~FilterExecuter()
{
    clear();
}

} /* namespace corecvs */

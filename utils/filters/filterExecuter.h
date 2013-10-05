#ifndef FILTER_EXECUTER_H_
#define FILTER_EXECUTER_H_
/**
 * \file filterExecuter.h
 * \brief Add Comment Here
 *
 * \date Jun 15, 2012
 * \author alexander
 */
#include <vector>
#include "filterSelector.h"

namespace corecvs {

using std::vector;

class FilterExecuter : AbstractFilter
{
public:
    FilterExecuter(const FilterSelectorParameters &params);
    FilterExecuter();

    vector<AbstractFilter *> mFilters;

    virtual bool setParameters(const void *newParameters);
    virtual G12Buffer *filter (G12Buffer *input);

    void clear();

    virtual ~FilterExecuter();
};

} /* namespace corecvs */

#endif /* FILTER_EXECUTER_H_ */

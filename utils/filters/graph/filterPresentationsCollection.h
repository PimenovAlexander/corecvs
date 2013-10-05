#ifndef FILTERPRESENTATIONSCOLLECTION_H
#define FILTERPRESENTATIONSCOLLECTION_H

#include "filterBlockPresentation.h"
#include "compoundBlockPresentation.h"

class FilterPresentationsCollection : public QObject
{
Q_OBJECT

public:
    FilterPresentationsCollection(FiltersCollection* collection) : fCollection(collection)
    {}

    FilterBlockPresentation* presentationByName(const int row,
                                                FilterBlock* filter,
                                                FilterGraphPresentation* parent,
                                                DiagramScene* scene);

    FiltersCollection* fCollection;
    set<int> openedFilters;

    bool filterIsEdited(int _typeid);
};

#endif // FILTERPRESENTATIONSCOLLECTION_H
